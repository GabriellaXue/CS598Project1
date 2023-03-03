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

class KeyValueStoreClient {
 public:
  explicit KeyValueStoreClient(std::shared_ptr<Channel> channel)
      : stub_(KeyValueStore::NewStub(channel)) {}

  void GetValue(const std::string& key) {
    CombinedRPC(key, "");
  }

  void SetValue(const std::string& key, const std::string& val_flag) {
    CombinedRPC(key, val_flag);
  }

  void CombinedRPC(const std::string& key, const std::string& val_flag) {
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    Request request;
    request.set_key(key);
    request.set_value(val_flag);

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

      if (call->status.ok())
        std::cout << "Value received: " << call->response.value() << std::endl;
      else
        std::cout << "RPC failed" << std::endl;

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
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).
  // In this example, we are using a cache which has been added in as an
  // interceptor.

  std::string target_str1;
  target_str1 = "localhost:50051";
  
  KeyValueStoreClient client(
      grpc::CreateChannel(target_str1, grpc::InsecureChannelCredentials()));


  // Spawn reader thread that loops indefinitely
  std::thread thread_ = std::thread(&KeyValueStoreClient::AsyncCompleteRpc, &client);


  char c = '0';
  
  // The below commented out code would async get vals for the given keys

  // for (int i = 10; i < 100; i++) {
  //   key = c + std::to_string(i);
  //   client.GetValue(key);  // The actual RPC call!
  // }

  // std::string key = argv[1];

  client.GetValue("100");
  client.GetValue("096");
  // std::string value = "2";
  client.SetValue("101", "2");
  client.SetValue("100", "3");
  client.GetValue("100");

  std::cout << "Press control-c to quit" << std::endl << std::endl;
  thread_.join();


  return 0;
}