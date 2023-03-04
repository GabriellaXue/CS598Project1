/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <map>
#include <tuple>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "examples/protos/keyvaluestore.grpc.pb.h"
#else
#include "keyvaluestore.grpc.pb.h"
#endif

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using keyvaluestore::KeyValueStore;
using keyvaluestore::Request;
using keyvaluestore::Response;

# define KV_SIZE 100

// KV data structure
std::map<std::string, std::string> myMap;

// Global lock to prevent data race
absl::Mutex mu_;

// Global time to keep track of the most recent ACK time
// Reject incoming writer if it has a smaller timestamp than local time.
time_t localTime = 0;

// Global function used in the KeyValueStoreServiceImpl
std::tuple<std::string, std::string, std::string> value_from_map(std::string key, std::string val_flag, std::string timestamp, std::string clientID) {
  time_t timestamp_int = std::stoi(timestamp);

  mu_.Lock();

  // if receiving SetValue RPC
  if (val_flag != "") {
    std::map<std::string,std::string>::iterator itr;
    itr = myMap.find(key);
    if (itr != myMap.end() && timestamp_int > localTime) {
      // set value equal to value specified in the flag
      itr->second = val_flag;
      localTime = timestamp_int;
      mu_.Unlock();
      return std::make_tuple(std::to_string(timestamp_int), clientID, val_flag); // I think the algorithm we implement is supposed to send back ACK here
    } else {
      mu_.Unlock();
      return std::make_tuple("-1", "0", "0");
    }

  // if receiving GetValue RPC
  } else {
      std::string val = myMap.at(key);
      mu_.Unlock();
      std::cout << "local time! " << std::to_string(localTime) << std::endl;
      return std::make_tuple(std::to_string(localTime), clientID, val);
  }
}

class ServerImpl final {
 public:
  ~ServerImpl() {
    server_->Shutdown();
    // Always shutdown the completion queue after the server.
    cq_->Shutdown();
  }

  // There is no shutdown handling in this code.
  void Run() {
    std::string server_address("0.0.0.0:50051");

    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service_" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *asynchronous* service.
    builder.RegisterService(&service_);
    // Get hold of the completion queue used for the asynchronous communication
    // with the gRPC runtime.
    cq_ = builder.AddCompletionQueue();
    // Finally assemble the server.
    server_ = builder.BuildAndStart();
    std::cout << "Server listening on " << server_address << std::endl;

    // Proceed to the server's main loop.
    HandleRpcs();
  }

 private:
  // Class encompasing the state and logic needed to serve a request.
  class CallData {
   public:
    // Take in the "service" instance (in this case representing an asynchronous
    // server) and the completion queue "cq" used for asynchronous communication
    // with the gRPC runtime.
    CallData(KeyValueStore::AsyncService* service, ServerCompletionQueue* cq)
        : service_(service), cq_(cq), responder_(&ctx_), status_(CREATE) {
      // Invoke the serving logic right away.
      Proceed();
    }

    void Proceed() {
      if (status_ == CREATE) {
        // Make this instance progress to the PROCESS state.
        status_ = PROCESS;

        // As part of the initial CREATE state, we *request* that the system
        // start processing SayHello requests. In this request, "this" acts are
        // the tag uniquely identifying the request (so that different CallData
        // instances can serve different requests concurrently), in this case
        // the memory address of this CallData instance.

        service_->RequestCombinedRPC(&ctx_, &request_, &responder_, cq_, cq_,
                                  this);
        
      } else if (status_ == PROCESS) {
        // Spawn a new CallData instance to serve new clients while we process
        // the one for this CallData. The instance will deallocate itself as
        // part of its FINISH state.
        new CallData(service_, cq_);

        // The actual processing.
        std::tuple<std::string, std::string, std::string> responseTuple = 
          value_from_map(request_.key().c_str(), request_.value().c_str(), request_.timestamp(), request_.id().c_str());
        response_.set_timestamp(std::get<0>(responseTuple));
        response_.set_id(std::get<1>(responseTuple));
        response_.set_value(std::get<2>(responseTuple));
        
        // And we are done! Let the gRPC runtime know we've finished, using the
        // memory address of this instance as the uniquely identifying tag for
        // the event.
        status_ = FINISH;
        responder_.Finish(response_, Status::OK, this);
      } else {
        GPR_ASSERT(status_ == FINISH);
        // Once in the FINISH state, deallocate ourselves (CallData).
        delete this;
      }
    }

   private:
    // The means of communication with the gRPC runtime for an asynchronous
    // server.
    KeyValueStore::AsyncService* service_;
    // The producer-consumer queue where for asynchronous server notifications.
    ServerCompletionQueue* cq_;
    // Context for the rpc, allowing to tweak aspects of it such as the use
    // of compression, authentication, as well as to send metadata back to the
    // client.
    ServerContext ctx_;

    // What we get from the client.
    Request request_;
    // What we send back to the client.
    Response response_;

    // The means to get back to the client.
    ServerAsyncResponseWriter<Response> responder_;

    // Let's implement a tiny state machine with the following states.
    enum CallStatus { CREATE, PROCESS, FINISH };
    CallStatus status_;  // The current serving state.
  };

  // This can be run in multiple threads if needed.
  void HandleRpcs() {
    // Spawn a new CallData instance to serve new clients.
    new CallData(&service_, cq_.get());

    void* tag;  // uniquely identifies a request.
    bool ok;

    while (true) {
      // Block waiting to read the next event from the completion queue. The
      // event is uniquely identified by its tag, which in this case is the
      // memory address of a CallData instance.
      // The return value of Next should always be checked. This return value
      // tells us whether there is any kind of event or cq_ is shutting down.
      GPR_ASSERT(cq_->Next(&tag, &ok));
      GPR_ASSERT(ok);
      static_cast<CallData*>(tag)->Proceed();
    }
  }

  std::unique_ptr<ServerCompletionQueue> cq_;
  KeyValueStore::AsyncService service_;
  std::unique_ptr<Server> server_;
};

int main(int argc, char** argv) {

  int keyLen = log10(KV_SIZE) + 1; // can hard code to 24 once final implementation
  int valLen = log10(KV_SIZE) + 1; // can hard code to 10 once final implementation

  // Build the KV store myMap, a std::map<> structure, with KV_SIZE key-val pairs
  for (int i = 1; i < KV_SIZE+1; ++i) {
    int currentLen = log10(i) + 1;
    std::string key;
    std::string val;

    // Padding the beginning of key and val with 0's
    char c = '0';
    int KeyLength = (keyLen - currentLen);
    int ValLength = (valLen - currentLen);
    std::string padKey = std::string(KeyLength, c);
    std::string padVal = std::string(ValLength, c);
    key = padKey + std::to_string(i);
    val = padVal + std::to_string(i);

    // Insert into KV
    myMap.insert(std::pair<std::string,std::string>(key,val));

    // Printed out to serverWU.cc terminal
    std::cout << "key created: " << key << std::endl;
    std::cout << "val created: " << val << std::endl;
  }
  ServerImpl server;
  server.Run();

  return 0;
}