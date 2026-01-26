// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "mqtt/connection/connection.hpp"

#include <mqtt/async_client.h>
#include <mqtt/connect_options.h>
#include <mqtt/delivery_token.h>
#include <mqtt/exception.h>
#include <mqtt/iasync_client.h>
#include <mqtt/message.h>
#include <mqtt/token.h>
#include <spdlog/spdlog.h>

#include <atomic>
#include <chrono>
#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/errors.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "mqtt/credentials.hpp"
#include "mqtt/introspection.hpp"
#include "mqtt/persistence.hpp"

namespace AstarteDeviceSdk::mqtt_connection {

using config::Credential;

namespace {

auto build_mqtt_options(config::MqttConfig& cfg)
    -> astarte_tl::expected<mqtt::connect_options, AstarteError> {
  auto conn_opts = mqtt::connect_options_builder::v3();
  auto conn_timeout = cfg.connection_timeout();
  auto keepalive = cfg.keepalive();

  if (keepalive <= conn_timeout) {
    return astarte_tl::unexpected(AstarteDeviceSdk::AstartePairingConfigError(
        astarte_fmt::format("Keep alive ({}s) should be greater than the connection timeout ({}s)",
                            keepalive, conn_timeout)));
  }

  conn_opts.keep_alive_interval(std::chrono::seconds(keepalive))
      .connect_timeout(std::chrono::seconds(conn_timeout))
      .automatic_reconnect(std::chrono::seconds(2), std::chrono::minutes(1))
      .clean_session(true);

  auto ssl_opts =
      mqtt::ssl_options_builder()
          .ssl_version(3)
          .enable_server_cert_auth(true)
          .verify(false)
          // Astarte MQTT broker requires client authentication (mutual TLS),
          .key_store(Credential::get_device_certificate_path(cfg.store_dir()))
          .private_key(Credential::get_device_key_path(cfg.store_dir()))
          .error_handler([](const std::string& msg) { spdlog::error("TLS error: {}", msg); })
          .finalize();

  // Add the SSL options to the connection builder
  conn_opts.ssl(std::move(ssl_opts));

  return conn_opts.finalize();
}

}  // namespace

// ============================================================================
// Connection Implementation
// ============================================================================

auto Connection::create(config::MqttConfig& cfg) -> astarte_tl::expected<Connection, AstarteError> {
  auto realm = cfg.realm();
  auto device_id = cfg.device_id();
  auto pairing_url = cfg.pairing_url();
  auto credential_secret = cfg.credential_secret();
  if (!credential_secret) {
    constexpr std::string_view reason =
        "Connection creation is only supported using a credential secret.";
    spdlog::error(reason);
    return astarte_tl::unexpected(AstarteMqttConnectionError(astarte_fmt::format(reason)));
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

  auto certificate_key_pair = api.get_device_key_and_certificate(credential_secret.value());
  if (!certificate_key_pair) {
    return astarte_tl::unexpected(certificate_key_pair.error());
  }
  auto [device_private_key, device_certificate] = certificate_key_pair.value();

  auto crypto_setup = Credential::store_device_key_and_certificate(
      device_private_key, device_certificate, cfg.store_dir());
  if (!crypto_setup) {
    spdlog::error("failed to setup crypto info. Error: {}", crypto_setup.error());
    return astarte_tl::unexpected(crypto_setup.error());
  }

  auto options = build_mqtt_options(cfg);
  if (!options) {
    spdlog::error("failed to build Astarte MQTT options. Error: {}", options.error());
    return astarte_tl::unexpected(options.error());
  }

  auto client_id = astarte_fmt::format("{}/{}", realm, device_id);
  auto client = std::make_unique<mqtt::async_client>(broker_url.value(), client_id);

  return Connection(std::move(cfg), std::move(options.value()), std::move(client), std::move(api));
}

Connection::Connection(config::MqttConfig cfg, mqtt::connect_options options,
                       std::unique_ptr<mqtt::async_client> client, PairingApi pairing_api)
    : cfg_(std::move(cfg)),
      connect_options_(std::move(options)),
      client_(std::move(client)),
      connected_(std::make_shared<std::atomic<bool>>(false)),
      session_setup_tokens_(std::make_shared<mqtt::thread_queue<mqtt::token_ptr>>()),
      pairing_api_(std::move(pairing_api)) {}

auto Connection::connect(std::shared_ptr<Introspection> introspection)
    -> astarte_tl::expected<void, AstarteError> {
  try {
    spdlog::debug("Setting up connection callback...");

    auto credential_secret = cfg_.credential_secret();
    if (!credential_secret) {
      constexpr std::string_view reason =
          "Attempting a connection when the credential secret is missing.";
      spdlog::error(reason);
      return astarte_tl::unexpected(AstarteMqttConnectionError(astarte_fmt::format(reason)));
    }

    auto cert_is_valid = Credential::validate_client_certificate(
        pairing_api_, credential_secret.value(), cfg_.store_dir());
    if (!cert_is_valid) {
      return astarte_tl::unexpected(cert_is_valid.error());
    }
    if (!cert_is_valid.value()) {
      auto certificate_key_pair =
          pairing_api_.get_device_key_and_certificate(credential_secret.value());
      if (!certificate_key_pair) {
        return astarte_tl::unexpected(certificate_key_pair.error());
      }
      auto [device_private_key, device_certificate] = certificate_key_pair.value();

      auto crypto_setup = Credential::store_device_key_and_certificate(
          device_private_key, device_certificate, cfg_.store_dir());
      if (!crypto_setup) {
        spdlog::error("failed to setup crypto info. Error: {}", crypto_setup.error());
        return astarte_tl::unexpected(crypto_setup.error());
      }
    }

    // TODO(sorru94): this could be moved in the constructor if the Introspection is also passed
    // during object instantiation
    callback_ = std::make_unique<Callback>(client_.get(), std::string(cfg_.realm()),
                                           std::string(cfg_.device_id()), std::move(introspection),
                                           connected_, session_setup_tokens_);
    client_->set_callback(*callback_);

    spdlog::debug("Connecting device to the Astarte MQTT broker...");
    client_->connect(connect_options_)->wait();

    Credential::delete_client_certificate_and_key(cfg_.store_dir());

    // TODO(sorru94): check if connection is fully established and add timeout if needed.

  } catch (const mqtt::exception& e) {
    spdlog::error("Error while trying to connect to Astarte: {}", e.what());
    return astarte_tl::unexpected(AstarteMqttConnectionError(
        astarte_fmt::format("Mqtt connection error (ID {}): {}", e.get_reason_code(), e.what())));
  }

  return {};
}

auto Connection::is_connected() const -> bool { return connected_->load(); }

auto Connection::send(std::string_view interface_name, std::string_view path, uint8_t qos,
                      const std::span<uint8_t> data) -> astarte_tl::expected<void, AstarteError> {
  if (!path.starts_with('/')) {
    return astarte_tl::unexpected(AstarteMqttError(
        astarte_fmt::format("couldn't publish since path doesn't starts with /: {}", path)));
  }

  if (qos > 2) {
    return astarte_tl::unexpected(
        AstarteMqttError(astarte_fmt::format("couldn't publish since QoS is {}", qos)));
  }

  auto topic =
      astarte_fmt::format("{}/{}/{}{}", cfg_.realm(), cfg_.device_id(), interface_name, path);
  spdlog::debug("publishing on topic {}", topic);

  try {
    auto token = client_->publish(topic, data.data(), data.size(), qos, false);
    auto message = token->get_message();
    spdlog::trace("Publishing... Topic: {}, Qos: {},", message->get_topic(), message->get_qos());
    token->wait();
  } catch (...) {
    // TODO(rgallor): catch the correct paho error and report it inside the log.
    // TODO(rgallor): determine whether the exception is due to a connection error, if it caused the
    // device disconection (eventually reconnect) connected_->store(false);
    spdlog::error("failed to publish astarte individual");
    return astarte_tl::unexpected(AstarteMqttError("failed to publish astarte individual"));
  }

  return {};
}

auto Connection::disconnect() -> astarte_tl::expected<void, AstarteError> {
  try {
    auto toks = client_->get_pending_delivery_tokens();
    if (!toks.empty()) {
      spdlog::error("There are pending delivery tokens!");
    }

    spdlog::debug("Disconnecting device from Astarte...");
    session_setup_tokens_->clear();
    client_->disconnect(static_cast<int>(cfg_.disconnection_timeout().count()))->wait();
    connected_->store(false);
    spdlog::info("Device disconnected from Astarte requested.");
  } catch (const mqtt::exception& e) {
    return astarte_tl::unexpected(AstarteMqttConnectionError(astarte_fmt::format(
        "Mqtt disconnection error (ID {}): {}", e.get_reason_code(), e.what())));
  }

  return {};
}

}  // namespace AstarteDeviceSdk::mqtt_connection
