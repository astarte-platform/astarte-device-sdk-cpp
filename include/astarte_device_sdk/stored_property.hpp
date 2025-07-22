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
#include <string>
#include <string_view>

#include "astarte_device_sdk/data.hpp"
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
  [[nodiscard]] auto get_interface_name() const -> const std::string &;
  /**
   * @brief Get the path contained within the object.
   * @return A constant reference to the path string.
   */
  [[nodiscard]] auto get_path() const -> const std::string &;
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
  [[nodiscard]] auto get_ownership() const -> const AstarteOwnership &;
  /**
   * @brief Get the value contained within the object.
   * @return A constant reference to the data, if any.
   */
  [[nodiscard]] auto get_value() const -> const AstarteData &;
  /**
   * @brief Overloader for the comparison operator ==.
   * @param other The object to compare to.
   * @return True when equal, false otherwise.
   */
  [[nodiscard]] auto operator==(const AstarteStoredProperty &other) const -> bool;
  /**
   * @brief Overloader for the comparison operator !=.
   * @param other The object to compare to.
   * @return True when different, false otherwise.
   */
  [[nodiscard]] auto operator!=(const AstarteStoredProperty &other) const -> bool;
#if defined(ASTARTE_FORMAT_ENABLED)
  /**
   * @brief Pretty format the Astarte stored individual property.
   * @return A string representing in human readable format the content of the class instance.
   */
  [[nodiscard]] auto format() const -> std::string;
#endif

 private:
  std::string interface_name_;
  std::string path_;
  int32_t version_major_;
  AstarteOwnership ownership_;
  AstarteData data_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_STORED_PROPERTY_H
