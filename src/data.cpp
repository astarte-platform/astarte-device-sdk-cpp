// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/data.hpp"

#if defined(ASTARTE_FORMAT_ENABLED)
#if __has_include(<format>)
#include <format>
#else  // __has_include(<format>)
#include <iomanip>
#endif  // __has_include(<format>)
#endif

#include <chrono>
#include <cstdint>
#include <ctime>
#include <sstream>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "astarte_device_sdk/type.hpp"

namespace AstarteDeviceSdk {

auto AstarteData::get_type() const -> AstarteType {
  struct Visitor {
    auto operator()(const int32_t & /*unused*/) -> AstarteType { return kInteger; }
    auto operator()(const int64_t & /*unused*/) -> AstarteType { return kLongInteger; }
    auto operator()(const double & /*unused*/) -> AstarteType { return kDouble; }
    auto operator()(const bool & /*unused*/) -> AstarteType { return kBoolean; }
    auto operator()(const std::string & /*unused*/) -> AstarteType { return kString; }
    auto operator()(const std::vector<uint8_t> & /*unused*/) -> AstarteType { return kBinaryBlob; }
    auto operator()(const std::chrono::system_clock::time_point & /*unused*/) -> AstarteType {
      return kDatetime;
    }
    auto operator()(const std::vector<int32_t> & /*unused*/) -> AstarteType {
      return kIntegerArray;
    }
    auto operator()(const std::vector<int64_t> & /*unused*/) -> AstarteType {
      return kLongIntegerArray;
    }
    auto operator()(const std::vector<double> & /*unused*/) -> AstarteType { return kDoubleArray; }
    auto operator()(const std::vector<bool> & /*unused*/) -> AstarteType { return kBooleanArray; }
    auto operator()(const std::vector<std::string> & /*unused*/) -> AstarteType {
      return kStringArray;
    }
    auto operator()(const std::vector<std::vector<uint8_t>> & /*unused*/) -> AstarteType {
      return kBinaryBlobArray;
    }
    auto operator()(const std::vector<std::chrono::system_clock::time_point> & /*unused*/)
        -> AstarteType {
      return kDatetimeArray;
    }
  };
  return std::visit(Visitor{}, data_);
}

auto AstarteData::get_raw_data() const
    -> const std::variant<int32_t, int64_t, double, bool, std::string, std::vector<uint8_t>,
                          std::chrono::system_clock::time_point, std::vector<int32_t>,
                          std::vector<int64_t>, std::vector<double>, std::vector<bool>,
                          std::vector<std::string>, std::vector<std::vector<uint8_t>>,
                          std::vector<std::chrono::system_clock::time_point>> & {
  return this->data_;
}

auto AstarteData::operator==(const AstarteData &other) const -> bool {
  return this->data_ == other.get_raw_data();
}
auto AstarteData::operator!=(const AstarteData &other) const -> bool {
  return this->data_ != other.get_raw_data();
}

#if defined(ASTARTE_FORMAT_ENABLED)
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

auto format_vector_bool(const std::vector<bool> &data) -> std::string {
  std::ostringstream oss;
  oss << "[";
  for (size_t i = 0; i < data.size(); ++i) {
    oss << (data[i] ? "true" : "false");
    if (i != data.size() - 1) {
      oss << ", ";
    }
  }
  oss << "]";
  return oss.str();
}

auto format_vector_string(const std::vector<std::string> &data) -> std::string {
  std::ostringstream oss;
  oss << "[";
  for (size_t i = 0; i < data.size(); ++i) {
    oss << "\"" << data[i] << "\"";
    if (i != data.size() - 1) {
      oss << ", ";
    }
  }
  oss << "]";
  return oss.str();
}

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
auto format_base64(const std::vector<uint8_t> &data) -> std::string {
  static constexpr std::string_view base64_chars =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz"
      "0123456789+/";
  std::string encoded_string;
  encoded_string.reserve(((data.size() + 2) / 3) * 4);

  size_t idx = 0;
  const size_t len = data.size();

  while (idx + 2 < len) {
    const uint32_t chunk = (data[idx] << 16) | (data[idx + 1] << 8) | data[idx + 2];
    encoded_string += base64_chars[(chunk >> 18) & 0x3F];
    encoded_string += base64_chars[(chunk >> 12) & 0x3F];
    encoded_string += base64_chars[(chunk >> 6) & 0x3F];
    encoded_string += base64_chars[chunk & 0x3F];
    idx += 3;
  }

  if (idx < len) {
    uint32_t chunk = data[idx] << 16;
    if (idx + 1 < len) {
      chunk |= data[idx + 1] << 8;
    }
    encoded_string += base64_chars[(chunk >> 18) & 0x3F];
    encoded_string += base64_chars[(chunk >> 12) & 0x3F];
    if (idx + 1 < len) {
      encoded_string += base64_chars[(chunk >> 6) & 0x3F];
      encoded_string += '=';
    } else {
      encoded_string += "==";
    }
  }

  std::ostringstream oss;
  oss << "\"" << encoded_string << "\"";
  return oss.str();
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

auto format_vector_binaryblob(const std::vector<std::vector<uint8_t>> &data) -> std::string {
  std::ostringstream oss;
  oss << "[";
  for (size_t i = 0; i < data.size(); ++i) {
    oss << format_base64(data[i]);
    if (i != data.size() - 1) {
      oss << ", ";
    }
  }
  oss << "]";
  return oss.str();
}

// This function is only used for pretty printing
// NOLINTBEGIN(concurrency-mt-unsafe)
auto format_timestamp(const std::chrono::system_clock::time_point &data) -> std::string {
  std::ostringstream oss;
#if __has_include(<format>)
  oss << std::format("{0:%F}T{0:%T}Z",
                     std::chrono::time_point_cast<std::chrono::milliseconds>(data));
#else   // __has_include(<format>)
  const std::time_t time = std::chrono::system_clock::to_time_t(data);
  const std::tm utc_tm = *std::gmtime(&time);
  oss << std::put_time(&utc_tm, "%FT%T.000Z");
#endif  // __has_include(<format>)
  return oss.str();
}
// NOLINTEND(concurrency-mt-unsafe)

auto format_vector_timestamp(const std::vector<std::chrono::system_clock::time_point> &data)
    -> std::string {
  std::ostringstream oss;
  oss << "[";
  for (size_t i = 0; i < data.size(); ++i) {
    oss << "\"" << format_timestamp(data[i]) << "\"";
    if (i != data.size() - 1) {
      oss << ", ";
    }
  }
  oss << "]";
  return oss.str();
}
}  // namespace

// This function while not very readable has an acceptable size
// NOLINTBEGIN(readability-function-size)
auto AstarteData::format() const -> std::string {
  std::ostringstream oss;
  if (std::holds_alternative<int32_t>(data_)) {
    oss << std::get<int32_t>(data_);
  } else if (std::holds_alternative<int64_t>(data_)) {
    oss << std::get<int64_t>(data_);
  } else if (std::holds_alternative<double>(data_)) {
    oss << std::get<double>(data_);
  } else if (std::holds_alternative<bool>(data_)) {
    oss << (std::get<bool>(data_) ? "true" : "false");
  } else if (std::holds_alternative<std::string>(data_)) {
    oss << "\"" << std::get<std::string>(data_) << "\"";
  } else if (std::holds_alternative<std::vector<uint8_t>>(data_)) {
    oss << format_base64(std::get<std::vector<uint8_t>>(data_));
  } else if (std::holds_alternative<std::chrono::system_clock::time_point>(data_)) {
    oss << "\"" << format_timestamp(std::get<std::chrono::system_clock::time_point>(data_)) << "\"";
  } else if (std::holds_alternative<std::vector<int32_t>>(data_)) {
    oss << format_vector(std::get<std::vector<int32_t>>(data_));
  } else if (std::holds_alternative<std::vector<int64_t>>(data_)) {
    oss << format_vector(std::get<std::vector<int64_t>>(data_));
  } else if (std::holds_alternative<std::vector<double>>(data_)) {
    oss << format_vector(std::get<std::vector<double>>(data_));
  } else if (std::holds_alternative<std::vector<bool>>(data_)) {
    oss << format_vector_bool(std::get<std::vector<bool>>(data_));
  } else if (std::holds_alternative<std::vector<std::string>>(data_)) {
    oss << format_vector_string(std::get<std::vector<std::string>>(data_));
  } else if (std::holds_alternative<std::vector<std::vector<uint8_t>>>(data_)) {
    oss << format_vector_binaryblob(std::get<std::vector<std::vector<uint8_t>>>(data_));
  } else if (std::holds_alternative<std::vector<std::chrono::system_clock::time_point>>(data_)) {
    oss << format_vector_timestamp(
        std::get<std::vector<std::chrono::system_clock::time_point>>(data_));
  }
  return oss.str();
}
// NOLINTEND(readability-function-size)
#endif

}  // namespace AstarteDeviceSdk
