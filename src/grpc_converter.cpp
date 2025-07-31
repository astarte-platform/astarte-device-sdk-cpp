// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "grpc_converter.hpp"

#include <astarteplatform/msghub/astarte_message.pb.h>
#include <astarteplatform/msghub/astarte_type.pb.h>
#include <google/protobuf/timestamp.pb.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/exceptions.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"

namespace AstarteDeviceSdk {

using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::chrono::seconds;

using gRPCAstarteDataType = astarteplatform::msghub::AstarteDataType;
using gRPCAstarteBinaryBlobArray = astarteplatform::msghub::AstarteBinaryBlobArray;
using gRPCAstarteBooleanArray = astarteplatform::msghub::AstarteBooleanArray;
using gRPCAstarteDateTimeArray = astarteplatform::msghub::AstarteDateTimeArray;
using gRPCAstarteDoubleArray = astarteplatform::msghub::AstarteDoubleArray;
using gRPCAstarteIntegerArray = astarteplatform::msghub::AstarteIntegerArray;
using gRPCAstarteLongIntegerArray = astarteplatform::msghub::AstarteLongIntegerArray;
using gRPCAstarteStringArray = astarteplatform::msghub::AstarteStringArray;
using gRPCAstarteDataTypeIndividual = astarteplatform::msghub::AstarteDataTypeIndividual;
using gRPCAstarteDataTypeObject = astarteplatform::msghub::AstarteDataTypeObject;
using gRPCAstarteMessage = astarteplatform::msghub::AstarteMessage;

auto GrpcConverter::operator()(int32_t value) -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  individual->set_astarte_integer(value);
  spdlog::trace("AstarteData converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto GrpcConverter::operator()(int64_t value) -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  individual->set_astarte_long_integer(value);
  spdlog::trace("AstarteData converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto GrpcConverter::operator()(double value) -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  individual->set_astarte_double(value);
  spdlog::trace("AstarteData converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto GrpcConverter::operator()(bool value) -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  individual->set_astarte_boolean(value);
  spdlog::trace("AstarteData converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto GrpcConverter::operator()(const std::string &value) -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  individual->set_astarte_string(value);
  spdlog::trace("AstarteData converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto GrpcConverter::operator()(const std::vector<uint8_t> &value)
    -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  std::string str_vector(value.begin(), value.end());
  individual->set_astarte_binary_blob(str_vector);
  spdlog::trace("AstarteData converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto GrpcConverter::operator()(std::chrono::system_clock::time_point value)
    -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  const std::chrono::system_clock::duration t_duration = value.time_since_epoch();
  const seconds sec = duration_cast<seconds>(t_duration);
  const nanoseconds nano = duration_cast<nanoseconds>(t_duration) - sec;
  auto *timestamp = new google::protobuf::Timestamp();
  timestamp->set_seconds(static_cast<int64_t>(sec.count()));
  timestamp->set_nanos(static_cast<int32_t>(nano.count()));
  individual->set_allocated_astarte_date_time(timestamp);
  spdlog::trace("AstarteData converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto GrpcConverter::operator()(const std::vector<int32_t> &values)
    -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  auto *data_array = new gRPCAstarteIntegerArray();
  for (const int32_t &value : values) {
    data_array->add_values(value);
  }
  individual->set_allocated_astarte_integer_array(data_array);
  spdlog::trace("AstarteData converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto GrpcConverter::operator()(const std::vector<int64_t> &values)
    -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  auto *data_array = new gRPCAstarteLongIntegerArray();
  for (const int64_t &value : values) {
    data_array->add_values(value);
  }
  individual->set_allocated_astarte_long_integer_array(data_array);
  spdlog::trace("AstarteData converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto GrpcConverter::operator()(const std::vector<double> &values)
    -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  auto *data_array = new gRPCAstarteDoubleArray();
  for (const double &value : values) {
    data_array->add_values(value);
  }
  individual->set_allocated_astarte_double_array(data_array);
  spdlog::trace("AstarteData converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto GrpcConverter::operator()(const std::vector<bool> &values) -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  auto *data_array = new gRPCAstarteBooleanArray();
  for (const bool &value : values) {
    data_array->add_values(value);
  }
  individual->set_allocated_astarte_boolean_array(data_array);
  spdlog::trace("AstarteData converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto GrpcConverter::operator()(const std::vector<std::string> &values)
    -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  auto *data_array = new gRPCAstarteStringArray();
  for (const std::string &value : values) {
    data_array->add_values(value);
  }
  individual->set_allocated_astarte_string_array(data_array);
  spdlog::trace("AstarteData converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto GrpcConverter::operator()(const std::vector<std::vector<uint8_t>> &values)
    -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  auto *data_array = new gRPCAstarteBinaryBlobArray();
  for (const std::vector<uint8_t> &value : values) {
    const std::string str_value(value.begin(), value.end());
    data_array->add_values(str_value);
  }
  individual->set_allocated_astarte_binary_blob_array(data_array);
  spdlog::trace("AstarteData converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto GrpcConverter::operator()(const std::vector<std::chrono::system_clock::time_point> &values)
    -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  auto *data_array = new gRPCAstarteDateTimeArray();
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
  spdlog::trace("AstarteData converted to gRPC: {}", individual->DebugString());
  return individual;
}

// NOLINTBEGIN(readability-function-size)
auto GrpcConverter::operator()(const gRPCAstarteDataTypeIndividual &value) -> AstarteData {
  switch (value.individual_data_case()) {
    case gRPCAstarteDataTypeIndividual::kAstarteDouble:
      return AstarteData(value.astarte_double());
    case gRPCAstarteDataTypeIndividual::kAstarteInteger:
      return AstarteData(value.astarte_integer());
    case gRPCAstarteDataTypeIndividual::kAstarteBoolean:
      return AstarteData(value.astarte_boolean());
    case gRPCAstarteDataTypeIndividual::kAstarteLongInteger:
      return AstarteData(value.astarte_long_integer());
    case gRPCAstarteDataTypeIndividual::kAstarteString:
      return AstarteData(value.astarte_string());
    case gRPCAstarteDataTypeIndividual::kAstarteBinaryBlob:
      return AstarteData(std::vector<uint8_t>(value.astarte_binary_blob().begin(),
                                              value.astarte_binary_blob().end()));
    case gRPCAstarteDataTypeIndividual::kAstarteDateTime: {
      const google::protobuf::Timestamp &timestamp = value.astarte_date_time();
      auto secs = std::chrono::seconds{timestamp.seconds()};
      auto nanos = std::chrono::nanoseconds{timestamp.nanos()};
      auto duration = std::chrono::duration_cast<std::chrono::system_clock::duration>(secs + nanos);
      const std::chrono::system_clock::time_point timepoint(duration);
      return AstarteData(timepoint);
    }
    case gRPCAstarteDataTypeIndividual::kAstarteDoubleArray:
      return AstarteData(std::vector<double>(value.astarte_double_array().values().begin(),
                                             value.astarte_double_array().values().end()));
    case gRPCAstarteDataTypeIndividual::kAstarteIntegerArray:
      return AstarteData(std::vector<int32_t>(value.astarte_integer_array().values().begin(),
                                              value.astarte_integer_array().values().end()));
    case gRPCAstarteDataTypeIndividual::kAstarteBooleanArray:
      return AstarteData(std::vector<bool>(value.astarte_boolean_array().values().begin(),
                                           value.astarte_boolean_array().values().end()));
    case gRPCAstarteDataTypeIndividual::kAstarteLongIntegerArray:
      return AstarteData(std::vector<int64_t>(value.astarte_long_integer_array().values().begin(),
                                              value.astarte_long_integer_array().values().end()));
    case gRPCAstarteDataTypeIndividual::kAstarteStringArray:
      return AstarteData(std::vector<std::string>(value.astarte_string_array().values().begin(),
                                                  value.astarte_string_array().values().end()));
    case gRPCAstarteDataTypeIndividual::kAstarteBinaryBlobArray: {
      std::vector<std::vector<uint8_t>> binblob_vect;
      for (const auto &str : value.astarte_binary_blob_array().values()) {
        binblob_vect.emplace_back(str.begin(), str.end());
      }
      return AstarteData(binblob_vect);
    }
    case gRPCAstarteDataTypeIndividual::kAstarteDateTimeArray: {
      std::vector<std::chrono::system_clock::time_point> timestamp_vect;
      for (const auto &timestamp : value.astarte_date_time_array().values()) {
        auto secs = std::chrono::seconds{timestamp.seconds()};
        auto nanos = std::chrono::nanoseconds{timestamp.nanos()};
        auto duration =
            std::chrono::duration_cast<std::chrono::system_clock::duration>(secs + nanos);
        timestamp_vect.emplace_back(duration);
      }
      return AstarteData(timestamp_vect);
    }
    default:
      break;
  }
  throw AstarteInternalException("Found an unrecognized gRPC gRPCAstarteDataTypeIndividual.");
}
// NOLINTEND(readability-function-size)

auto GrpcConverter::operator()(const gRPCAstarteMessage &value) -> AstarteMessage {
  std::optional<std::variant<AstarteData, AstarteObject>> parsed_data;
  if (value.has_astarte_data()) {
    const gRPCAstarteDataType &data = value.astarte_data();
    if (data.has_astarte_individual()) {
      const gRPCAstarteDataTypeIndividual &individual = data.astarte_individual();
      GrpcConverter converter;
      parsed_data = converter(individual);
    } else if (data.has_astarte_object()) {
      GrpcConverter converter;
      parsed_data = converter(data.astarte_object());
    } else {
      throw AstarteInternalException("Found an unrecognized gRPC gRPCAstarteDataType.");
    }
  } else if (value.has_astarte_unset()) {
    parsed_data = std::nullopt;
  }
  return {value.interface_name(), value.path(), parsed_data};
}

auto GrpcConverter::operator()(const AstarteObject &value) -> gRPCAstarteDataTypeObject * {
  auto *grpc_object = new gRPCAstarteDataTypeObject();
  google::protobuf::Map<std::string, gRPCAstarteDataTypeIndividual> *grpc_map =
      grpc_object->mutable_object_data();
  for (const auto &pair : value) {
    const std::string &path = pair.first;
    const AstarteData &data = pair.second;
    gRPCAstarteDataTypeIndividual *grpc_individual =
        std::visit(GrpcConverter(), data.get_raw_data());
    // TODO(simone): This could be a memory leak. It should be investigated.
    (*grpc_map)[path] = *grpc_individual;
  }
  return grpc_object;
}

auto GrpcConverter::operator()(const gRPCAstarteDataTypeObject &value) -> AstarteObject {
  AstarteObject object;
  const google::protobuf::Map<std::string, gRPCAstarteDataTypeIndividual> &object_data =
      value.object_data();
  GrpcConverter converter;
  for (const auto &[key, data] : object_data) {
    object.insert(key, converter(data));
  }
  return object;
}

}  // namespace AstarteDeviceSdk
