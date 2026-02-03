// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "grpc/grpc_interceptors.hpp"

#include <grpcpp/support/interceptor.h>

#include <string>
#include <utility>

namespace astarte::device::grpc {

using ::grpc::experimental::InterceptionHookPoints;

NodeIdInterceptor::NodeIdInterceptor(std::string node_id) : node_id_(std::move(node_id)) {}

void NodeIdInterceptor::Intercept(InterceptorBatchMethods* methods) {
  if (methods->QueryInterceptionHookPoint(InterceptionHookPoints::PRE_SEND_INITIAL_METADATA)) {
    // Add the node ID to the metadata
    auto* metadata = methods->GetSendInitialMetadata();
    metadata->insert({"node-id", node_id_});
  }
  methods->Proceed();
}

NodeIdInterceptorFactory::NodeIdInterceptorFactory(std::string node_id)
    : node_id_(std::move(node_id)) {}

auto NodeIdInterceptorFactory::CreateClientInterceptor(ClientRpcInfo* info) -> Interceptor* {
  (void)info;
  return new NodeIdInterceptor(node_id_);
}

}  // namespace astarte::device::grpc
