// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/mqtt/device_mqtt.hpp"

#include <chrono>
#include <filesystem>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/errors.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/stored_property.hpp"
#include "mqtt/device_mqtt_impl.hpp"

namespace AstarteDeviceSdk {

auto AstarteDeviceMqtt::create(MqttConfig cfg)
    -> astarte_tl::expected<AstarteDeviceMqtt, AstarteError> {
  auto impl_result = AstarteDeviceMqttImpl::create(cfg);
  if (!impl_result) {
    return astarte_tl::unexpected(impl_result.error());
  }

  std::shared_ptr<AstarteDeviceMqttImpl> impl_ptr = std::move(impl_result.value());

  return AstarteDeviceMqtt(std::move(impl_ptr));
}

AstarteDeviceMqtt::AstarteDeviceMqtt(std::shared_ptr<AstarteDeviceMqttImpl> impl)
    : astarte_device_impl_{std::move(impl)} {}

AstarteDeviceMqtt::~AstarteDeviceMqtt() = default;

auto AstarteDeviceMqtt::add_interface_from_file(const std::filesystem::path& json_file)
    -> astarte_tl::expected<void, AstarteError> {
  return astarte_device_impl_->add_interface_from_file(json_file);
}

auto AstarteDeviceMqtt::add_interface_from_str(std::string_view json)
    -> astarte_tl::expected<void, AstarteError> {
  return astarte_device_impl_->add_interface_from_str(json);
}

auto AstarteDeviceMqtt::remove_interface(const std::string& interface_name)
    -> astarte_tl::expected<void, AstarteError> {
  return astarte_device_impl_->remove_interface(interface_name);
}

auto AstarteDeviceMqtt::connect() -> astarte_tl::expected<void, AstarteError> {
  return astarte_device_impl_->connect();
}

auto AstarteDeviceMqtt::is_connected() const -> bool {
  return astarte_device_impl_->is_connected();
}

auto AstarteDeviceMqtt::disconnect() -> astarte_tl::expected<void, AstarteError> {
  return astarte_device_impl_->disconnect();
}

auto AstarteDeviceMqtt::send_individual(std::string_view interface_name, std::string_view path,
                                        const AstarteData& data,
                                        const std::chrono::system_clock::time_point* timestamp)
    -> astarte_tl::expected<void, AstarteError> {
  return astarte_device_impl_->send_individual(interface_name, path, data, timestamp);
}

auto AstarteDeviceMqtt::send_object(std::string_view interface_name, std::string_view path,
                                    const AstarteDatastreamObject& object,
                                    const std::chrono::system_clock::time_point* timestamp)
    -> astarte_tl::expected<void, AstarteError> {
  return astarte_device_impl_->send_object(interface_name, path, object, timestamp);
}

auto AstarteDeviceMqtt::set_property(std::string_view interface_name, std::string_view path,
                                     const AstarteData& data)
    -> astarte_tl::expected<void, AstarteError> {
  return astarte_device_impl_->set_property(interface_name, path, data);
}

auto AstarteDeviceMqtt::unset_property(std::string_view interface_name, std::string_view path)
    -> astarte_tl::expected<void, AstarteError> {
  return astarte_device_impl_->unset_property(interface_name, path);
}

auto AstarteDeviceMqtt::poll_incoming(const std::chrono::milliseconds& timeout)
    -> std::optional<AstarteMessage> {
  return astarte_device_impl_->poll_incoming(timeout);
}

auto AstarteDeviceMqtt::get_all_properties(const std::optional<AstarteOwnership>& ownership)
    -> astarte_tl::expected<std::list<AstarteStoredProperty>, AstarteError> {
  return astarte_device_impl_->get_all_properties(ownership);
}

auto AstarteDeviceMqtt::get_properties(std::string_view interface_name)
    -> astarte_tl::expected<std::list<AstarteStoredProperty>, AstarteError> {
  return astarte_device_impl_->get_properties(interface_name);
}

auto AstarteDeviceMqtt::get_property(std::string_view interface_name, std::string_view path)
    -> astarte_tl::expected<AstartePropertyIndividual, AstarteError> {
  return astarte_device_impl_->get_property(interface_name, path);
}

}  // namespace AstarteDeviceSdk
