// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_STORED_PROPERTY_H
#define ASTARTE_DEVICE_SDK_STORED_PROPERTY_H

/**
 * @file astarte_device_sdk/stored_property.hpp
 * @brief Astarte stored property class and its related methods.
 */

#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/ownership.hpp"

namespace AstarteDeviceSdk {

/** @brief Representing data for a stored property. */
class AstarteStoredProperty {
 public:
  /**
   * @brief Constructor for the AstarteStoredProperty class.
   * @param interface_name The name of the interface of the property.
   * @param path The path for the property.
   * @param version_major The major version for the interface of the property.
   * @param ownership The ownership for the interface of the property.
   * @param data The Astarte data for the property.
   */
  explicit AstarteStoredProperty(std::string_view interface_name, std::string_view path,
                                 int32_t version_major, AstarteOwnership ownership,
                                 AstarteData data);
  /**
   * @brief Get the interface name contained within the object.
   * @return A constant reference to the interface name string.
   */
  [[nodiscard]] auto get_interface_name() const -> const std::string&;
  /**
   * @brief Get the path contained within the object.
   * @return A constant reference to the path string.
   */
  [[nodiscard]] auto get_path() const -> const std::string&;
  /**
   * @brief Get the major version within the object.
   * @details The major version is the major version of the interface of the property.
   * @return The major version.
   */
  [[nodiscard]] auto get_version_major() const -> int32_t;
  /**
   * @brief Get the ownership contained within the object.
   * @details The ownership is the ownership of the interface of the property.
   * @return A constant reference to the ownership.
   */
  [[nodiscard]] auto get_ownership() const -> const AstarteOwnership&;
  /**
   * @brief Get the value contained within the object.
   * @return A constant reference to the data, if any.
   */
  [[nodiscard]] auto get_value() const -> const AstarteData&;
  /**
   * @brief Overloader for the comparison operator ==.
   * @param other The object to compare to.
   * @return True when equal, false otherwise.
   */
  [[nodiscard]] auto operator==(const AstarteStoredProperty& other) const -> bool;
  /**
   * @brief Overloader for the comparison operator !=.
   * @param other The object to compare to.
   * @return True when different, false otherwise.
   */
  [[nodiscard]] auto operator!=(const AstarteStoredProperty& other) const -> bool;

 private:
  std::string interface_name_;
  std::string path_;
  int32_t version_major_;
  AstarteOwnership ownership_;
  AstarteData data_;
};

}  // namespace AstarteDeviceSdk

/**
 * @brief astarte_fmt::formatter specialization for AstarteDeviceSdk::AstarteStoredProperty.
 */
template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::AstarteStoredProperty> {
  /**
   * @brief Parse the format string. Default implementation.
   * @param ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Format the AstarteStoredProperty object.
   * @param prop The AstarteStoredProperty to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::AstarteStoredProperty& prop, FormatContext& ctx) const {
    return astarte_fmt::format_to(ctx.out(),
                                  "Interface: {} v{}, Path: {}, Ownership: {}, Value: {}",
                                  prop.get_interface_name(), prop.get_version_major(),
                                  prop.get_path(), prop.get_ownership(), prop.get_value());
  }
};

/**
 * @brief Stream insertion operator for AstarteStoredProperty.
 * @param out The output stream.
 * @param prop The AstarteStoredProperty object to output.
 * @return Reference to the output stream.
 */
inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::AstarteStoredProperty& prop)
    -> std::ostream& {
  out << astarte_fmt::format("{}", prop);
  return out;
}

#endif  // ASTARTE_DEVICE_SDK_STORED_PROPERTY_H
