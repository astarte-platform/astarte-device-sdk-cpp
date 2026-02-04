// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/stored_property.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <utility>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/ownership.hpp"

namespace astarte::device {

StoredProperty::StoredProperty(std::string_view interface_name, std::string_view path,
                               int32_t version_major, Ownership ownership, Data data)
    : interface_name_(interface_name),
      path_(path),
      version_major_(version_major),
      ownership_(ownership),
      data_(std::move(data)) {}

auto StoredProperty::get_interface_name() const -> const std::string& { return interface_name_; }

auto StoredProperty::get_path() const -> const std::string& { return path_; }

auto StoredProperty::get_version_major() const -> int32_t { return version_major_; }

auto StoredProperty::get_ownership() const -> const Ownership& { return ownership_; }

auto StoredProperty::get_value() const -> const Data& { return data_; }

auto StoredProperty::operator==(const StoredProperty& other) const -> bool {
  return (interface_name_ == other.interface_name_) && (path_ == other.path_) &&
         (version_major_ == other.version_major_) && (ownership_ == other.ownership_) &&
         (data_ == other.data_);
}

auto StoredProperty::operator!=(const StoredProperty& other) const -> bool {
  return !(*this == other);
}

}  // namespace astarte::device
