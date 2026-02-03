// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "mqtt/interface.hpp"

#include <spdlog/spdlog.h>

#include <cstdint>
#include <format>
#include <limits>
#include <map>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/errors.hpp"
#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/type.hpp"
#include "mqtt/helpers.hpp"
#include "mqtt/mapping.hpp"

namespace astarte::device {

namespace {
/**
 * @brief Converts and validates an interface version number.
 *
 * Checks if the version is non-negative and fits within a uint32_t.
 *
 * @param version_type A string literal ("major" or "minor") used for error messages.
 * @param version The version number as int64_t (from JSON parsing).
 * @return The version number as uint32_t, an error if the version is negative or too large.
 */
auto convert_version(std::string_view version_type, int64_t version)
    -> astarte_tl::expected<uint32_t, Error> {
  if (std::cmp_less(version, 0)) {
    return astarte_tl::unexpected(InvalidInterfaceVersionError(
        astarte_fmt::format("received negative {} version value: {}", version_type, version)));
  }

  if (std::cmp_greater(version, std::numeric_limits<uint32_t>::max())) {
    return astarte_tl::unexpected(InvalidInterfaceVersionError(
        astarte_fmt::format("{} version value too large: {}", version_type, version)));
  }

  return static_cast<uint32_t>(version);
}

auto parse_version_field(const json& interface, std::string_view key, std::string_view name)
    -> astarte_tl::expected<uint32_t, Error> {
  auto field_json = json_helper::get_field(interface, key, json::value_t::number_integer);
  if (!field_json) {
    return astarte_tl::unexpected(field_json.error());
  }
  return convert_version(name, field_json.value().get<int64_t>());
}

auto parse_interface_type(const json& interface) -> astarte_tl::expected<InterfaceType, Error> {
  constexpr std::string_view kType = "type";
  auto type_json = json_helper::get_field(interface, kType, json::value_t::string);
  if (!type_json) {
    return astarte_tl::unexpected(type_json.error());
  }
  return InterfaceType::try_from_str(type_json.value().get<std::string>());
}

auto parse_ownership(const json& interface) -> astarte_tl::expected<AstarteOwnership, Error> {
  constexpr std::string_view kOwnership = "ownership";
  auto own_json = json_helper::get_field(interface, kOwnership, json::value_t::string);
  if (!own_json) {
    return astarte_tl::unexpected(own_json.error());
  }
  return ownership_from_str(own_json.value().get<std::string>());
}

auto parse_aggregation(const json& interface)
    -> astarte_tl::expected<std::optional<InterfaceAggregation>, Error> {
  constexpr std::string_view kAggregation = "aggregation";
  if (!interface.contains(kAggregation)) {
    return std::nullopt;
  }

  const auto& agg_val = interface.at(kAggregation);
  if (!agg_val.is_string()) {
    return astarte_tl::unexpected(InterfaceValidationError("aggregation must be a string"));
  }

  auto res = InterfaceAggregation::try_from_str(agg_val.get<std::string>());
  if (!res) {
    return astarte_tl::unexpected(res.error());
  }

  return std::optional<InterfaceAggregation>(res.value());
}

/**
 * @brief Parses the "mappings" array from an interface JSON object.
 *
 * @param interface The JSON object representing an Astarte interface.
 * @return A vector of Mapping objects parsed from the interface, an error otherwise.
 */
auto mappings_from_interface_json(const json& interface)
    -> astarte_tl::expected<std::vector<Mapping>, Error> {
  auto mappings_field = json_helper::get_field(interface, "mappings", json::value_t::array);
  if (!mappings_field) {
    return astarte_tl::unexpected(mappings_field.error());
  }

  const auto& mappings_json = mappings_field.value();
  std::vector<Mapping> mappings;
  // reserve memory to avoid reallocations
  mappings.reserve(mappings_json.size());

  for (const auto& mapping : mappings_json) {
    auto res = Mapping::try_from_json(mapping);
    if (!res) {
      return astarte_tl::unexpected(res.error());
    }
    mappings.emplace_back(res.value());
  }

  return mappings;
}

}  // namespace

auto Interface::try_from_json(const json& interface) -> astarte_tl::expected<Interface, Error> {
  auto name_json = json_helper::get_field(interface, "interface_name", json::value_t::string);
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

  auto description = json_helper::optional_value_from_json<std::string>(interface, "description");
  auto doc = json_helper::optional_value_from_json<std::string>(interface, "doc");

  auto mappings_res = mappings_from_interface_json(interface);
  if (!mappings_res) {
    return astarte_tl::unexpected(mappings_res.error());
  }
  auto mappings = mappings_res.value();

  if (mappings.empty()) {
    return astarte_tl::unexpected(InterfaceValidationError("There must be at least one mapping"));
  }

  return Interface(interface_name, maj_res.value(), min_res.value(), type_res.value(),
                   own_res.value(), agg_res.value(), description, doc, mappings);
}

auto Interface::get_mapping(std::string_view path) const
    -> astarte_tl::expected<const Mapping*, Error> {
  for (const auto& mapping : mappings_) {
    if (mapping.match_path(path)) {
      return &mapping;
    }
  }

  return astarte_tl::unexpected(
      InterfaceValidationError(astarte_fmt::format("couldn't find mapping with path {}", path)));
}

auto Interface::validate_individual(std::string_view path, const Data& data,
                                    const std::chrono::system_clock::time_point* timestamp) const
    -> astarte_tl::expected<void, Error> {
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
    return astarte_tl::unexpected(InterfaceValidationError(astarte_fmt::format(
        "Explicit timestamp required for interface {}, path {}", interface_name_, path)));
  }

  if ((explicit_ts.has_value() && !explicit_ts.value()) && timestamp != nullptr) {
    spdlog::error("Explicit timestamp not supported for interface {}, path {}", interface_name_,
                  path);
    return astarte_tl::unexpected(InterfaceValidationError(astarte_fmt::format(
        "Explicit timestamp not supported for interface {}, path {}", interface_name_, path)));
  }

  return {};
}

auto Interface::validate_object(std::string_view common_path, const AstarteDatastreamObject& object,
                                const std::chrono::system_clock::time_point* timestamp) const
    -> astarte_tl::expected<void, Error> {
  for (const auto& [endpoint_path, data] : object) {
    auto path = astarte_fmt::format("{}/{}", common_path, endpoint_path);
    auto res = this->validate_individual(path, data, timestamp);
    if (!res) {
      return astarte_tl::unexpected(res.error());
    }
  }

  return {};
}

auto Interface::get_qos(std::string_view path) const -> astarte_tl::expected<uint8_t, Error> {
  auto mapping_exp = [&]() -> astarte_tl::expected<const Mapping*, Error> {
    if (!aggregation_.has_value() || aggregation_.value().is_individual()) {
      auto mapping_res = get_mapping(path);
      if (!mapping_res) {
        return astarte_tl::unexpected(mapping_res.error());
      }
      return mapping_res.value();
    }

    // object InterfaceAggregation (return the first mapping)
    if (mappings_.empty()) {
      return astarte_tl::unexpected(MqttError("Interface has no mappings"));
    }
    return &mappings_.at(0);
  }();

  if (!mapping_exp) {
    return astarte_tl::unexpected(mapping_exp.error());
  }

  const Mapping* map_ptr = mapping_exp.value();

  auto reliability = map_ptr->reliability();
  if (!reliability) {
    return astarte_tl::unexpected(MqttError("the interface mapping doesn't contain the qos value"));
  }

  return reliability->get_qos();
}

}  // namespace astarte::device
