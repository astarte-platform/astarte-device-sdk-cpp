// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "mqtt/interface.hpp"

#include <spdlog/spdlog.h>

#include <astarte_device_sdk/errors.hpp>
#include <astarte_device_sdk/ownership.hpp>
#include <astarte_device_sdk/type.hpp>
#include <cstdint>
#include <format>
#include <limits>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace {
// helper function to pop the next segment off the front of the view and advances the view.
// Equivalent to finding the next '/' and moving the pointer past it.
auto pop_next_segment(std::string_view& str) -> std::string_view {
  auto pos = str.find('/');
  auto segment = str.substr(0, pos);

  if (pos == std::string_view::npos) {
    // no slash found: this is the last segment. Consume the whole string.
    str = {};
  } else {
    // slash found: Move view past the slash.
    str.remove_prefix(pos + 1);
  }
  return segment;
}

// helper function to check if the specific segment matches (handles logic for %{params})
auto is_segment_match(std::string_view pattern, std::string_view path_seg) -> bool {
  // check for parameter format to start with "%{" and end with "}"
  if (pattern.size() >= 3 && pattern.starts_with("%{") && pattern.ends_with("}")) {
    // path segment cannot be empty
    if (path_seg.empty()) {
      return false;
    }

    // path segment cannot contain forbidden chars
    return path_seg.find_first_of("#+") == std::string_view::npos;
  }

  return pattern == path_seg;
}

}  // namespace

