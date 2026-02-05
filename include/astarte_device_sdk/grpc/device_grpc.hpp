// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_DEVICE_GRPC_H
#define ASTARTE_DEVICE_SDK_DEVICE_GRPC_H

/**
 * @file astarte_device_sdk/grpc/device_grpc.hpp
 * @brief Astarte device object and its related methods for the gRPC transport layer.
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
#include "astarte_device_sdk/errors.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/property.hpp"

/// @brief Namespace for Astarte device functionality using the gRPC transport layer.
namespace astarte::device::grpc {

/**
 * @brief Class for the Astarte devices.
 * @details This class should be instantiated once and then used to communicate with Astarte
 * via the gRPC transport layer.
 */
class DeviceGrpc : public Device {
 public:
  /**
   * @brief Constructor for the Astarte device class.
   * @param[in] server_addr The gRPC server address of the Astarte message hub.
   * @param[in] node_uuid The UUID identifier for this device with the Astarte message hub.
   */
  DeviceGrpc(const std::string& server_addr, const std::string& node_uuid);

  /// @brief Virtual destructor.
  ~DeviceGrpc() override;

  /// @brief Device is non-copyable.
  DeviceGrpc(DeviceGrpc& other) = delete;

  /// @brief Device is non-moveable.
  DeviceGrpc(DeviceGrpc&& other) = delete;

  /// @brief Device is non-copyable.
  auto operator=(DeviceGrpc& other) -> DeviceGrpc& = delete;

  /// @brief Device is non-moveable.
  auto operator=(DeviceGrpc&& other) -> DeviceGrpc& = delete;

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
   * @details This is an asynchronous operation. It initializes the transport layer
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
  struct DeviceGrpcImpl;
  std::shared_ptr<DeviceGrpcImpl> astarte_device_impl_;
};

}  // namespace astarte::device::grpc

#endif  // ASTARTE_DEVICE_SDK_DEVICE_GRPC_H
