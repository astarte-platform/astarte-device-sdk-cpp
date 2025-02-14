// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/individual.hpp"

#include <astarteplatform/msghub/astarte_type.pb.h>
#include <google/protobuf/timestamp.pb.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <ios>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

#include "astarte_device_sdk/exceptions.hpp"
#include "astarte_device_sdk/type.hpp"
#include "individual_private.hpp"

namespace AstarteDeviceSdk {

using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::chrono::seconds;

using gRPCAstarteBinaryBlobArray = astarteplatform::msghub::AstarteBinaryBlobArray;
using gRPCAstarteBooleanArray = astarteplatform::msghub::AstarteBooleanArray;
using gRPCAstarteDateTimeArray = astarteplatform::msghub::AstarteDateTimeArray;
using gRPCAstarteDoubleArray = astarteplatform::msghub::AstarteDoubleArray;
using gRPCAstarteIntegerArray = astarteplatform::msghub::AstarteIntegerArray;
using gRPCAstarteLongIntegerArray = astarteplatform::msghub::AstarteLongIntegerArray;
using gRPCAstarteStringArray = astarteplatform::msghub::AstarteStringArray;
using gRPCAstarteDataTypeIndividual = astarteplatform::msghub::AstarteDataTypeIndividual;

namespace {
template <typename T>
auto format_vector(const std::vector<T> &data) -> std::string {
  std::ostringstream oss;
  oss << "[";
  for (size_t i = 0; i < data.size(); ++i) {
    oss << data[i];
    if (i != data.size() - 1) {
      oss << ", ";
    }
  }
  oss << "]";
  return oss.str();
}

auto format_hex(const std::vector<uint8_t> &data) -> std::string {
  std::ostringstream oss;
  for (size_t i = 0; i < data.size(); ++i) {
    oss << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
    if (i != data.size() - 1) {
      oss << " ";
    }
  }
  return oss.str();
}

// This function is only used for pretty printing
// NOLINTBEGIN(concurrency-mt-unsafe)
auto format_time(const std::chrono::system_clock::time_point &data) -> std::string {
  std::ostringstream oss;
  const std::time_t time = std::chrono::system_clock::to_time_t(data);
  const std::tm utc_tm = *std::gmtime(&time);
  oss << std::put_time(&utc_tm, "%Y-%m-%d %H:%M:%S %Z");
  return oss.str();
}
// NOLINTEND(concurrency-mt-unsafe)

}  // namespace

auto AstarteIndividual::get_type() const -> AstarteType {
  struct Visitor {
    auto operator()(const int32_t & /*unused*/) -> AstarteType { return INTEGER; }
    auto operator()(const int64_t & /*unused*/) -> AstarteType { return LONGINTEGER; }
    auto operator()(const double & /*unused*/) -> AstarteType { return DOUBLE; }
    auto operator()(const bool & /*unused*/) -> AstarteType { return BOOLEAN; }
    auto operator()(const std::string & /*unused*/) -> AstarteType { return STRING; }
    auto operator()(const std::vector<uint8_t> & /*unused*/) -> AstarteType { return BINARYBLOB; }
    auto operator()(const std::chrono::system_clock::time_point & /*unused*/) -> AstarteType {
      return DATETIME;
    }
    auto operator()(const std::vector<int32_t> & /*unused*/) -> AstarteType { return INTEGERARRAY; }
    auto operator()(const std::vector<int64_t> & /*unused*/) -> AstarteType {
      return LONGINTEGERARRAY;
    }
    auto operator()(const std::vector<double> & /*unused*/) -> AstarteType { return DOUBLEARRAY; }
    auto operator()(const std::vector<bool> & /*unused*/) -> AstarteType { return BOOLEANARRAY; }
    auto operator()(const std::vector<std::string> & /*unused*/) -> AstarteType {
      return STRINGARRAY;
    }
    auto operator()(const std::vector<std::vector<uint8_t>> & /*unused*/) -> AstarteType {
      return BINARYBLOBARRAY;
    }
    auto operator()(const std::vector<std::chrono::system_clock::time_point> & /*unused*/)
        -> AstarteType {
      return DATETIMEARRAY;
    }
  };
  return std::visit(Visitor{}, data_);
}

// This function while not very readable has an acceptable size
// NOLINTBEGIN(readability-function-size)
auto AstarteIndividual::format() const -> std::string {
  std::ostringstream oss;
  if (std::holds_alternative<int32_t>(data_)) {
    oss << std::get<int32_t>(data_);
  } else if (std::holds_alternative<int64_t>(data_)) {
    oss << std::get<int64_t>(data_);
  } else if (std::holds_alternative<double>(data_)) {
    oss << std::get<double>(data_);
  } else if (std::holds_alternative<bool>(data_)) {
    oss << std::get<bool>(data_);
  } else if (std::holds_alternative<std::string>(data_)) {
    oss << std::get<std::string>(data_);
  } else if (std::holds_alternative<std::vector<uint8_t>>(data_)) {
    oss << format_hex(std::get<std::vector<uint8_t>>(data_));
  } else if (std::holds_alternative<std::chrono::system_clock::time_point>(data_)) {
    oss << format_time(std::get<std::chrono::system_clock::time_point>(data_));
  } else if (std::holds_alternative<std::vector<int32_t>>(data_)) {
    oss << format_vector(std::get<std::vector<int32_t>>(data_));
  } else if (std::holds_alternative<std::vector<int64_t>>(data_)) {
    oss << format_vector(std::get<std::vector<int64_t>>(data_));
  } else if (std::holds_alternative<std::vector<double>>(data_)) {
    oss << format_vector(std::get<std::vector<double>>(data_));
  } else if (std::holds_alternative<std::vector<bool>>(data_)) {
    oss << format_vector(std::get<std::vector<bool>>(data_));
  } else if (std::holds_alternative<std::vector<std::string>>(data_)) {
    oss << format_vector(std::get<std::vector<std::string>>(data_));
  } else if (std::holds_alternative<std::vector<std::vector<uint8_t>>>(data_)) {
    const auto &data = std::get<std::vector<std::vector<uint8_t>>>(data_);
    oss << "[";
    for (size_t i = 0; i < data.size(); ++i) {
      oss << format_hex(data[i]);
      if (i != data.size() - 1) {
        oss << ", ";
      }
    }
    oss << "]";
  } else if (std::holds_alternative<std::vector<std::chrono::system_clock::time_point>>(data_)) {
    const auto &data = std::get<std::vector<std::chrono::system_clock::time_point>>(data_);
    oss << "[";
    for (size_t i = 0; i < data.size(); ++i) {
      oss << format_time(data[i]);
      if (i != data.size() - 1) {
        oss << ", ";
      }
    }
    oss << "]";
  }
  return oss.str();
}
// NOLINTEND(readability-function-size)

auto AstarteIndividual::get_raw_data() const
    -> const std::variant<int32_t, int64_t, double, bool, std::string, std::vector<uint8_t>,
                          std::chrono::system_clock::time_point, std::vector<int32_t>,
                          std::vector<int64_t>, std::vector<double>, std::vector<bool>,
                          std::vector<std::string>, std::vector<std::vector<uint8_t>>,
                          std::vector<std::chrono::system_clock::time_point>> & {
  return this->data_;
}

auto AstarteIndividualToAstarteDataTypeIndividual::operator()(int32_t value)
    -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  individual->set_astarte_integer(value);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto AstarteIndividualToAstarteDataTypeIndividual::operator()(int64_t value)
    -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  individual->set_astarte_long_integer(value);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto AstarteIndividualToAstarteDataTypeIndividual::operator()(double value)
    -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  individual->set_astarte_double(value);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto AstarteIndividualToAstarteDataTypeIndividual::operator()(bool value)
    -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  individual->set_astarte_boolean(value);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto AstarteIndividualToAstarteDataTypeIndividual::operator()(const std::string &value)
    -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  individual->set_astarte_string(value);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto AstarteIndividualToAstarteDataTypeIndividual::operator()(const std::vector<uint8_t> &value)
    -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  std::string str_vector(value.begin(), value.end());
  individual->set_astarte_binary_blob(str_vector);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto AstarteIndividualToAstarteDataTypeIndividual::operator()(
    std::chrono::system_clock::time_point value) -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
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
auto AstarteIndividualToAstarteDataTypeIndividual::operator()(const std::vector<int32_t> &values)
    -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  auto *data_array = new gRPCAstarteIntegerArray();
  for (const int32_t &value : values) {
    data_array->add_values(value);
  }
  individual->set_allocated_astarte_integer_array(data_array);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto AstarteIndividualToAstarteDataTypeIndividual::operator()(const std::vector<int64_t> &values)
    -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  auto *data_array = new gRPCAstarteLongIntegerArray();
  for (const int64_t &value : values) {
    data_array->add_values(value);
  }
  individual->set_allocated_astarte_long_integer_array(data_array);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto AstarteIndividualToAstarteDataTypeIndividual::operator()(const std::vector<double> &values)
    -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  auto *data_array = new gRPCAstarteDoubleArray();
  for (const double &value : values) {
    data_array->add_values(value);
  }
  individual->set_allocated_astarte_double_array(data_array);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto AstarteIndividualToAstarteDataTypeIndividual::operator()(const std::vector<bool> &values)
    -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  auto *data_array = new gRPCAstarteBooleanArray();
  for (const bool &value : values) {
    data_array->add_values(value);
  }
  individual->set_allocated_astarte_boolean_array(data_array);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto AstarteIndividualToAstarteDataTypeIndividual::operator()(
    const std::vector<std::string> &values) -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  auto *data_array = new gRPCAstarteStringArray();
  for (const std::string &value : values) {
    data_array->add_values(value);
  }
  individual->set_allocated_astarte_string_array(data_array);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto AstarteIndividualToAstarteDataTypeIndividual::operator()(
    const std::vector<std::vector<uint8_t>> &values) -> gRPCAstarteDataTypeIndividual * {
  auto *individual = new gRPCAstarteDataTypeIndividual();
  auto *data_array = new gRPCAstarteBinaryBlobArray();
  for (const std::vector<uint8_t> &value : values) {
    const std::string str_value(value.begin(), value.end());
    data_array->add_values(str_value);
  }
  individual->set_allocated_astarte_binary_blob_array(data_array);
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}
auto AstarteIndividualToAstarteDataTypeIndividual::operator()(
    const std::vector<std::chrono::system_clock::time_point> &values)
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
  spdlog::trace("AstarteIndividual converted to gRPC: {}", individual->DebugString());
  return individual;
}

