// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_INTROSPECTION_H
#define ASTARTE_MQTT_INTROSPECTION_H

#include <spdlog/spdlog.h>

#include <astarte_device_sdk/errors.hpp>
#include <astarte_device_sdk/ownership.hpp>
#include <astarte_device_sdk/type.hpp>
#include <format>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace AstarteDeviceSdk {

/**
 * @brief alias for nlohmann json
 */
using json = nlohmann::json;

/**
 * @brief Extract an optional value from a JSON object.
 *
 * @tparam T The type of the value to extract.
 * @param interface The JSON object to parse.
 * @param key The key of the value to extract.
 * @return An std::optional<T> containing the value if the key exists, or std::nullopt otherwise.
 */
// TODO: remove inline once the function definition is moved to its respective .cpp file
template <typename T>
inline auto optional_value_from_json_interface(const json& interface, std::string_view key)
    -> std::optional<T> {
  return interface.contains(key) ? std::optional<T>(interface.at(key)) : std::nullopt;
}

/**
 * @brief Define the type of an Astarte interface.
 */
enum InterfaceType {
  /**
   * @brief A datastream interface, used for time-series data.
   */
  kDatastream,
  /**
   * @brief A properties interface, used for device state properties.
   */
  kProperty,
};

/**
 * @brief Convert a string to an InterfaceType enum.
 *
 * @param typ The string representation of the interface type.
 * @return The corresponding InterfaceType enum value, an error if the string is not a valid
 * interface type.
 */
// TODO: remove inline once the function definition is moved to its respective .cpp file
inline auto interface_type_from_str(std::string typ)
    -> astarte_tl::expected<InterfaceType, AstarteError> {
  if (typ == "datastream") {
    return InterfaceType::kDatastream;
  } else if (typ == "properties") {
    return InterfaceType::kProperty;
  } else {
    return astarte_tl::unexpected(
        AstarteInvalidInterfaceTypeError(std::format("interface type not valid: {}", typ)));
  }
}

/**
 * @brief Define the aggregation type for interface mappings.
 */
enum Aggregation {
  /**
   * @brief Data is collected as individual, distinct values.
   */
  kIndividual,
  /**
   * @brief Data is collected as a single object or document.
   */
  kObject,
};

/**
 * @brief Convert a string to an Aggregation enum.
 *
 * @param aggr The string representation of the aggregation (e.g., "individual", "object").
 * @return The corresponding Aggregation enum value, an error if the string is not a valid
 * aggregation type.
 */
// TODO: remove inline once the function definition is moved to its respective .cpp file
inline auto aggregation_from_str(std::string aggr)
    -> astarte_tl::expected<Aggregation, AstarteError> {
  if (aggr == "individual") {
    return Aggregation::kIndividual;
  } else if (aggr == "object") {
    return Aggregation::kObject;
  } else {
    return astarte_tl::unexpected(
        AstarteInvalidAggregationError(std::format("Aggregation not valid: {}", aggr)));
  }
}

/**
 * @brief Reliability of a datastream.
 *
 * Defines whether the sent data should be considered delivered. Properties have always a unique
 * reliability. See
 * [Reliability](https://docs.astarte-platform.org/astarte/latest/040-interface_schema.html#astarte-mapping-schema-reliability)
 * for more information.
 */
enum Reliability {
  /**
   * @brief If the transport sends the data. Default.
   */
  kUnreliable,
  /**
   * @brief When we know the data has been received at least once.
   */
  kGuaranteed,
  /**
   * @brief When we know the data has been received exactly once.
   */
  kUnique,
};

NLOHMANN_JSON_SERIALIZE_ENUM(Reliability, {
                                              {kUnreliable, "unreliable"},
                                              {kGuaranteed, "guaranteed"},
                                              {kUnique, "unique"},
                                          })

/**
 * @brief Define the retention of a datastream.
 *
 * Describes what to do with the sent data if the transport is incapable of delivering it.
 * See
 * [Retention](https://docs.astarte-platform.org/astarte/latest/040-interface_schema.html#astarte-mapping-schema-retention)
 * for more information.
 */
enum Retention {
  /**
   * @brief Data is discarded. Default.
   */
  kDiscard,
  /**
   * @brief Data is kept in a cache in memory.
   */
  kVolatile,
  /**
   * @brief Data is kept on non-volatile storage.
   */
  kStored,
};

