// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DEVICE_MQTT_IMPL_H
#define DEVICE_MQTT_IMPL_H

/**
 * @file private/mqtt/device_mqtt_impl.hpp
 * @brief Private implementation of the DeviceMqtt class.
 *
 * @details This file contains the declaration of the DeviceMqttImpl class, which handles
 * the direct interaction with the Astarte MQTT broker, managing the connection state,
 * message loop, and data transmission.
 */

#include <atomic>
#include <chrono>
#include <filesystem>
#include <list>
#include <map>
#include <optional>
#include <string>
#include <string_view>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/device_mqtt.hpp"
#include "astarte_device_sdk/mqtt/errors.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/stored_property.hpp"
#include "mqtt/connection/connection.hpp"
#include "mqtt/introspection.hpp"

namespace astarte::device::mqtt {

/**
 * @brief Implementation class for the MQTT-based Astarte Device.
 *
 * @details Implements the logic declared in DeviceMqtt using the PIMPL idiom.
 * It manages the lifecycle of the MQTT connection, handles the introspection
 * payload, and processes incoming/outgoing messages.
 */
struct DeviceMqtt::DeviceMqttImpl {
 public:
  /**
   * @brief Constructs and initializes a DeviceMqttImpl instance.
   *
   * @details Factory method to create a shared pointer to the implementation.
   *
   * @param[in] cfg Set of MQTT configuration options used to connect a device to Astarte.
   * @return A shared pointer to the DeviceMqttImpl object on success, or an Error on failure.
   */
  static auto create(Config& cfg) -> astarte_tl::expected<std::shared_ptr<DeviceMqttImpl>, Error>;

  /// @brief Destructor for the implementation class. */
  ~DeviceMqttImpl();

  /// @brief DeviceMqttImpl is non-copyable.
  DeviceMqttImpl(DeviceMqttImpl& other) = delete;

  /// @brief DeviceMqttImpl is non-moveable.
  DeviceMqttImpl(DeviceMqttImpl&& other) = delete;

  /// @brief DeviceMqttImpl is non-copyable.
  auto operator=(DeviceMqttImpl& other) -> DeviceMqttImpl& = delete;

  /// @brief DeviceMqttImpl is non-moveable.
  auto operator=(DeviceMqttImpl&& other) -> DeviceMqttImpl& = delete;

  /**
   * @brief Parses an interface definition from a JSON file and adds it to the device.
   * @details The file content is read and stored internally. It will be included in the
   * introspection sent to Astarte upon connection.
   *
   * @param[in] json_file The filesystem path to the .json interface file.
   * @return An expected conforming to std::expected containing void on success or Error on failure.
   */
  auto add_interface_from_file(const std::filesystem::path& json_file)
      -> astarte_tl::expected<void, Error>;

  /**
   * @brief Parses an interface definition from a JSON string and adds it to the device.
   *
   * @param[in] interface_str The interface definition as a JSON string view.
   * @return An expected conforming to std::expected containing void on success or Error on failure.
   */
  auto add_interface_from_str(std::string_view interface_str) -> astarte_tl::expected<void, Error>;

  /**
   * @brief Removes an installed interface.
   *
   * @param[in] interface_name The name of the interface to remove.
   * @return An expected conforming to std::expected containing void on success or Error on failure.
   */
  auto remove_interface(const std::string& interface_name) -> astarte_tl::expected<void, Error>;

  /**
   * @brief Connects the device to Astarte.
   * @details This is an asynchronous function. It starts a background thread that manages
   * the MQTT connection lifecycle, including automatic reconnections.
   *
   * @return An expected conforming to std::expected containing void on success or Error on failure.
   */
  auto connect() -> astarte_tl::expected<void, Error>;

  /**
   * @brief Checks if the device is connected.
   * @return True if the device is currently connected to the Astarte broker, false otherwise.
   */
  [[nodiscard]] auto is_connected() const -> bool;

  /**
   * @brief Disconnects from Astarte.
   * @details Gracefully terminates the connection. If the session is not persistent,
   * this may clear the session state on the broker.
   *
   * @return An expected conforming to std::expected containing void on success or Error on failure.
   */
  auto disconnect() -> astarte_tl::expected<void, Error>;

