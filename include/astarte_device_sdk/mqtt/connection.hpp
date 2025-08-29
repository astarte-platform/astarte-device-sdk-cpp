// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_CONNECTION_H
#define ASTARTE_MQTT_CONNECTION_H

#include <spdlog/spdlog.h>

#include <format>

#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "mqtt/async_client.h"

namespace AstarteDeviceSdk {

namespace {

/**
 * @brief Retrieve and persist device crypto credentials.
.
 * @param api The PairingApi instance used to request the credentials.
 * @param secret The credential secret used to authenticate the request.
 * @param store_dir The directory path where the certificate and private key files will be created.
 * @return an error if the API request fails or file writing errors occur.
*/
auto setup_crypto_files(PairingApi& api, const std::string_view secret,
                        const std::string_view store_dir)
    -> astarte_tl::expected<void, AstarteError> {
  auto key_cert_res = api.get_device_key_and_cert(secret);
  if (!key_cert_res) {
    return astarte_tl::unexpected(key_cert_res.error());
  }

  auto [client_priv_key, client_cert] = key_cert_res.value();

  const std::vector<std::pair<std::string, std::string>> files = {
      {std::format("{}/{}", store_dir, CLIENT_CERTIFICATE_FILE), client_cert},
      {std::format("{}/{}", store_dir, PRIVATE_KEY_FILE), client_priv_key}};

  for (const auto& [path, content] : files) {
    auto write_res = write_to_file(path, content);
    if (!write_res) {
      spdlog::error("Failed to write to {}. Error: {}", path, write_res.error());
      return astarte_tl::unexpected(write_res.error());
    }
  }

  return {};
}

}  // namespace

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
  static auto create(MqttConfig cfg) -> astarte_tl::expected<MqttConnection, AstarteError> {
    auto realm = cfg.realm();
    auto device_id = cfg.device_id();
    auto pairing_url = cfg.pairing_url();

    auto credential_secret = cfg.read_secret_or_register();
    if (!credential_secret) {
      spdlog::error("failed to read credential secret or register the device. Error: {}",
                    credential_secret.error());
      return astarte_tl::unexpected(credential_secret.error());
    }

    auto res = PairingApi::create(realm, device_id, pairing_url);
    if (!res) {
      spdlog::error("failed to create PairingApi instance. Error: {}", res.error());
      return astarte_tl::unexpected(res.error());
    }
    auto api = res.value();

    auto broker_url = api.get_broker_url(credential_secret.value());
    if (!broker_url) {
      spdlog::error("failed to retrieve Astarte MQTT broker URL. Error: {}", broker_url.error());
      return astarte_tl::unexpected(broker_url.error());
    }

    auto crypto_setup = setup_crypto_files(api, credential_secret.value(), cfg.store_dir());
    if (!crypto_setup) {
      spdlog::error("failed to setup crypto info. Error: {}", crypto_setup.error());
      return astarte_tl::unexpected(crypto_setup.error());
    }

    auto options = cfg.build_mqtt_options();
    if (!options) {
      spdlog::error("failed to build Astarte MQTT options. Error: {}", options.error());
      return astarte_tl::unexpected(options.error());
    }

    auto client_id = std::format("{}/{}", realm, device_id);
    auto client = std::make_unique<mqtt::async_client>(broker_url.value(), client_id);

    return MqttConnection(std::move(cfg), std::move(options.value()), std::move(client));
  }

  /**
   * @brief Connect the client to the Astarte MQTT broker.
   * @return an error if the connection operation fails.
   */
  auto connect() -> astarte_tl::expected<void, AstarteError> {
    try {
      spdlog::debug("connecting device to the Astarte MQTT broker...");
      client_->connect(options_)->wait();
    } catch (const mqtt::exception& e) {
      return astarte_tl::unexpected(AstarteMqttConnectionError(
          astarte_fmt::format("Mqtt connection error (ID {}): {}", e.get_reason_code(), e.what())));
    }

    spdlog::info("device connected to Astarte");
    return {};
  }

  /**
   * @brief Disconnect the client from the Astarte MQTT broker.
   * @return an error if the disconnection operation fails.
   */
  auto disconnect() -> astarte_tl::expected<void, AstarteError> {
    try {
      spdlog::debug("disconnecting device from astarte...");
      client_->disconnect()->wait();
    } catch (const mqtt::exception& e) {
      return astarte_tl::unexpected(AstarteMqttConnectionError(astarte_fmt::format(
          "Mqtt disconnection error (ID {}): {}", e.get_reason_code(), e.what())));
    }

    spdlog::info("device disconnected from Astarte");
    return {};
  }

 private:
  MqttConnection(MqttConfig cfg, mqtt::connect_options options,
                 std::unique_ptr<mqtt::async_client> client)
      : cfg_(std::move(cfg)), options_(std::move(options)), client_(std::move(client)) {}

  /// @brief The MQTT configuration object.
  MqttConfig cfg_;
  /// @brief The Paho MQTT connection options.
  mqtt::connect_options options_;
  /// @brief The underlying Paho MQTT async client.
  std::unique_ptr<mqtt::async_client> client_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_MQTT_CONNECTION_H
