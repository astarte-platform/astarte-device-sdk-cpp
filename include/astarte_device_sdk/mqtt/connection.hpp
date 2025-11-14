// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_CONNECTION_H
#define ASTARTE_MQTT_CONNECTION_H

#include <spdlog/spdlog.h>

#include <format>
#include <string_view>

#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/errors.hpp"
#include "astarte_device_sdk/mqtt/formatter.hpp"
#include "astarte_device_sdk/mqtt/introspection.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "mqtt/async_client.h"
#include "mqtt/iasync_client.h"

namespace AstarteDeviceSdk {
/**
 * @brief Implement MQTT callbacks for handling connection events.
 */
class ConnectionCallback : public virtual mqtt::callback, public virtual mqtt::iaction_listener {
  /**
   * @brief Subscribe the client to all required Astarte topics.
   *
   * This includes the control topic and all topics for server-owned
   * interfaces defined in the device's introspection.
   */
  void setup_subscriptions();

  /**
   * @brief Publishe the device's introspection to Astarte.
   */
  void send_introspection();

  /**
   * @brief Send an "emptyCache" message to Astarte.
   */
  void send_emptycache();

  // (Re)connection success
  void connected(const std::string& cause) override;

  // Callback for when the connection is lost.
  // This will initiate the attempt to manually reconnect.
  void connection_lost(const std::string& cause) override;

  // Callback for when a message arrives.
  void message_arrived(mqtt::const_message_ptr msg) override;

  /**
   * @brief Called when a message delivery is complete.
   * @param token The delivery token associated with the message.
   */
  void delivery_complete(mqtt::delivery_token_ptr token) override;

  // Re-connection failure
  void on_failure(const mqtt::token& tok) override;

  // (Re)connection success
  void on_success(const mqtt::token& tok) override;

 private:
  /// @brief Flag used to prevent a double execution of on_success method once the device is
  /// connected
  bool initial_setup_done_{false};

 public:
  /**
   * @brief Construct a new Connection Callback object.
   *
   * @param client Pointer to the MQTT asynchronous client.
   * @param options The MQTT Paho connection options.
   * @param device_id The Astarte Device ID.
   * @param introspection A reference to the vector of device interfaces.
   */
  ConnectionCallback(mqtt::iasync_client* client, mqtt::connect_options options,
                     std::string device_id, std::vector<Interface>& introspection);

  /// @brief Pointer to the MQTT client, used for operations like subscribe.
  mqtt::iasync_client* client_;
  /// @brief MQTT connection options, used for reconnection.
  mqtt::connect_options options_;
  /// @brief The Astarte Device ID.
  std::string device_id_;
  /// @brief Reference to the device's introspection (list of interfaces).
  std::vector<Interface>& introspection_;
};

/**
 * @brief Manage the MQTT connection to an Astarte instance.
 */
class MqttConnection {
 public:
  /** @brief Copy constructor for the MqttConnection class. */
  MqttConnection(const MqttConnection&) = delete;
  /** @brief Copy assignment operator for the MqttConnection class. */
  MqttConnection& operator=(const MqttConnection&) = delete;
  /** @brief Move constructor for the MqttConnection class. */
  MqttConnection(MqttConnection&&) = default;
  /** @brief Move assignment operator for the MqttConnection class. */
  MqttConnection& operator=(MqttConnection&&) = default;

  /**
   * @brief Construct a new Mqtt Connection object.
   *
   * Initialize the connection parameters by performing pairing with the Astarte
   * instance specified in the configuration. It retrieves the broker URL and
   * sets up the MQTT client options.
   *
   * @param cfg The MQTT configuration object containing connection details.
   * @return The MQTT connection object, an error otherwise.
   */
  static auto create(MqttConfig cfg) -> astarte_tl::expected<MqttConnection, AstarteError>;

  /**
   * @brief Connect the client to the Astarte MQTT broker.
   * @param introspection A vector of interfaces defining the device.
   * @return an error if the connection operation fails.
   */
  auto connect(std::vector<Interface>& introspection) -> astarte_tl::expected<void, AstarteError>;

  /**
   * @brief Disconnect the client from the Astarte MQTT broker.
   * @return an error if the disconnection operation fails.
   */
  auto disconnect() -> astarte_tl::expected<void, AstarteError>;

 private:
  MqttConnection(MqttConfig cfg, mqtt::connect_options options,
                 std::unique_ptr<mqtt::async_client> client);

  /// @brief The MQTT configuration object.
  MqttConfig cfg_;
  /// @brief The Paho MQTT connection options.
  mqtt::connect_options options_;
  /// @brief The underlying Paho MQTT async client.
  std::unique_ptr<mqtt::async_client> client_;
  /// @brief The callback handler for MQTT events.
  std::unique_ptr<ConnectionCallback> cb_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_MQTT_CONNECTION_H
