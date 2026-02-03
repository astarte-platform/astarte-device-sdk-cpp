// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef GRPC_CONVERTER_H
#define GRPC_CONVERTER_H

#include <astarteplatform/msghub/astarte_data.pb.h>
#include <astarteplatform/msghub/astarte_message.pb.h>
#include <astarteplatform/msghub/interface.pb.h>
#include <astarteplatform/msghub/property.pb.h>

#include <chrono>
#include <cstdint>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/errors.hpp"
#include "astarte_device_sdk/individual.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/stored_property.hpp"

namespace astarte::device {

using gRPCAstarteData = astarteplatform::msghub::AstarteData;
using gRPCAstarteDatastreamIndividual = astarteplatform::msghub::AstarteDatastreamIndividual;
using gRPCAstarteDatastreamObject = astarteplatform::msghub::AstarteDatastreamObject;
using gRPCAstartePropertyIndividual = astarteplatform::msghub::AstartePropertyIndividual;
using gRPCAstarteMessage = astarteplatform::msghub::AstarteMessage;
using gRPCStoredProperties = astarteplatform::msghub::StoredProperties;
using gRPCOwnership = astarteplatform::msghub::Ownership;

class GrpcConverterTo {
 public:
  auto operator()(int32_t value) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(int64_t value) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(double value) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(bool value) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(const std::string& value) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(const std::vector<uint8_t>& value) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(std::chrono::system_clock::time_point value) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(const std::vector<int32_t>& values) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(const std::vector<int64_t>& values) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(const std::vector<double>& values) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(const std::vector<bool>& values) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(const std::vector<std::string>& values) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(const std::vector<std::vector<uint8_t>>& values)
      -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(const std::vector<std::chrono::system_clock::time_point>& values)
      -> std::unique_ptr<gRPCAstarteData>;

  auto operator()(const Data& value, const std::chrono::system_clock::time_point* timestamp)
      -> std::unique_ptr<gRPCAstarteDatastreamIndividual>;
  auto operator()(const AstarteDatastreamObject& value,
                  const std::chrono::system_clock::time_point* timestamp)
      -> std::unique_ptr<gRPCAstarteDatastreamObject>;
  auto operator()(const std::optional<Data>& value)
      -> std::unique_ptr<gRPCAstartePropertyIndividual>;
};

class GrpcConverterFrom {
 public:
  auto operator()(const gRPCAstarteData& value) -> astarte_tl::expected<Data, Error>;
  auto operator()(const gRPCAstarteDatastreamIndividual& value)
      -> astarte_tl::expected<DatastreamIndividual, Error>;
  auto operator()(const gRPCAstarteDatastreamObject& value)
      -> astarte_tl::expected<AstarteDatastreamObject, Error>;
  auto operator()(const gRPCAstartePropertyIndividual& value)
      -> astarte_tl::expected<AstartePropertyIndividual, Error>;
  auto operator()(const gRPCAstarteMessage& value) -> astarte_tl::expected<Message, Error>;
  auto operator()(const gRPCOwnership& value) -> AstarteOwnership;
  auto operator()(const gRPCStoredProperties& value)
      -> astarte_tl::expected<std::list<AstarteStoredProperty>, Error>;
};

}  // namespace astarte::device

#endif  // GRPC_CONVERTER_H
