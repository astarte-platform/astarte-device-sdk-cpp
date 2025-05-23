// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/individual.hpp"

#include <sstream>
#include <string>
#include <utility>

#include "astarte_device_sdk/data.hpp"

namespace AstarteDeviceSdk {

AstarteIndividualDatastream::AstarteIndividualDatastream(AstarteData data)
    : data_(std::move(data)) {}

auto AstarteIndividualDatastream::get_value() const -> const AstarteData & { return data_; }

auto AstarteIndividualDatastream::operator==(const AstarteIndividualDatastream &other) const
    -> bool {
  return this->get_value() == other.get_value();
}
auto AstarteIndividualDatastream::operator!=(const AstarteIndividualDatastream &other) const
    -> bool {
  return this->get_value() != other.get_value();
}
auto AstarteIndividualDatastream::format() const -> std::string {
  std::ostringstream oss;
  oss << data_.format();
  return oss.str();
}

}  // namespace AstarteDeviceSdk
