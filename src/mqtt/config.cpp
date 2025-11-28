// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/mqtt/config.hpp"

#include <spdlog/spdlog.h>

#include <chrono>
#include <format>
#include <string>
#include <utility>

#include "astarte_device_sdk/mqtt/errors.hpp"
#include "astarte_device_sdk/mqtt/formatter.hpp"
#include "mqtt/connect_options.h"

namespace AstarteDeviceSdk {

auto MqttConfig::build_mqtt_options() -> astarte_tl::expected<mqtt::connect_options, AstarteError> {
  auto conn_opts = mqtt::connect_options_builder::v3();

  if (keepalive_ <= conn_timeout_) {
    return astarte_tl::unexpected(AstartePairingConfigError(
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

}  // namespace AstarteDeviceSdk
