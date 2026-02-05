// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_CONNECTION_H
#define ASTARTE_MQTT_CONNECTION_H

/**
 * @file private/mqtt/connection/connection.hpp
 * @brief Astarte MQTT Connection Manager.
 *
 * @details This file defines the `Connection` class, which wraps the Paho MQTT client
 * and orchestrates the entire connection lifecycle, including pairing, SSL/TLS configuration,
 * session setup, and message transmission.
 */

#include <spdlog/spdlog.h>

#include <atomic>
#include <format>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/errors.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "mqtt/async_client.h"
#include "mqtt/connection/callbacks.hpp"
#include "mqtt/connection/listener.hpp"
#include "mqtt/iasync_client.h"
#include "mqtt/introspection.hpp"

namespace astarte::device::mqtt::connection {

namespace paho_mqtt = ::mqtt;

/**
 * @brief Manages the MQTT connection to an Astarte instance.
 *
 * @details The `Connection` class is responsible for initializing the MQTT client with
 * correct credentials (potentially obtained via Pairing), managing the connection options (SSL,
 * Persistence), and handling data transmission.
 */
class Connection {
 public:
  /// @brief Default destructor for the Connection class.
  ~Connection() = default;

  /// @brief Connection is non-copyable.
  Connection(const Connection&) = delete;

  /// @brief Move constructor for the Connection class.
  Connection(Connection&&) noexcept = default;

  /// @brief Connection is non-copyable.
  auto operator=(const Connection&) -> Connection& = delete;

  /**
   * @brief Move assignment operator for the Connection class.
   * @return A reference to this Connection object.
   */
  auto operator=(Connection&&) noexcept -> Connection& = default;

  /**
   * @brief Constructs a new Mqtt Connection object.
   *
   * @details Initializes connection parameters, performs pairing if necessary,
   * and configures the MQTT client.
   *
   * @param[in] cfg The MQTT configuration object containing connection details (Realm, ID, Secret,
   * etc.).
   * @return An expected containing the Connection object on success, or an Error on failure.
   */
  static auto create(Config& cfg) -> astarte_tl::expected<Connection, Error>;

  /**
   * @brief Connects the client to the Astarte MQTT broker.
   *
   * @details Initiates the connection process using the provided introspection to set up
   * subscriptions. This is an asynchronous operation that sets up the callback handlers.
   *
   * @param[in] introspection A collection of interfaces defining the device structure.
   * @return An expected conforming to std::expected containing void on success or Error on failure.
   */
  auto connect(std::shared_ptr<Introspection> introspection) -> astarte_tl::expected<void, Error>;

  /**
   * @brief Checks if the device is connected.
   * @return True if the device is fully connected to the Astarte broker, false otherwise.
   */
  [[nodiscard]] auto is_connected() const -> bool;

  /**
   * @brief Sends individual or object data to Astarte.
   *
   * @param[in] interface_name The interface on which data will be sent.
   * @param[in] path The mapping path of the Astarte interface on which data will be sent.
   * @param[in] qos The quality of service value (0, 1, or 2).
   * @param[in] data A span of bytes containing the BSON serialized data to send.
   * @return An expected conforming to std::expected containing void on success or Error on failure.
   */
  auto send(std::string_view interface_name, std::string_view path, uint8_t qos,
            std::span<uint8_t> data) -> astarte_tl::expected<void, Error>;

  /**
   * @brief Disconnects the client from the Astarte MQTT broker.
   * @details Performs a graceful shutdown of the MQTT session.
   * @return An expected conforming to std::expected containing void on success or Error on failure.
   */
  auto disconnect() -> astarte_tl::expected<void, Error>;

 private:
  /**
   * @brief Private constructor.
   * @param[in] cfg Configuration options.
   * @param[in] options Paho Connect options.
   * @param[in] client The Paho async client.
   * @param[in] pairing_api The pairing API instance.
   */
  Connection(Config cfg, paho_mqtt::connect_options options,
             std::unique_ptr<paho_mqtt::async_client> client, PairingApi pairing_api);

  /// @brief Pairing API object.
  PairingApi pairing_api_;
  /// @brief The MQTT configuration object.
  Config cfg_;
  /// @brief The Paho MQTT connection options.
  paho_mqtt::connect_options connect_options_;
  /// @brief The underlying Paho MQTT async client.
  std::unique_ptr<paho_mqtt::async_client> client_;
  /// @brief The callback handler for MQTT events.
  std::unique_ptr<Callback> callback_;
  /// @brief Flag stating if the device is successfully connected to Astarte.
  std::shared_ptr<std::atomic<bool>> connected_;
  /// @brief Queue containing the tokens used during session setup.
  std::shared_ptr<paho_mqtt::thread_queue<paho_mqtt::token_ptr>> session_setup_tokens_;
};

}  // namespace astarte::device::mqtt::connection

#endif  // ASTARTE_MQTT_CONNECTION_H
