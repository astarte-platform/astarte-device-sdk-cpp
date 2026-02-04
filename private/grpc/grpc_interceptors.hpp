// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef GRPC_INTERCEPTOR_H
#define GRPC_INTERCEPTOR_H

#include <grpcpp/support/client_interceptor.h>
#include <grpcpp/support/interceptor.h>

#include <string>

namespace astarte::device::grpc {

using ::grpc::experimental::ClientInterceptorFactoryInterface;
using ::grpc::experimental::ClientRpcInfo;
using ::grpc::experimental::Interceptor;
using ::grpc::experimental::InterceptorBatchMethods;

class NodeIdInterceptor : public Interceptor {
 public:
  explicit NodeIdInterceptor(std::string node_id);
  void Intercept(InterceptorBatchMethods* methods) override;

 private:
  std::string node_id_;
};

class NodeIdInterceptorFactory : public ClientInterceptorFactoryInterface {
 public:
  explicit NodeIdInterceptorFactory(std::string node_id);
  auto CreateClientInterceptor(ClientRpcInfo* info) -> Interceptor* override;

 private:
  std::string node_id_;
};

}  // namespace astarte::device::grpc

#endif  // GRPC_INTERCEPTOR_H
