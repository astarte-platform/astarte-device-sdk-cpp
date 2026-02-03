// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_DEVICE_H
#define ASTARTE_DEVICE_SDK_DEVICE_H

/**
 * @file astarte_device_sdk/device.hpp
 * @brief Abstract Astarte device object and its related methods.
 */

#include <chrono>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/errors.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/stored_property.hpp"

/** @brief Umbrella namespace for the Astarte device SDK */
namespace astarte::device {

/**
 * @brief Interface for an Astarte device.
 * @details This class defines a transport independent abstract class to be used by derived
 * classes to implement transport dependent device classes.
 */
class Device {
 public:
  /**
   * @brief Virtual destructor.
   * @details Ensures proper cleanup of derived classes through a base pointer.
   */
  virtual ~Device() = default;
  /** @brief Copy constructor for the Astarte device class. */
  Device(const Device& other) = delete;
  /**
   * @brief Move constructor for the Astarte device class.
   * @param other Object to move.
   */
  Device(Device&& other) = default;
  /** @brief Copy assignment operator for the Astarte device class. */
  auto operator=(const Device& other) -> Device& = delete;
  /**
   * @brief Move assignment operator for the Astarte device class.
   * @param other Object to move.
   * @return Moved object.
   */
  auto operator=(Device&& other) -> Device& = default;
  /**
   * @brief Add an interface for the device from a JSON file.
   * @param json_file The path to the .json interface file.
   * @return An error if generated.
   */
  virtual auto add_interface_from_file(const std::filesystem::path& json_file)
      -> astarte_tl::expected<void, Error> = 0;
  /**
   * @brief Add an interface for the device from a JSON string view.
   * @param json The interface definition as a JSON string view.
   * @return An error if generated.
   */
  virtual auto add_interface_from_str(std::string_view json)
      -> astarte_tl::expected<void, Error> = 0;
  /**
   * @brief Remove an installed interface.
   * @param interface_name The interface name.
   * @return An error if generated.
   */
  virtual auto remove_interface(const std::string& interface_name)
      -> astarte_tl::expected<void, Error> = 0;
  /**
   * @brief Connect the device to Astarte.
   * @details This is an asynchronous function. It starts a management process that will handle
   * the device's connectivity in the background.
   * @return An error if generated.
   */
  virtual auto connect() -> astarte_tl::expected<void, Error> = 0;
  /**
   * @brief Check if the device is connected to the Astarte message hub.
   * @return True if the device is connected, false otherwise.
   */
  [[nodiscard]] virtual auto is_connected() const -> bool = 0;
  /**
   * @brief Disconnect the device from Astarte.
   * @return An error if generated.
   */
  virtual auto disconnect() -> astarte_tl::expected<void, Error> = 0;
  /**
   * @brief Send an individual data payload to Astarte.
   * @param interface_name The name of the target interface.
   * @param path The specific endpoint path within the interface.
   * @param data The data payload to send.
   * @param timestamp An optional timestamp for the data point. If nullptr, Astarte will assign one.
   * @return An error if generated.
   */
  virtual auto send_individual(std::string_view interface_name, std::string_view path,
                               const Data& data,
                               const std::chrono::system_clock::time_point* timestamp)
      -> astarte_tl::expected<void, Error> = 0;
  /**
   * @brief Send an aggregate object data payload to Astarte.
   * @param interface_name The name of the target interface.
   * @param path The common base path for the data object.
   * @param object The aggregate data object to send.
   * @param timestamp An optional timestamp for the data. If nullptr, Astarte will assign one.
   * @return An error if generated.
   */
  virtual auto send_object(std::string_view interface_name, std::string_view path,
                           const DatastreamObject& object,
                           const std::chrono::system_clock::time_point* timestamp)
      -> astarte_tl::expected<void, Error> = 0;
  /**
   * @brief Set a device property on Astarte.
   * @param interface_name The name of the interface containing the property.
   * @param path The full path to the property.
   * @param data The value to set for the property.
   * @return An error if generated.
   */
  virtual auto set_property(std::string_view interface_name, std::string_view path,
                            const Data& data) -> astarte_tl::expected<void, Error> = 0;
  /**
   * @brief Unset a device property on Astarte.
   * @param interface_name The name of the interface containing the property.
   * @param path The full path to the property to unset.
   * @return An error if generated.
   */
  virtual auto unset_property(std::string_view interface_name, std::string_view path)
      -> astarte_tl::expected<void, Error> = 0;
  /**
   * @brief Poll for incoming messages from Astarte.
   * @param timeout The maximum time to block waiting for a message.
   * @return An std::optional containing the received Message if one was available,
   * or std::nullopt if the timeout was reached.
   */
  virtual auto poll_incoming(const std::chrono::milliseconds& timeout)
      -> std::optional<Message> = 0;
  /**
   * @brief Get the current value for all properties matching the input filter.
   * @param ownership Optional ownership filter.
   * @return A list of properties matching the filter.
   */
  virtual auto get_all_properties(const std::optional<Ownership>& ownership)
      -> astarte_tl::expected<std::list<StoredProperty>, Error> = 0;
  /**
   * @brief Get the current value for properties matching the interface.
   * @param interface_name The name of the interface for the properties.
   * @return A list of properties matching the interface.
   */
  virtual auto get_properties(std::string_view interface_name)
      -> astarte_tl::expected<std::list<StoredProperty>, Error> = 0;
  /**
   * @brief Get a single property matching the interface name and path.
   * @param interface_name The name of the interface for the property.
   * @param path Exact path for the property.
   * @return The property value.
   */
  virtual auto get_property(std::string_view interface_name, std::string_view path)
      -> astarte_tl::expected<PropertyIndividual, Error> = 0;

 protected:
  /**
   * @brief Protected default constructor.
   * @details Prevents direct instantiation of the interface.
   */
  Device() = default;
};

}  // namespace astarte::device

#endif  // ASTARTE_DEVICE_SDK_DEVICE_H
