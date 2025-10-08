// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_DEVICE_MQTT_H
#define ASTARTE_DEVICE_SDK_DEVICE_MQTT_H

/**
 * @file astarte_device_sdk/device_mqtt.hpp
 * @brief Astarte device object and its related methods for the MQTT transport layer.
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
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/stored_property.hpp"

/** @brief Umbrella namespace for the Astarte device SDK */
namespace AstarteDeviceSdk {

/**
 * @brief Class for the Astarte devices.
 * @details This class should be instantiated once and then used to communicate with Astarte.
 */
class AstarteDeviceMQTT : public AstarteDevice {
 public:
  /**
   * @brief Construct an AstarteDeviceMQTTImpl instance.
   * @param cfg set of MQTT configuration options used to connect a device to Astarte.
   */
  AstarteDeviceMQTT(const MqttConfig cfg);
  /** @brief Destructor for the Astarte device class. */
  ~AstarteDeviceMQTT() override;
  /** @brief Copy constructor for the Astarte device class. */
  AstarteDeviceMQTT(AstarteDeviceMQTT& other) = delete;
  /** @brief Copy assignment operator for the Astarte device class. */
  auto operator=(AstarteDeviceMQTT& other) -> AstarteDeviceMQTT& = delete;
  /** @brief Move constructor for the Astarte device class. */
  AstarteDeviceMQTT(AstarteDeviceMQTT&& other) = delete;
  /** @brief Move assignment operator for the Astarte device class. */
  auto operator=(AstarteDeviceMQTT&& other) -> AstarteDeviceMQTT& = delete;

  /**
   * @brief Add an interface for the device from a json file.
   * @param json_file The path to the .json interface file.
   */
  void add_interface_from_file(const std::filesystem::path& json_file) override;
  /**
   * @brief Add an interface for the device from a json file.
   * @param json The interface to add.
   */
  void add_interface_from_str(std::string_view json) override;
  /**
   * @brief Remove an installed interface.
   * @param interface_name The interface name.
   */
  void remove_interface(const std::string& interface_name) override;
  /**
   * @brief Connect the device to Astarte.
   * @details This is an asynchronous funciton. It will start a management thread that will
   * manage the device connectivity.
   */
  void connect() override;
  /**
   * @brief Check if the device is connected.
   * @return True if the device is connected to the message hub, false otherwise.
   */
  // NOLINTNEXTLINE(misc-include-cleaner)
  [[nodiscard]] auto is_connected() const -> bool override;
  /** @brief Disconnect from Astarte. */
  void disconnect() override;
  /**
   * @brief Send individual data to Astarte.
   * @param interface_name The name of the interface on which to send the data.
   * @param path The path to the interface endpoint to use for sending.
   * @param data The data to send.
   * @param timestamp The timestamp for the data, this might be a nullptr.
   */
  void send_individual(std::string_view interface_name, std::string_view path,
                       const AstarteData& data,
                       const std::chrono::system_clock::time_point* timestamp) override;
  /**
   * @brief Send object data to Astarte.
   * @param interface_name The name of the interface on which to send the data.
   * @param path The common path to the interface endpoint to use for sending.
   * @param object The data to send.
   * @param timestamp The timestamp for the data, this might be a nullptr.
   */
  void send_object(std::string_view interface_name, std::string_view path,
                   const AstarteDatastreamObject& object,
                   const std::chrono::system_clock::time_point* timestamp) override;
  /**
   * @brief Set a device property.
   * @param interface_name The name of the interface for the property.
   * @param path The property full path.
   * @param data The property data.
   */
  void set_property(std::string_view interface_name, std::string_view path,
                    const AstarteData& data) override;
  /**
   * @brief Unset a device property.
   * @param interface_name The name of the interface for the property.
   * @param path The property full path.
   */
  void unset_property(std::string_view interface_name, std::string_view path) override;
  /**
   * @brief Poll incoming messages.
   * @param timeout Will block for this timeout if no message is present.
   * @return The received message when present, std::nullopt otherwise.
   */
  auto poll_incoming(const std::chrono::milliseconds& timeout)
      -> std::optional<AstarteMessage> override;
  /**
   * @brief Get all stored properties matching the input filter.
   * @param ownership Optional ownership filter.
   * @return A list of stored properties, as returned by the message hub.
   */
  auto get_all_properties(const std::optional<AstarteOwnership>& ownership)
      -> std::list<AstarteStoredProperty>;
  /**
   * @brief Get stored properties matching the interface.
   * @param interface_name The name of the interface for the properties.
   * @return A list of stored properties, as returned by the message hub.
   */
  auto get_properties(std::string_view interface_name) -> std::list<AstarteStoredProperty>;
  /**
   * @brief Get a single stored property matching the interface name and path.
   * @param interface_name The name of the interface for the property.
   * @param path Exact path for the property.
   * @return The stored property, as returned by the message hub.
   */
  auto get_property(std::string_view interface_name, std::string_view path)
      -> AstartePropertyIndividual;

 private:
  struct AstarteDeviceMQTTImpl;
  std::shared_ptr<AstarteDeviceMQTTImpl> astarte_device_impl_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_DEVICE_MQTT_H
