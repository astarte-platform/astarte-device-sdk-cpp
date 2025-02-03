// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <astarteplatform/msghub/astarte_type.pb.h>
#include <google/protobuf/timestamp.pb.h>

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

#include "individual_private.h"
#include "spdlog/spdlog.h"

namespace AstarteDeviceSdk {

using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::chrono::seconds;

using astarteplatform::msghub::AstarteBinaryBlobArray;
using astarteplatform::msghub::AstarteBooleanArray;
using astarteplatform::msghub::AstarteDateTimeArray;
using astarteplatform::msghub::AstarteDoubleArray;
using astarteplatform::msghub::AstarteIntegerArray;
using astarteplatform::msghub::AstarteLongIntegerArray;
using astarteplatform::msghub::AstarteStringArray;

auto AstarteIndividualToAstarteDataTypeIndividual::operator()(int32_t value) const
    -> AstarteDataTypeIndividual * {
  auto *individual = new AstarteDataTypeIndividual();
  individual->set_astarte_integer(value);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}

auto AstarteIndividualToAstarteDataTypeIndividual::operator()(int64_t value) const
    -> AstarteDataTypeIndividual * {
  auto *individual = new AstarteDataTypeIndividual();
  individual->set_astarte_long_integer(value);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}

auto AstarteIndividualToAstarteDataTypeIndividual::operator()(double value) const
    -> AstarteDataTypeIndividual * {
  auto *individual = new AstarteDataTypeIndividual();
  individual->set_astarte_double(value);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}

auto AstarteIndividualToAstarteDataTypeIndividual::operator()(bool value) const
    -> AstarteDataTypeIndividual * {
  auto *individual = new AstarteDataTypeIndividual();
  individual->set_astarte_boolean(value);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}

auto AstarteIndividualToAstarteDataTypeIndividual::operator()(std::string value) const
    -> AstarteDataTypeIndividual * {
  auto *individual = new AstarteDataTypeIndividual();
  individual->set_astarte_string(value);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}

auto AstarteIndividualToAstarteDataTypeIndividual::operator()(std::vector<uint8_t> value) const
    -> AstarteDataTypeIndividual * {
  auto *individual = new AstarteDataTypeIndividual();
  std::string str_vector(value.begin(), value.end());
  individual->set_astarte_binary_blob(str_vector);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}

auto AstarteIndividualToAstarteDataTypeIndividual::operator()(
    std::chrono::system_clock::time_point value) const -> AstarteDataTypeIndividual * {
  auto *individual = new AstarteDataTypeIndividual();
  const std::chrono::system_clock::duration t_duration = value.time_since_epoch();
  const seconds sec = duration_cast<seconds>(t_duration);
  const nanoseconds nano = duration_cast<nanoseconds>(t_duration) - sec;
  auto *timestamp = new google::protobuf::Timestamp();
  timestamp->set_seconds(static_cast<int64_t>(sec.count()));
  timestamp->set_nanos(static_cast<int32_t>(nano.count()));
  individual->set_allocated_astarte_date_time(timestamp);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}

auto AstarteIndividualToAstarteDataTypeIndividual::operator()(
    const std::vector<int32_t> &values) const -> AstarteDataTypeIndividual * {
  auto *individual = new AstarteDataTypeIndividual();
  auto *data_array = new AstarteIntegerArray();
  for (const int32_t &value : values) {
    data_array->add_values(value);
  }
  individual->set_allocated_astarte_integer_array(data_array);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}

auto AstarteIndividualToAstarteDataTypeIndividual::operator()(
    const std::vector<int64_t> &values) const -> AstarteDataTypeIndividual * {
  auto *individual = new AstarteDataTypeIndividual();
  auto *data_array = new AstarteLongIntegerArray();
  for (const int64_t &value : values) {
    data_array->add_values(value);
  }
  individual->set_allocated_astarte_long_integer_array(data_array);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}

auto AstarteIndividualToAstarteDataTypeIndividual::operator()(
    const std::vector<double> &values) const -> AstarteDataTypeIndividual * {
  auto *individual = new AstarteDataTypeIndividual();
  auto *data_array = new AstarteDoubleArray();
  for (const double &value : values) {
    data_array->add_values(value);
  }
  individual->set_allocated_astarte_double_array(data_array);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}

auto AstarteIndividualToAstarteDataTypeIndividual::operator()(const std::vector<bool> &values) const
    -> AstarteDataTypeIndividual * {
  auto *individual = new AstarteDataTypeIndividual();
  auto *data_array = new AstarteBooleanArray();
  for (const bool &value : values) {
    data_array->add_values(value);
  }
  individual->set_allocated_astarte_boolean_array(data_array);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}

auto AstarteIndividualToAstarteDataTypeIndividual::operator()(
    const std::vector<std::string> &values) const -> AstarteDataTypeIndividual * {
  auto *individual = new AstarteDataTypeIndividual();
  auto *data_array = new AstarteStringArray();
  for (const std::string &value : values) {
    data_array->add_values(value);
  }
  individual->set_allocated_astarte_string_array(data_array);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}

auto AstarteIndividualToAstarteDataTypeIndividual::operator()(
    const std::vector<std::vector<uint8_t>> &values) const -> AstarteDataTypeIndividual * {
  auto *individual = new AstarteDataTypeIndividual();
  auto *data_array = new AstarteBinaryBlobArray();
  for (const std::vector<uint8_t> &value : values) {
    const std::string str_value(value.begin(), value.end());
    data_array->add_values(str_value);
  }
  individual->set_allocated_astarte_binary_blob_array(data_array);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}

auto AstarteIndividualToAstarteDataTypeIndividual::operator()(
    const std::vector<std::chrono::system_clock::time_point> &values) const
    -> AstarteDataTypeIndividual * {
  auto *individual = new AstarteDataTypeIndividual();
  auto *data_array = new AstarteDateTimeArray();
  for (const std::chrono::system_clock::time_point &value : values) {
    const std::chrono::system_clock::duration t_duration = value.time_since_epoch();
    const std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(t_duration);
    const std::chrono::nanoseconds nano =
        std::chrono::duration_cast<std::chrono::nanoseconds>(t_duration) - sec;
    // Instantiate the protobuf timestamp
    google::protobuf::Timestamp *timestamp = data_array->add_values();
    timestamp->set_seconds(static_cast<int64_t>(sec.count()));
    timestamp->set_nanos(static_cast<int32_t>(nano.count()));
  }
  individual->set_allocated_astarte_date_time_array(data_array);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}

}  // namespace AstarteDeviceSdk