NLOHMANN_JSON_SERIALIZE_ENUM(Retention, {
                                            {kDiscard, "discard"},
                                            {kVolatile, "volatile"},
                                            {kStored, "stored"},
                                        })

/**
 * @brief Define whether data should expire from the database after a given interval.
 *
 * See [Database Retention
 * Policy](https://docs.astarte-platform.org/astarte/latest/040-interface_schema.html#astarte-mapping-schema-database_retention_policy)
 * for more information.
 */
enum DatabaseRetentionPolicy {
  /**
   * @brief The data will never expiry. Default.
   */
  kNoTtl,
  /**
   * @brief The data will expire after the ttl.
   *
   * The field `database_retention_ttl` will be used to determine how many seconds the data is kept
   * in the database.
   */
  kUseTtl,
};

NLOHMANN_JSON_SERIALIZE_ENUM(DatabaseRetentionPolicy, {
                                                          {kNoTtl, "no_ttl"},
                                                          {kUseTtl, "use_ttl"},
                                                      })

struct Mapping {
 public:
  /**
   * @brief Path of the mapping.
   *
   * It can be parametrized (e.g. `/foo/%{path}/baz`).
   */
  std::string endpoint;
  /**
   * @brief Define the type of the mapping.
   *
   * This represent the data that will be published on the mapping.
   */
  AstarteType mapping_type;
  /**
   * @brief Allow to set a custom timestamp.
   */
  std::optional<bool> explicit_timestamp;
  /**
   * @brief Define when to consider the data delivered.
   *
   * Useful only with datastream. Defines whether the sent data should be considered delivered
   * when the transport successfully sends the data (unreliable), when we know that the data has
   * been received at least once (guaranteed) or when we know that the data has been received
   * exactly once (unique). Unreliable by default.
   */
  std::optional<Reliability> reliability;
  /**
   * @brief Retention of the data when not deliverable.
   *
   * Useful only with datastream. Defines whether the sent data should be discarded if the
   * transport is temporarily uncapable of delivering it (discard) or should be kept in a cache in
   * memory (volatile) or on disk (stored), and guaranteed to be delivered in the timeframe
   * defined by the expiry.
   */
  std::optional<Retention> retention;
  /**
   * @brief Expiry for the retain data.
   *
   * Useful when retention is stored. Defines after how many seconds a specific data entry should
   * be kept before giving up and erasing it from the persistent cache. A value <= 0 means the
   * persistent cache never expires, and is the default.
   */
  std::optional<int64_t> expiry;
  /**
   * @brief Expiry for the retain data.
   *
   * Useful only with datastream. Defines whether data should expire from the database after a
   * given interval. Valid values are: `no_ttl` and `use_ttl`.
   */
  std::optional<DatabaseRetentionPolicy> database_retention_policy;
  /**
   * @brief Seconds to keep the data in the database.
   *
   * Useful when `database_retention_policy` is "`use_ttl`". Defines how many seconds a specific
   * data entry should be kept before erasing it from the database.
   */
  std::optional<int64_t> database_retention_ttl;
  /**
   * @brief Allow the property to be unset.
   *
   * Used only with properties.
   */
  std::optional<bool> allow_unset;
  /**
   * @brief An optional description of the mapping.
   */
  std::optional<std::string> description;
  /**
   * @brief A string containing documentation that will be injected in the generated client code.
   */
  std::optional<std::string> doc;
};

/**
 * @brief Parses the "mappings" array from an interface JSON object.
 *
 * @param interface The JSON object representing an Astarte interface.
 * @return A vector of Mapping objects parsed from the interface, an error otherwise.
 */
// TODO: remove inline once the function definition is moved to its respective .cpp file
inline auto mappings_from_interface(json& interface)
    -> astarte_tl::expected<std::vector<Mapping>, AstarteError> {
  std::vector<Mapping> mappings;

  for (const auto& mapping : interface.at("mappings")) {
    auto endpoint = mapping.at("endpoint");
    auto mapping_type = astarte_type_from_str(mapping.at("type"));
    if (!mapping_type) {
      return astarte_tl::unexpected(mapping_type.error());
    }
    auto explicit_timestamp =
        optional_value_from_json_interface<bool>(mapping, "explicit_timestamp");
    auto reliability = optional_value_from_json_interface<Reliability>(mapping, "reliability");
    auto retention = optional_value_from_json_interface<Retention>(mapping, "retention");
    auto expiry = optional_value_from_json_interface<int64_t>(mapping, "expiry");
    auto database_retention_policy = optional_value_from_json_interface<DatabaseRetentionPolicy>(
        mapping, "database_retention_policy");
    auto database_retention_ttl =
        optional_value_from_json_interface<int64_t>(mapping, "database_retention_ttl");
    auto allow_unset = optional_value_from_json_interface<bool>(mapping, "allow_unset");
    auto description = optional_value_from_json_interface<std::string>(mapping, "description");
    auto doc = optional_value_from_json_interface<std::string>(mapping, "doc");

    auto m = Mapping{
        endpoint, mapping_type.value(),      explicit_timestamp,     reliability, retention,
        expiry,   database_retention_policy, database_retention_ttl, allow_unset, description,
        doc};

    mappings.push_back(m);
  }

  return mappings;
}

