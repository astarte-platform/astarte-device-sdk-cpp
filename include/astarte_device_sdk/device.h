// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_DEVICE_H
#define ASTARTE_DEVICE_SDK_DEVICE_H

/**
 * @file astarte_device_sdk/device.h
 * @brief Astarte device object and its related methods.
 */

#include <astarteplatform/msghub/astarte_message.pb.h>
#include <astarteplatform/msghub/message_hub_service.grpc.pb.h>
#include <grpcpp/grpcpp.h>

#include <chrono>
#include <filesystem>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "astarte_device_sdk/individual.h"

/** @brief Umbrella namespace for the Astarte device SDK */
namespace AstarteDeviceSdk {

using grpc::ClientContext;
using grpc::ClientReader;

using astarteplatform::msghub::MessageHub;
using astarteplatform::msghub::MessageHubEvent;

/**
 * @brief Class for the Astarte device objects.
 * @details This class should be instantiated once and then used to communicate with Astarte.
 */
class AstarteDevice {
 public:
  /**
   * @brief Constructor for the Astarte device class.
   * @param server_addr The gRPC server address of the Astarte message hub.
   * @param node_uuid The UUID identifier for this device with the Astarte message hub.
   */
  AstarteDevice(std::string server_addr, std::string node_uuid);
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
   * @brief Stream individual data to Astarte.
   * @param interface_name The name of the interface on which to stream the data.
   * @param path The path to the interface endpoint to use for streaming.
   * @param individual The data to stream.
   * @param timestamp The timestamp for the data, this might be a nullptr.
   */
  void stream_individual(const std::string &interface_name, const std::string &path,
                         AstarteIndividual &individual,
                         std::chrono::system_clock::time_point *timestamp);
  /**
   * @brief Stream aggregated data to Astarte.
   * @param interface_name The name of the interface on which to stream the data.
   * @param path The common path to the interface endpoint to use for streaming.
   * @param aggregated The data to stream.
   * @param timestamp The timestamp for the data, this might be a nullptr.
   */
  void stream_aggregated(const std::string &interface_name, const std::string &path,
                         std::unordered_map<std::string, AstarteIndividual> &aggregated,
                         std::chrono::system_clock::time_point *timestamp);
  /**
   * @brief Set a device property.
   * @param interface_name The name of the interface for the property.
   * @param path The property full path.
   * @param data The property data.
   */
  void set_property(const std::string &interface_name, const std::string &path,
                    AstarteIndividual &data);
  /**
   * @brief Unset a device property.
   * @param interface_name The name of the interface for the property.
   * @param path The property full path.
   */
  void unset_property(const std::string &interface_name, const std::string &path);

 private:
  /** @brief Handle the Astarte message hub events stream. */
  static void handle_events(std::unique_ptr<ClientReader<MessageHubEvent>> reader);

  /** @brief gRPC server address for the Astarte message hub. */
  std::string server_addr_;
  /** @brief Unique identifier for the device connection with the Astarte message hub. */
  std::string node_uuid_;
  /** @brief Stub for the gRPC message hub service. */
  std::unique_ptr<MessageHub::Stub> stub_;
  /** @brief List of json interfaces. Stored as binaries. */
  std::vector<std::string> interfaces_bins_;
  /** @brief Thread that handles the message hub events stream. */
  std::thread event_handler_;
  /**
   * @brief gRPC context for the client, unused.
   * @details It could be used to convey extra information to the server and/or tweak certain RPC
   * behaviors.
   */
  ClientContext client_context_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_DEVICE_H
