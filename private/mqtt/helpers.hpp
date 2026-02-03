// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_HELPERS_H
#define ASTARTE_MQTT_HELPERS_H

#include <nlohmann/json.hpp>
#include <optional>
#include <string_view>
#include <type_traits>

#include "astarte_device_sdk/errors.hpp"
#include "mqtt/mapping.hpp"

namespace AstarteDeviceSdk::json_helper {

using json = nlohmann::json;

/**
 * @brief Safely get a reference to a JSON field with type validation.
 * @param interface The JSON object to search within.
 * @param key The key of the field to retrieve.
 * @param expected_type The expected JSON type for validation.
 * @return An expected containing the JSON field reference on success, or an AstarteError
 * if the field is missing or has the wrong type.
 */
inline auto get_field(const json& interface, std::string_view key, json::value_t expected_type)
    -> astarte_tl::expected<json, AstarteDeviceSdk::AstarteError> {
  auto field = interface.find(key);
  if (field == interface.end()) {
    return astarte_tl::unexpected(
        AstarteInterfaceValidationError(astarte_fmt::format("Missing required field: {}", key)));
  }

  // if we expect a signed integer, also accept an unsigned one (since the json library we use may
  // automatically convert in a wrond interger type).
  bool type_match = (field->type() == expected_type);
  if (expected_type == json::value_t::number_integer &&
      field->type() == json::value_t::number_unsigned) {
    type_match = true;
  }

  if (!type_match) {
    return astarte_tl::unexpected(
        AstarteInterfaceValidationError(astarte_fmt::format("Field {} has invalid type", key)));
  }
  return *field;
};

/**
 * @brief Helper to map C++ types to their corresponding Astarte JSON types.
 *
 * This compile-time helper ensures that the type validation in optional_value_from_json
 * correctly identifies strings, integers, booleans, and floats.
 *
 * @tparam T The C++ type to map.
 * @return The corresponding nlohmann::json::value_t.
 */
template <typename T>
constexpr auto get_json_type() -> json::value_t {
  if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, Reliability> ||
                std::is_same_v<T, Retention> || std::is_same_v<T, DatabaseRetentionPolicy>) {
    return json::value_t::string;
  }
  if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
    return json::value_t::number_integer;
  }
  if constexpr (std::is_same_v<T, bool>) {
    return json::value_t::boolean;
  }
  if constexpr (std::is_floating_point_v<T>) {
    return json::value_t::number_float;
  }
  // default fallback
  return json::value_t::discarded;
}

/**
 * @brief Extract an optional value from a JSON object.
 *
 * @tparam T The type of the value to extract.
 * @param interface The JSON object to parse.
 * @param key The key of the value to extract.
 * @return An std::optional<T> containing the value if the key exists, or std::nullopt otherwise.
 */
template <typename T>
auto optional_value_from_json(const nlohmann::json& interface, std::string_view key)
    -> std::optional<T> {
  auto field = interface.find(key);

  // check existsence
  if (field == interface.end() || field->is_null()) {
    return std::nullopt;
  }

  // validate type
  auto expected = get_json_type<T>();
  bool type_match = (field->type() == expected);

  // handle the signed/unsigned integer case (same as get_field)
  if (expected == json::value_t::number_integer &&
      field->type() == json::value_t::number_unsigned) {
    type_match = true;
  }

  // if type is wrong, we return nullopt
  if (!type_match) {
    return std::nullopt;
  }

  try {
    return std::optional<T>(field->get<T>());
  } catch (const nlohmann::json::exception&) {
    return std::nullopt;
  }
}

}  // namespace AstarteDeviceSdk::json_helper

#endif  // ASTARTE_MQTT_HELPERS_H
