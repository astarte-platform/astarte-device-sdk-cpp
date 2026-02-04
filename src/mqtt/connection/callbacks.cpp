// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "mqtt/connection/callbacks.hpp"

namespace astarte::device::mqtt::connection {

Callback::Callback(
    paho_mqtt::iasync_client* client, std::string realm, std::string device_id,
    std::shared_ptr<Introspection> introspection,
    const std::shared_ptr<std::atomic<bool>>& connected,
    const std::shared_ptr<paho_mqtt::thread_queue<paho_mqtt::token_ptr>>& session_setup_tokens)
    : client_(client),
      realm_(std::move(realm)),
      device_id_(std::move(device_id)),
      introspection_(std::move(introspection)),
      session_setup_tokens_(session_setup_tokens),
      connected_(connected),
      session_setup_listener_(
          std::make_shared<SessionSetupListener>(session_setup_tokens, connected)),
      disconnection_listener_(std::make_shared<DisconnectionListener>(connected)) {}

// TODO(rgallor): Perform additional checks. The "handshake" with astarte should have been completed
// in a previous connection and the device introspection should not have changed since the last
// connection.
auto Callback::perform_session_setup(bool session_present) -> astarte_tl::expected<void, Error> {
  if (!session_present) {
    auto res = setup_subscriptions();
    if (!res) {
      return astarte_tl::unexpected(res.error());
    }
    spdlog::debug("Subscription to Astarte topics transmitted.");

    res = send_introspection();
    if (!res) {
      return astarte_tl::unexpected(res.error());
    }
    spdlog::debug("Introspection sent to Astarte.");

    res = send_emptycache();
    if (!res) {
      return astarte_tl::unexpected(res.error());
    }
    spdlog::debug("EmptyCache sent to Astarte.");
  } else {
    spdlog::debug("Session present: skipping subscription and introspection setup.");
  }

  return {};
}

auto Callback::setup_subscriptions() -> astarte_tl::expected<void, Error> {
  // Define a collection of topics to subscribe to
  auto topics = paho_mqtt::string_collection();
  auto qoss = paho_mqtt::iasync_client::qos_collection();

  spdlog::debug("Subscribing to topic /control/consumer/properties");
  topics.push_back(astarte_fmt::format("{}/{}/control/consumer/properties", realm_, device_id_));
  qoss.push_back(2);

  for (const auto& interface : introspection_->values()) {
    // consider only server-owned properties
    if (interface->ownership() == Ownership::kDevice) {
      continue;
    }

    auto topic = astarte_fmt::format("{}/{}/{}/#", realm_, device_id_, interface->interface_name());
    spdlog::debug("Subscribing to topic {}", topic);
    topics.push_back(std::move(topic));
    qoss.push_back(2);
  }

  if (!topics.empty()) {
    try {
      const paho_mqtt::token_ptr sub_token =
          client_->subscribe(std::make_shared<paho_mqtt::string_collection>(topics), qoss, nullptr,
                             *session_setup_listener_);
      session_setup_tokens_->put(sub_token);
    } catch (...) {
      spdlog::error("failed to setup subscriptions");
      return astarte_tl::unexpected(MqttConnectionError("failed to setup subscriptions"));
    }
  }

  return {};
}

auto Callback::send_introspection() -> astarte_tl::expected<void, Error> {
  // Create the stringified representation of the introspection to send to Astarte
  auto introspection_str = std::string();
  for (const auto& interface : introspection_->values()) {
    introspection_str +=
        astarte_fmt::format("{}:{}:{};", interface->interface_name(), interface->version_major(),
                            interface->version_minor());
  }
  // Remove last unnecessary ";"
  if (!introspection_str.empty()) {
    introspection_str.pop_back();
  }

  auto base_topic = astarte_fmt::format("{}/{}", realm_, device_id_);
  try {
    const paho_mqtt::const_message_ptr message =
        paho_mqtt::message::create(base_topic, introspection_str, 2, false);
    const paho_mqtt::token_ptr pub_token =
        client_->publish(message, nullptr, *session_setup_listener_);
    session_setup_tokens_->put(pub_token);
    return {};
  } catch (...) {
    spdlog::error("failed to publish introspection");
    return astarte_tl::unexpected(MqttConnectionError("failed to publish introspection"));
  }
}

auto Callback::send_emptycache() -> astarte_tl::expected<void, Error> {
  auto emptycache_topic = astarte_fmt::format("{}/{}/control/emptyCache", realm_, device_id_);
  try {
    const paho_mqtt::const_message_ptr message =
        paho_mqtt::message::create(emptycache_topic, "1", 2, false);
    const paho_mqtt::token_ptr pub_token =
        client_->publish(message, nullptr, *session_setup_listener_);
    session_setup_tokens_->put(pub_token);
    return {};
  } catch (...) {
    spdlog::error("failed to perform empty cache");
    return astarte_tl::unexpected(MqttConnectionError("failed to perform empty cache"));
  }
}

void Callback::connected(const std::string& /* cause */) {
  spdlog::info("Device connected to Astarte.");
  auto res = perform_session_setup(false);
  if (!res) {
    spdlog::warn("Session setup failed.");
    session_setup_tokens_->clear();
    client_->disconnect(nullptr, *disconnection_listener_);
  }
}

void Callback::connection_lost(const std::string& cause) {
  spdlog::warn("Connection lost: {}, waiting for auto-reconnect...", cause);
  session_setup_tokens_->clear();
  connected_->store(false);
}

void Callback::message_arrived(paho_mqtt::const_message_ptr msg) {
  // TODO(rgallor): handle message reception
  spdlog::debug("Message received at {}: {}", msg->get_topic(), msg->get_payload_str());
}

void Callback::delivery_complete(paho_mqtt::delivery_token_ptr token) {
  auto message = token->get_message();
  auto payload = message->get_payload_str();
  auto topic = message->get_topic();
  auto qos = message->get_qos();
  spdlog::debug("Delivery completed. Payload: {}, Topic: {}, Qos: {},", payload, topic, qos);
}

}  // namespace astarte::device::mqtt::connection
