// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_MSG_H
#define ASTARTE_DEVICE_SDK_MSG_H

/**
 * @file astarte_device_sdk/msg.hpp
 * @brief Astarte message class and its related methods.
 */

#include <string>
#include <variant>

#include "astarte_device_sdk/individual.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/property.hpp"

namespace AstarteDeviceSdk {

/** @brief Astarte message class, represents a full message for/from Astarte. */
class AstarteMessage {
 public:
  /**
   * @brief Constructor for the AstarteMessage class.
   * @param interface The interface for the message.
   * @param path The path for the message.
   * @param data The data for the message.
   */
  AstarteMessage(
      std::string interface, std::string path,
      std::variant<AstarteIndividualDatastream, AstarteObjectDatastream, AstarteIndividualProperty>
          data);

  /**
   * @brief Get the interface of the message.
   * @return The interface.
   */
  [[nodiscard]] auto get_interface() const -> const std::string&;
  /**
   * @brief Get the path of the message.
   * @return The path.
   */
  [[nodiscard]] auto get_path() const -> const std::string&;
  /**
   * @brief Get the content of the message.
   * @return The value contained in the message.
   */
  [[nodiscard]] auto into() const
      -> const std::variant<AstarteIndividualDatastream, AstarteObjectDatastream,
                            AstarteIndividualProperty>&;
#if defined(ASTARTE_FORMAT_ENABLED)
  /**
   * @brief Pretty format the Astarte message.
   * @return A string representing in human readable format the content of the class instance.
   */
  [[nodiscard]] auto format() const -> std::string;
#endif
  /**
   * @brief Overloader for the comparison operator ==.
   * @param other The object to compare to.
   * @return True when equal, false otherwise.
   */
  [[nodiscard]] auto operator==(const AstarteMessage& other) const -> bool;
  /**
   * @brief Overloader for the comparison operator !=.
   * @param other The object to compare to.
   * @return True when different, false otherwise.
   */
  [[nodiscard]] auto operator!=(const AstarteMessage& other) const -> bool;

 private:
  std::string interface_;
  std::string path_;
  std::variant<AstarteIndividualDatastream, AstarteObjectDatastream, AstarteIndividualProperty>
      data_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_MSG_H
