// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef GRPC_INTERCEPTOR_H
#define GRPC_INTERCEPTOR_H

/**
 * @file private/grpc/grpc_interceptors.hpp
 * @brief gRPC Interceptors for the Astarte SDK.
 *
 * @details Defines client interceptors used to inject metadata (such as the Node ID)
 * into gRPC calls made by the Astarte device.
 */

#include <grpcpp/support/client_interceptor.h>
#include <grpcpp/support/interceptor.h>

#include <string>

namespace astarte::device::grpc {

using ::grpc::experimental::ClientInterceptorFactoryInterface;
using ::grpc::experimental::ClientRpcInfo;
using ::grpc::experimental::Interceptor;
using ::grpc::experimental::InterceptorBatchMethods;

/// @brief Interceptor that adds the Node ID to the metadata of outgoing gRPC calls.
class NodeIdInterceptor : public Interceptor {
 public:
  /**
   * @brief Constructs a NodeIdInterceptor.
   * @param[in] node_id The UUID of the node to act as authentication token.
   */
  explicit NodeIdInterceptor(std::string node_id);

  /**
   * @brief Intercepts a batch of gRPC methods.
   * @details Injects the node ID metadata before the call proceeds.
   * @param[in,out] methods Pointer to the batch methods object.
   */
  void Intercept(InterceptorBatchMethods* methods) override;

 private:
  std::string node_id_;
};

/// @brief Factory class for creating NodeIdInterceptor instances.
class NodeIdInterceptorFactory : public ClientInterceptorFactoryInterface {
 public:
  /**
   * @brief Constructs a NodeIdInterceptorFactory.
   * @param[in] node_id The UUID of the node to be passed to the interceptors.
   */
  explicit NodeIdInterceptorFactory(std::string node_id);

  /**
   * @brief Creates a new client interceptor.
   * @param[in] info Information about the RPC call.
   * @return A pointer to the newly created Interceptor.
   */
  auto CreateClientInterceptor(ClientRpcInfo* info) -> Interceptor* override;

 private:
  std::string node_id_;
};

}  // namespace astarte::device::grpc

#endif  // GRPC_INTERCEPTOR_H
