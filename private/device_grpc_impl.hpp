// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DEVICE_GRPC_IMPL_H
#define DEVICE_GRPC_IMPL_H

#include <astarteplatform/msghub/astarte_message.pb.h>
#include <astarteplatform/msghub/message_hub_service.grpc.pb.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/support/client_interceptor.h>

#include <atomic>
#include <chrono>
#include <filesystem>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/device_grpc.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/stored_property.hpp"
#include "shared_queue.hpp"

namespace AstarteDeviceSdk {

using gRPCMessageHub = astarteplatform::msghub::MessageHub;
using gRPCMessageHubEvent = astarteplatform::msghub::MessageHubEvent;

struct AstarteDeviceGRPC::AstarteDeviceGRPCImpl {
 public:
  /**
   * @brief Construct an AstarteDeviceGRPCImpl instance.
   * @param server_addr The gRPC server address for the Astarte message hub.
   * @param node_uuid The unique identifier for the device connection.
   */
  AstarteDeviceGRPCImpl(std::string server_addr, std::string node_uuid);
  /** @brief Destructor for the Astarte device class. */
  ~AstarteDeviceGRPCImpl();
  /** @brief Copy constructor for the Astarte device class. */
  AstarteDeviceGRPCImpl(AstarteDeviceGRPCImpl& other) = delete;
  /** @brief Copy assignment operator for the Astarte device class. */
  auto operator=(AstarteDeviceGRPCImpl& other) -> AstarteDeviceGRPCImpl& = delete;
  /** @brief Move constructor for the Astarte device class. */
  AstarteDeviceGRPCImpl(AstarteDeviceGRPCImpl&& other) = delete;
  /** @brief Move assignment operator for the Astarte device class. */
  auto operator=(AstarteDeviceGRPCImpl&& other) -> AstarteDeviceGRPCImpl& = delete;

  /**
   * @brief Parse an interface definition from a JSON file and adds it to the device.
   * @details The file content is read and stored internally for use during the connection phase.
   * @param json_file The filesystem path to the .json interface file.
   */
  void add_interface_from_json(const std::filesystem::path& json_file);
  /**
   * @brief Parse an interface definition from a JSON string and adds it to the device.
   * @param json The interface to add.
   */
  void add_interface_from_str(std::string_view json);
  /**
   * @brief Connect the device to Astarte.
   * @details This is an asynchronous funciton. It will start a management thread that will
   * manage the device connectivity.
   */
  void connect();
  /**
   * @brief Check if the device is connected.
   * @param timeout This is the maximum timeout used to check if the device is connected.
   * @return True if the device is connected to the message hub, false otherwise.
   */
  [[nodiscard]] auto is_connected(const std::chrono::milliseconds& timeout) const -> bool;
  /**
   * @brief Disconnect from the Astarte message hub.
   * @details Gracefully terminates the connection by sending a Detach message.
   */
  void disconnect();
  /**
   * @brief Send an individual datastream value to an interface.
   * @param interface_name The name of the interface to send data to.
   * @param path The path within the interface (e.g., "/endpoint/value").
   * @param data The data point to send.
   * @param timestamp An optional timestamp for the data point.
   */
  void send_individual(std::string_view interface_name, std::string_view path,
                       const AstarteData& data,
                       const std::chrono::system_clock::time_point* timestamp);
  /**
   * @brief Send a datastream object to an interface.
   * @param interface_name The name of the interface to send data to.
   * @param path The base path for the object within the interface.
   * @param object The key-value map representing the object to send.
   * @param timestamp An optional timestamp for the data.
   */
  void send_object(std::string_view interface_name, std::string_view path,
                   const AstarteDatastreamObject& object,
                   const std::chrono::system_clock::time_point* timestamp);
  /**
   * @brief Set a device property on an interface.
   * @param interface_name The name of the interface where the property is defined.
   * @param path The path of the property to set.
   * @param data The value to set for the property.
   */
  void set_property(std::string_view interface_name, std::string_view path,
                    const AstarteData& data);
  /**
   * @brief Unset a device property on an interface.
   * @details This sends a message to the server to clear the value of a specific property.
   * @param interface_name The name of the interface where the property is defined.
   * @param path The path of the property to unset.
   */
  void unset_property(std::string_view interface_name, std::string_view path);
  /**
   * @brief Poll for a new message received from the message hub.
   * @details This method checks an internal queue for parsed messages from the server.
   * @param timeout Will block for this timeout if no message is present.
   * @return An std::optional containing an AstarteMessage if one was available, otherwise
   * std::nullopt.
   */
  auto poll_incoming(const std::chrono::milliseconds& timeout) -> std::optional<AstarteMessage>;
  /**
   * @brief Get all stored properties matching the input filter.
   * @param ownership Optional ownership filter.
   * @return A list of stored properties, as returned by the message hub.
   */
  auto get_all_properties(const std::optional<AstarteOwnership>& ownership)
      -> std::list<AstarteStoredProperty>;
  /**
   * @brief Get stored propertied matching the interface.
   * @param interface_name The name of the interface for the property.
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
  void connection_attempt();
  void handle_events(std::unique_ptr<grpc::ClientContext> context,
                     std::unique_ptr<grpc::ClientReader<gRPCMessageHubEvent>> reader);
  static auto parse_message_hub_event(const gRPCMessageHubEvent& event)
      -> std::optional<AstarteMessage>;
  void connection_loop();

  std::string server_addr_;
  std::string node_uuid_;
  std::unique_ptr<gRPCMessageHub::Stub> stub_;
  std::vector<std::string> interfaces_bins_;
  std::thread connection_thread_;
  std::atomic_bool connection_stop_flag_;
  std::atomic_bool grpc_stream_error_;
  std::thread event_handler_;
  SharedQueue<AstarteMessage> rcv_queue_;
};

}  // namespace AstarteDeviceSdk

#endif  // DEVICE_GRPC_IMPL_H