/**
 * @brief Converts and validates an interface version number.
 *
 * Checks if the version is non-negative and fits within a u_int32_t.
 *
 * @param maj_min A string literal ("major" or "minor") used for error messages.
 * @param version The version number as int64_t (from JSON parsing).
 * @return The version number as u_int32_t, an error if the version is negative or too large.
 */
// TODO: remove inline once the function definition is moved to its respective .cpp file
inline auto convert_version(std::string_view maj_min, int64_t version)
    -> astarte_tl::expected<u_int32_t, AstarteError> {
  if (std::cmp_less(version, 0)) {
    return astarte_tl::unexpected(AstarteInvalidVersionError(
        std::format("received negative {} version value: {}", maj_min, version)));
  }

  if (std::cmp_greater(version, std::numeric_limits<u_int32_t>::max())) {
    return astarte_tl::unexpected(AstarteInvalidVersionError(
        std::format("{} version value too large: {}", maj_min, version)));
  }

  return static_cast<u_int32_t>(version);
}

/**
 * @brief Represents a parsed Astarte interface.
 */
class Interface {
 public:
  /**
   * @brief Try to convert a json into an Interface object.
   * @param interface json representation of the Astarte interface.
   * @return An Interface object containg all the parsed Astarte interface information.
   */
  static auto try_from_json(json interface) -> astarte_tl::expected<Interface, AstarteError> {
    auto interface_name = interface.at("interface_name");
    auto version_major = convert_version("major", interface.at("version_major"));
    if (!version_major) {
      return astarte_tl::unexpected(version_major.error());
    }
    auto version_minor = convert_version("minor", interface.at("version_minor"));
    if (!version_minor) {
      return astarte_tl::unexpected(version_minor.error());
    }
    auto interface_type = interface_type_from_str(interface.at("type"));
    if (!interface_type) {
      return astarte_tl::unexpected(interface_type.error());
    }
    auto ownership = ownership_from_str(interface.at("ownership"));
    if (!ownership) {
      return astarte_tl::unexpected(ownership.error());
    }
    std::optional<Aggregation> aggregation = std::nullopt;
    auto contains_aggr = interface.contains("aggregation");
    if (contains_aggr) {
      auto res = aggregation_from_str(interface.at("aggregation"));
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

    return Interface{
        interface_name,    version_major.value(), version_minor.value(), interface_type.value(),
        ownership.value(), aggregation,           description,           doc,
        mappings.value()};
  }

  /**
   * @brief The name of the interface.
   */
  std::string interface_name;
  /**
   * @brief The Major version qualifier for this interface.
   */
  u_int32_t version_major;
  /**
   * @brief The Minor version qualifier for this interface.
   */
  u_int32_t version_minor;
  /**
   * @brief Identify the type of this Interface. It could be Datastream or Property.
   */
  InterfaceType interface_type;
  /**
   * @brief Identify the quality of the interface.
   */
  AstarteOwnership ownership;
  /**
   * @brief Identify the aggregation of the mappings of the interface. It could be Individual or
   * Object.
   */
  std::optional<Aggregation> aggregation;
  /**
   * @brief An optional description of the interface.
   */
  std::optional<std::string> description;
  /**
   * @brief A string containing documentation that will be injected in the generated client code.
   */
  std::optional<std::string> doc;
  /**
   * @brief Define the endpoint of the interface.
   *
   * They are defined as relative URLs (e.g. /my/path) and can be parametrized (e.g.:
   * /%{myparam}/path). A valid interface must have no mappings clash, which means that every
   * mapping must resolve to a unique path or collection of paths (including
   * parametrization). Every mapping acquires type, quality and aggregation of the interface.
   */
  std::vector<Mapping> mappings;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_MQTT_INTROSPECTION_H
