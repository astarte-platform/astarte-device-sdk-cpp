// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/msg.hpp"

#include <sstream>
#include <string>
#include <variant>

#include "astarte_device_sdk/individual.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/property.hpp"

namespace AstarteDeviceSdk {

auto AstarteMessage::get_interface() const -> const std::string & { return interface_; }

auto AstarteMessage::get_path() const -> const std::string & { return path_; }

auto AstarteMessage::is_datastream() const -> bool {
  return std::holds_alternative<AstarteIndividualDatastream>(data_) ||
         std::holds_alternative<AstarteObjectDatastream>(data_);
}

auto AstarteMessage::is_individual() const -> bool {
  return std::holds_alternative<AstarteIndividualDatastream>(data_) ||
         std::holds_alternative<AstarteIndividualProperty>(data_);
}

auto AstarteMessage::get_raw_data() const
    -> const std::variant<AstarteIndividualDatastream, AstarteObjectDatastream,
                          AstarteIndividualProperty> & {
  return this->data_;
}

auto AstarteMessage::operator==(const AstarteMessage &other) const -> bool {
  return this->interface_ == other.get_interface() && this->path_ == other.get_path() &&
         this->data_ == other.get_raw_data();
}
auto AstarteMessage::operator!=(const AstarteMessage &other) const -> bool {
  return this->interface_ != other.get_interface() || this->path_ != other.get_path() ||
         this->data_ != other.get_raw_data();
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
