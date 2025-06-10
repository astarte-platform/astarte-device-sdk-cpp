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
#include <utility>
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
  template <typename T>
  AstarteMessage(std::string interface, std::string path, T data)
      : interface_(std::move(interface)), path_(std::move(path)), data_(data) {}

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
   * @brief Check if this message contains a datastream.
   * @return True if the message contains a datastream, false otherwise.
   */
  [[nodiscard]] auto is_datastream() const -> bool;
  /**
   * @brief Check if this message contains individual data.
   * @return True if the message contains individual data, false otherwise.
   */
  [[nodiscard]] auto is_individual() const -> bool;
  /**
   * @brief Get the content of the message.
   * @return The value contained in the message.
   */
  template <typename T>
  [[nodiscard]] auto into() const -> const T& {
    return std::get<T>(data_);
  }
  /**
   * @brief Return the raw data contained in this class instance.
   * @return The raw data contained in this class instance.
   */
  [[nodiscard]] auto get_raw_data() const
      -> const std::variant<AstarteDatastreamIndividual, AstarteDatastreamObject,
                            AstartePropertyIndividual>&;
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
  std::variant<AstarteDatastreamIndividual, AstarteDatastreamObject, AstartePropertyIndividual>
      data_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_MSG_H
