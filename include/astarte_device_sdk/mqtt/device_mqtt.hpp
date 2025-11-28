// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_DEVICE_MQTT_H
#define ASTARTE_DEVICE_SDK_DEVICE_MQTT_H

/**
 * @file astarte_device_sdk/mqtt/device_mqtt.hpp
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
#include "astarte_device_sdk/mqtt/errors.hpp"
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
class AstarteDeviceMqtt : public AstarteDevice {
 public:
  /**
   * @brief static fallable constructor method.
   * @param cfg set of MQTT configuration options used to connect a device to Astarte.
   * @return An AstarteDeviceMqtt object, an error otherwise.
   */
  [[nodiscard]] static auto create(MqttConfig cfg)
      -> astarte_tl::expected<AstarteDeviceMqtt, AstarteError>;
  /** @brief Destructor for the Astarte device class. */
  ~AstarteDeviceMqtt() override;
  /** @brief Copy constructor for the Astarte device class. */
  AstarteDeviceMqtt(AstarteDeviceMqtt& other) = delete;
  /** @brief Copy assignment operator for the Astarte device class. */
  auto operator=(AstarteDeviceMqtt& other) -> AstarteDeviceMqtt& = delete;
  /**
   * @brief Move constructor for the Astarte device class.
   * @param other object to move.
   */
  AstarteDeviceMqtt(AstarteDeviceMqtt&& other) = default;
  /**
   * @brief Move assignment operator for the Astarte device class.
   * @param other object to move.
   * @return reference to the moved object.
   */
  auto operator=(AstarteDeviceMqtt&& other) -> AstarteDeviceMqtt& = default;

  /**
   * @brief Add an interface for the device from a json file.
   * @param json_file The path to the .json interface file.
   * @return An error if generated.
   */
  auto add_interface_from_file(const std::filesystem::path& json_file)
      -> astarte_tl::expected<void, AstarteError> override;
  /**
   * @brief Add an interface for the device from a json file.
   * @param json The interface to add.
   * @return An error if generated.
   */
  auto add_interface_from_str(std::string_view json)
      -> astarte_tl::expected<void, AstarteError> override;
  /**
   * @brief Remove an installed interface.
   * @param interface_name The interface name.
   * @return An error if generated.
   */
  auto remove_interface(const std::string& interface_name)
      -> astarte_tl::expected<void, AstarteError> override;
  /**
   * @brief Connect the device to Astarte.
   * @details This is an asynchronous funciton. It will start a management thread that will
   * manage the device connectivity.
   * @return An error if generated.
   */
  auto connect() -> astarte_tl::expected<void, AstarteError> override;
  /**
   * @brief Check if the device is connected.
   * @return True if the device is connected to the message hub, false otherwise.
   */
  // NOLINTNEXTLINE(misc-include-cleaner)
  [[nodiscard]] auto is_connected() const -> bool override;
  /**
   * @brief Disconnect from Astarte.
   * @return An error if generated.
   */
  auto disconnect() -> astarte_tl::expected<void, AstarteError> override;
  /**
   * @brief Send individual data to Astarte.
   * @param interface_name The name of the interface on which to send the data.
   * @param path The path to the interface endpoint to use for sending.
   * @param data The data to send.
   * @param timestamp The timestamp for the data, this might be a nullptr.
   * @return An error if generated.
   */
  auto send_individual(std::string_view interface_name, std::string_view path,
                       const AstarteData& data,
                       const std::chrono::system_clock::time_point* timestamp)
      -> astarte_tl::expected<void, AstarteError> override;
  /**
   * @brief Send object data to Astarte.
   * @param interface_name The name of the interface on which to send the data.
   * @param path The common path to the interface endpoint to use for sending.
   * @param object The data to send.
   * @param timestamp The timestamp for the data, this might be a nullptr.
   * @return An error if generated.
   */
  auto send_object(std::string_view interface_name, std::string_view path,
                   const AstarteDatastreamObject& object,
                   const std::chrono::system_clock::time_point* timestamp)
      -> astarte_tl::expected<void, AstarteError> override;
  /**
   * @brief Set a device property.
   * @param interface_name The name of the interface for the property.
   * @param path The property full path.
   * @param data The property data.
   * @return An error if generated.
   */
  auto set_property(std::string_view interface_name, std::string_view path, const AstarteData& data)
      -> astarte_tl::expected<void, AstarteError> override;
  /**
   * @brief Unset a device property.
   * @param interface_name The name of the interface for the property.
   * @param path The property full path.
   * @return An error if generated.
   */
  auto unset_property(std::string_view interface_name, std::string_view path)
      -> astarte_tl::expected<void, AstarteError> override;
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
   * @return A list of stored properties, or an error if the operation failed.
   */
  auto get_all_properties(const std::optional<AstarteOwnership>& ownership)
      -> astarte_tl::expected<std::list<AstarteStoredProperty>, AstarteError> override;
  /**
   * @brief Get stored properties matching the interface.
   * @param interface_name The name of the interface for the properties.
   * @return A list of stored properties, or an error if the operation failed.
   */
  auto get_properties(std::string_view interface_name)
      -> astarte_tl::expected<std::list<AstarteStoredProperty>, AstarteError> override;
  /**
   * @brief Get a single stored property matching the interface name and path.
   * @param interface_name The name of the interface for the property.
   * @param path Exact path for the property.
   * @return The stored property, or an error if the operation failed.
   */
  auto get_property(std::string_view interface_name, std::string_view path)
      -> astarte_tl::expected<AstartePropertyIndividual, AstarteError> override;

 private:
  struct AstarteDeviceMqttImpl;
  std::shared_ptr<AstarteDeviceMqttImpl> astarte_device_impl_;

  /**
   * @brief Construct an AstarteDeviceMqttImpl instance.
   * @param impl a shared pointer to the AstarteDeviceMqttImpl object.
   */
  AstarteDeviceMqtt(std::shared_ptr<AstarteDeviceMqttImpl> impl);
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_DEVICE_MQTT_H
