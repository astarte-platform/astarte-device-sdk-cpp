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
#include <stop_token>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/errors.hpp"
#include "astarte_device_sdk/grpc/device_grpc.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/stored_property.hpp"
#include "shared_queue.hpp"

namespace astarte::device {

using gRPCMessageHub = astarteplatform::msghub::MessageHub;
using gRPCMessageHubEvent = astarteplatform::msghub::MessageHubEvent;

struct AstarteDeviceGrpc::AstarteDeviceGrpcImpl {
 public:
  /**
   * @brief Construct an AstarteDeviceGrpcImpl instance.
   * @param server_addr The gRPC server address for the Astarte message hub.
   * @param node_uuid The unique identifier for the device connection.
   */
  AstarteDeviceGrpcImpl(std::string server_addr, std::string node_uuid);
  /** @brief Destructor for the Astarte device class. */
  ~AstarteDeviceGrpcImpl();
  /** @brief Copy constructor for the Astarte device class. */
  AstarteDeviceGrpcImpl(AstarteDeviceGrpcImpl& other) = delete;
  /** @brief Move constructor for the Astarte device class. */
  AstarteDeviceGrpcImpl(AstarteDeviceGrpcImpl&& other) = delete;
  /** @brief Copy assignment operator for the Astarte device class. */
  auto operator=(AstarteDeviceGrpcImpl& other) -> AstarteDeviceGrpcImpl& = delete;
  /** @brief Move assignment operator for the Astarte device class. */
  auto operator=(AstarteDeviceGrpcImpl&& other) -> AstarteDeviceGrpcImpl& = delete;

  /**
   * @brief Parse an interface definition from a JSON file and adds it to the device.
   * @details The file content is read and stored internally for use during the connection phase.
   * @param json_file The filesystem path to the .json interface file.
   */
  auto add_interface_from_file(const std::filesystem::path& json_file)
      -> astarte_tl::expected<void, Error>;
  /**
   * @brief Parse an interface definition from a JSON string and adds it to the device.
   * @param json The interface to add.
   */
  auto add_interface_from_str(std::string_view json) -> astarte_tl::expected<void, Error>;
  /**
   * @brief Remove an installed interface.
   * @param interface_name The interface name.
   */
  auto remove_interface(const std::string& interface_name) -> astarte_tl::expected<void, Error>;
  /**
   * @brief Connect the device to Astarte.
   * @details This is an asynchronous funciton. It will start a management thread that will
   * manage the device connectivity.
   */
  auto connect() -> astarte_tl::expected<void, Error>;
  /**
   * @brief Check if the device is connected.
   * @return True if the device is connected to the message hub, false otherwise.
   */
  [[nodiscard]] auto is_connected() const -> bool;
  /**
   * @brief Disconnect from the Astarte message hub.
   * @details Gracefully terminates the connection by sending a Detach message.
   */
  auto disconnect() -> astarte_tl::expected<void, Error>;
  /**
   * @brief Send an individual datastream value to an interface.
   * @param interface_name The name of the interface to send data to.
   * @param path The path within the interface (e.g., "/endpoint/value").
   * @param data The data point to send.
   * @param timestamp An optional timestamp for the data point.
   */
  auto send_individual(std::string_view interface_name, std::string_view path, const Data& data,
                       const std::chrono::system_clock::time_point* timestamp)
      -> astarte_tl::expected<void, Error>;
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
      -> astarte_tl::expected<void, Error>;
  /**
   * @brief Set a device property on an interface.
   * @param interface_name The name of the interface where the property is defined.
   * @param path The path of the property to set.
   * @param data The value to set for the property.
   */
  auto set_property(std::string_view interface_name, std::string_view path, const Data& data)
      -> astarte_tl::expected<void, Error>;
  /**
   * @brief Unset a device property on an interface.
   * @details This sends a message to the server to clear the value of a specific property.
   * @param interface_name The name of the interface where the property is defined.
   * @param path The path of the property to unset.
   */
  auto unset_property(std::string_view interface_name, std::string_view path)
      -> astarte_tl::expected<void, Error>;
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
      -> astarte_tl::expected<std::list<AstarteStoredProperty>, Error>;
  /**
   * @brief Get stored propertied matching the interface.
   * @param interface_name The name of the interface for the property.
   * @return A list of stored properties, as returned by the message hub.
   */
  auto get_properties(std::string_view interface_name)
      -> astarte_tl::expected<std::list<AstarteStoredProperty>, Error>;
  /**
   * @brief Get a single stored property matching the interface name and path.
   * @param interface_name The name of the interface for the property.
   * @param path Exact path for the property.
   * @return The stored property, as returned by the message hub.
   */
  auto get_property(std::string_view interface_name, std::string_view path)
      -> astarte_tl::expected<AstartePropertyIndividual, Error>;

 private:
  // Helper struct to hold the results of the Attach RPC call
  struct AttachResult {
    std::unique_ptr<grpc::ClientContext> context;
    std::unique_ptr<grpc::ClientReader<gRPCMessageHubEvent>> reader;
  };
  void setup_grpc_channel();
  auto perform_attach() -> astarte_tl::expected<AttachResult, Error>;
  auto connection_attempt(const std::stop_token& token) -> astarte_tl::expected<void, Error>;
  auto handle_events(const std::stop_token& token, std::unique_ptr<grpc::ClientContext> context,
                     std::unique_ptr<grpc::ClientReader<gRPCMessageHubEvent>> reader)
      -> astarte_tl::expected<void, Error>;
  static auto parse_message_hub_event(const gRPCMessageHubEvent& event)
      -> astarte_tl::expected<AstarteMessage, Error>;
  auto connection_loop(const std::stop_token& token) -> astarte_tl::expected<void, Error>;

  std::string server_addr_;
  std::string node_uuid_;
  std::unique_ptr<gRPCMessageHub::Stub> stub_;
  std::vector<std::string> interfaces_bins_;
  std::optional<std::jthread> connection_thread_;
  std::atomic_bool connected_{false};
  std::stop_source ssource_;
  std::atomic_bool grpc_stream_error_{false};
  SharedQueue<AstarteMessage> rcv_queue_;
};

}  // namespace astarte::device

#endif  // DEVICE_GRPC_IMPL_H
