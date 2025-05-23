// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/property.hpp"

#include <optional>
#include <sstream>
#include <string>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/exceptions.hpp"

namespace AstarteDeviceSdk {

AstarteIndividualProperty::AstarteIndividualProperty(const std::optional<AstarteData> &data)
    : data_(data) {}

auto AstarteIndividualProperty::has_value() const -> bool { return data_.has_value(); }
auto AstarteIndividualProperty::get_value() const -> const AstarteData & {
  if (data_.has_value()) {
    return data_.value();
  }
  throw AstarteBadOptionException("AstarteIndividualProperty does not hold a value.");
}

auto AstarteIndividualProperty::operator==(const AstarteIndividualProperty &other) const -> bool {
  return ((this->has_value() && other.has_value()) && (this->get_value() == other.get_value()) ||
          (!this->has_value() && !other.has_value()));
}
auto AstarteIndividualProperty::operator!=(const AstarteIndividualProperty &other) const -> bool {
  return ((this->has_value() && other.has_value()) && (this->get_value() != other.get_value()) ||
          (!this->has_value() && other.has_value()) || (this->has_value() && !other.has_value()));
}
auto AstarteIndividualProperty::format() const -> std::string {
  std::ostringstream oss;
  if (data_.has_value()) {
    oss << data_.value().format();
  }
  return oss.str();
}

}  // namespace AstarteDeviceSdk
