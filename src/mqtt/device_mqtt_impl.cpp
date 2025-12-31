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
#include "astarte_device_sdk/mqtt/device_mqtt.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/stored_property.hpp"

namespace AstarteDeviceSdk {

AstarteDeviceMqtt::AstarteDeviceMqttImpl::AstarteDeviceMqttImpl(const MqttConfig cfg)
    : cfg_(std::move(cfg)) {}

AstarteDeviceMqtt::AstarteDeviceMqttImpl::~AstarteDeviceMqttImpl() = default;

auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::add_interface_from_file(
    const std::filesystem::path& json_file) -> astarte_tl::expected<void, AstarteError> {
  TODO("not yet implemented");
}
auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::add_interface_from_str(std::string_view json)
    -> astarte_tl::expected<void, AstarteError> {
  TODO("not yet implemented");
}
auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::remove_interface(const std::string& interface_name)
    -> astarte_tl::expected<void, AstarteError> {
  TODO("not yet implemented");
}

auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::connect()
    -> astarte_tl::expected<void, AstarteError> {
  TODO("not yet implemented");
}

[[nodiscard]] auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::is_connected() const -> bool {
  TODO("not yet implemented");
}

auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::disconnect()
    -> astarte_tl::expected<void, AstarteError> {
  TODO("not yet implemented");
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