// NOLINTBEGIN(readability-function-size)
auto AstarteDataTypeIndividualToAstarteIndividual::operator()(
    const gRPCAstarteDataTypeIndividual &value) -> AstarteIndividual {
  switch (value.individual_data_case()) {
    case gRPCAstarteDataTypeIndividual::kAstarteDouble:
      return AstarteIndividual(value.astarte_double());
    case gRPCAstarteDataTypeIndividual::kAstarteInteger:
      return AstarteIndividual(value.astarte_integer());
    case gRPCAstarteDataTypeIndividual::kAstarteBoolean:
      return AstarteIndividual(value.astarte_boolean());
    case gRPCAstarteDataTypeIndividual::kAstarteLongInteger:
      return AstarteIndividual(value.astarte_long_integer());
    case gRPCAstarteDataTypeIndividual::kAstarteString:
      return AstarteIndividual(value.astarte_string());
    case gRPCAstarteDataTypeIndividual::kAstarteBinaryBlob:
      return AstarteIndividual(std::vector<uint8_t>(value.astarte_binary_blob().begin(),
                                                    value.astarte_binary_blob().end()));
    case gRPCAstarteDataTypeIndividual::kAstarteDateTime: {
      const google::protobuf::Timestamp &timestamp = value.astarte_date_time();
      return AstarteIndividual(std::chrono::system_clock::time_point{
          std::chrono::seconds{timestamp.seconds()} + std::chrono::nanoseconds{timestamp.nanos()}});
    }
    case gRPCAstarteDataTypeIndividual::kAstarteDoubleArray:
      return AstarteIndividual(std::vector<double>(value.astarte_double_array().values().begin(),
                                                   value.astarte_double_array().values().end()));
    case gRPCAstarteDataTypeIndividual::kAstarteIntegerArray:
      return AstarteIndividual(std::vector<int32_t>(value.astarte_integer_array().values().begin(),
                                                    value.astarte_integer_array().values().end()));
    case gRPCAstarteDataTypeIndividual::kAstarteBooleanArray:
      return AstarteIndividual(std::vector<bool>(value.astarte_boolean_array().values().begin(),
                                                 value.astarte_boolean_array().values().end()));
    case gRPCAstarteDataTypeIndividual::kAstarteLongIntegerArray:
      return AstarteIndividual(
          std::vector<int64_t>(value.astarte_long_integer_array().values().begin(),
                               value.astarte_long_integer_array().values().end()));
    case gRPCAstarteDataTypeIndividual::kAstarteStringArray:
      return AstarteIndividual(
          std::vector<std::string>(value.astarte_string_array().values().begin(),
                                   value.astarte_string_array().values().end()));
    case gRPCAstarteDataTypeIndividual::kAstarteBinaryBlobArray: {
      std::vector<std::vector<uint8_t>> binblob_vect;
      for (const auto &str : value.astarte_binary_blob_array().values()) {
        binblob_vect.emplace_back(str.begin(), str.end());
      }
      return AstarteIndividual(binblob_vect);
    }
    case gRPCAstarteDataTypeIndividual::kAstarteDateTimeArray: {
      std::vector<std::chrono::system_clock::time_point> timestamp_vect;
      for (const auto &timestamp : value.astarte_date_time_array().values()) {
        timestamp_vect.emplace_back(std::chrono::seconds{timestamp.seconds()} +
                                    std::chrono::nanoseconds{timestamp.nanos()});
      }
      return AstarteIndividual(timestamp_vect);
    }
    default:
      break;
  }
  throw AstarteInternalException("Found an unrecognized gRPC gRPCAstarteDataTypeIndividual.");
}
// NOLINTEND(readability-function-size)

}  // namespace AstarteDeviceSdk
