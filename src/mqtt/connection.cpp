// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/mqtt/connection.hpp"

#include <spdlog/spdlog.h>

#include <atomic>
#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// ... other includes kept as in your original file ...
#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/errors.hpp"
#include "astarte_device_sdk/mqtt/introspection.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "mqtt/async_client.h"
#include "mqtt/delivery_token.h"
#include "mqtt/exception.h"
#include "mqtt/iasync_client.h"
#include "mqtt/message.h"
#include "mqtt/token.h"

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
  // TODO(rgallor): instead of always generating new certificates, allow credential storage and
  // retrieval.
  auto key_cert_res = api.get_device_key_and_cert(secret);
  if (!key_cert_res) {
    return astarte_tl::unexpected(key_cert_res.error());
  }

  auto [client_priv_key, client_cert] = key_cert_res.value();

  const std::vector<std::pair<std::string, std::string>> files = {
      {astarte_fmt::format("{}/{}", store_dir, CLIENT_CERTIFICATE_FILE), client_cert},
      {astarte_fmt::format("{}/{}", store_dir, PRIVATE_KEY_FILE), client_priv_key}};

  for (const auto& [path, content] : files) {
    auto write_res = config::write_to_file(path, content);
    if (!write_res) {
      spdlog::error("Failed to write to {}. Error: {}", path, write_res.error());
      return astarte_tl::unexpected(write_res.error());
    }
  }

  return {};
}

}  // namespace

// ============================================================================
// ConnectionCallback Implementation
// ============================================================================

ConnectionCallback::ConnectionCallback(mqtt::iasync_client* client, std::string realm,
                                       std::string device_id, Introspection& introspection,
                                       std::shared_ptr<std::atomic<bool>> connected)
    : client_(client),
      realm_(std::move(realm)),
      device_id_(std::move(device_id)),
      introspection_(introspection),
      connected_(std::move(connected)) {}

void ConnectionCallback::perform_session_setup(bool session_present) {
  if (!session_present) {
    setup_subscriptions();
    spdlog::debug("Subscription to Astarte topics completed.");

    send_introspection();
    spdlog::debug("Introspection sent to Astarte.");

    send_emptycache();
    spdlog::debug("EmptyCache sent to Astarte.");
  } else {
    spdlog::debug("Session present: skipping subscription and introspection setup.");
  }
}

void ConnectionCallback::setup_subscriptions() {
  // Define a collection of topics to subscribe to
  auto topics = mqtt::string_collection();
  auto qoss = mqtt::iasync_client::qos_collection();

  spdlog::debug("Subscribing to topic /control/consumer/properties");
  topics.push_back(astarte_fmt::format("{}/{}/control/consumer/properties", realm_, device_id_));
  qoss.push_back(2);

  for (const auto& interface : introspection_.values()) {
    // consider only server-owned properties
    if (interface.ownership() == AstarteOwnership::kDevice) {
      continue;
    }

    auto topic = astarte_fmt::format("{}/{}/{}/#", realm_, device_id_, interface.interface_name());
    spdlog::debug("Subscribing to topic {}", topic);
    topics.push_back(std::move(topic));
    qoss.push_back(2);
  }

  if (!topics.empty()) {
    try {
      client_->subscribe(std::make_shared<mqtt::string_collection>(topics), qoss);
    } catch (...) {  // if something goes wrong, set the device to disconnected.
      spdlog::error("failed to subscribe to topic");
      connected_->store(false);
    }
  }
}

void ConnectionCallback::send_introspection() {
  // Create the stringified representation of the introspection to send to Astarte
  auto introspection_str = std::string();
  for (const auto& interface : introspection_.values()) {
    introspection_str += astarte_fmt::format("{}:{}:{};", interface.interface_name(),
                                             interface.version_major(), interface.version_minor());
  }
  // Remove last unnecessary ";"
  if (!introspection_str.empty()) {
    introspection_str.pop_back();
  }

  auto base_topic = astarte_fmt::format("{}/{}", realm_, device_id_);
  try {
    client_->publish(base_topic, introspection_str, 2, false);
  } catch (...) {  // if something goes wrong, set the device to disconnected.
    spdlog::error("failed to publish introspection");
    connected_->store(false);
  }
}

void ConnectionCallback::send_emptycache() {
  auto emptycache_topic = astarte_fmt::format("{}/{}/control/emptyCache", realm_, device_id_);
  try {
    client_->publish(emptycache_topic, "1", 2, false);
  } catch (...) {  // if something goes wrong, set the device to disconnected.
    spdlog::error("failed to perform empty cache");
    connected_->store(false);
  }
}

void ConnectionCallback::connected(const std::string& cause) {
  spdlog::info("Device connected to Astarte.");

  if (cause.find("automatic reconnect") != std::string::npos) {
    spdlog::info("Callback cause: {}", cause);

    // On auto-reconnect, we might want to ensure the session is set up correctly.
    // Passing false ensures we re-subscribe and re-send introspection.
    perform_session_setup(false);
  }
}

void ConnectionCallback::connection_lost(const std::string& cause) {
  spdlog::warn("Connection lost: {}, waiting for auto-reconnect...", cause);
  connected_->store(false);
}

