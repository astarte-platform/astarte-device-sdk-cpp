// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

// macro to mark unimplemented code. Throws an exception with file/line info.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TODO(msg)                                                                \
  throw std::logic_error("TODO: " + std::string(msg) + " at " + __FILE__ + ":" + \
                         std::to_string(__LINE__));

#include "mqtt/device_mqtt_impl.hpp"

#include <spdlog/spdlog.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iterator>
#include <list>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/device_mqtt.hpp"
#include "astarte_device_sdk/mqtt/errors.hpp"
#include "astarte_device_sdk/mqtt/introspection.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/stored_property.hpp"
#include "mqtt/connection.hpp"

namespace AstarteDeviceSdk {

using json = nlohmann::json;  // NOLINT(misc-include-cleaner)

auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::create(MqttConfig& cfg)
    -> astarte_tl::expected<std::shared_ptr<AstarteDeviceMqttImpl>, AstarteError> {
  auto conn = MqttConnection::create(cfg);
  if (!conn) {
    spdlog::error("failed to create a MQTT connection. Error: {}", conn.error());
    return astarte_tl::unexpected(conn.error());
  }

  return std::shared_ptr<AstarteDeviceMqttImpl>(
      new AstarteDeviceMqttImpl(cfg, std::move(conn.value())));
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

  // Read the entire JSON file content into a string
  const std::string interface_str((std::istreambuf_iterator<char>(interface_file)),
                                  std::istreambuf_iterator<char>());

  // Close the file
  interface_file.close();

  // Add the interface from the fetched string
  return add_interface_from_str(interface_str);
}

auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::add_interface_from_str(
    std::string_view interface_str) -> astarte_tl::expected<void, AstarteError> {
  spdlog::debug("Adding interface from string");

  if (is_connected()) {
    // TODO(rgallor): If the device is connected, communicate the new introspection to Astarte
  }

  json interface_json;
  try {
    interface_json = json::parse(interface_str);
  } catch (json::parse_error& e) {
    spdlog::error("failed to parse JSON Astarte interface: {}", e.what());
    return astarte_tl::unexpected(AstarteJsonParsingError(
        astarte_fmt::format("failed to parse interface from json: {}", e.what())));
  }

  auto interface = Interface::try_from_json(interface_json);
  if (!interface) {
    return astarte_tl::unexpected(interface.error());
  }

  return introspection_.checked_insert(std::move(interface.value()));
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::remove_interface(
    const std::string& /* interface_name */) -> astarte_tl::expected<void, AstarteError> {
  TODO("not yet implemented");
}

auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::connect()
    -> astarte_tl::expected<void, AstarteError> {
  return connection_.connect(introspection_);
}

[[nodiscard]] auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::is_connected() const -> bool {
  return connection_.is_connected();
}

auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::disconnect()
    -> astarte_tl::expected<void, AstarteError> {
  if (!is_connected()) {
    spdlog::debug("device already disconnected");
    return {};
  }

  return connection_.disconnect();
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::send_individual(
    std::string_view /* interface_name */, std::string_view /* path */,
    const AstarteData& /* data */, const std::chrono::system_clock::time_point* /* timestamp */)
    -> astarte_tl::expected<void, AstarteError> {
  TODO("not yet implemented");
}
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::send_object(
    std::string_view /* interface_name */, std::string_view /* path */,
    const AstarteDatastreamObject& /* object */,
    const std::chrono::system_clock::time_point* /* timestamp */)
    -> astarte_tl::expected<void, AstarteError> {
  TODO("not yet implemented");
}
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::set_property(std::string_view /* interface_name */,
                                                            std::string_view /* path */,
                                                            const AstarteData& /* data */)
    -> astarte_tl::expected<void, AstarteError> {
  TODO("not yet implemented");
}
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::unset_property(std::string_view /* interface_name */,
                                                              std::string_view /* path */)
    -> astarte_tl::expected<void, AstarteError> {
  TODO("not yet implemented");
}
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::poll_incoming(
    const std::chrono::milliseconds& /* timeout */) -> std::optional<AstarteMessage> {
  // TODO(rgallor): change which correct implementation. the actual one is used only to make e2e
  // tests work.
  return std::nullopt;
}
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::get_all_properties(
    const std::optional<AstarteOwnership>& /* ownership */)
    -> astarte_tl::expected<std::list<AstarteStoredProperty>, AstarteError> {
  TODO("not yet implemented");
}
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::get_properties(std::string_view /* interface_name */)
    -> astarte_tl::expected<std::list<AstarteStoredProperty>, AstarteError> {
  TODO("not yet implemented");
}
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::get_property(std::string_view /* interface_name */,
                                                            std::string_view /* path */)
    -> astarte_tl::expected<AstartePropertyIndividual, AstarteError> {
  TODO("not yet implemented");
}

}  // namespace AstarteDeviceSdk
