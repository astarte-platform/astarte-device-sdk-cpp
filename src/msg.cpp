// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/msg.hpp"

#include <sstream>
#include <string>
#include <utility>
#include <variant>

#include "astarte_device_sdk/individual.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/property.hpp"

namespace AstarteDeviceSdk {

AstarteMessage::AstarteMessage(
    std::string interface, std::string path,
    std::variant<AstarteIndividualDatastream, AstarteObjectDatastream, AstarteIndividualProperty>
        data)
    : interface_(std::move(interface)), path_(std::move(path)), data_(std::move(data)) {}

auto AstarteMessage::get_interface() const -> const std::string & { return interface_; }

auto AstarteMessage::get_path() const -> const std::string & { return path_; }

auto AstarteMessage::into() const
    -> const std::variant<AstarteIndividualDatastream, AstarteObjectDatastream,
                          AstarteIndividualProperty> & {
  return data_;
}

auto AstarteMessage::operator==(const AstarteMessage &other) const -> bool {
  return this->interface_ == other.get_interface() && this->path_ == other.get_path() &&
         this->data_ == other.into();
}
auto AstarteMessage::operator!=(const AstarteMessage &other) const -> bool {
  return this->interface_ != other.get_interface() || this->path_ != other.get_path() ||
         this->data_ != other.into();
}

#if defined(ASTARTE_FORMAT_ENABLED)
auto AstarteMessage::format() const -> std::string {
  std::ostringstream oss;
  oss << "{interface: " << interface_ << ", path: " << path_;
  const std::string formatted_data =
      std::visit([](const auto &arg) { return arg.format(); }, data_);
  if (!formatted_data.empty()) {
    oss << ", value: " << formatted_data;
  }
  oss << "}";
  return oss.str();
}
#endif

}  // namespace AstarteDeviceSdk
