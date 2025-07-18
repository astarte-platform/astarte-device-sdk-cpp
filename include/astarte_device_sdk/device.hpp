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
#include <string_view>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"

/** @brief Umbrella namespace for the Astarte device SDK */
namespace AstarteDeviceSdk {

/**
 * @brief Interface for an Astarte device.
 * @details This class defines a transport independent abstract class to be used by derived
 * classes to implement transport dependent device classes.
 */
class AstarteDevice {
 public:
  /**
   * @brief Virtual destructor.
   * @details Ensures proper cleanup of derived classes through a base pointer.
   */
  virtual ~AstarteDevice() = default;
  /** @brief Copy constructor for the Astarte device class. */
  AstarteDevice(const AstarteDevice &other) = delete;
  /** @brief Copy assignment operator for the Astarte device class. */
  auto operator=(const AstarteDevice &other) -> AstarteDevice & = delete;
  /**
   * @brief Move constructor for the Astarte device class.
   * @param other Object to move.
   */
  AstarteDevice(AstarteDevice &&other) = default;
  /**
   * @brief Move assignment operator for the Astarte device class.
   * @param other Object to move.
   * @return Moved object.
   */
  auto operator=(AstarteDevice &&other) -> AstarteDevice & = default;

  /**
   * @brief Add an interface for the device from a JSON file.
   * @param json_file The path to the .json interface file.
   */
  virtual void add_interface_from_json(const std::filesystem::path &json_file) = 0;
  /**
   * @brief Add an interface for the device from a JSON string view.
   * @param json The interface definition as a JSON string view.
   */
  virtual void add_interface_from_str(std::string_view json) = 0;
  /**
   * @brief Connect the device to Astarte.
   * @details This is an asynchronous function. It starts a management process that will handle
   * the device's connectivity in the background.
   */
  virtual void connect() = 0;
  /**
   * @brief Check if the device is connected to the Astarte message hub.
   * @param timeout The maximum time to wait for a connection confirmation.
   * @return True if the device is connected, false otherwise.
   */
  [[nodiscard]] virtual auto is_connected(const std::chrono::milliseconds &timeout) const
      -> bool = 0;
  /**
   * @brief Disconnect the device from Astarte.
   */
  virtual void disconnect() = 0;
  /**
   * @brief Send an individual data payload to Astarte.
   * @param interface_name The name of the target interface.
   * @param path The specific endpoint path within the interface.
   * @param data The data payload to send.
   * @param timestamp An optional timestamp for the data point. If nullptr, Astarte will assign one.
   */
  virtual void send_individual(std::string_view interface_name, std::string_view path,
                               const AstarteData &data,
                               const std::chrono::system_clock::time_point *timestamp) = 0;
  /**
   * @brief Send an aggregate object data payload to Astarte.
   * @param interface_name The name of the target interface.
   * @param path The common base path for the data object.
   * @param object The aggregate data object to send.
   * @param timestamp An optional timestamp for the data. If nullptr, Astarte will assign one.
   */
  virtual void send_object(std::string_view interface_name, std::string_view path,
                           const AstarteDatastreamObject &object,
                           const std::chrono::system_clock::time_point *timestamp) = 0;
  /**
   * @brief Set a device property on Astarte.
   * @param interface_name The name of the interface containing the property.
   * @param path The full path to the property.
   * @param data The value to set for the property.
   */
  virtual void set_property(std::string_view interface_name, std::string_view path,
                            const AstarteData &data) = 0;
  /**
   * @brief Unset a device property on Astarte.
   * @param interface_name The name of the interface containing the property.
   * @param path The full path to the property to unset.
   */
  virtual void unset_property(std::string_view interface_name, std::string_view path) = 0;
  /**
   * @brief Poll for incoming messages from Astarte.
   * @param timeout The maximum time to block waiting for a message.
   * @return An std::optional containing the received AstarteMessage if one was available,
   * or std::nullopt if the timeout was reached.
   */
  virtual auto poll_incoming(const std::chrono::milliseconds &timeout)
      -> std::optional<AstarteMessage> = 0;

 protected:
  /**
   * @brief Protected default constructor.
   * @details Prevents direct instantiation of the interface.
   */
  AstarteDevice() = default;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_DEVICE_H
