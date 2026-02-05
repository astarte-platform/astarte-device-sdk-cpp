// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_DEVICE_H
#define ASTARTE_DEVICE_SDK_DEVICE_H

/**
 * @file astarte_device_sdk/device.hpp
 * @brief Abstract Astarte device interface.
 *
 * @details This file defines the pure virtual interface that all Astarte device
 * implementations must adhere to, providing transport-agnostic connectivity and
 * data transmission capabilities.
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

// NOLINTBEGIN(modernize-concat-nested-namespaces) Not nested for doxygen
/// @brief Global namespace for all Astarte related functionality.
namespace astarte {
/// @brief Umbrella namespace for the Astarte device library.
namespace device {
// NOLINTEND(modernize-concat-nested-namespaces)

/**
 * @brief Interface for an Astarte device.
 *
 * @details This abstract class normalizes interactions with the Astarte IoT platform,
 * hiding transport-specific implementation details (e.g., MQTT, gRPC) from the user.
 * Users should instantiate a concrete implementation of this class.
 */
class Device {
 public:
  /// @brief Virtual destructor.
  virtual ~Device() = default;

  /// @brief Device is non-copyable.
  Device(const Device& other) = delete;

  /**
   * @brief Move constructor.
   * @param[in,out] other The device instance to move data from.
   */
  Device(Device&& other) = default;

  /// @brief Device is non-copyable.
  auto operator=(const Device& other) -> Device& = delete;

  /**
   * @brief Move assignment operator.
   * @param[in,out] other The device instance to move data from.
   * @return A reference to this device.
   */
  auto operator=(Device&& other) -> Device& = default;

  /**
   * @brief Adds an interface definition to the device from a JSON file.
   * @details Parses the JSON file to validate and register a new Astarte Interface.
   *
   * @param[in] json_file The filesystem path to the .json interface definition.
   * @return An expected containing void on success or Error on failure.
   */
  virtual auto add_interface_from_file(const std::filesystem::path& json_file)
      -> astarte_tl::expected<void, Error> = 0;

  /**
   * @brief Adds an interface definition to the device from a JSON string.
   *
   * @param[in] json The interface definition as a JSON string view.
   * @return An expected containing void on success or Error on failure.
   */
  virtual auto add_interface_from_str(std::string_view json)
      -> astarte_tl::expected<void, Error> = 0;

  /**
   * @brief Removes an installed interface from the device.
   *
   * @param[in] interface_name The name of the interface to remove.
   * @return An expected containing void on success or Error on failure.
   */
  virtual auto remove_interface(const std::string& interface_name)
      -> astarte_tl::expected<void, Error> = 0;

  /**
   * @brief Connects the device to the Astarte platform.
   *
   * @details This is an asynchronous operation. It initializes the transport layer
   * and starts a background routine to maintain connectivity.
   *
   * @return An expected containing void on success or Error on failure.
   */
  virtual auto connect() -> astarte_tl::expected<void, Error> = 0;

  /**
   * @brief Checks connectivity status.
   * @return True if the device is fully connected to Astarte, false otherwise.
   */
  [[nodiscard]] virtual auto is_connected() const -> bool = 0;

  /**
   * @brief Disconnects the device from Astarte.
   * @return An expected containing void on success or Error on failure.
   */
  virtual auto disconnect() -> astarte_tl::expected<void, Error> = 0;

  /**
   * @brief Sends an individual data point to an Astarte Interface.
   *
   * @param[in] interface_name The name of the target interface.
   * @param[in] path The specific mapping path within the interface (e.g., "/sensors/temp").
   * @param[in] data The value payload to transmit.
   * @param[in] timestamp Optional timestamp. Should match the interface timestamp configuration.
   * @return An expected containing void on success or Error on failure.
   */
  virtual auto send_individual(std::string_view interface_name, std::string_view path,
                               const Data& data,
                               const std::chrono::system_clock::time_point* timestamp)
      -> astarte_tl::expected<void, Error> = 0;

  /**
   * @brief Sends an aggregated object to an Astarte Interface.
   *
   * @param[in] interface_name The name of the target interface.
   * @param[in] path The common base path for the object aggregation.
   * @param[in] object The map of keys and values constituting the object.
   * @param[in] timestamp Optional timestamp. Should match the interface timestamp configuration.
   * @return An expected containing void on success or Error on failure.
   */
  virtual auto send_object(std::string_view interface_name, std::string_view path,
                           const DatastreamObject& object,
                           const std::chrono::system_clock::time_point* timestamp)
      -> astarte_tl::expected<void, Error> = 0;

  /**
   * @brief Updates a local device property and synchronize it with Astarte.
   *
   * @param[in] interface_name The name of the interface containing the property.
   * @param[in] path The specific path of the property.
   * @param[in] data The new value for the property.
   * @return An expected containing void on success or Error on failure.
   */
  virtual auto set_property(std::string_view interface_name, std::string_view path,
                            const Data& data) -> astarte_tl::expected<void, Error> = 0;

  /**
   * @brief Unsets (deletes) a device property.
   *
   * @param[in] interface_name The name of the interface containing the property.
   * @param[in] path The specific path of the property to unset.
   * @return An expected containing void on success or Error on failure.
   */
  virtual auto unset_property(std::string_view interface_name, std::string_view path)
      -> astarte_tl::expected<void, Error> = 0;

  /**
   * @brief Polls for incoming messages from Astarte.
   *
   * @details This method blocks the calling thread until a message is received or the
   * timeout expires.
   *
   * @param[in] timeout The maximum duration to block waiting for a message.
   * @return std::optional containing the Message if received, or std::nullopt if the
   * timeout was reached.
   */
  virtual auto poll_incoming(const std::chrono::milliseconds& timeout)
      -> std::optional<Message> = 0;

  /**
   * @brief Retrieves all stored properties matching an ownership filter.
   *
   * @param[in] ownership Optional filter, if std::nullopt, returns all properties.
   * @return An expected containing the list of properties on success or Error on failure.
   */
  virtual auto get_all_properties(const std::optional<Ownership>& ownership)
      -> astarte_tl::expected<std::list<StoredProperty>, Error> = 0;

  /**
   * @brief Retrieves all stored properties belonging to a specific interface.
   *
   * @param[in] interface_name The name of the interface to query.
   * @return An expected containing the list of properties on success or Error on failure.
   */
  virtual auto get_properties(std::string_view interface_name)
      -> astarte_tl::expected<std::list<StoredProperty>, Error> = 0;

  /**
   * @brief Retrieves a specific property value.
   *
   * @param[in] interface_name The name of the interface.
   * @param[in] path The exact path of the property.
   * @return An expected containing the property on success or Error on failure.
   */
  virtual auto get_property(std::string_view interface_name, std::string_view path)
      -> astarte_tl::expected<PropertyIndividual, Error> = 0;

 protected:
  Device() = default;
};

}  // namespace device
}  // namespace astarte

#endif  // ASTARTE_DEVICE_SDK_DEVICE_H
