// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/object.hpp"

#include <initializer_list>
#include <string>

#include "astarte_device_sdk/data.hpp"

namespace astarte::device {

// Default constructor
DatastreamObject::DatastreamObject() = default;
// Constructor with initializer list
DatastreamObject::DatastreamObject(std::initializer_list<MapType::value_type> init) : data_(init) {}
// Access element by key (modifiable)
auto DatastreamObject::at(const std::string& key) -> Data& { return data_.at(key); }
// Access element by key (const)
auto DatastreamObject::at(const std::string& key) const -> const Data& { return data_.at(key); }
// Begin iterator (modifiable)
auto DatastreamObject::begin() -> iterator { return data_.begin(); }
// Begin iterator (const)
auto DatastreamObject::begin() const -> const_iterator { return data_.begin(); }
// End iterator (modifiable)
auto DatastreamObject::end() -> iterator { return data_.end(); }
// End iterator (const)
auto DatastreamObject::end() const -> const_iterator { return data_.end(); }
// Get size of the map
auto DatastreamObject::size() const -> size_type { return data_.size(); }
// Check if map is empty
auto DatastreamObject::empty() const -> bool { return data_.empty(); }
// Insert element into the map
void DatastreamObject::insert(const std::string& key, const Data& data) {
  data_.insert({key, data});
}
// Erase element by key
auto DatastreamObject::erase(const std::string& key) -> size_type { return data_.erase(key); }
// Clear the map
void DatastreamObject::clear() { data_.clear(); }
// Find element by key (modifiable)
auto DatastreamObject::find(const std::string& key) -> iterator { return data_.find(key); }
// Find element by key (const)
auto DatastreamObject::find(const std::string& key) const -> const_iterator {
  return data_.find(key);
}

auto DatastreamObject::get_raw_data() const -> const MapType& { return this->data_; }

auto DatastreamObject::operator==(const DatastreamObject& other) const -> bool {
  return this->data_ == other.get_raw_data();
}
auto DatastreamObject::operator!=(const DatastreamObject& other) const -> bool {
  return this->data_ != other.get_raw_data();
}

}  // namespace astarte::device
