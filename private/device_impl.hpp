// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DEVICE_IMPL_H
#define DEVICE_IMPL_H

#include <astarteplatform/msghub/astarte_message.pb.h>
#include <astarteplatform/msghub/message_hub_service.grpc.pb.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/support/client_interceptor.h>

#include <atomic>
#include <chrono>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/device.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "shared_queue.hpp"

namespace AstarteDeviceSdk {

using gRPCMessageHub = astarteplatform::msghub::MessageHub;
using gRPCMessageHubEvent = astarteplatform::msghub::MessageHubEvent;

struct AstarteDevice::AstarteDeviceImpl {
 public:
  /**
   * @brief Constructs an AstarteDeviceImpl instance.
   * @param server_addr The gRPC server address for the Astarte message hub.
   * @param node_uuid The unique identifier for the device connection.
   */
  AstarteDeviceImpl(std::string server_addr, std::string node_uuid);
  /** @brief Destructor for the Astarte device class. */
  ~AstarteDeviceImpl();
  /** @brief Copy constructor for the Astarte device class. */
  AstarteDeviceImpl(AstarteDeviceImpl& other) = delete;
  /** @brief Copy assignment operator for the Astarte device class. */
  auto operator=(AstarteDeviceImpl& other) -> AstarteDeviceImpl& = delete;
  /** @brief Move constructor for the Astarte device class. */
  AstarteDeviceImpl(AstarteDeviceImpl&& other) = delete;
  /** @brief Move assignment operator for the Astarte device class. */
  auto operator=(AstarteDeviceImpl&& other) -> AstarteDeviceImpl& = delete;

  /**
   * @brief Parses an interface definition from a JSON file and adds it to the device.
   * @details The file content is read and stored internally for use during the connection phase.
   * @param json_file The filesystem path to the .json interface file.
   */
  void add_interface_from_json(const std::filesystem::path& json_file);
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
  auto is_connected(std::chrono::milliseconds timeout) -> bool;
  /**
   * @brief Disconnects from the Astarte message hub.
   * @details Gracefully terminates the connection by sending a Detach message.
   */
  void disconnect();
  /**
   * @brief Sends an individual datastream value to an interface.
   * @param interface_name The name of the interface to send data to.
   * @param path The path within the interface (e.g., "/endpoint/value").
   * @param data The data point to send.
   * @param timestamp An optional timestamp for the data point.
   */
  void send_individual(const std::string& interface_name, const std::string& path,
                       const AstarteData& data,
                       const std::chrono::system_clock::time_point* timestamp);
  /**
   * @brief Sends a datastream object to an interface.
   * @param interface_name The name of the interface to send data to.
   * @param path The base path for the object within the interface.
   * @param object The key-value map representing the object to send.
   * @param timestamp An optional timestamp for the data.
   */
  void send_object(const std::string& interface_name, const std::string& path,
                   const AstarteDatastreamObject& object,
                   const std::chrono::system_clock::time_point* timestamp);
  /**
   * @brief Sets a device property on an interface.
   * @param interface_name The name of the interface where the property is defined.
   * @param path The path of the property to set.
   * @param data The value to set for the property.
   */
  void set_property(const std::string& interface_name, const std::string& path,
                    const AstarteData& data);
  /**
   * @brief Unsets a device property on an interface.
   * @details This sends a message to the server to clear the value of a specific property.
   * @param interface_name The name of the interface where the property is defined.
   * @param path The path of the property to unset.
   */
  void unset_property(const std::string& interface_name, const std::string& path);
  /**
   * @brief Polls for a new message received from the message hub.
   * @details This method checks an internal queue for parsed messages from the server. It is
   * non-blocking.
   * @return An std::optional containing an AstarteMessage if one was available, otherwise
   * std::nullopt.
   */
  auto poll_incoming() -> std::optional<AstarteMessage>;

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
  std::shared_ptr<std::atomic_bool> connection_stop_flag_;
  std::thread event_handler_;
  SharedQueue<AstarteMessage> rcv_queue_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_DEVICE_H
