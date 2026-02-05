// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_DEVICE_MQTT_H
#define ASTARTE_DEVICE_SDK_DEVICE_MQTT_H

/**
 * @file astarte_device_sdk/mqtt/device_mqtt.hpp
 * @brief Astarte device implementation using MQTT.
 *
 * @details This file provides the concrete implementation of the Astarte Device interface
 * utilizing the MQTT protocol for transport.
 */

#include <chrono>
#include <filesystem>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/device.hpp"
#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/errors.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/stored_property.hpp"

/// @brief Namespace for Astarte device functionality using the MQTT transport protocol.
namespace astarte::device::mqtt {

/**
 * @brief MQTT implementation of an Astarte device.
 *
 * @details This class implements the abstract Device interface using MQTT as the underlying
 * transport protocol. It handles connection management, data serialization, and
 * protocol-specific communication with the Astarte platform.
 */
class DeviceMqtt : public Device {
 public:
  /**
   * @brief Creates a new instance of the MQTT device.
   *
   * @param[in] cfg The configuration options used to connect the device to Astarte.
   * @return An expected containing the DeviceMqtt instance on success or Error on failure.
   */
  [[nodiscard]] static auto create(Config cfg) -> astarte_tl::expected<DeviceMqtt, Error>;

  /// @brief Virtual destructor.
  ~DeviceMqtt() override;

  /// @brief Device is non-copyable.
  DeviceMqtt(DeviceMqtt& other) = delete;

  /// @brief Device is non-copyable.
  auto operator=(DeviceMqtt& other) -> DeviceMqtt& = delete;

  /**
   * @brief Move constructor.
   * @param[in,out] other The device instance to move data from.
   */
  DeviceMqtt(DeviceMqtt&& other) = default;

  /**
   * @brief Move assignment operator.
   * @param[in,out] other The device instance to move data from.
   * @return A reference to this device.
   */
  auto operator=(DeviceMqtt&& other) -> DeviceMqtt& = default;

  /**
   * @brief Adds an interface definition to the device from a JSON file.
   * @details Parses the JSON file to validate and register a new Astarte Interface.
   *
   * @param[in] json_file The filesystem path to the .json interface definition.
   * @return An expected containing void on success or Error on failure.
   */
  auto add_interface_from_file(const std::filesystem::path& json_file)
      -> astarte_tl::expected<void, Error> override;

  /**
   * @brief Adds an interface definition to the device from a JSON string.
   *
   * @param[in] json The interface definition as a JSON string view.
   * @return An expected containing void on success or Error on failure.
   */
  auto add_interface_from_str(std::string_view json) -> astarte_tl::expected<void, Error> override;

  /**
   * @brief Removes an installed interface from the device.
   *
   * @param[in] interface_name The name of the interface to remove.
   * @return An expected containing void on success or Error on failure.
   */
  auto remove_interface(const std::string& interface_name)
      -> astarte_tl::expected<void, Error> override;

  /**
   * @brief Connects the device to the Astarte platform.
   *
   * @details This is an asynchronous operation. It initializes the MQTT transport layer
   * and starts a background routine to maintain connectivity.
   *
   * @return An expected containing void on success or Error on failure.
   */
  auto connect() -> astarte_tl::expected<void, Error> override;

  /**
   * @brief Checks connectivity status.
   * @return True if the device is fully connected to Astarte, false otherwise.
   */
  [[nodiscard]] auto is_connected() const -> bool override;

  /**
   * @brief Disconnects the device from Astarte.
   * @return An expected containing void on success or Error on failure.
   */
  auto disconnect() -> astarte_tl::expected<void, Error> override;

  /**
   * @brief Sends an individual data point to an Astarte Interface.
   *
   * @param[in] interface_name The name of the target interface.
   * @param[in] path The specific mapping path within the interface (e.g., "/sensors/temp").
   * @param[in] data The value payload to transmit.
   * @param[in] timestamp Optional timestamp. Should match the interface timestamp configuration.
   * @return An expected containing void on success or Error on failure.
   */
  auto send_individual(std::string_view interface_name, std::string_view path, const Data& data,
                       const std::chrono::system_clock::time_point* timestamp)
      -> astarte_tl::expected<void, Error> override;

  /**
   * @brief Sends an aggregated object to an Astarte Interface.
   *
   * @param[in] interface_name The name of the target interface.
   * @param[in] path The common base path for the object aggregation.
   * @param[in] object The map of keys and values constituting the object.
   * @param[in] timestamp Optional timestamp. Should match the interface timestamp configuration.
   * @return An expected containing void on success or Error on failure.
   */
  auto send_object(std::string_view interface_name, std::string_view path,
                   const DatastreamObject& object,
                   const std::chrono::system_clock::time_point* timestamp)
      -> astarte_tl::expected<void, Error> override;

  /**
   * @brief Updates a local device property and synchronize it with Astarte.
   *
   * @param[in] interface_name The name of the interface containing the property.
   * @param[in] path The specific path of the property.
   * @param[in] data The new value for the property.
   * @return An expected containing void on success or Error on failure.
   */
  auto set_property(std::string_view interface_name, std::string_view path, const Data& data)
      -> astarte_tl::expected<void, Error> override;

  /**
   * @brief Unsets (deletes) a device property.
   *
   * @param[in] interface_name The name of the interface containing the property.
   * @param[in] path The specific path of the property to unset.
   * @return An expected containing void on success or Error on failure.
   */
  auto unset_property(std::string_view interface_name, std::string_view path)
      -> astarte_tl::expected<void, Error> override;

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
  auto poll_incoming(const std::chrono::milliseconds& timeout) -> std::optional<Message> override;

  /**
   * @brief Retrieves all stored properties matching an ownership filter.
   *
   * @param[in] ownership Optional filter, if std::nullopt, returns all properties.
   * @return An expected containing the list of properties on success or Error on failure.
   */
  auto get_all_properties(const std::optional<Ownership>& ownership)
      -> astarte_tl::expected<std::list<StoredProperty>, Error> override;

  /**
   * @brief Retrieves all stored properties belonging to a specific interface.
   *
   * @param[in] interface_name The name of the interface to query.
   * @return An expected containing the list of properties on success or Error on failure.
   */
  auto get_properties(std::string_view interface_name)
      -> astarte_tl::expected<std::list<StoredProperty>, Error> override;

  /**
   * @brief Retrieves a specific property value.
   *
   * @param[in] interface_name The name of the interface.
   * @param[in] path The exact path of the property.
   * @return An expected containing the property on success or Error on failure.
   */
  auto get_property(std::string_view interface_name, std::string_view path)
      -> astarte_tl::expected<PropertyIndividual, Error> override;

 private:
  struct DeviceMqttImpl;
  std::shared_ptr<DeviceMqttImpl> astarte_device_impl_;

  /**
   * @brief Constructs a DeviceMqtt instance from an implementation pointer.
   * @param[in] impl A shared pointer to the DeviceMqttImpl object.
   */
  explicit DeviceMqtt(std::shared_ptr<DeviceMqttImpl> impl);
};

}  // namespace astarte::device::mqtt

#endif  // ASTARTE_DEVICE_SDK_DEVICE_MQTT_H
