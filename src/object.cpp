// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/object.hpp"

#include <initializer_list>
#include <string>

#include "astarte_device_sdk/data.hpp"

namespace astarte::device {

// Default constructor
AstarteDatastreamObject::AstarteDatastreamObject() = default;
// Constructor with initializer list
AstarteDatastreamObject::AstarteDatastreamObject(std::initializer_list<MapType::value_type> init)
    : data_(init) {}
// Access element by key (modifiable)
auto AstarteDatastreamObject::at(const std::string& key) -> AstarteData& { return data_.at(key); }
// Access element by key (const)
auto AstarteDatastreamObject::at(const std::string& key) const -> const AstarteData& {
  return data_.at(key);
}
// Begin iterator (modifiable)
auto AstarteDatastreamObject::begin() -> iterator { return data_.begin(); }
// Begin iterator (const)
auto AstarteDatastreamObject::begin() const -> const_iterator { return data_.begin(); }
// End iterator (modifiable)
auto AstarteDatastreamObject::end() -> iterator { return data_.end(); }
// End iterator (const)
auto AstarteDatastreamObject::end() const -> const_iterator { return data_.end(); }
// Get size of the map
auto AstarteDatastreamObject::size() const -> size_type { return data_.size(); }
// Check if map is empty
auto AstarteDatastreamObject::empty() const -> bool { return data_.empty(); }
// Insert element into the map
void AstarteDatastreamObject::insert(const std::string& key, const AstarteData& data) {
  data_.insert({key, data});
}
// Erase element by key
auto AstarteDatastreamObject::erase(const std::string& key) -> size_type {
  return data_.erase(key);
}
// Clear the map
void AstarteDatastreamObject::clear() { data_.clear(); }
// Find element by key (modifiable)
auto AstarteDatastreamObject::find(const std::string& key) -> iterator { return data_.find(key); }
// Find element by key (const)
auto AstarteDatastreamObject::find(const std::string& key) const -> const_iterator {
  return data_.find(key);
}

auto AstarteDatastreamObject::get_raw_data() const -> const MapType& { return this->data_; }

auto AstarteDatastreamObject::operator==(const AstarteDatastreamObject& other) const -> bool {
  return this->data_ == other.get_raw_data();
}
auto AstarteDatastreamObject::operator!=(const AstarteDatastreamObject& other) const -> bool {
  return this->data_ != other.get_raw_data();
}

}  // namespace astarte::device
