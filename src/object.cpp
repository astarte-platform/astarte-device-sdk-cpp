// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/object.hpp"

#include <initializer_list>
#include <sstream>
#include <string>
#include <utility>

#include "astarte_device_sdk/data.hpp"

namespace AstarteDeviceSdk {

// Default constructor
AstarteObjectDatastream::AstarteObjectDatastream() = default;
// Constructor with initializer list
AstarteObjectDatastream::AstarteObjectDatastream(std::initializer_list<MapType::value_type> init)
    : data_(init) {}
// Access element by key (modifiable)
auto AstarteObjectDatastream::at(const std::string& key) -> AstarteData& { return data_.at(key); }
// Access element by key (const)
auto AstarteObjectDatastream::at(const std::string& key) const -> const AstarteData& {
  return data_.at(key);
}
// Begin iterator (modifiable)
auto AstarteObjectDatastream::begin() -> iterator { return data_.begin(); }
// Begin iterator (const)
auto AstarteObjectDatastream::begin() const -> const_iterator { return data_.begin(); }
// End iterator (modifiable)
auto AstarteObjectDatastream::end() -> iterator { return data_.end(); }
// End iterator (const)
auto AstarteObjectDatastream::end() const -> const_iterator { return data_.end(); }
// Get size of the map
auto AstarteObjectDatastream::size() const -> size_type { return data_.size(); }
// Check if map is empty
auto AstarteObjectDatastream::empty() const -> bool { return data_.empty(); }
// Insert element into the map
void AstarteObjectDatastream::insert(const std::string& key, const AstarteData& data) {
  data_.insert({key, data});
}
// Erase element by key
auto AstarteObjectDatastream::erase(const std::string& key) -> size_type {
  return data_.erase(key);
}
// Clear the map
void AstarteObjectDatastream::clear() { data_.clear(); }
// Find element by key (modifiable)
auto AstarteObjectDatastream::find(const std::string& key) -> iterator { return data_.find(key); }
// Find element by key (const)
auto AstarteObjectDatastream::find(const std::string& key) const -> const_iterator {
  return data_.find(key);
}
#if defined(ASTARTE_FORMAT_ENABLED)
// Format the map content
auto AstarteObjectDatastream::format() const -> std::string {
  std::ostringstream oss;
  bool first = true;
  for (const auto& pair : data_) {
    if (!first) {
      oss << ", ";
    }
    oss << "\"" << pair.first << "\": " << pair.second.format();
    first = false;
  }
  return oss.str();
}
#endif

auto AstarteObjectDatastream::get_raw_data() const -> const MapType& { return this->data_; }

auto AstarteObjectDatastream::operator==(const AstarteObjectDatastream& other) const -> bool {
  return this->data_ == other.get_raw_data();
}
auto AstarteObjectDatastream::operator!=(const AstarteObjectDatastream& other) const -> bool {
  return this->data_ != other.get_raw_data();
}

}  // namespace AstarteDeviceSdk
