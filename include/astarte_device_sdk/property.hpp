// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_PROPERTY_H
#define ASTARTE_DEVICE_SDK_PROPERTY_H

/**
 * @file astarte_device_sdk/individual.hpp
 * @brief Astarte individual datastream class and its related methods.
 */

#include <optional>
#include <string>

#include "astarte_device_sdk/data.hpp"

namespace AstarteDeviceSdk {

/** @brief Representing the Astarte individual datastream data. */
class AstartePropertyIndividual {
 public:
  /**
   * @brief Constructor for the AstarteDatastreamIndividual class.
   * @param data The wrapped Astarte data type.
   */
  explicit AstartePropertyIndividual(const std::optional<AstarteData> &data);
  /**
   * @brief Check if the object contains a value.
   * @return True if the object contains a value, false otherwise.
   */
  [[nodiscard]] auto has_value() const -> bool;
  /**
   * @brief Get the value contained within the object.
   * @return A constant reference to the data.
   */
  [[nodiscard]] auto get_value() const -> const AstarteData &;
  /**
   * @brief Overloader for the comparison operator ==.
   * @param other The object to compare to.
   * @return True when equal, false otherwise.
   */
  [[nodiscard]] auto operator==(const AstartePropertyIndividual &other) const -> bool;
  /**
   * @brief Overloader for the comparison operator !=.
   * @param other The object to compare to.
   * @return True when different, false otherwise.
   */
  [[nodiscard]] auto operator!=(const AstartePropertyIndividual &other) const -> bool;
  /**
   * @brief Pretty format the Astarte individual property.
   * @return A string representing in human readable format the content of the class instance.
   */
  [[nodiscard]] auto format() const -> std::string;

 private:
  std::optional<AstarteData> data_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_PROPERTY_H
