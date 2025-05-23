// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_INDIVIDUAL_H
#define ASTARTE_DEVICE_SDK_INDIVIDUAL_H

/**
 * @file astarte_device_sdk/individual.hpp
 * @brief Astarte individual datastream class and its related methods.
 */

#include <string>

#include "astarte_device_sdk/data.hpp"

namespace AstarteDeviceSdk {

/** @brief Representing the Astarte individual datastream data. */
class AstarteIndividualDatastream {
 public:
  /**
   * @brief Constructor for the AstarteIndividualDatastream class.
   * @param data The wrapped Astarte data type.
   */
  explicit AstarteIndividualDatastream(AstarteData data);
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
  [[nodiscard]] auto operator==(const AstarteIndividualDatastream &other) const -> bool;
  /**
   * @brief Overloader for the comparison operator !=.
   * @param other The object to compare to.
   * @return True when different, false otherwise.
   */
  [[nodiscard]] auto operator!=(const AstarteIndividualDatastream &other) const -> bool;
  /**
   * @brief Pretty format the Astarte individual datastream.
   * @return A string representing in human readable format the content of the class instance.
   */
  [[nodiscard]] auto format() const -> std::string;

 private:
  AstarteData data_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_INDIVIDUAL_H