  /**
   * @brief Sends an individual datastream value to an interface.
   *
   * @param[in] interface_name The name of the interface to send data to.
   * @param[in] path The path within the interface (e.g., "/endpoint/value").
   * @param[in] data The data point to send.
   * @param[in] timestamp An optional timestamp for the data point.
   * @return An expected conforming to std::expected containing void on success or Error on failure.
   */
  auto send_individual(std::string_view interface_name, std::string_view path, const Data& data,
                       const std::chrono::system_clock::time_point* timestamp)
      -> astarte_tl::expected<void, Error>;

  /**
   * @brief Sends a datastream object to an interface.
   *
   * @param[in] interface_name The name of the interface to send data to.
   * @param[in] path The base path for the object within the interface.
   * @param[in] object The key-value map representing the object to send.
   * @param[in] timestamp An optional timestamp for the data.
   * @return An expected conforming to std::expected containing void on success or Error on failure.
   */
  auto send_object(std::string_view interface_name, std::string_view path,
                   const DatastreamObject& object,
                   const std::chrono::system_clock::time_point* timestamp)
      -> astarte_tl::expected<void, Error>;

  /**
   * @brief Sets a device property on an interface.
   *
   * @param[in] interface_name The name of the interface where the property is defined.
   * @param[in] path The path of the property to set.
   * @param[in] data The value to set for the property.
   * @return An expected conforming to std::expected containing void on success or Error on failure.
   */
  auto set_property(std::string_view interface_name, std::string_view path, const Data& data)
      -> astarte_tl::expected<void, Error>;

  /**
   * @brief Unsets a device property on an interface.
   * @details Sends an empty payload to the specific topic to clear the value of the property
   * on the server side.
   *
   * @param[in] interface_name The name of the interface where the property is defined.
   * @param[in] path The path of the property to unset.
   * @return An expected conforming to std::expected containing void on success or Error on failure.
   */
  auto unset_property(std::string_view interface_name, std::string_view path)
      -> astarte_tl::expected<void, Error>;

  /**
   * @brief Polls for a new message received from Astarte.
   * @details Checks an internal queue for parsed messages received from the broker.
   * Blocks execution until a message arrives or the timeout occurs.
   *
   * @param[in] timeout The maximum duration to block waiting for a message.
   * @return An std::optional containing a Message if one was available, otherwise std::nullopt.
   */
  auto poll_incoming(const std::chrono::milliseconds& timeout) -> std::optional<Message>;

  /**
   * @brief Gets all stored properties matching the input filter.
   *
   * @param[in] ownership Optional ownership filter.
   * @return An expected containing the list of properties on success, or an Error on failure.
   */
  auto get_all_properties(const std::optional<Ownership>& ownership)
      -> astarte_tl::expected<std::list<StoredProperty>, Error>;

  /**
   * @brief Gets stored properties matching the interface.
   *
   * @param[in] interface_name The name of the interface for the property.
   * @return An expected containing the list of properties on success, or an Error on failure.
   */
  auto get_properties(std::string_view interface_name)
      -> astarte_tl::expected<std::list<StoredProperty>, Error>;

  /**
   * @brief Gets a single stored property matching the interface name and path.
   *
   * @param[in] interface_name The name of the interface for the property.
   * @param[in] path Exact path for the property.
   * @return An expected containing the property on success, or an Error on failure.
   */
  auto get_property(std::string_view interface_name, std::string_view path)
      -> astarte_tl::expected<PropertyIndividual, Error>;

 private:
  /**
   * @brief Private constructor for a DeviceMqttImpl instance.
   * @param[in] cfg Set of MQTT configuration options used to connect a device to Astarte.
   * @param[in] connection The connection object managing the Paho MQTT client.
   */
  DeviceMqttImpl(Config cfg, connection::Connection connection);

  Config cfg_;
  connection::Connection connection_;
  std::shared_ptr<Introspection> introspection_ = std::make_shared<Introspection>();
};

}  // namespace astarte::device::mqtt

#endif  // DEVICE_MQTT_IMPL_H
