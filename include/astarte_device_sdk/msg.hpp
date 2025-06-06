// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_MSG_H
#define ASTARTE_DEVICE_SDK_MSG_H

/**
 * @file astarte_device_sdk/msg.hpp
 * @brief Astarte message class and its related methods.
 */

#include <cstdint>
#include <optional>
#include <string>
#include <variant>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/object.hpp"

namespace AstarteDeviceSdk {

/** @brief Possible Astarte message types. */
enum class AstarteMessageType : int8_t {
  /** @brief Datastream individual. */
  DATASTREAM_INDIVIDUAL,
  /** @brief Datastream aggregated object. */
  DATASTREAM_OBJECT,
  /** @brief Property individual. */
  PROPERTY_INDIVIDUAL
};

/** @brief Astarte message class, represents a full message for/from Astarte. */
class AstarteMessage {
 public:
  /**
   * @brief Constructor for the AstarteMessage class.
   * @param interface The interface for the message.
   * @param path The path for the message.
   * @param type The type of the message.
   * @param data The data for the message. This may be one of:
   *             - A data value used for individual datastreams and properties
   *             - An object message used for object datastreams
   *             - The no-option value used for unsetting properties (there is no new value)
   */
  AstarteMessage(std::string interface, std::string path, AstarteMessageType type,
                 std::optional<std::variant<AstarteData, AstarteObject>> data);

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
   * @brief Get the type of the message.
   * @return The message type.
   */
  [[nodiscard]] auto get_message_type() const -> const AstarteMessageType&;
  /**
   * @brief Get the content of the message.
   * @return The value contained in the message.
   */
  [[nodiscard]] auto into() const -> const std::optional<std::variant<AstarteData, AstarteObject>>&;
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
  AstarteMessageType type_;
  std::optional<std::variant<AstarteData, AstarteObject>> data_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_MSG_H
