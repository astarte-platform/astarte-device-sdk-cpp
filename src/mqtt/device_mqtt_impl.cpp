// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

// macro to mark unimplemented code. Throws an exception with file/line info.
#define TODO(msg)                                                                \
  throw std::logic_error("TODO: " + std::string(msg) + " at " + __FILE__ + ":" + \
                         std::to_string(__LINE__));

#include "mqtt/device_mqtt_impl.hpp"

#include <spdlog/spdlog.h>

#include <chrono>
#include <filesystem>
#include <list>
#include <nlohmann/json.hpp>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/exceptions.hpp"
#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/connection.hpp"
#include "astarte_device_sdk/mqtt/device_mqtt.hpp"
#include "astarte_device_sdk/mqtt/formatter.hpp"
#include "astarte_device_sdk/mqtt/introspection.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/stored_property.hpp"

namespace AstarteDeviceSdk {

using json = nlohmann::json;

AstarteDeviceMQTT::AstarteDeviceMQTTImpl::AstarteDeviceMQTTImpl(const MqttConfig cfg)
    : cfg_(cfg), connection_(std::move(cfg)) {}

AstarteDeviceMQTT::AstarteDeviceMQTTImpl::~AstarteDeviceMQTTImpl() = default;

void AstarteDeviceMQTT::AstarteDeviceMQTTImpl::add_interface_from_file(
    const std::filesystem::path& json_file) {
  spdlog::debug("Adding interface from file: {}", json_file.string());

  // Check file validity
  std::ifstream interface_file(json_file, std::ios::in);
  if (!interface_file.is_open()) {
    spdlog::error("Could not open the interface file: {}", json_file.string());
    throw AstarteFileOpenException(json_file.string());
  }

  // Read the entire JSON file content into a string
  const std::string interface_str((std::istreambuf_iterator<char>(interface_file)),
                                  std::istreambuf_iterator<char>());

  // Close the file
  interface_file.close();

  // Add the interface from the fetched string
  add_interface_from_str(interface_str);
}

void AstarteDeviceMQTT::AstarteDeviceMQTTImpl::add_interface_from_str(
    std::string_view interface_str) {
  spdlog::debug("Adding interface from string");

  if (is_connected()) {
    // TODO: If the device is connected, communicate the new introspection to Astarte
  }

  try {
    auto interface_json = json::parse(interface_str);
    auto interface = Interface::try_from_json(interface_json);
    spdlog::trace("Added interface: \n{}", interface);
    introspection_.emplace_back(std::move(interface));
  } catch (json::parse_error& e) {
    spdlog::error("failed to parse JSON Astarte interface: {}", e.what());
    throw e;  // TODO: convert to a library exception type
  }
}

void AstarteDeviceMQTT::AstarteDeviceMQTTImpl::remove_interface(const std::string& interface_name) {
  TODO("not yet implemented");
}

void AstarteDeviceMQTT::AstarteDeviceMQTTImpl::connect() {
  try {
    connection_.connect();
  } catch (const std::exception& e) {
    throw e;
  }

  connected_.store(true);
}

[[nodiscard]] auto AstarteDeviceMQTT::AstarteDeviceMQTTImpl::is_connected() const -> bool {
  return connected_.load();
}

void AstarteDeviceMQTT::AstarteDeviceMQTTImpl::disconnect() {
  if (!is_connected()) {
    spdlog::debug("device already disconnected");
    return;
  }

  try {
    connection_.disconnect();
  } catch (const std::exception& e) {
    throw e;
  }

  connected_.store(false);
}

void AstarteDeviceMQTT::AstarteDeviceMQTTImpl::send_individual(
    std::string_view interface_name, std::string_view path, const AstarteData& data,
    const std::chrono::system_clock::time_point* timestamp) {
  TODO("not yet implemented");
}
void AstarteDeviceMQTT::AstarteDeviceMQTTImpl::send_object(
    std::string_view interface_name, std::string_view path, const AstarteDatastreamObject& object,
    const std::chrono::system_clock::time_point* timestamp) {
  TODO("not yet implemented");
}
void AstarteDeviceMQTT::AstarteDeviceMQTTImpl::set_property(std::string_view interface_name,
                                                            std::string_view path,
                                                            const AstarteData& data) {
  TODO("not yet implemented");
}
void AstarteDeviceMQTT::AstarteDeviceMQTTImpl::unset_property(std::string_view interface_name,
                                                              std::string_view path) {
  TODO("not yet implemented");
}
auto AstarteDeviceMQTT::AstarteDeviceMQTTImpl::poll_incoming(
    const std::chrono::milliseconds& timeout) -> std::optional<AstarteMessage> {
  TODO("not yet implemented");
}
auto AstarteDeviceMQTT::AstarteDeviceMQTTImpl::get_all_properties(
    const std::optional<AstarteOwnership>& ownership) -> std::list<AstarteStoredProperty> {
  TODO("not yet implemented");
}
auto AstarteDeviceMQTT::AstarteDeviceMQTTImpl::get_properties(std::string_view interface_name)
    -> std::list<AstarteStoredProperty> {
  TODO("not yet implemented");
}
auto AstarteDeviceMQTT::AstarteDeviceMQTTImpl::get_property(std::string_view interface_name,
                                                            std::string_view path)
    -> AstartePropertyIndividual {
  TODO("not yet implemented");
}

}  // namespace AstarteDeviceSdk
