// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "mqtt/introspection.hpp"

#include <spdlog/spdlog.h>

#include <astarte_device_sdk/errors.hpp>
#include <astarte_device_sdk/mqtt/formatter.hpp>
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

inline auto aggregation_from_str(std::string aggr)
    -> astarte_tl::expected<Aggregation, AstarteError> {
  if (aggr == "individual") {
    return Aggregation::kIndividual;
  }
  if (aggr == "object") {
    return Aggregation::kObject;
  }

  return astarte_tl::unexpected(AstarteInvalidInterfaceAggregationError(
      astarte_fmt::format("Aggregation not valid: {}", aggr)));
}

auto mappings_from_interface(const json& interface)
    -> astarte_tl::expected<std::vector<Mapping>, AstarteError> {
  if (!interface.contains("mappings")) {
    return astarte_tl::unexpected(
        AstarteInterfaceValidationError("Missing required field: mappings"));
  }

  const auto& mappings_json = interface["mappings"];

  if (!mappings_json.is_array()) {
    return astarte_tl::unexpected(
        AstarteInterfaceValidationError("'mappings' field must be an array"));
  }

  std::vector<Mapping> mappings;

  // reserve memory to avoid reallocations
  mappings.reserve(mappings_json.size());

  for (const auto& mapping : mappings_json) {
    if (!mapping.contains("endpoint")) {
      return astarte_tl::unexpected(
          AstarteInterfaceValidationError("Mapping missing required field: endpoint"));
    }
    auto endpoint = mapping["endpoint"].get<std::string>();

    if (!mapping.contains("type")) {
      return astarte_tl::unexpected(
          AstarteInterfaceValidationError("Mapping missing required field: type"));
    }
    auto type_res = astarte_type_from_str(mapping["type"].get<std::string>());
    if (!type_res) {
      return astarte_tl::unexpected(type_res.error());
    }
    auto type = type_res.value();

    auto explicit_timestamp =
        optional_value_from_json_interface<bool>(mapping, "explicit_timestamp");

    // Handle reliability default
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

    // Construct and move into vector
    mappings.emplace_back(Mapping{.endpoint_ = std::move(endpoint),
                                  .type_ = type,
                                  .explicit_timestamp_ = explicit_timestamp,
                                  .reliability_ = reliability,
                                  .retention_ = retention,
                                  .expiry_ = expiry,
                                  .database_retention_policy_ = database_retention_policy,
                                  .database_retention_ttl_ = database_retention_ttl,
                                  .allow_unset_ = allow_unset,
                                  .description_ = description,
                                  .doc_ = doc});
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

// NOLINTNEXTLINE(readability-function-size)
auto Interface::try_from_json(const json& interface)
    -> astarte_tl::expected<Interface, AstarteError> {
  // helper lambda to safely get a reference to a JSON field
  auto get_field = [&](std::string_view key) -> astarte_tl::expected<json, AstarteError> {
    if (!interface.contains(key)) {
      return astarte_tl::unexpected(
          AstarteInterfaceValidationError(astarte_fmt::format("Missing required field: {}", key)));
    }
    return interface.at(key);
  };

  // retrieve interface fields
  auto name_json = get_field("interface_name");
  if (!name_json) {
    return astarte_tl::unexpected(name_json.error());
  }
  const auto interface_name = name_json.value().get<std::string>();

  auto maj_json = get_field("version_major");
  if (!maj_json) {
    return astarte_tl::unexpected(maj_json.error());
  }

  auto version_major = convert_version("major", maj_json.value().get<int64_t>());
  if (!version_major) {
    return astarte_tl::unexpected(version_major.error());
  }

  auto min_json = get_field("version_minor");
  if (!min_json) {
    return astarte_tl::unexpected(min_json.error());
  }

  auto version_minor = convert_version("minor", min_json.value().get<int64_t>());
  if (!version_minor) {
    return astarte_tl::unexpected(version_minor.error());
  }

  auto type_json = get_field("type");
  if (!type_json) {
    return astarte_tl::unexpected(type_json.error());
  }

  auto interface_type = interface_type_from_str(type_json.value().get<std::string>());
  if (!interface_type) {
    return astarte_tl::unexpected(interface_type.error());
  }

  auto own_json = get_field("ownership");
  if (!own_json) {
    return astarte_tl::unexpected(own_json.error());
  }

  auto ownership = ownership_from_str(own_json.value().get<std::string>());
  if (!ownership) {
    return astarte_tl::unexpected(ownership.error());
  }

  std::optional<Aggregation> aggregation = std::nullopt;
  if (interface.contains("aggregation")) {
    auto res = aggregation_from_str(interface.at("aggregation").get<std::string>());
    if (!res) {
      return astarte_tl::unexpected(res.error());
    }
    aggregation = std::optional(res.value());
  }

  auto description = optional_value_from_json_interface<std::string>(interface, "description");
  auto doc = optional_value_from_json_interface<std::string>(interface, "doc");

  auto mappings = mappings_from_interface(interface);
  if (!mappings) {
    return astarte_tl::unexpected(mappings.error());
  }

  return Interface(interface_name, version_major.value(), version_minor.value(),
                   interface_type.value(), ownership.value(), aggregation, description, doc,
                   mappings.value());
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

  if ((mapping->explicit_timestamp_.has_value() && mapping->explicit_timestamp_.value()) &&
      timestamp == nullptr) {
    spdlog::error("Explicit timestamp required for interface {}, path {}", interface_name_, path);
    return astarte_tl::unexpected(AstarteInterfaceValidationError(astarte_fmt::format(
        "Explicit timestamp required for interface {}, path {}", interface_name_, path)));
  }

  if ((mapping->explicit_timestamp_.has_value() && !mapping->explicit_timestamp_.value()) &&
      timestamp != nullptr) {
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
    if (aggregation_.has_value() && (aggregation_.value() == Aggregation::kIndividual)) {
      auto mapping_res = get_mapping(path);
      if (!mapping_res) {
        return astarte_tl::unexpected(mapping_res.error());
      }
      return mapping_res.value();
    }

    // object Aggregation (return the first mapping)
    if (mappings_.empty()) {
      return astarte_tl::unexpected(AstarteMqttError("Interface has no mappings"));
    }
    return &mappings_.at(0);
  }();

  if (!mapping_exp) {
    return astarte_tl::unexpected(mapping_exp.error());
  }

  const Mapping* map_ptr = mapping_exp.value();

  if (!map_ptr->reliability_) {
    return astarte_tl::unexpected(
        AstarteMqttError("the interface mapping doesn't contain the qos value"));
  }

  return static_cast<int8_t>(map_ptr->reliability_.value());
}

auto Introspection::checked_insert(Interface interface)
    -> astarte_tl::expected<void, AstarteError> {
  if (!interfaces_.contains(interface.interface_name())) {
    spdlog::debug("Adding new interface {}", interface.interface_name());
    interfaces_.insert_or_assign(interface.interface_name(), std::move(interface));
    return {};
  }

  // if the interface is already present in the introspection, do some checks before updating it
  const auto& stored = interfaces_.at(interface.interface_name());
  if (stored.ownership() != interface.ownership()) {
    spdlog::error("the new interface has a different ownership");
    return astarte_tl::unexpected(AstarteInvalidInterfaceOwnershipeError(
        astarte_fmt::format("the new interface has a different ownership. Expected {}, got {}",
                            stored.ownership(), interface.ownership())));
  }

  if (stored.interface_type() != interface.interface_type()) {
    spdlog::error("the new interface has a different type");
    return astarte_tl::unexpected(AstarteInvalidAstarteTypeError(
        astarte_fmt::format("the new interface has a different type. Expected {}, got {}",
                            stored.interface_type(), interface.interface_type())));
  }

  if (interface.version_major() < stored.version_major()) {
    spdlog::error("the new interface must have a major version greater or equal than {}",
                  stored.version_major());
    return astarte_tl::unexpected(AstarteInvalidInterfaceVersionError(
        astarte_fmt::format("the new major version is lower than the actual one. Expected value "
                            "greater than {}, got {}",
                            stored.version_major(), interface.version_major())));
  }

  if ((interface.version_major() == stored.version_major()) &&
      (interface.version_minor() < stored.version_minor())) {
    spdlog::error("the new interface must have a minor version greater or equal than {}",
                  stored.version_minor());
    return astarte_tl::unexpected(AstarteInvalidInterfaceVersionError(astarte_fmt::format(
        "the new minor version is lower than the actual one Expected value greater than {}, got {}",
        stored.version_minor(), interface.version_minor())));
  }

  spdlog::debug("overwriting the old interface with the new one");
  interfaces_.insert_or_assign(interface.interface_name(), std::move(interface));
  return {};
}

auto Introspection::get(const std::string& interface_name)
    -> astarte_tl::expected<Interface*, AstarteError> {
  if (!interfaces_.contains(interface_name)) {
    return astarte_tl::unexpected(AstarteMqttError(
        astarte_fmt::format("couldn't find interface {} in the introspection", interface_name)));
  }

  return &interfaces_.at(interface_name);
}

}  // namespace AstarteDeviceSdk
