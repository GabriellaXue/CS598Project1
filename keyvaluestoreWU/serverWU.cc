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
#include <map>

#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "examples/protos/keyvaluestore.grpc.pb.h"
#else
#include "keyvaluestore.grpc.pb.h"
#endif

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using keyvaluestore::KeyValueStore;
using keyvaluestore::Request;
using keyvaluestore::Response;

# define KV_SIZE 1000

// KV data structure
std::map<std::string, std::string> myMap;

// Global function used in the KeyValueStoreServiceImpl
std::string get_value_from_map(std::string key) {
  std::string val = myMap.at(key); 
  return val;
}

// Logic and data behind the server's behavior.
class KeyValueStoreServiceImpl final : public KeyValueStore::Service {
  Status GetValue(ServerContext* context,
                  const Request* request, Response* response) override {
    response->set_value(get_value_from_map(request->key().c_str()));
    return Status::OK;
  }
};

void RunServer() {

  std::string server_address("0.0.0.0:50051");
  KeyValueStoreServiceImpl service;

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case, it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

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
  RunServer();

  return 0;
}
