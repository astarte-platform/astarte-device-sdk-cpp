// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/stored_property.hpp"

#include <cstdint>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/ownership.hpp"

namespace AstarteDeviceSdk {

AstarteStoredProperty::AstarteStoredProperty(std::string_view interface_name, std::string_view path,
                                             int32_t version_major, AstarteOwnership ownership,
                                             AstarteData data)
    : interface_name_(interface_name),
      path_(path),
      version_major_(version_major),
      ownership_(ownership),
      data_(std::move(data)) {}

auto AstarteStoredProperty::get_interface_name() const -> const std::string & {
  return interface_name_;
}

auto AstarteStoredProperty::get_path() const -> const std::string & { return path_; }

auto AstarteStoredProperty::get_version_major() const -> int32_t { return version_major_; }

auto AstarteStoredProperty::get_ownership() const -> const AstarteOwnership & { return ownership_; }

auto AstarteStoredProperty::get_value() const -> const AstarteData & { return data_; }

auto AstarteStoredProperty::operator==(const AstarteStoredProperty &other) const -> bool {
  return (interface_name_ == other.interface_name_) && (path_ == other.path_) &&
         (version_major_ == other.version_major_) && (ownership_ == other.ownership_) &&
         (data_ == other.data_);
}

auto AstarteStoredProperty::operator!=(const AstarteStoredProperty &other) const -> bool {
  return !(*this == other);
}

#if defined(ASTARTE_FORMAT_ENABLED)
auto AstarteStoredProperty::format() const -> std::string {
  std::ostringstream stream;
  stream << "Interface: " << interface_name_ << " v" << version_major_ << ", Path: " << path_
         << ", Ownership: " << (ownership_ == AstarteOwnership::kDevice ? "device" : "server")
         << ", Value: " << data_.format();
  return stream.str();
}
#endif

}  // namespace AstarteDeviceSdk
