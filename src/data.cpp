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

auto Data::get_type() const -> Type {
  struct Visitor {
    auto operator()(const int32_t& /*unused*/) -> Type { return kInteger; }
    auto operator()(const int64_t& /*unused*/) -> Type { return kLongInteger; }
    auto operator()(const double& /*unused*/) -> Type { return kDouble; }
    auto operator()(const bool& /*unused*/) -> Type { return kBoolean; }
    auto operator()(const std::string& /*unused*/) -> Type { return kString; }
    auto operator()(const std::vector<uint8_t>& /*unused*/) -> Type { return kBinaryBlob; }
    auto operator()(const std::chrono::system_clock::time_point& /*unused*/) -> Type {
      return kDatetime;
    }
    auto operator()(const std::vector<int32_t>& /*unused*/) -> Type { return kIntegerArray; }
    auto operator()(const std::vector<int64_t>& /*unused*/) -> Type { return kLongIntegerArray; }
    auto operator()(const std::vector<double>& /*unused*/) -> Type { return kDoubleArray; }
    auto operator()(const std::vector<bool>& /*unused*/) -> Type { return kBooleanArray; }
    auto operator()(const std::vector<std::string>& /*unused*/) -> Type { return kStringArray; }
    auto operator()(const std::vector<std::vector<uint8_t>>& /*unused*/) -> Type {
      return kBinaryBlobArray;
    }
    auto operator()(const std::vector<std::chrono::system_clock::time_point>& /*unused*/) -> Type {
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
