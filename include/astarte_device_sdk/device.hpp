// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_DEVICE_H
#define ASTARTE_DEVICE_SDK_DEVICE_H

/**
 * @file astarte_device_sdk/device.hpp
 * @brief Astarte device object and its related methods.
 */

#include <chrono>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"

/** @brief Umbrella namespace for the Astarte device SDK */
namespace AstarteDeviceSdk {

/**
 * @brief Class for the Astarte devices.
 * @details This class should be instantiated once and then used to communicate with Astarte.
 */
class AstarteDevice {
 public:
  /**
   * @brief Constructor for the Astarte device class.
   * @param server_addr The gRPC server address of the Astarte message hub.
   * @param node_uuid The UUID identifier for this device with the Astarte message hub.
   */
  AstarteDevice(const std::string &server_addr, const std::string &node_uuid);
  /** @brief Destructor for the Astarte device class. */
  ~AstarteDevice();
  /** @brief Copy constructor for the Astarte device class. */
  AstarteDevice(AstarteDevice &other) = delete;
  /** @brief Copy assignment operator for the Astarte device class. */
  auto operator=(AstarteDevice &other) -> AstarteDevice & = delete;
  /** @brief Move constructor for the Astarte device class. */
  AstarteDevice(AstarteDevice &&other) = delete;
  /** @brief Move assignment operator for the Astarte device class. */
  auto operator=(AstarteDevice &&other) -> AstarteDevice & = delete;

  /**
   * @brief Add an interface for the device from a json file.
   * @param json_file The path to the .json interface file.
   */
  void add_interface_from_json(const std::filesystem::path &json_file);
  /** @brief Connect to Astarte. */
  void connect();
  /** @brief Disconnect from Astarte. */
  void disconnect();
  /**
   * @brief Send individual data to Astarte.
   * @param interface_name The name of the interface on which to send the data.
   * @param path The path to the interface endpoint to use for sending.
   * @param data The data to send.
   * @param timestamp The timestamp for the data, this might be a nullptr.
   */
  void send_individual(const std::string &interface_name, const std::string &path,
                       const AstarteData &data,
                       const std::chrono::system_clock::time_point *timestamp);
  /**
   * @brief Send object data to Astarte.
   * @param interface_name The name of the interface on which to send the data.
   * @param path The common path to the interface endpoint to use for sending.
   * @param object The data to send.
   * @param timestamp The timestamp for the data, this might be a nullptr.
   */
  void send_object(const std::string &interface_name, const std::string &path,
                   const AstarteObjectDatastream &object,
                   const std::chrono::system_clock::time_point *timestamp);
  /**
   * @brief Set a device property.
   * @param interface_name The name of the interface for the property.
   * @param path The property full path.
   * @param data The property data.
   */
  void set_property(const std::string &interface_name, const std::string &path,
                    const AstarteData &data);
  /**
   * @brief Unset a device property.
   * @param interface_name The name of the interface for the property.
   * @param path The property full path.
   */
  void unset_property(const std::string &interface_name, const std::string &path);
  /**
   * @brief Poll incoming messages.
   * @return The received message when present, std::nullopt otherwise.
   */
  auto poll_incoming() -> std::optional<AstarteMessage>;

 private:
  struct AstarteDeviceImpl;
  std::shared_ptr<AstarteDeviceImpl> astarte_device_impl_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_DEVICE_H
