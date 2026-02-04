// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/individual.hpp"

#include <utility>

#include "astarte_device_sdk/data.hpp"

namespace astarte::device {

DatastreamIndividual::DatastreamIndividual(Data data) : data_(std::move(data)) {}

auto DatastreamIndividual::get_value() const -> const Data& { return data_; }

auto DatastreamIndividual::operator==(const DatastreamIndividual& other) const -> bool {
  return this->get_value() == other.get_value();
}
auto DatastreamIndividual::operator!=(const DatastreamIndividual& other) const -> bool {
  return this->get_value() != other.get_value();
}

}  // namespace astarte::device
