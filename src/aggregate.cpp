// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <astarteplatform/msghub/astarte_type.pb.h>

#include <string>
#include <unordered_map>
#include <variant>

#include "aggregate_private.h"
#include "astarte_device_sdk/individual.h"
#include "individual_private.h"

namespace AstarteDeviceSdk {

using astarteplatform::msghub::AstarteDataTypeIndividual;

auto AstarteAggregateToAstarteDataTypeObject::operator()(
    std::unordered_map<std::string, AstarteIndividual>& value) const -> AstarteDataTypeObject* {
  auto* grpc_object = new AstarteDataTypeObject();
  google::protobuf::Map<std::string, AstarteDataTypeIndividual>* grpc_map =
      grpc_object->mutable_object_data();
  for (const auto& pair : value) {
    const std::string& path = pair.first;
    const AstarteIndividual& individual = pair.second;
    AstarteDataTypeIndividual* grpc_individual =
        std::visit(AstarteIndividualToAstarteDataTypeIndividual(), individual);
    // TODO(simone): This could be a memory leak. It should be investigated.
    (*grpc_map)[path] = *grpc_individual;
  }
  return grpc_object;
}

}  // namespace AstarteDeviceSdk
