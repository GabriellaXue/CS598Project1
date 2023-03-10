// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: keyvaluestore.proto
// Original file comments:
// Copyright 2018 gRPC authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#ifndef GRPC_keyvaluestore_2eproto__INCLUDED
#define GRPC_keyvaluestore_2eproto__INCLUDED

#include "keyvaluestore.pb.h"

#include <functional>
#include <grpcpp/generic/async_generic_service.h>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/client_context.h>
#include <grpcpp/completion_queue.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/proto_utils.h>
#include <grpcpp/impl/rpc_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/support/status.h>
#include <grpcpp/support/stub_options.h>
#include <grpcpp/support/sync_stream.h>

namespace keyvaluestore {

// A simple key-value storage service
class KeyValueStore final {
 public:
  static constexpr char const* service_full_name() {
    return "keyvaluestore.KeyValueStore";
  }
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    // Single RPC to communicate with server and client
    virtual ::grpc::Status CombinedRPC(::grpc::ClientContext* context, const ::keyvaluestore::Request& request, ::keyvaluestore::Response* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::keyvaluestore::Response>> AsyncCombinedRPC(::grpc::ClientContext* context, const ::keyvaluestore::Request& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::keyvaluestore::Response>>(AsyncCombinedRPCRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::keyvaluestore::Response>> PrepareAsyncCombinedRPC(::grpc::ClientContext* context, const ::keyvaluestore::Request& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::keyvaluestore::Response>>(PrepareAsyncCombinedRPCRaw(context, request, cq));
    }
    class async_interface {
     public:
      virtual ~async_interface() {}
      // Single RPC to communicate with server and client
      virtual void CombinedRPC(::grpc::ClientContext* context, const ::keyvaluestore::Request* request, ::keyvaluestore::Response* response, std::function<void(::grpc::Status)>) = 0;
      virtual void CombinedRPC(::grpc::ClientContext* context, const ::keyvaluestore::Request* request, ::keyvaluestore::Response* response, ::grpc::ClientUnaryReactor* reactor) = 0;
    };
    typedef class async_interface experimental_async_interface;
    virtual class async_interface* async() { return nullptr; }
    class async_interface* experimental_async() { return async(); }
   private:
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::keyvaluestore::Response>* AsyncCombinedRPCRaw(::grpc::ClientContext* context, const ::keyvaluestore::Request& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::keyvaluestore::Response>* PrepareAsyncCombinedRPCRaw(::grpc::ClientContext* context, const ::keyvaluestore::Request& request, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub final : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());
    ::grpc::Status CombinedRPC(::grpc::ClientContext* context, const ::keyvaluestore::Request& request, ::keyvaluestore::Response* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::keyvaluestore::Response>> AsyncCombinedRPC(::grpc::ClientContext* context, const ::keyvaluestore::Request& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::keyvaluestore::Response>>(AsyncCombinedRPCRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::keyvaluestore::Response>> PrepareAsyncCombinedRPC(::grpc::ClientContext* context, const ::keyvaluestore::Request& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::keyvaluestore::Response>>(PrepareAsyncCombinedRPCRaw(context, request, cq));
    }
    class async final :
      public StubInterface::async_interface {
     public:
      void CombinedRPC(::grpc::ClientContext* context, const ::keyvaluestore::Request* request, ::keyvaluestore::Response* response, std::function<void(::grpc::Status)>) override;
      void CombinedRPC(::grpc::ClientContext* context, const ::keyvaluestore::Request* request, ::keyvaluestore::Response* response, ::grpc::ClientUnaryReactor* reactor) override;
     private:
      friend class Stub;
      explicit async(Stub* stub): stub_(stub) { }
      Stub* stub() { return stub_; }
      Stub* stub_;
    };
    class async* async() override { return &async_stub_; }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    class async async_stub_{this};
    ::grpc::ClientAsyncResponseReader< ::keyvaluestore::Response>* AsyncCombinedRPCRaw(::grpc::ClientContext* context, const ::keyvaluestore::Request& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::keyvaluestore::Response>* PrepareAsyncCombinedRPCRaw(::grpc::ClientContext* context, const ::keyvaluestore::Request& request, ::grpc::CompletionQueue* cq) override;
    const ::grpc::internal::RpcMethod rpcmethod_CombinedRPC_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    // Single RPC to communicate with server and client
    virtual ::grpc::Status CombinedRPC(::grpc::ServerContext* context, const ::keyvaluestore::Request* request, ::keyvaluestore::Response* response);
  };
  template <class BaseClass>
  class WithAsyncMethod_CombinedRPC : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithAsyncMethod_CombinedRPC() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_CombinedRPC() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status CombinedRPC(::grpc::ServerContext* /*context*/, const ::keyvaluestore::Request* /*request*/, ::keyvaluestore::Response* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestCombinedRPC(::grpc::ServerContext* context, ::keyvaluestore::Request* request, ::grpc::ServerAsyncResponseWriter< ::keyvaluestore::Response>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_CombinedRPC<Service > AsyncService;
  template <class BaseClass>
  class WithCallbackMethod_CombinedRPC : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithCallbackMethod_CombinedRPC() {
      ::grpc::Service::MarkMethodCallback(0,
          new ::grpc::internal::CallbackUnaryHandler< ::keyvaluestore::Request, ::keyvaluestore::Response>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::keyvaluestore::Request* request, ::keyvaluestore::Response* response) { return this->CombinedRPC(context, request, response); }));}
    void SetMessageAllocatorFor_CombinedRPC(
        ::grpc::MessageAllocator< ::keyvaluestore::Request, ::keyvaluestore::Response>* allocator) {
      ::grpc::internal::MethodHandler* const handler = ::grpc::Service::GetHandler(0);
      static_cast<::grpc::internal::CallbackUnaryHandler< ::keyvaluestore::Request, ::keyvaluestore::Response>*>(handler)
              ->SetMessageAllocator(allocator);
    }
    ~WithCallbackMethod_CombinedRPC() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status CombinedRPC(::grpc::ServerContext* /*context*/, const ::keyvaluestore::Request* /*request*/, ::keyvaluestore::Response* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* CombinedRPC(
      ::grpc::CallbackServerContext* /*context*/, const ::keyvaluestore::Request* /*request*/, ::keyvaluestore::Response* /*response*/)  { return nullptr; }
  };
  typedef WithCallbackMethod_CombinedRPC<Service > CallbackService;
  typedef CallbackService ExperimentalCallbackService;
  template <class BaseClass>
  class WithGenericMethod_CombinedRPC : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithGenericMethod_CombinedRPC() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_CombinedRPC() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status CombinedRPC(::grpc::ServerContext* /*context*/, const ::keyvaluestore::Request* /*request*/, ::keyvaluestore::Response* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithRawMethod_CombinedRPC : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawMethod_CombinedRPC() {
      ::grpc::Service::MarkMethodRaw(0);
    }
    ~WithRawMethod_CombinedRPC() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status CombinedRPC(::grpc::ServerContext* /*context*/, const ::keyvaluestore::Request* /*request*/, ::keyvaluestore::Response* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestCombinedRPC(::grpc::ServerContext* context, ::grpc::ByteBuffer* request, ::grpc::ServerAsyncResponseWriter< ::grpc::ByteBuffer>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithRawCallbackMethod_CombinedRPC : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawCallbackMethod_CombinedRPC() {
      ::grpc::Service::MarkMethodRawCallback(0,
          new ::grpc::internal::CallbackUnaryHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::grpc::ByteBuffer* request, ::grpc::ByteBuffer* response) { return this->CombinedRPC(context, request, response); }));
    }
    ~WithRawCallbackMethod_CombinedRPC() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status CombinedRPC(::grpc::ServerContext* /*context*/, const ::keyvaluestore::Request* /*request*/, ::keyvaluestore::Response* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* CombinedRPC(
      ::grpc::CallbackServerContext* /*context*/, const ::grpc::ByteBuffer* /*request*/, ::grpc::ByteBuffer* /*response*/)  { return nullptr; }
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_CombinedRPC : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithStreamedUnaryMethod_CombinedRPC() {
      ::grpc::Service::MarkMethodStreamed(0,
        new ::grpc::internal::StreamedUnaryHandler<
          ::keyvaluestore::Request, ::keyvaluestore::Response>(
            [this](::grpc::ServerContext* context,
                   ::grpc::ServerUnaryStreamer<
                     ::keyvaluestore::Request, ::keyvaluestore::Response>* streamer) {
                       return this->StreamedCombinedRPC(context,
                         streamer);
                  }));
    }
    ~WithStreamedUnaryMethod_CombinedRPC() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status CombinedRPC(::grpc::ServerContext* /*context*/, const ::keyvaluestore::Request* /*request*/, ::keyvaluestore::Response* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status StreamedCombinedRPC(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::keyvaluestore::Request,::keyvaluestore::Response>* server_unary_streamer) = 0;
  };
  typedef WithStreamedUnaryMethod_CombinedRPC<Service > StreamedUnaryService;
  typedef Service SplitStreamedService;
  typedef WithStreamedUnaryMethod_CombinedRPC<Service > StreamedService;
};

}  // namespace keyvaluestore


#endif  // GRPC_keyvaluestore_2eproto__INCLUDED
