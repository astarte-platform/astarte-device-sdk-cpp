// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/data.hpp"

#include <chrono>
#include <cstdint>
#include <ctime>
#include <string>
#include <variant>
#include <vector>

#include "astarte_device_sdk/type.hpp"

namespace astarte::device {

auto Data::get_type() const -> AstarteType {
  struct Visitor {
    auto operator()(const int32_t& /*unused*/) -> AstarteType { return kInteger; }
    auto operator()(const int64_t& /*unused*/) -> AstarteType { return kLongInteger; }
    auto operator()(const double& /*unused*/) -> AstarteType { return kDouble; }
    auto operator()(const bool& /*unused*/) -> AstarteType { return kBoolean; }
    auto operator()(const std::string& /*unused*/) -> AstarteType { return kString; }
    auto operator()(const std::vector<uint8_t>& /*unused*/) -> AstarteType { return kBinaryBlob; }
    auto operator()(const std::chrono::system_clock::time_point& /*unused*/) -> AstarteType {
      return kDatetime;
    }
    auto operator()(const std::vector<int32_t>& /*unused*/) -> AstarteType { return kIntegerArray; }
    auto operator()(const std::vector<int64_t>& /*unused*/) -> AstarteType {
      return kLongIntegerArray;
    }
    auto operator()(const std::vector<double>& /*unused*/) -> AstarteType { return kDoubleArray; }
    auto operator()(const std::vector<bool>& /*unused*/) -> AstarteType { return kBooleanArray; }
    auto operator()(const std::vector<std::string>& /*unused*/) -> AstarteType {
      return kStringArray;
    }
    auto operator()(const std::vector<std::vector<uint8_t>>& /*unused*/) -> AstarteType {
      return kBinaryBlobArray;
    }
    auto operator()(const std::vector<std::chrono::system_clock::time_point>& /*unused*/)
        -> AstarteType {
      return kDatetimeArray;
    }
  };
  return std::visit(Visitor{}, data_);
}

auto Data::get_raw_data() const
    -> const std::variant<int32_t, int64_t, double, bool, std::string, std::vector<uint8_t>,
                          std::chrono::system_clock::time_point, std::vector<int32_t>,
                          std::vector<int64_t>, std::vector<double>, std::vector<bool>,
                          std::vector<std::string>, std::vector<std::vector<uint8_t>>,
                          std::vector<std::chrono::system_clock::time_point>>& {
  return this->data_;
}

auto Data::operator==(const Data& other) const -> bool {
  return this->data_ == other.get_raw_data();
}
auto Data::operator!=(const Data& other) const -> bool {
  return this->data_ != other.get_raw_data();
}

}  // namespace astarte::device
