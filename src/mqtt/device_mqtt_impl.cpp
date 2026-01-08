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
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/property.hpp"
#include "astarte_device_sdk/stored_property.hpp"
#include "mqtt/connection.hpp"
#include "mqtt/introspection.hpp"

namespace AstarteDeviceSdk {

using json = nlohmann::json;  // NOLINT(misc-include-cleaner)

namespace bson {

/**
 * @brief Helper to patch BSON type tags.
 * nlohmann/json serializes integers as Int64 (0x12).
 * This patches the type byte to Date (0x09) if required.
 */
void patch_bson_tag(std::vector<uint8_t>& bson_data, const std::string& key_name,
                    uint8_t target_type) {
  // skip document size
  size_t i = 4;
  while (i < bson_data.size() - 1) {
    uint8_t type = bson_data[i];
    if (type == 0x00) break;
    // skip type byte
    i++;

    // read Key
    std::string current_key;
    while (i < bson_data.size() && bson_data[i] != 0x00) {
      current_key += (char)bson_data[i];
      i++;
    }
    // skip null terminator
    i++;

    // check if this is the key we want to patch
    if (current_key == key_name) {
      // calculate position of the type byte we skipped
      // pos = current (i) - null(1) - keylen - typebyte(1)
      size_t type_offset = i - 1 - current_key.size() - 1;

      // if it is currently Int64 (0x12) and we want Date (0x09), patch it.
      if (bson_data[type_offset] == 0x12 && target_type == 0x09) {
        bson_data[type_offset] = target_type;
      }
      return;
    }

    // generic Skipper (Simplified for flat {"t":..., "v":...} structure)
    // 64-bit
    if (type == 0x12 || type == 0x09 || type == 0x01) i += 8;
    // 32-bit
    else if (type == 0x10)
      i += 4;
    // bool
    else if (type == 0x08)
      i += 1;
    // binary or string
    else if (type == 0x05 || type == 0x02) {
      int32_t len;
      std::memcpy(&len, &bson_data[i], 4);  // read length
      i += 4 + len;
      if (type == 0x05) i += 1;  // binary has extra subtype byte
    }
  }
}

/**
 * @brief Serialize AstarteData to BSON bytes.
 */
auto serialize_astarte(const AstarteData& data,
                       const std::chrono::system_clock::time_point* timestamp)
    -> std::vector<uint8_t> {
  json j;

  bool needs_date_patch = false;

  // access the raw variant from your class
  std::visit(
      [&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;

        // handle Binary Blob
        if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
          j["v"] = json::binary(arg, 0x00);
        }
        // handle DateTime (std::chrono::system_clock::time_point)
        else if constexpr (std::is_same_v<T, std::chrono::system_clock::time_point>) {
          // Convert to milliseconds
          auto millis =
              std::chrono::duration_cast<std::chrono::milliseconds>(arg.time_since_epoch()).count();

          j["v"] = millis;
          needs_date_patch = true;  // Flag for post-processing
        }
        // handle Array of Binaries
        else if constexpr (std::is_same_v<T, std::vector<std::vector<uint8_t>>>) {
          std::vector<json> binary_array;
          for (const auto& bin : arg) {
            binary_array.push_back(json::binary(bin, 0x00));
          }
          j["v"] = binary_array;
        }
        // handle Array of DateTimes
        else if constexpr (std::is_same_v<T, std::vector<std::chrono::system_clock::time_point>>) {
          std::vector<int64_t> dates;
          for (const auto& tp : arg) {
            dates.push_back(
                std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch())
                    .count());
          }
          j["v"] = dates;
        }
        // default handler (int, double, string, bool, and the respective vectors)
        else {
          j["v"] = arg;
        }
      },
      data.get_raw_data());

  if (timestamp) {
    j["t"] = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp->time_since_epoch())
                 .count();
  }

  // convert to BSON
  std::vector<uint8_t> bson_bytes = json::to_bson(j);

  // apply patch if needed (convert Int64 tag to Date tag)
  if (needs_date_patch) {
    patch_bson_tag(bson_bytes, "v", 0x09);
  }

  return bson_bytes;
}

}  // namespace bson

auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::create(config::MqttConfig& cfg)
    -> astarte_tl::expected<std::shared_ptr<AstarteDeviceMqttImpl>, AstarteError> {
  auto conn = MqttConnection::create(cfg);
  if (!conn) {
    spdlog::error("failed to create a MQTT connection. Error: {}", conn.error());
    return astarte_tl::unexpected(conn.error());
  }

  return std::shared_ptr<AstarteDeviceMqttImpl>(
      new AstarteDeviceMqttImpl(std::move(cfg), std::move(conn.value())));
}

AstarteDeviceMqtt::AstarteDeviceMqttImpl::AstarteDeviceMqttImpl(config::MqttConfig cfg,
                                                                MqttConnection connection)
    : cfg_(std::move(cfg)), connection_(std::move(connection)) {}

AstarteDeviceMqtt::AstarteDeviceMqttImpl::~AstarteDeviceMqttImpl() = default;

auto AstarteDeviceMqtt::AstarteDeviceMqttImpl::add_interface_from_file(
    const std::filesystem::path& json_file) -> astarte_tl::expected<void, AstarteError> {
  spdlog::trace("Adding interface from file: {}", json_file.string());

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
  spdlog::trace("Adding interface {} from string", interface_str);

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
    std::string_view interface_name, std::string_view path, const AstarteData& data,
    const std::chrono::system_clock::time_point* timestamp)
    -> astarte_tl::expected<void, AstarteError> {
  if (!connection_.is_connected()) {
    spdlog::error("couldn't send data since the device is not connected");
    return astarte_tl::unexpected(
        AstarteMqttError("couldn't send data since the device is not connected"));
  }

  // check if the interface exists in the device introspection
  auto interface_res = introspection_.get(std::string(interface_name));
  if (!interface_res) {
    spdlog::error("couldn't send data since the interface {} is not in the device introspection",
                  interface_name);
    return astarte_tl::unexpected(AstarteMqttError(astarte_fmt::format(
        "couldn't send data since the interface {} is not in the device introspection",
        interface_name)));
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
  auto data_bson = serialize_astarte(data, timestamp);

  // check that the generated bson is not 0 size
  if(data_bson.size() == 0){
    return astarte_tl::unexpected(AstarteDataSerializationError("Failed to serialize data to BSON"));
  }

  // TODO: send data

  return {};
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
