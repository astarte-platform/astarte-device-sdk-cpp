// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/mqtt/config.hpp"

#include <spdlog/spdlog.h>

#include <chrono>
#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "astarte_device_sdk/mqtt/errors.hpp"
#include "astarte_device_sdk/mqtt/formatter.hpp"
#include "mqtt/connect_options.h"
#include "mqtt/credentials.hpp"

namespace config {

MqttConfig::~MqttConfig() = default;
MqttConfig::MqttConfig(MqttConfig&&) noexcept = default;
auto MqttConfig::operator=(MqttConfig&&) noexcept -> MqttConfig& = default;

MqttConfig::MqttConfig(std::string_view realm, std::string_view device_id,
                       std::unique_ptr<Credential> credential, std::string_view pairing_url,
                       std::string_view store_dir)
    : realm_(realm),
      device_id_(device_id),
      credential_(std::move(credential)),
      pairing_url_(pairing_url),
      store_dir_(store_dir),
      ignore_ssl_(false),
      keepalive_(DEFAULT_KEEP_ALIVE),
      conn_timeout_(DEFAULT_CONNECTION_TIMEOUT) {}

auto MqttConfig::with_credential_secret(std::string_view realm, std::string_view device_id,
                                        std::string_view credential, std::string_view pairing_url,
                                        std::string_view store_dir) -> MqttConfig {
  auto cred_ptr = std::make_unique<Credential>(Credential::secret(credential));
  return {realm, device_id, std::move(cred_ptr), pairing_url, store_dir};
}

auto MqttConfig::with_pairing_token(std::string_view realm, std::string_view device_id,
                                    std::string_view credential, std::string_view pairing_url,
                                    std::string_view store_dir) -> MqttConfig {
  auto cred_ptr = std::make_unique<Credential>(Credential::pairing_token(credential));
  return {realm, device_id, std::move(cred_ptr), pairing_url, store_dir};
}

auto MqttConfig::cred_is_pairing_token() -> bool { return credential_->is_pairing_token(); }

auto MqttConfig::cred_is_credential_secret() -> bool { return credential_->is_credential_secret(); }

auto MqttConfig::cred_value() -> std::string { return credential_->value(); }

auto MqttConfig::build_mqtt_options() -> astarte_tl::expected<mqtt::connect_options, AstarteError> {
  auto conn_opts = mqtt::connect_options_builder::v3();

  if (keepalive_ <= conn_timeout_) {
    return astarte_tl::unexpected(AstarteDeviceSdk::AstartePairingConfigError(
        astarte_fmt::format("Keep alive ({}s) should be greater than the connection timeout ({}s)",
                            keepalive_, conn_timeout_)));
  }

  conn_opts.keep_alive_interval(std::chrono::seconds(keepalive_))
      .connect_timeout(std::chrono::seconds(conn_timeout_))
      .automatic_reconnect(std::chrono::seconds(2), std::chrono::minutes(1))
      .clean_session(true);

  auto ssl_opts =
      mqtt::ssl_options_builder()
          .ssl_version(3)
          .enable_server_cert_auth(true)
          .verify(false)
          // Astarte MQTT broker requires client authentication (mutual TLS),
          .key_store(astarte_fmt::format("{}/{}", store_dir_, CLIENT_CERTIFICATE_FILE))
          .private_key(astarte_fmt::format("{}/{}", store_dir_, PRIVATE_KEY_FILE))
          .error_handler([](const std::string& msg) { spdlog::error("TLS error: {}", msg); })
          .finalize();

  // Add the SSL options to the connection builder
  conn_opts.ssl(std::move(ssl_opts));

  return conn_opts.finalize();
}

}  // namespace config
