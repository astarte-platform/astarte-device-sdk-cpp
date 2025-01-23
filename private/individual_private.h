// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef INDIVIDUAL_PRIVATE_H
#define INDIVIDUAL_PRIVATE_H

#include <astarteplatform/msghub/astarte_type.pb.h>

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace AstarteDeviceSdk {

using astarteplatform::msghub::AstarteDataTypeIndividual;

struct AstarteIndividualToAstarteDataTypeIndividual {
  auto operator()(int32_t value) const -> AstarteDataTypeIndividual *;
  auto operator()(int64_t value) const -> AstarteDataTypeIndividual *;
  auto operator()(double value) const -> AstarteDataTypeIndividual *;
  auto operator()(bool value) const -> AstarteDataTypeIndividual *;
  auto operator()(std::string value) const -> AstarteDataTypeIndividual *;
  auto operator()(std::vector<uint8_t> value) const -> AstarteDataTypeIndividual *;
  auto operator()(std::chrono::system_clock::time_point value) const -> AstarteDataTypeIndividual *;
  auto operator()(const std::vector<int32_t> &values) const -> AstarteDataTypeIndividual *;
  auto operator()(const std::vector<int64_t> &values) const -> AstarteDataTypeIndividual *;
  auto operator()(const std::vector<double> &values) const -> AstarteDataTypeIndividual *;
  auto operator()(const std::vector<bool> &values) const -> AstarteDataTypeIndividual *;
  auto operator()(const std::vector<std::string> &values) const -> AstarteDataTypeIndividual *;
  auto operator()(const std::vector<std::vector<uint8_t>> &values) const
      -> AstarteDataTypeIndividual *;
  auto operator()(const std::vector<std::chrono::system_clock::time_point> &values) const
      -> AstarteDataTypeIndividual *;
};

}  // namespace AstarteDeviceSdk

#endif  // INDIVIDUAL_PRIVATE_H
