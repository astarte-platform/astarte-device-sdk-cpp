// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_STORED_PROPERTY_H
#define ASTARTE_DEVICE_SDK_STORED_PROPERTY_H

/**
 * @file astarte_device_sdk/stored_property.hpp
 * @brief Astarte stored property class.
 *
 * @details This file defines the StoredProperty class, which acts as a container for
 * a property's value and its associated metadata (interface name, path, version, ownership).
 */

#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/ownership.hpp"

namespace astarte::device {

/**
 * @brief Represents a stored property on the device.
 *
 * @details This class encapsulates the data and metadata of a property that is stored
 * locally on the device, often used for synchronization with the Astarte platform.
 */
class StoredProperty {
 public:
  /**
   * @brief Constructs a new Stored Property instance.
   *
   * @param[in] interface_name The name of the interface the property belongs to.
   * @param[in] path The specific path of the property.
   * @param[in] version_major The major version of the interface.
   * @param[in] ownership The ownership of the interface.
   * @param[in] data The value payload of the property.
   */
  explicit StoredProperty(std::string_view interface_name, std::string_view path,
                          int32_t version_major, Ownership ownership, Data data);
  /**
   * @brief Retrieves the interface name.
   * @return A constant reference to the interface name string.
   */
  [[nodiscard]] auto get_interface_name() const -> const std::string&;
  /**
   * @brief Retrieves the property path.
   * @return A constant reference to the path string.
   */
  [[nodiscard]] auto get_path() const -> const std::string&;
  /**
   * @brief Retrieves the major version of the interface.
   * @return The major version integer.
   */
  [[nodiscard]] auto get_version_major() const -> int32_t;
  /**
   * @brief Retrieves the ownership of the interface.
   * @return A constant reference to the ownership enumeration.
   */
  [[nodiscard]] auto get_ownership() const -> const Ownership&;
  /**
   * @brief Retrieves the value of the property.
   * @return A constant reference to the Data object containing the value.
   */
  [[nodiscard]] auto get_value() const -> const Data&;
  /**
   * @brief Equality comparison operator.
   * @param[in] other The StoredProperty object to compare with.
   * @return True if the properties are equal, false otherwise.
   */
  [[nodiscard]] auto operator==(const StoredProperty& other) const -> bool;
  /**
   * @brief Inequality comparison operator.
   * @param[in] other The StoredProperty object to compare with.
   * @return True if the properties are different, false otherwise.
   */
  [[nodiscard]] auto operator!=(const StoredProperty& other) const -> bool;

 private:
  std::string interface_name_;
  std::string path_;
  int32_t version_major_;
  Ownership ownership_;
  Data data_;
};

}  // namespace astarte::device

/// @brief astarte_fmt::formatter specialization for astarte::device::StoredProperty.
template <>
struct astarte_fmt::formatter<astarte::device::StoredProperty> {
  /**
   * @brief Parses the format string.
   * @details Default implementation required by the formatter interface.
   *
   * @param[in,out] ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Formats the StoredProperty object.
   *
   * @param[in] prop The StoredProperty instance to format.
   * @param[in,out] ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const astarte::device::StoredProperty& prop, FormatContext& ctx) const {
    return astarte_fmt::format_to(ctx.out(),
                                  "Interface: {} v{}, Path: {}, Ownership: {}, Value: {}",
                                  prop.get_interface_name(), prop.get_version_major(),
                                  prop.get_path(), prop.get_ownership(), prop.get_value());
  }
};

/**
 * @brief Stream insertion operator for StoredProperty.
 *
 * @param[in,out] out The output stream.
 * @param[in] prop The StoredProperty object to output.
 * @return A reference to the output stream.
 */
inline auto operator<<(std::ostream& out, const astarte::device::StoredProperty& prop)
    -> std::ostream& {
  out << astarte_fmt::format("{}", prop);
  return out;
}

#endif  // ASTARTE_DEVICE_SDK_STORED_PROPERTY_H
