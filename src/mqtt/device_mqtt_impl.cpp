// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

// macro to mark unimplemented code. Throws an exception with file/line info.
#define TODO(msg)                                                                \
  throw std::logic_error("TODO: " + std::string(msg) + " at " + __FILE__ + ":" + \
                         std::to_string(__LINE__));

#include "mqtt/device_mqtt_impl.hpp"

#include <chrono>
#include <filesystem>
#include <list>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/connection.hpp"
#include "astarte_device_sdk/mqtt/device_mqtt.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/stored_property.hpp"

namespace AstarteDeviceSdk {

auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::create(const MqttConfig cfg)
    -> astarte_tl::expected<std::shared_ptr<AstarteDeviceMqttImpl>, AstarteError> {
  auto conn = MqttConnection::create(cfg);
  if (!conn) {
    spdlog::error("failed to create a MQTT connection. Error: {}", conn.error());
    return astarte_tl::unexpected(conn.error());
  }

  std::shared_ptr<AstarteDeviceMqttImpl> impl(
      new AstarteDeviceMqttImpl(cfg, std::move(conn.value())));

  return impl;
}

AstarteDeviceMqtt::AstarteDeviceMqttImpl::AstarteDeviceMqttImpl(MqttConfig cfg,
                                                                MqttConnection connection)
    : cfg_(std::move(cfg)), connection_(std::move(connection)) {}

AstarteDeviceMqtt::AstarteDeviceMqttImpl::~AstarteDeviceMqttImpl() = default;

auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::add_interface_from_file(
    const std::filesystem::path& json_file) -> astarte_tl::expected<void, AstarteError> {
  spdlog::debug("Adding interface from file: {}", json_file.string());

  std::ifstream interface_file(json_file, std::ios::in);
  if (!interface_file.is_open()) {
    spdlog::error("Could not open the interface file: {}", json_file.string());
    return astarte_tl::unexpected(AstarteFileOpenError(json_file.string()));
  }

  const std::string interface_json((std::istreambuf_iterator<char>(interface_file)),
                                   std::istreambuf_iterator<char>());
  interface_file.close();

  return add_interface_from_str(interface_json);
}

auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::add_interface_from_str(std::string_view json)
    -> astarte_tl::expected<void, AstarteError> {
  spdlog::debug("Adding interface from string");

  // If the device is connected, notify the message hub
  if (is_connected()) {
    // gRPCInterfacesJson grpc_interfaces_json;
    // grpc_interfaces_json.add_interfaces_json(json);
    // ClientContext context;
    // google::protobuf::Empty response;
    // const Status status = stub_->AddInterfaces(&context, grpc_interfaces_json, &response);
    // if (!status.ok()) {
    //   spdlog::error("{}: {}", static_cast<int>(status.error_code()), status.error_message());
    //   return;
    // }
  }

  introspection_.emplace_back(json);
  spdlog::trace("Added interface: \n{}", json);
  return {};
}

auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::remove_interface(const std::string& interface_name)
    -> astarte_tl::expected<void, AstarteError> {
  TODO("not yet implemented");
}

auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::connect()
    -> astarte_tl::expected<void, AstarteError> {
  return connection_.connect().map([this]() { connected_.store(true); });
}

[[nodiscard]] auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::is_connected() const -> bool {
  return connected_.load();
}

auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::disconnect()
    -> astarte_tl::expected<void, AstarteError> {
  if (!is_connected()) {
    spdlog::debug("device already disconnected");
    return {};
  }

  return connection_.disconnect().map([this]() { connected_.store(false); });
}

auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::send_individual(
    std::string_view interface_name, std::string_view path, const AstarteData& data,
    const std::chrono::system_clock::time_point* timestamp)
    -> astarte_tl::expected<void, AstarteError> {
  TODO("not yet implemented");
}
auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::send_object(
    std::string_view interface_name, std::string_view path, const AstarteDatastreamObject& object,
    const std::chrono::system_clock::time_point* timestamp)
    -> astarte_tl::expected<void, AstarteError> {
  TODO("not yet implemented");
}
auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::set_property(std::string_view interface_name,
                                                            std::string_view path,
                                                            const AstarteData& data)
    -> astarte_tl::expected<void, AstarteError> {
  TODO("not yet implemented");
}
auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::unset_property(std::string_view interface_name,
                                                              std::string_view path)
    -> astarte_tl::expected<void, AstarteError> {
  TODO("not yet implemented");
}
auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::poll_incoming(
    const std::chrono::milliseconds& timeout) -> std::optional<AstarteMessage> {
  TODO("not yet implemented");
}
auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::get_all_properties(
    const std::optional<AstarteOwnership>& ownership)
    -> astarte_tl::expected<std::list<AstarteStoredProperty>, AstarteError> {
  TODO("not yet implemented");
}
auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::get_properties(std::string_view interface_name)
    -> astarte_tl::expected<std::list<AstarteStoredProperty>, AstarteError> {
  TODO("not yet implemented");
}
auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::get_property(std::string_view interface_name,
                                                            std::string_view path)
    -> astarte_tl::expected<AstartePropertyIndividual, AstarteError> {
  TODO("not yet implemented");
}

}  // namespace AstarteDeviceSdk
