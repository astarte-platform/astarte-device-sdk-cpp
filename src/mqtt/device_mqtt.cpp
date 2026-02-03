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

namespace astarte::device::mqtt {

auto DeviceMqtt::create(mqtt::Config cfg) -> astarte_tl::expected<DeviceMqtt, Error> {
  auto impl_result = DeviceMqttImpl::create(cfg);
  if (!impl_result) {
    return astarte_tl::unexpected(impl_result.error());
  }

  std::shared_ptr<DeviceMqttImpl> impl_ptr = std::move(impl_result.value());

  return DeviceMqtt(std::move(impl_ptr));
}

DeviceMqtt::DeviceMqtt(std::shared_ptr<DeviceMqttImpl> impl)
    : astarte_device_impl_{std::move(impl)} {}

DeviceMqtt::~DeviceMqtt() = default;

auto DeviceMqtt::add_interface_from_file(const std::filesystem::path& json_file)
    -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->add_interface_from_file(json_file);
}

auto DeviceMqtt::add_interface_from_str(std::string_view json)
    -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->add_interface_from_str(json);
}

auto DeviceMqtt::remove_interface(const std::string& interface_name)
    -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->remove_interface(interface_name);
}

auto DeviceMqtt::connect() -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->connect();
}

auto DeviceMqtt::is_connected() const -> bool { return astarte_device_impl_->is_connected(); }

auto DeviceMqtt::disconnect() -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->disconnect();
}

auto DeviceMqtt::send_individual(std::string_view interface_name, std::string_view path,
                                 const Data& data,
                                 const std::chrono::system_clock::time_point* timestamp)
    -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->send_individual(interface_name, path, data, timestamp);
}

auto DeviceMqtt::send_object(std::string_view interface_name, std::string_view path,
                             const DatastreamObject& object,
                             const std::chrono::system_clock::time_point* timestamp)
    -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->send_object(interface_name, path, object, timestamp);
}

auto DeviceMqtt::set_property(std::string_view interface_name, std::string_view path,
                              const Data& data) -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->set_property(interface_name, path, data);
}

auto DeviceMqtt::unset_property(std::string_view interface_name, std::string_view path)
    -> astarte_tl::expected<void, Error> {
  return astarte_device_impl_->unset_property(interface_name, path);
}

auto DeviceMqtt::poll_incoming(const std::chrono::milliseconds& timeout) -> std::optional<Message> {
  return astarte_device_impl_->poll_incoming(timeout);
}

auto DeviceMqtt::get_all_properties(const std::optional<Ownership>& ownership)
    -> astarte_tl::expected<std::list<StoredProperty>, Error> {
  return astarte_device_impl_->get_all_properties(ownership);
}

auto DeviceMqtt::get_properties(std::string_view interface_name)
    -> astarte_tl::expected<std::list<StoredProperty>, Error> {
  return astarte_device_impl_->get_properties(interface_name);
}

auto DeviceMqtt::get_property(std::string_view interface_name, std::string_view path)
    -> astarte_tl::expected<PropertyIndividual, Error> {
  return astarte_device_impl_->get_property(interface_name, path);
}

}  // namespace astarte::device::mqtt
