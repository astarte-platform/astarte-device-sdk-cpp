// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/msg.hpp"

#include <string>
#include <variant>

#include "astarte_device_sdk/individual.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/property.hpp"

namespace astarte::device {

auto Message::get_interface() const -> const std::string& { return interface_; }

auto Message::get_path() const -> const std::string& { return path_; }

auto Message::is_datastream() const -> bool {
  return std::holds_alternative<DatastreamIndividual>(data_) ||
         std::holds_alternative<AstarteDatastreamObject>(data_);
}

auto Message::is_individual() const -> bool {
  return std::holds_alternative<DatastreamIndividual>(data_) ||
         std::holds_alternative<AstartePropertyIndividual>(data_);
}

auto Message::get_raw_data() const -> const
    std::variant<DatastreamIndividual, AstarteDatastreamObject, AstartePropertyIndividual>& {
  return this->data_;
}

auto Message::operator==(const Message& other) const -> bool {
  return this->interface_ == other.get_interface() && this->path_ == other.get_path() &&
         this->data_ == other.get_raw_data();
}
auto Message::operator!=(const Message& other) const -> bool {
  return this->interface_ != other.get_interface() || this->path_ != other.get_path() ||
         this->data_ != other.get_raw_data();
}

}  // namespace astarte::device
