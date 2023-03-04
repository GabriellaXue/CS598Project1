/*
 *
 * Copyright 2018 gRPC authors.
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
#include <vector>
#include <thread>
#include <unistd.h>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "examples/protos/keyvaluestore.grpc.pb.h"
#else
#include "keyvaluestore.grpc.pb.h"
#endif

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;
using keyvaluestore::KeyValueStore;
using keyvaluestore::Request;
using keyvaluestore::Response;

// TODO: add timeout

// Unique client id to differentiate it from each other
// TODO: find the correct id from connection info, use 0 for placeholder for now
std::string clientID = "0";

// Get majority of acks to continue with the operation.
int ackCount = 0;
time_t lastUpdate = 0;
std::string valProposed = "";
bool result_flag = false;

class KeyValueStoreClient {
 public:
  explicit KeyValueStoreClient(std::shared_ptr<Channel> channel)
      : stub_(KeyValueStore::NewStub(channel)) {}

  void GetValue(const std::string& key, const std::string& timestamp, const std::string& id) {
    CombinedRPC(key, "", timestamp, id);
  }

  void SetValue(const std::string& key, const std::string& val_flag, const std::string& timestamp, const std::string& id) {
    CombinedRPC(key, val_flag, timestamp, id);
  }

  void CombinedRPC(const std::string& key, const std::string& val_flag, const std::string& timestamp, const std::string& id) {
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    Request request;
    request.set_key(key);
    request.set_value(val_flag);
    request.set_timestamp(timestamp);
    request.set_id(id);

    // Call object to store rpc data
    AsyncClientCall* call = new AsyncClientCall;

    call->response_reader =
      stub_->PrepareAsyncCombinedRPC(&call->context, request, &cq_);

    call->response_reader->StartCall();

    call->response_reader->Finish(&call->response, &call->status, (void*)call);
    //std::cout << "set stat " << status << std::endl;
  }

  // Loop while listening for completed responses.
  // Prints out the response from the server.
  void AsyncCompleteRpc() {
    void* got_tag;
    bool ok = false;

    // Block until the next result is available in the completion queue "cq".
    while (cq_.Next(&got_tag, &ok)) {

      // The tag in this example is the memory location of the call object
      AsyncClientCall* call = static_cast<AsyncClientCall*>(got_tag);

      // Verify that the request was completed successfully. Note that "ok"
      // corresponds solely to the request for updates introduced by Finish().
      GPR_ASSERT(ok);

      if (call->status.ok() && std::stoi(call->response.timestamp()) >= 0) {
        time_t c_timestamp = std::stoi(call->response.timestamp());
        if (lastUpdate == 0) {
          lastUpdate = c_timestamp;
          valProposed = call->response.value();
        } else {
          if (lastUpdate < c_timestamp) {
            lastUpdate = c_timestamp;
            valProposed = call->response.value();
          }
        }
        ackCount ++;
        if (ackCount >= 3 && !result_flag) {
          if (valProposed == "w") std::cout << "Writer upated server successfully" << std::endl;
          else std::cout << "Read value " << valProposed << std::endl;
          result_flag = true;
        } 
      } else {
        std::cout << "RPC failed" << std::endl;
      }

      // Once we're complete, deallocate the call object.
      delete call;
    }
  }

  private:
    // struct for keeping state and data information
    struct AsyncClientCall {
      // Container for the data we expect from the server.
      Response response;

      // Context for the client. It could be used to convey extra information to
      // the server and/or tweak certain RPC behaviors.
      ClientContext context;

      // Storage for the status of the RPC upon completion.
      Status status;

      std::unique_ptr<ClientAsyncResponseReader<Response>> response_reader;
    };
  
  // Out of the passed in Channel comes the stub, stored here, our view of the
  // server's exposed services.
  std::unique_ptr<KeyValueStore::Stub> stub_;

  // The producer-consumer queue we use to communicate asynchronously with the
  // gRPC runtime.
  CompletionQueue cq_;
};

int main(int argc, char** argv) {

  std::string target_str1, target_str2, target_str3, target_str4, target_str5;
  target_str1 = "localhost:50051";
  target_str2 = "localhost:50052";
  target_str3 = "localhost:50053";
  target_str4 = "localhost:50054";
  target_str5 = "localhost:50055";
  
  KeyValueStoreClient client1(
      grpc::CreateChannel(target_str1, grpc::InsecureChannelCredentials()));

  KeyValueStoreClient client2(
      grpc::CreateChannel(target_str2, grpc::InsecureChannelCredentials()));

  KeyValueStoreClient client3(
      grpc::CreateChannel(target_str3, grpc::InsecureChannelCredentials()));
        
  KeyValueStoreClient client4(
      grpc::CreateChannel(target_str4, grpc::InsecureChannelCredentials()));

  KeyValueStoreClient client5(
      grpc::CreateChannel(target_str5, grpc::InsecureChannelCredentials()));

  // Spawn reader thread that loops indefinitely
  std::thread thread_1 = std::thread(&KeyValueStoreClient::AsyncCompleteRpc, &client1);
  std::thread thread_2 = std::thread(&KeyValueStoreClient::AsyncCompleteRpc, &client2);
  std::thread thread_3 = std::thread(&KeyValueStoreClient::AsyncCompleteRpc, &client3);
  std::thread thread_4 = std::thread(&KeyValueStoreClient::AsyncCompleteRpc, &client4);
  std::thread thread_5 = std::thread(&KeyValueStoreClient::AsyncCompleteRpc, &client5);
  
  if (argc == 2) {
    std::string key_given = argv[1];
    std::string timestamp_given = std::to_string(time(0));
    client1.GetValue(key_given, timestamp_given, clientID);
    client2.GetValue(key_given, timestamp_given, clientID);
    client3.GetValue(key_given, timestamp_given, clientID);
    client4.GetValue(key_given, timestamp_given, clientID);
    client5.GetValue(key_given, timestamp_given, clientID);
  } else if (argc == 3) {
    std::string key_given = argv[1];
    std::string val_given = argv[2];
    std::string timestamp_given = std::to_string(time(0));
    client1.SetValue(key_given,val_given, timestamp_given, clientID);
    client2.SetValue(key_given,val_given, timestamp_given, clientID);
    client3.SetValue(key_given,val_given, timestamp_given, clientID);
    client4.SetValue(key_given,val_given, timestamp_given, clientID);
    client5.SetValue(key_given,val_given, timestamp_given, clientID);
  } else {
    std::cout << "Invalid number of parameters, check usage." << std::endl;
  }

  std::cout << "Press control-c to quit" << std::endl << std::endl;
  thread_1.join();
  thread_2.join();
  thread_3.join();
  thread_4.join();
  thread_5.join();

  return 0;
}