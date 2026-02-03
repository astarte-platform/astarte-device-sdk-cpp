// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

// macro to mark unimplemented code. Throws an exception with file/line info.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TODO(msg)                                                                \
  throw std::logic_error("TODO: " + std::string(msg) + " at " + __FILE__ + ":" + \
                         std::to_string(__LINE__));

#include "mqtt/device_mqtt_impl.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
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
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/stored_property.hpp"
#include "mqtt/connection/connection.hpp"
#include "mqtt/introspection.hpp"
#include "mqtt/serialize.hpp"

namespace astarte::device::mqtt {

using namespace std::chrono_literals;

using json = nlohmann::json;

auto DeviceMqtt::DeviceMqttImpl::create(mqtt::Config& cfg)
    -> astarte_tl::expected<std::shared_ptr<DeviceMqttImpl>, Error> {
  auto conn = connection::Connection::create(cfg);
  if (!conn) {
    spdlog::error("failed to create a MQTT connection. Error: {}", conn.error());
    return astarte_tl::unexpected(conn.error());
  }

  return std::shared_ptr<DeviceMqttImpl>(
      new DeviceMqttImpl(std::move(cfg), std::move(conn.value())));
}

DeviceMqtt::DeviceMqttImpl::DeviceMqttImpl(mqtt::Config cfg, connection::Connection connection)
    : cfg_(std::move(cfg)), connection_(std::move(connection)) {}

DeviceMqtt::DeviceMqttImpl::~DeviceMqttImpl() = default;

auto DeviceMqtt::DeviceMqttImpl::add_interface_from_file(const std::filesystem::path& json_file)
    -> astarte_tl::expected<void, Error> {
  spdlog::trace("Adding interface from file: {}", json_file.string());

  std::ifstream interface_file(json_file, std::ios::in);
  if (!interface_file.is_open()) {
    spdlog::error("Could not open the interface file: {}", json_file.string());
    return astarte_tl::unexpected(FileOpenError(json_file.string()));
  }

  // Read the entire JSON file content into a string
  const std::string interface_str((std::istreambuf_iterator<char>(interface_file)),
                                  std::istreambuf_iterator<char>());

  // Close the file
  interface_file.close();

  // Add the interface from the fetched string
  return add_interface_from_str(interface_str);
}

auto DeviceMqtt::DeviceMqttImpl::add_interface_from_str(std::string_view interface_str)
    -> astarte_tl::expected<void, Error> {
  spdlog::trace("Adding interface {} from string", interface_str);

  if (is_connected()) {
    // TODO(rgallor): If the device is connected, communicate the new introspection to Astarte
  }

  json interface_json;
  try {
    interface_json = json::parse(interface_str);
  } catch (json::parse_error& e) {
    spdlog::error("failed to parse JSON Astarte interface: {}", e.what());
    return astarte_tl::unexpected(
        JsonParsingError(astarte_fmt::format("failed to parse interface from json: {}", e.what())));
  }

  auto interface = Interface::try_from_json(interface_json);
  if (!interface) {
    return astarte_tl::unexpected(interface.error());
  }

  return introspection_->checked_insert(std::move(interface.value()));
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto DeviceMqtt::DeviceMqttImpl::remove_interface(const std::string& /* interface_name */)
    -> astarte_tl::expected<void, Error> {
  TODO("not yet implemented");
}

auto DeviceMqtt::DeviceMqttImpl::connect() -> astarte_tl::expected<void, Error> {
  return connection_.connect(introspection_);
}

[[nodiscard]] auto DeviceMqtt::DeviceMqttImpl::is_connected() const -> bool {
  return connection_.is_connected();
}

auto DeviceMqtt::DeviceMqttImpl::disconnect() -> astarte_tl::expected<void, Error> {
  if (!is_connected()) {
    spdlog::debug("device already disconnected");
    return {};
  }
  return connection_.disconnect();
}

auto DeviceMqtt::DeviceMqttImpl::send_individual(
    std::string_view interface_name, std::string_view path, const Data& data,
    const std::chrono::system_clock::time_point* timestamp) -> astarte_tl::expected<void, Error> {
  if (!connection_.is_connected()) {
    spdlog::error("couldn't send data since the device is not connected");
    return astarte_tl::unexpected(
        MqttError("couldn't send data since the device is not connected"));
  }

  // check if the interface exists in the device introspection
  auto interface_res = introspection_->get(std::string(interface_name));
  if (!interface_res) {
    auto msg = astarte_fmt::format(
        "couldn't send data since the interface {} not found in introspection", interface_name);
    spdlog::error(msg);
    return astarte_tl::unexpected(MqttError(msg));
  }

  auto interface = interface_res.value();

  // validate data
  auto res = interface->validate_individual(path, data, timestamp);
  if (!res) {
    return astarte_tl::unexpected(res.error());
  }

  // get qos
  auto qos_res = interface->get_qos(path);
  if (!qos_res) {
    return astarte_tl::unexpected(qos_res.error());
  }
  auto qos = qos_res.value();

  // serialize data to bson ({"v": <data>})
  // if timestamp is set add it ({"v": <data>, "t": <timestamp>})
  json bson;
  bson::serialize_astarte_individual(bson, "v", data, timestamp);

  // check that the generated bson is not 0 size
  if (bson.empty()) {
    return astarte_tl::unexpected(
        DataSerializationError("Failed to serialize individual data to BSON"));
  }

  spdlog::trace("dump individual: {}", bson.dump());

  // convert BSON to bytes
  std::vector<uint8_t> bson_bytes = json::to_bson(bson);

  return connection_.send(interface_name, path, qos, bson_bytes);
}

auto DeviceMqtt::DeviceMqttImpl::send_object(std::string_view interface_name, std::string_view path,
                                             const DatastreamObject& object,
                                             const std::chrono::system_clock::time_point* timestamp)
    -> astarte_tl::expected<void, Error> {
  if (!connection_.is_connected()) {
    spdlog::error("couldn't send data since the device is not connected");
    return astarte_tl::unexpected(
        MqttError("couldn't send data since the device is not connected"));
  }

  // check if the interface exists in the device introspection
  auto interface_res = introspection_->get(std::string(interface_name));
  if (!interface_res) {
    auto msg = astarte_fmt::format(
        "couldn't send data since the interface {} not found in introspection", interface_name);
    spdlog::error(msg);
    return astarte_tl::unexpected(MqttError(msg));
  }
  auto interface = interface_res.value();

  if (interface->mappings().size() != object.size()) {
    spdlog::error("incomplete aggregated datastream");
    return astarte_tl::unexpected(InterfaceValidationError(astarte_fmt::format(
        "incomplete aggregated datastream: the interface contains {} mappings, provided {}",
        interface->mappings().size(), object.size())));
  }

  // validate data
  auto validate_res = interface->validate_object(path, object, timestamp);
  if (!validate_res) {
    return validate_res;
  }

  // get qos
  auto qos_res = interface->get_qos(path);
  if (!qos_res) {
    return astarte_tl::unexpected(qos_res.error());
  }
  auto qos = qos_res.value();

  // serialize data to bson ({"v": {<path1>: <data1>, <path2>: <data2>, ...}})
  // if timestamp is set add it ({"v": {<path1>: <data1>, <path2>: <data2>, ...}, "t": <timestamp>})
  json bson;
  bson::serialize_astarte_object(bson, object, timestamp);

  // check that the generated bson is not 0 size
  if (bson.empty()) {
    return astarte_tl::unexpected(
        DataSerializationError("Failed to serialize object data to BSON"));
  }

  spdlog::trace("dump object: {}", bson.dump());

  // convert BSON to bytes
  std::vector<uint8_t> bson_bytes = json::to_bson(bson);

  return connection_.send(interface_name, path, qos, bson_bytes);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto DeviceMqtt::DeviceMqttImpl::set_property(std::string_view /* interface_name */,
                                              std::string_view /* path */, const Data& /* data */)
    -> astarte_tl::expected<void, Error> {
  TODO("not yet implemented");
}
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto DeviceMqtt::DeviceMqttImpl::unset_property(std::string_view /* interface_name */,
                                                std::string_view /* path */)
    -> astarte_tl::expected<void, Error> {
  TODO("not yet implemented");
}
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto DeviceMqtt::DeviceMqttImpl::poll_incoming(const std::chrono::milliseconds& /* timeout */)
    -> std::optional<Message> {
  // TODO(rgallor): change which correct implementation. the actual one is used only to make e2e
  // tests work.
  return std::nullopt;
}
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto DeviceMqtt::DeviceMqttImpl::get_all_properties(const std::optional<Ownership>& /* ownership */)
    -> astarte_tl::expected<std::list<StoredProperty>, Error> {
  TODO("not yet implemented");
}
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto DeviceMqtt::DeviceMqttImpl::get_properties(std::string_view /* interface_name */)
    -> astarte_tl::expected<std::list<StoredProperty>, Error> {
  TODO("not yet implemented");
}
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto DeviceMqtt::DeviceMqttImpl::get_property(std::string_view /* interface_name */,
                                              std::string_view /* path */)
    -> astarte_tl::expected<PropertyIndividual, Error> {
  TODO("not yet implemented");
}

}  // namespace astarte::device::mqtt
