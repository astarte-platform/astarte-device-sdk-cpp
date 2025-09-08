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
#include "astarte_device_sdk/device_mqtt.hpp"
#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/stored_property.hpp"

namespace AstarteDeviceSdk {

AstarteDeviceMQTT::AstarteDeviceMQTTImpl::AstarteDeviceMQTTImpl(const MqttConfig cfg)
    : cfg_(std::move(cfg)) {}

AstarteDeviceMQTT::AstarteDeviceMQTTImpl::~AstarteDeviceMQTTImpl() = default;

void AstarteDeviceMQTT::AstarteDeviceMQTTImpl::add_interface_from_file(
    const std::filesystem::path& json_file) {
  TODO("not yet implemented");
}
void AstarteDeviceMQTT::AstarteDeviceMQTTImpl::add_interface_from_str(std::string_view json) {
  TODO("not yet implemented");
}
void AstarteDeviceMQTT::AstarteDeviceMQTTImpl::remove_interface(const std::string& interface_name) {
  TODO("not yet implemented");
}

void AstarteDeviceMQTT::AstarteDeviceMQTTImpl::connect() {
  // FIXME: at the moment this method just serves to retrieve the credential secret from Astarte,
  // so that the device appears to be registered to astarte. It does not establishing an effective
  // connection to Astarte.
  auto pairing_api = PairingApi(cfg_.realm(), cfg_.device_id(), cfg_.pairing_url());
  pairing_api.register_device(cfg_.pairing_token().value());
  connected_ = true;
}

[[nodiscard]] auto AstarteDeviceMQTT::AstarteDeviceMQTTImpl::is_connected() const -> bool {
  return connected_.load();
}

void AstarteDeviceMQTT::AstarteDeviceMQTTImpl::disconnect() { connected_ = false; }

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
  return std::nullopt;
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