void ConnectionCallback::message_arrived(mqtt::const_message_ptr msg) {
  // TODO(rgallor): handle message reception
  spdlog::debug("Message received at {}: {}", msg->get_topic(), msg->get_payload_str());
}

void ConnectionCallback::delivery_complete(mqtt::delivery_token_ptr /* token */) {
  spdlog::debug("Delivery completed.");
}

// NOLINTNEXTLINE(readability-function-size)
auto MqttConnection::create(MqttConfig& cfg) -> astarte_tl::expected<MqttConnection, AstarteError> {
  auto realm = cfg.realm();
  auto device_id = cfg.device_id();
  auto pairing_url = cfg.pairing_url();

  auto res = PairingApi::create(realm, device_id, pairing_url);
  if (!res) {
    spdlog::error("failed to create PairingApi instance. Error: {}", res.error());
    return astarte_tl::unexpected(res.error());
  }
  auto api = res.value();

  auto credential_secret = cfg.cred_value();
  if (cfg.cred_is_pairing_token()) {
    auto res = api.register_device(cfg.cred_value());
    if (!res) {
      spdlog::error("failed to register the device. Error {}", res.error());
      return astarte_tl::unexpected(res.error());
    }
    credential_secret = res.value();
  }

  auto broker_url = api.get_broker_url(credential_secret);
  if (!broker_url) {
    spdlog::error("failed to retrieve Astarte MQTT broker URL. Error: {}", broker_url.error());
    return astarte_tl::unexpected(broker_url.error());
  }

  auto crypto_setup = setup_crypto_files(api, credential_secret, cfg.store_dir());
  if (!crypto_setup) {
    spdlog::error("failed to setup crypto info. Error: {}", crypto_setup.error());
    return astarte_tl::unexpected(crypto_setup.error());
  }

  auto options = cfg.build_mqtt_options();
  if (!options) {
    spdlog::error("failed to build Astarte MQTT options. Error: {}", options.error());
    return astarte_tl::unexpected(options.error());
  }

  auto client_id = astarte_fmt::format("{}/{}", realm, device_id);
  auto client = std::make_unique<mqtt::async_client>(broker_url.value(), client_id);

  return MqttConnection(std::move(cfg), std::move(options.value()), std::move(client));
}

MqttConnection::MqttConnection(MqttConfig cfg, mqtt::connect_options options,
                               std::unique_ptr<mqtt::async_client> client)
    : cfg_(std::move(cfg)),
      options_(std::move(options)),
      client_(std::move(client)),
      connected_(std::make_shared<std::atomic<bool>>(false)) {}

auto MqttConnection::connect(Introspection& introspection)
    -> astarte_tl::expected<void, AstarteError> {
  try {
    spdlog::debug("Setting up connection callback...");

    cb_ = std::make_unique<ConnectionCallback>(client_.get(), std::string(cfg_.realm()),
                                               std::string(cfg_.device_id()), introspection,
                                               connected_);
    client_->set_callback(*cb_);

    spdlog::debug("Connecting device to the Astarte MQTT broker...");
    auto tok = client_->connect(options_);
    tok->wait();

    // we remove the client certificate and private key from filesystem
    auto res = config::secure_shred_file(
                   astarte_fmt::format("{}/{}", cfg_.store_dir(), CLIENT_CERTIFICATE_FILE))
                   .and_then([&]() {
                     return config::secure_shred_file(
                         astarte_fmt::format("{}/{}", cfg_.store_dir(), PRIVATE_KEY_FILE));
                   });
    if (!res) {
      spdlog::error("failed to delete client cert or private key from filesystem. Error: {}",
                    res.error());
      return astarte_tl::unexpected(res.error());
    }

    // MQTT connection is now established. Check Session Present.
    auto conn_res = tok->get_connect_response();
    const bool session_present = conn_res.is_session_present();

    if (session_present) {
      spdlog::info("Session resumed from broker.");
    } else {
      spdlog::info("Starting a new session...");
    }

    cb_->perform_session_setup(session_present);
    connected_->store(true);
  } catch (const mqtt::exception& e) {
    spdlog::error("Error while trying to connect to Astarte: {}", e.what());
    return astarte_tl::unexpected(AstarteMqttConnectionError(
        astarte_fmt::format("Mqtt connection error (ID {}): {}", e.get_reason_code(), e.what())));
  }

  return {};
}

auto MqttConnection::is_connected() const -> bool { return connected_->load(); }

auto MqttConnection::disconnect() -> astarte_tl::expected<void, AstarteError> {
  try {
    auto toks = client_->get_pending_delivery_tokens();
    if (!toks.empty()) {
      spdlog::error("There are pending delivery tokens!");
    }

    spdlog::debug("Disconnecting device from Astarte...");
    client_->disconnect()->wait();
    spdlog::info("Device disconnected from Astarte.");
  } catch (const mqtt::exception& e) {
    return astarte_tl::unexpected(AstarteMqttConnectionError(astarte_fmt::format(
        "Mqtt disconnection error (ID {}): {}", e.get_reason_code(), e.what())));
  }

  return {};
}

}  // namespace AstarteDeviceSdk
