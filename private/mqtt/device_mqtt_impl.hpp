// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DEVICE_MQTT_IMPL_H
#define DEVICE_MQTT_IMPL_H

#include <atomic>
#include <chrono>
#include <filesystem>
#include <list>
#include <optional>
#include <string>
#include <string_view>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/device_mqtt.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/stored_property.hpp"

namespace AstarteDeviceSdk {

struct AstarteDeviceMQTT::AstarteDeviceMQTTImpl {
 public:
  /**
   * @brief Construct an AstarteDeviceMQTTImpl instance.
   * @param cfg set of MQTT configuration options used to connect a device to Astarte.
   */
  AstarteDeviceMQTTImpl(const MqttConfig cfg);
  /** @brief Destructor for the Astarte device class. */
  ~AstarteDeviceMQTTImpl();
  /** @brief Copy constructor for the Astarte device class. */
  AstarteDeviceMQTTImpl(AstarteDeviceMQTTImpl& other) = delete;
  /** @brief Move constructor for the Astarte device class. */
  AstarteDeviceMQTTImpl(AstarteDeviceMQTTImpl&& other) = delete;
  /** @brief Copy assignment operator for the Astarte device class. */
  auto operator=(AstarteDeviceMQTTImpl& other) -> AstarteDeviceMQTTImpl& = delete;
  /** @brief Move assignment operator for the Astarte device class. */
  auto operator=(AstarteDeviceMQTTImpl&& other) -> AstarteDeviceMQTTImpl& = delete;

  /**
   * @brief Parse an interface definition from a JSON file and adds it to the device.
   * @details The file content is read and stored internally for use during the connection phase.
   * @param json_file The filesystem path to the .json interface file.
   */
  auto add_interface_from_file(const std::filesystem::path& json_file)
      -> astarte_tl::expected<void, AstarteError>;
  /**
   * @brief Parse an interface definition from a JSON string and adds it to the device.
   * @param json The interface to add.
   */
  auto add_interface_from_str(std::string_view json) -> astarte_tl::expected<void, AstarteError>;
  /**
   * @brief Remove an installed interface.
   * @param interface_name The interface name.
   */
  auto remove_interface(const std::string& interface_name)
      -> astarte_tl::expected<void, AstarteError>;
  /**
   * @brief Connect the device to Astarte.
   * @details This is an asynchronous funciton. It will start a management thread that will
   * manage the device connectivity.
   */
  auto connect() -> astarte_tl::expected<void, AstarteError>;
  /**
   * @brief Check if the device is connected.
   * @return True if the device is connected to Astarte, false otherwise.
   */
  [[nodiscard]] auto is_connected() const -> bool;
  /**
   * @brief Disconnect from Astarte.
   * @details Gracefully terminates the connection by sending a Detach message.
   */
  auto disconnect() -> astarte_tl::expected<void, AstarteError>;
  /**
   * @brief Send an individual datastream value to an interface.
   * @param interface_name The name of the interface to send data to.
   * @param path The path within the interface (e.g., "/endpoint/value").
   * @param data The data point to send.
   * @param timestamp An optional timestamp for the data point.
   */
  auto send_individual(std::string_view interface_name, std::string_view path,
                       const AstarteData& data,
                       const std::chrono::system_clock::time_point* timestamp)
      -> astarte_tl::expected<void, AstarteError>;
  /**
   * @brief Send a datastream object to an interface.
   * @param interface_name The name of the interface to send data to.
   * @param path The base path for the object within the interface.
   * @param object The key-value map representing the object to send.
   * @param timestamp An optional timestamp for the data.
   */
  auto send_object(std::string_view interface_name, std::string_view path,
                   const AstarteDatastreamObject& object,
                   const std::chrono::system_clock::time_point* timestamp)
      -> astarte_tl::expected<void, AstarteError>;
  /**
   * @brief Set a device property on an interface.
   * @param interface_name The name of the interface where the property is defined.
   * @param path The path of the property to set.
   * @param data The value to set for the property.
   */
  auto set_property(std::string_view interface_name, std::string_view path, const AstarteData& data)
      -> astarte_tl::expected<void, AstarteError>;
  /**
   * @brief Unset a device property on an interface.
   * @details This sends a message to the server to clear the value of a specific property.
   * @param interface_name The name of the interface where the property is defined.
   * @param path The path of the property to unset.
   */
  auto unset_property(std::string_view interface_name, std::string_view path)
      -> astarte_tl::expected<void, AstarteError>;
  /**
   * @brief Poll for a new message received from Astarte.
   * @details This method checks an internal queue for parsed messages from the server.
   * @param timeout Will block for this timeout if no message is present.
   * @return An std::optional containing an AstarteMessage if one was available, otherwise
   * std::nullopt.
   */
  auto poll_incoming(const std::chrono::milliseconds& timeout) -> std::optional<AstarteMessage>;
  /**
   * @brief Get all stored properties matching the input filter.
   * @param ownership Optional ownership filter.
   * @return A list of stored properties, as returned by Astarte.
   */
  auto get_all_properties(const std::optional<AstarteOwnership>& ownership)
      -> astarte_tl::expected<std::list<AstarteStoredProperty>, AstarteError>;
  /**
   * @brief Get stored propertied matching the interface.
   * @param interface_name The name of the interface for the property.
   * @return A list of stored properties, as returned by Astarte.
   */
  auto get_properties(std::string_view interface_name)
      -> astarte_tl::expected<std::list<AstarteStoredProperty>, AstarteError>;
  /**
   * @brief Get a single stored property matching the interface name and path.
   * @param interface_name The name of the interface for the property.
   * @param path Exact path for the property.
   * @return The stored property, as returned by Astarte.
   */
  auto get_property(std::string_view interface_name, std::string_view path)
      -> astarte_tl::expected<AstartePropertyIndividual, AstarteError>;

 private:
  MqttConfig cfg_;
  std::atomic_bool connected_{false};
};

}  // namespace AstarteDeviceSdk

#endif  // DEVICE_MQTT_IMPL_H
