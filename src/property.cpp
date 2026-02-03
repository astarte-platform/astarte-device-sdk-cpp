// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/property.hpp"

#include <optional>

#include "astarte_device_sdk/data.hpp"

namespace astarte::device {

PropertyIndividual::PropertyIndividual(const std::optional<Data>& data) : data_(data) {}

auto PropertyIndividual::get_value() const -> const std::optional<Data>& { return data_; }

auto PropertyIndividual::operator==(const PropertyIndividual& other) const -> bool {
  return this->get_value() == other.get_value();
}
auto PropertyIndividual::operator!=(const PropertyIndividual& other) const -> bool {
  return this->get_value() != other.get_value();
}

}  // namespace astarte::device
