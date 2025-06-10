// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef GRPC_CONVERTER_H
#define GRPC_CONVERTER_H

#include <astarteplatform/msghub/astarte_data.pb.h>
#include <astarteplatform/msghub/astarte_message.pb.h>

#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/individual.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/property.hpp"

namespace AstarteDeviceSdk {

using gRPCAstarteData = astarteplatform::msghub::AstarteData;
using gRPCAstarteDatastreamIndividual = astarteplatform::msghub::AstarteDatastreamIndividual;
using gRPCAstarteDatastreamObject = astarteplatform::msghub::AstarteDatastreamObject;
using gRPCAstartePropertyIndividual = astarteplatform::msghub::AstartePropertyIndividual;
using gRPCAstarteMessage = astarteplatform::msghub::AstarteMessage;

class GrpcConverterTo {
 public:
  auto operator()(int32_t value) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(int64_t value) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(double value) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(bool value) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(const std::string &value) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(const std::vector<uint8_t> &value) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(std::chrono::system_clock::time_point value) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(const std::vector<int32_t> &values) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(const std::vector<int64_t> &values) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(const std::vector<double> &values) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(const std::vector<bool> &values) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(const std::vector<std::string> &values) -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(const std::vector<std::vector<uint8_t>> &values)
      -> std::unique_ptr<gRPCAstarteData>;
  auto operator()(const std::vector<std::chrono::system_clock::time_point> &values)
      -> std::unique_ptr<gRPCAstarteData>;

  auto operator()(const AstarteData &value, const std::chrono::system_clock::time_point *timestamp)
      -> std::unique_ptr<gRPCAstarteDatastreamIndividual>;
  auto operator()(const AstarteObject &value,
                  const std::chrono::system_clock::time_point *timestamp)
      -> std::unique_ptr<gRPCAstarteDatastreamObject>;
  auto operator()(const std::optional<AstarteData> &value)
      -> std::unique_ptr<gRPCAstartePropertyIndividual>;
};

class GrpcConverterFrom {
 public:
  auto operator()(const gRPCAstarteData &value) -> AstarteData;
  auto operator()(const gRPCAstarteDatastreamIndividual &value) -> AstarteIndividualDatastream;
  auto operator()(const gRPCAstarteDatastreamObject &value) -> AstarteObject;
  auto operator()(const gRPCAstartePropertyIndividual &value) -> AstarteIndividualProperty;
  auto operator()(const gRPCAstarteMessage &value) -> AstarteMessage;
};

}  // namespace AstarteDeviceSdk

#endif  // GRPC_CONVERTER_H
