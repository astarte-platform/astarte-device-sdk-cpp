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
#include <map>
#include <nlohmann/json.hpp>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>

namespace AstarteDeviceSdk {

/**
 * @brief alias for nlohmann json
 */
using json = nlohmann::json;  // NOLINT(misc-include-cleaner)

/**
 * @brief Extract an optional value from a JSON object.
 *
 * @tparam T The type of the value to extract.
 * @param interface The JSON object to parse.
 * @param key The key of the value to extract.
 * @return An std::optional<T> containing the value if the key exists, or std::nullopt otherwise.
 */
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
auto interface_type_from_str(std::string typ) -> astarte_tl::expected<InterfaceType, AstarteError>;

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
auto aggregation_from_str(std::string aggr) -> astarte_tl::expected<Aggregation, AstarteError>;

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
auto mappings_from_interface(json& interface)
    -> astarte_tl::expected<std::vector<Mapping>, AstarteError>;

/**
 * @brief Converts and validates an interface version number.
 *
 * Checks if the version is non-negative and fits within a uint32_t.
 *
 * @param maj_min A string literal ("major" or "minor") used for error messages.
 * @param version The version number as int64_t (from JSON parsing).
 * @return The version number as uint32_t, an error if the version is negative or too large.
 */

auto convert_version(std::string_view maj_min, int64_t version)
    -> astarte_tl::expected<uint32_t, AstarteError>;

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
  static auto try_from_json(json interface) -> astarte_tl::expected<Interface, AstarteError>;

  /**
   * @brief The name of the interface.
   */
  std::string interface_name;
  /**
   * @brief The Major version qualifier for this interface.
   */
  uint32_t version_major;
  /**
   * @brief The Minor version qualifier for this interface.
   */
  uint32_t version_minor;
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

/**
 * @brief Represents a collection of Astarte interface.
 *
 * The introspection represents the set of device supported interfaces.
 * See the Astarte documentation for more details.
 * https://docs.astarte-platform.org/astarte/latest/080-mqtt-v1-protocol.html#introspection
 */
class Introspection {
 public:
  /**
   * @brief Construct an empty Introspection.
   */
  Introspection() = default;

  /**
   * @brief Try to insert and Interface into the Introspection.
   *
   * @param interface The interface to add.
   * @return an error if the operation fails
   */
  // TODO: change return value into std::except<void, AstarteError> if the operation failed.
  auto checked_insert(Interface interface) -> astarte_tl::expected<void, AstarteError>;

  /**
   * @brief Return a view over the introspection values.
   *
   * @return a view over the introspection interfaces.
   */
  auto values() const { return std::views::values(interfaces_); }

 private:
  /**
   * @brief A map containing the interfaces in the Device Introspection synced with Astarte.
   */
  std::map<std::string, Interface> interfaces_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_MQTT_INTROSPECTION_H