namespace AstarteDeviceSdk {

namespace {

// helper function to safely get a reference to a JSON field
auto get_field(const json& interface, std::string_view key, json::value_t expected_type)
    -> astarte_tl::expected<json, AstarteError> {
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

// helper to map C++ types to JSON types
template <typename T>
constexpr auto get_json_type() -> json::value_t {
  if constexpr (std::is_same_v<T, std::string> || std::is_enum_v<T>) {
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
inline auto optional_value_from_json_interface(const nlohmann::json& interface,
                                               std::string_view key) -> std::optional<T> {
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

}  // namespace

auto interface_type_from_str(std::string typ) -> astarte_tl::expected<InterfaceType, AstarteError> {
  if (typ == "datastream") {
    return InterfaceType::kDatastream;
  }
  if (typ == "properties") {
    return InterfaceType::kProperty;
  }

  return astarte_tl::unexpected(
      AstarteInvalidInterfaceTypeError(astarte_fmt::format("interface type not valid: {}", typ)));
}

auto aggregation_from_str(std::string aggr)
    -> astarte_tl::expected<InterfaceAggregation, AstarteError> {
  if (aggr == "individual") {
    return InterfaceAggregation::kIndividual;
  }
  if (aggr == "object") {
    return InterfaceAggregation::kObject;
  }

  return astarte_tl::unexpected(AstarteInvalidInterfaceAggregationError(
      astarte_fmt::format("interface aggregation not valid: {}", aggr)));
}

auto mappings_from_interface_json(const json& interface)
    -> astarte_tl::expected<std::vector<Mapping>, AstarteError> {
  auto mappings_field = get_field(interface, "mappings", json::value_t::array);
  if (!mappings_field) {
    return astarte_tl::unexpected(mappings_field.error());
  }

  const auto& mappings_json = mappings_field.value();
  std::vector<Mapping> mappings;
  // reserve memory to avoid reallocations
  mappings.reserve(mappings_json.size());

  for (const auto& mapping : mappings_json) {
    // ensure each element in the array is actually an object
    if (!mapping.is_object()) {
      return astarte_tl::unexpected(
          AstarteInterfaceValidationError("Each element in 'mappings' must be an object"));
    }

    // extract required endpoint (string)
    auto endpoint_json = get_field(mapping, "endpoint", json::value_t::string);
    if (!endpoint_json) {
      return astarte_tl::unexpected(endpoint_json.error());
    }
    auto endpoint = endpoint_json.value().get<std::string>();

    // extract required type (string)
    auto type_json = get_field(mapping, "type", json::value_t::string);
    if (!type_json) {
      return astarte_tl::unexpected(type_json.error());
    }
    auto type_res = astarte_type_from_str(type_json.value().get<std::string>());
    if (!type_res) {
      return astarte_tl::unexpected(type_res.error());
    }
    auto type = type_res.value();

    // extract optional fields
    auto explicit_timestamp =
        optional_value_from_json_interface<bool>(mapping, "explicit_timestamp");

    auto reliability_opt = optional_value_from_json_interface<Reliability>(mapping, "reliability");
    auto reliability = std::optional(reliability_opt.value_or(Reliability::kUnreliable));

    auto retention = optional_value_from_json_interface<Retention>(mapping, "retention");
    auto expiry = optional_value_from_json_interface<int64_t>(mapping, "expiry");
    auto database_retention_policy = optional_value_from_json_interface<DatabaseRetentionPolicy>(
        mapping, "database_retention_policy");
    auto database_retention_ttl =
        optional_value_from_json_interface<int64_t>(mapping, "database_retention_ttl");
    auto allow_unset = optional_value_from_json_interface<bool>(mapping, "allow_unset");
    auto description = optional_value_from_json_interface<std::string>(mapping, "description");
    auto doc = optional_value_from_json_interface<std::string>(mapping, "doc");

    mappings.emplace_back(std::move(endpoint), type, explicit_timestamp, reliability, retention,
                          expiry, database_retention_policy, database_retention_ttl, allow_unset,
                          description, doc);
  }

  return mappings;
}

auto convert_version(std::string_view version_type, int64_t version)
    -> astarte_tl::expected<uint32_t, AstarteError> {
  if (std::cmp_less(version, 0)) {
    return astarte_tl::unexpected(AstarteInvalidInterfaceVersionError(
        astarte_fmt::format("received negative {} version value: {}", version_type, version)));
  }

  if (std::cmp_greater(version, std::numeric_limits<uint32_t>::max())) {
    return astarte_tl::unexpected(AstarteInvalidInterfaceVersionError(
        astarte_fmt::format("{} version value too large: {}", version_type, version)));
  }

  return static_cast<uint32_t>(version);
}

// Helpers for Interface::try_from_json to reduce function size/complexity
namespace {

constexpr std::string_view kType = "type";
constexpr std::string_view kOwnership = "ownership";
constexpr std::string_view kAggregation = "aggregation";

auto parse_version_field(const json& interface, std::string_view key, std::string_view name)
    -> astarte_tl::expected<uint32_t, AstarteError> {
  auto field_json = get_field(interface, key, json::value_t::number_integer);
  if (!field_json) {
    return astarte_tl::unexpected(field_json.error());
  }
  return convert_version(name, field_json.value().get<int64_t>());
}

auto parse_interface_type(const json& interface)
    -> astarte_tl::expected<InterfaceType, AstarteError> {
  auto type_json = get_field(interface, kType, json::value_t::string);
  if (!type_json) {
    return astarte_tl::unexpected(type_json.error());
  }
  return interface_type_from_str(type_json.value().get<std::string>());
}

auto parse_ownership(const json& interface)
    -> astarte_tl::expected<AstarteOwnership, AstarteError> {
  auto own_json = get_field(interface, kOwnership, json::value_t::string);
  if (!own_json) {
    return astarte_tl::unexpected(own_json.error());
  }
  return ownership_from_str(own_json.value().get<std::string>());
}

auto parse_aggregation(const json& interface)
    -> astarte_tl::expected<std::optional<InterfaceAggregation>, AstarteError> {
  if (!interface.contains(kAggregation)) {
    return std::nullopt;
  }

  const auto& agg_val = interface.at(kAggregation);
  if (!agg_val.is_string()) {
    return astarte_tl::unexpected(AstarteInterfaceValidationError("aggregation must be a string"));
  }

  auto res = aggregation_from_str(agg_val.get<std::string>());
  if (!res) {
    return astarte_tl::unexpected(res.error());
  }

  return std::optional<InterfaceAggregation>(res.value());
}

}  // namespace

auto Mapping::match_path(std::string_view path) const -> bool {
  // copy the endpoint
  std::string_view endpoint = endpoint_;

  // check lengths and trailing slash
  if (path.length() < 2 || path.back() == '/') {
    return false;
  }
  // check leading slash consistency
  if (endpoint.empty() || path.front() != endpoint.front()) {
    return false;
  }

  // remove the leading slash to prepare for segment iteration
  // (we know both start with same char, usually '/')
  endpoint.remove_prefix(1);
  path.remove_prefix(1);

  while (!endpoint.empty() && !path.empty()) {
    const std::string_view endpoint_seg = pop_next_segment(endpoint);
    const std::string_view path_seg = pop_next_segment(path);

    if (!is_segment_match(endpoint_seg, path_seg)) {
      return false;
    }
  }

  // both strings must be fully consumed. If one has leftovers there is a length mismatch.
  return endpoint.empty() && path.empty();
}

auto Mapping::check_data_type(const AstarteData& data) const
    -> astarte_tl::expected<void, AstarteError> {
  if (type_ != data.get_type()) {
    spdlog::error("Astarte data type and mapping type do not match");
    return astarte_tl::unexpected(
        AstarteInterfaceValidationError("Astarte data type and mapping type do not match"));
  }

  if ((type_ == AstarteType::kDouble) && (!std::isfinite(data.into<double>()))) {
    spdlog::error("Astarte data double is not a number");
    return astarte_tl::unexpected(
        AstarteInterfaceValidationError("Astarte data double is not a number"));
  }

  if (type_ == AstarteType::kDoubleArray) {
    for (const double value : data.into<std::vector<double>>()) {
      if (!std::isfinite(value)) {
        spdlog::error("Astarte data double is not a number");
        return astarte_tl::unexpected(
            AstarteInterfaceValidationError("Astarte data double is not a number"));
      }
    }
  }

  return {};
}

auto Interface::try_from_json(const json& interface)
    -> astarte_tl::expected<Interface, AstarteError> {
  auto name_json = get_field(interface, "interface_name", json::value_t::string);
  if (!name_json) {
    return astarte_tl::unexpected(name_json.error());
  }
  const auto interface_name = name_json.value().get<std::string>();

  auto maj_res = parse_version_field(interface, "version_major", "major");
  if (!maj_res) {
    return astarte_tl::unexpected(maj_res.error());
  }

  auto min_res = parse_version_field(interface, "version_minor", "minor");
  if (!min_res) {
    return astarte_tl::unexpected(min_res.error());
  }

  auto type_res = parse_interface_type(interface);
  if (!type_res) {
    return astarte_tl::unexpected(type_res.error());
  }

  auto own_res = parse_ownership(interface);
  if (!own_res) {
    return astarte_tl::unexpected(own_res.error());
  }

  auto agg_res = parse_aggregation(interface);
  if (!agg_res) {
    return astarte_tl::unexpected(agg_res.error());
  }

  auto description = optional_value_from_json_interface<std::string>(interface, "description");
  auto doc = optional_value_from_json_interface<std::string>(interface, "doc");

  auto mappings_res = mappings_from_interface_json(interface);
  if (!mappings_res) {
    return astarte_tl::unexpected(mappings_res.error());
  }
  auto mappings = mappings_res.value();

  if (mappings.empty()) {
    return astarte_tl::unexpected(
        AstarteInterfaceValidationError("There must be at least one mapping"));
  }

  return Interface(interface_name, maj_res.value(), min_res.value(), type_res.value(),
                   own_res.value(), agg_res.value(), description, doc, mappings);
}

auto Interface::get_mapping(std::string_view path) const
    -> astarte_tl::expected<const Mapping*, AstarteError> {
  for (const auto& mapping : mappings_) {
    if (mapping.match_path(path)) {
      return &mapping;
    }
  }

  return astarte_tl::unexpected(AstarteInterfaceValidationError(
      astarte_fmt::format("couldn't find mapping with path {}", path)));
}

auto Interface::validate_individual(std::string_view path, const AstarteData& data,
                                    const std::chrono::system_clock::time_point* timestamp) const
    -> astarte_tl::expected<void, AstarteError> {
  auto mapping_res = get_mapping(path);
  if (!mapping_res) {
    return astarte_tl::unexpected(mapping_res.error());
  }
  const Mapping* mapping = mapping_res.value();

  auto res = mapping->check_data_type(data);
  if (!res) {
    return astarte_tl::unexpected(res.error());
  }

  auto explicit_ts = mapping->explicit_timestamp();
  if ((explicit_ts.has_value() && explicit_ts.value()) && timestamp == nullptr) {
    spdlog::error("Explicit timestamp required for interface {}, path {}", interface_name_, path);
    return astarte_tl::unexpected(AstarteInterfaceValidationError(astarte_fmt::format(
        "Explicit timestamp required for interface {}, path {}", interface_name_, path)));
  }

  if ((explicit_ts.has_value() && !explicit_ts.value()) && timestamp != nullptr) {
    spdlog::error("Explicit timestamp not supported for interface {}, path {}", interface_name_,
                  path);
    return astarte_tl::unexpected(AstarteInterfaceValidationError(astarte_fmt::format(
        "Explicit timestamp not supported for interface {}, path {}", interface_name_, path)));
  }

  return {};
}

auto Interface::validate_object(std::string_view common_path, const AstarteDatastreamObject& object,
                                const std::chrono::system_clock::time_point* timestamp) const
    -> astarte_tl::expected<void, AstarteError> {
  for (const auto& [endpoint_path, data] : object) {
    auto path = astarte_fmt::format("{}/{}", common_path, endpoint_path);
    auto res = this->validate_individual(path, data, timestamp);
    if (!res) {
      return astarte_tl::unexpected(res.error());
    }
  }

  return {};
}

auto Interface::get_qos(std::string_view path) const
    -> astarte_tl::expected<uint8_t, AstarteError> {
  auto mapping_exp = [&]() -> astarte_tl::expected<const Mapping*, AstarteError> {
    if (!aggregation_.has_value() || (aggregation_.value() == InterfaceAggregation::kIndividual)) {
      auto mapping_res = get_mapping(path);
      if (!mapping_res) {
        return astarte_tl::unexpected(mapping_res.error());
      }
      return mapping_res.value();
    }

    // object InterfaceAggregation (return the first mapping)
    if (mappings_.empty()) {
      return astarte_tl::unexpected(AstarteMqttError("Interface has no mappings"));
    }
    return &mappings_.at(0);
  }();

  if (!mapping_exp) {
    return astarte_tl::unexpected(mapping_exp.error());
  }

  const Mapping* map_ptr = mapping_exp.value();

  auto reliability = map_ptr->reliability();
  if (!reliability) {
    return astarte_tl::unexpected(
        AstarteMqttError("the interface mapping doesn't contain the qos value"));
  }

  return static_cast<int8_t>(*reliability);
}

}  // namespace AstarteDeviceSdk
