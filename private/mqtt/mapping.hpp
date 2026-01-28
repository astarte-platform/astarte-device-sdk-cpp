// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MAPPING_H
#define ASTARTE_MAPPING_H

#include <spdlog/spdlog.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <format>
#include <functional>
#include <map>
#include <nlohmann/json.hpp>
#include <optional>
#include <ostream>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/errors.hpp"
#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/type.hpp"

namespace AstarteDeviceSdk {

using json = nlohmann::json;

class Reliability {
 public:
  enum Value : uint8_t {
    kUnreliable,
    kGuaranteed,
    kUnique,
  };

  Reliability() : value_(kUnreliable) {}
  explicit Reliability(Value val) : value_(val) {}

  constexpr auto operator==(const Reliability& other) const -> bool = default;
  constexpr auto operator==(Value val) const -> bool { return value_ == val; }

  [[nodiscard]] constexpr auto to_string() const -> std::string_view {
    switch (value_) {
      case kUnreliable:
        return "unreliable";
      case kGuaranteed:
        return "guaranteed";
      case kUnique:
        return "unique";
    }
    return "unreachable";
  }

  static auto try_from_str(std::string_view str)
      -> astarte_tl::expected<Reliability, AstarteError> {
    if (str == "unreliable") {
      return Reliability(kUnreliable);
    }
    if (str == "guaranteed") {
      return Reliability(kGuaranteed);
    }
    if (str == "unique") {
      return Reliability(kUnique);
    }
    return astarte_tl::unexpected(
        AstarteInvalidReliabilityError(astarte_fmt::format("reliability not valid: {}", str)));
  }

  [[nodiscard]] auto get_qos() const -> int8_t { return static_cast<int8_t>(value_); }

  friend void from_json(const nlohmann::json& json, Reliability& rel) {
    if (json.is_string()) {
      auto res = try_from_str(json.get<std::string>());
      if (res) {
        rel = res.value();
        return;
      }
    }
    rel.value_ = kUnreliable;
  }

 private:
  Value value_;
};

class Retention {
 public:
  enum Value : uint8_t {
    kDiscard,
    kVolatile,
    kStored,
  };

  Retention() : value_(kDiscard) {}
  explicit Retention(Value val) : value_(val) {}

  constexpr auto operator==(const Retention& other) const -> bool = default;
  constexpr auto operator==(Value val) const -> bool { return value_ == val; }

  [[nodiscard]] constexpr auto to_string() const -> std::string_view {
    switch (value_) {
      case kDiscard:
        return "discard";
      case kVolatile:
        return "volatile";
      case kStored:
        return "stored";
    }
    return "unreachable";
  }

  static auto try_from_str(std::string_view str) -> astarte_tl::expected<Retention, AstarteError> {
    if (str == "discard") {
      return Retention(kDiscard);
    }
    if (str == "volatile") {
      return Retention(kVolatile);
    }
    if (str == "stored") {
      return Retention(kStored);
    }
    return astarte_tl::unexpected(
        AstarteInvalidRetentionError(astarte_fmt::format("retention not valid: {}", str)));
  }

  friend void from_json(const nlohmann::json& json, Retention& ret) {
    if (json.is_string()) {
      auto res = try_from_str(json.get<std::string>());
      if (res) {
        ret = res.value();
        return;
      }
    }
    ret.value_ = kDiscard;
  }

 private:
  Value value_;
};

class DatabaseRetentionPolicy {
 public:
  enum Value : uint8_t {
    kNoTtl,
    kUseTtl,
  };

  DatabaseRetentionPolicy() : value_(kNoTtl) {}
  explicit DatabaseRetentionPolicy(Value val) : value_(val) {}

  constexpr auto operator==(const DatabaseRetentionPolicy& other) const -> bool = default;
  constexpr auto operator==(Value val) const -> bool { return value_ == val; }

  [[nodiscard]] constexpr auto to_string() const -> std::string_view {
    switch (value_) {
      case kNoTtl:
        return "no_ttl";
      case kUseTtl:
        return "use_ttl";
    }
    return "unreachable";
  }

  static auto try_from_str(std::string_view str)
      -> astarte_tl::expected<DatabaseRetentionPolicy, AstarteError> {
    if (str == "no_ttl") {
      return DatabaseRetentionPolicy(kNoTtl);
    }
    if (str == "use_ttl") {
      return DatabaseRetentionPolicy(kUseTtl);
    }
    return astarte_tl::unexpected(AstarteInvalidDatabaseRetentionPolicyError(
        astarte_fmt::format("database retention policy not valid: {}", str)));
  }

  friend void from_json(const nlohmann::json& json, DatabaseRetentionPolicy& pol) {
    if (json.is_string()) {
      auto res = try_from_str(json.get<std::string>());
      if (res) {
        pol = res.value();
        return;
      }
    }
    pol.value_ = kNoTtl;
  }

 private:
  Value value_;
};

class Mapping {
 public:
  Mapping(std::string endpoint, AstarteType type, std::optional<bool> explicit_timestamp,
          std::optional<Reliability> reliability, std::optional<Retention> retention,
          std::optional<int64_t> expiry,
          std::optional<DatabaseRetentionPolicy> database_retention_policy,
          std::optional<int64_t> database_retention_ttl, std::optional<bool> allow_unset,
          std::optional<std::string> description, std::optional<std::string> doc)
      : endpoint_(std::move(endpoint)),
        type_(type),
        explicit_timestamp_(explicit_timestamp),
        reliability_(reliability),
        retention_(retention),
        expiry_(expiry),
        database_retention_policy_(database_retention_policy),
        database_retention_ttl_(database_retention_ttl),
        allow_unset_(allow_unset),
        description_(std::move(description)),
        doc_(std::move(doc)) {}

  /**
   * @brief Check that the mapping endpoint matches a given path.
   *
   * @param path The Astarte interface path to check.
   * @return a boolean stating if the mapping endpoint matches the path or not.
   */
  [[nodiscard]] auto match_path(std::string_view path) const -> bool;

  /**
   * @brief Check that the Astarte data matches the mapping type
   *
   * @param data The AstarteData to check.
   * @return an error if the check fails.
   */
  [[nodiscard]] auto check_data_type(const AstarteData& data) const
      -> astarte_tl::expected<void, AstarteError>;

  /**
   * @brief Path of the mapping.
   *
   * It can be parametrized (e.g. `/foo/%{path}/baz`).
   */
  [[nodiscard]] auto endpoint() const -> const std::string& { return endpoint_; }

  /**
   * @brief Define the type of the mapping.
   *
   * This represent the data that will be published on the mapping.
   */
  [[nodiscard]] auto type() const -> AstarteType { return type_; }

  /**
   * @brief Allow to set a custom timestamp.
   */
  [[nodiscard]] auto explicit_timestamp() const -> std::optional<bool> {
    return explicit_timestamp_;
  }

  /**
   * @brief Define when to consider the data delivered.
   *
   * Useful only with datastream. Defines whether the sent data should be considered delivered
   * when the transport successfully sends the data (unreliable), when we know that the data has
   * been received at least once (guaranteed) or when we know that the data has been received
   * exactly once (unique). Unreliable by default.
   */
  [[nodiscard]] auto reliability() const -> std::optional<Reliability> { return reliability_; }

  /**
   * @brief Retention of the data when not deliverable.
   *
   * Useful only with datastream. Defines whether the sent data should be discarded if the
   * transport is temporarily uncapable of delivering it (discard) or should be kept in a cache in
   * memory (volatile) or on disk (stored), and guaranteed to be delivered in the timeframe
   * defined by the expiry.
   */
  [[nodiscard]] auto retention() const -> std::optional<Retention> { return retention_; }

  /**
   * @brief Expiry for the retain data.
   *
   * Useful when retention is stored. Defines after how many seconds a specific data entry should
   * be kept before giving up and erasing it from the persistent cache. A value <= 0 means the
   * persistent cache never expires, and is the default.
   */
  [[nodiscard]] auto expiry() const -> std::optional<int64_t> { return expiry_; }

  /**
   * @brief Expiry for the retain data.
   *
   * Useful only with datastream. Defines whether data should expire from the database after a
   * given interval. Valid values are: `no_ttl` and `use_ttl`.
   */
  [[nodiscard]] auto database_retention_policy() const -> std::optional<DatabaseRetentionPolicy> {
    return database_retention_policy_;
  }

  /**
   * @brief Seconds to keep the data in the database.
   *
   * Useful when `database_retention_policy` is "`use_ttl`". Defines how many seconds a specific
   * data entry should be kept before erasing it from the database.
   */
  [[nodiscard]] auto database_retention_ttl() const -> std::optional<int64_t> {
    return database_retention_ttl_;
  }

  /**
   * @brief Allow the property to be unset.
   *
   * Used only with properties.
   */
  [[nodiscard]] auto allow_unset() const -> std::optional<bool> { return allow_unset_; }

  /**
   * @brief An optional description of the mapping.
   */
  [[nodiscard]] auto description() const -> const std::optional<std::string>& {
    return description_;
  }

  /**
   * @brief A string containing documentation that will be injected in the generated client code.
   */
  [[nodiscard]] auto doc() const -> const std::optional<std::string>& { return doc_; }

 private:
  std::string endpoint_;
  AstarteType type_;
  std::optional<bool> explicit_timestamp_;
  std::optional<Reliability> reliability_;
  std::optional<Retention> retention_;
  std::optional<int64_t> expiry_;
  std::optional<DatabaseRetentionPolicy> database_retention_policy_;
  std::optional<int64_t> database_retention_ttl_;
  std::optional<bool> allow_unset_;
  std::optional<std::string> description_;
  std::optional<std::string> doc_;
};

// Function to safely get a reference to a JSON field
auto get_field(const json& interface, std::string_view key, json::value_t expected_type)
    -> astarte_tl::expected<json, AstarteError>;

// helper to map C++ types to JSON types
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

/**
 * @brief Parses the "mappings" array from an interface JSON object.
 *
 * @param interface The JSON object representing an Astarte interface.
 * @return A vector of Mapping objects parsed from the interface, an error otherwise.
 */
auto mappings_from_interface_json(const json& interface)
    -> astarte_tl::expected<std::vector<Mapping>, AstarteError>;

}  // namespace AstarteDeviceSdk

// ------------------------------------------------------------------------------------------------
// FORAMATTING
// ------------------------------------------------------------------------------------------------

/**
 * @brief astarte_fmt::formatter specialization for Reliability.
 */
template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::Reliability> {
  /**
   * @brief Parse the format string. Default implementation.
   * @param ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Format the Reliability enum.
   * @param msg The Reliability to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::Reliability& rel, FormatContext& ctx) const {
    return astarte_fmt::format_to(ctx.out(), "{}", rel.to_string());
  }
};

inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::Reliability rel)
    -> std::ostream& {
  out << astarte_fmt::format("{}", rel);
  return out;
}

/**
 * @brief astarte_fmt::formatter specialization for Retention.
 */
template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::Retention> {
  /**
   * @brief Parse the format string. Default implementation.
   * @param ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Format the Retention enum.
   * @param msg The Retention to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::Retention& ret, FormatContext& ctx) const {
    return astarte_fmt::format_to(ctx.out(), "{}", ret.to_string());
  }
};

inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::Retention ret) -> std::ostream& {
  out << astarte_fmt::format("{}", ret);
  return out;
}

/**
 * @brief astarte_fmt::formatter specialization for DatabaseRetentionPolicy.
 */
template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::DatabaseRetentionPolicy> {
  /**
   * @brief Parse the format string. Default implementation.
   * @param ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Format the DatabaseRetentionPolicy enum.
   * @param msg The DatabaseRetentionPolicy to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const AstarteDeviceSdk::DatabaseRetentionPolicy& ret_pol, FormatContext& ctx) const {
    return astarte_fmt::format_to(ctx.out(), "{}", ret_pol.to_string());
  }
};

inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::DatabaseRetentionPolicy ret_pol)
    -> std::ostream& {
  out << astarte_fmt::format("{}", ret_pol);
  return out;
}

/**
 * @brief astarte_fmt::formatter specialization for Mapping.
 */
template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::Mapping> {
  /**
   * @brief Parse the format string. Default implementation.
   * @param ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Format the Mapping enum.
   * @param msg The Mapping to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  // NOLINTNEXTLINE(readability-function-size)
  auto format(const AstarteDeviceSdk::Mapping& mapping, FormatContext& ctx) const {
    auto out = ctx.out();

    astarte_fmt::format_to(out, "Mapping {{");
    astarte_fmt::format_to(out, "endpoint: {}", mapping.endpoint());
    astarte_fmt::format_to(out, ", type: {}", mapping.type());
    if (auto val = mapping.explicit_timestamp()) {
      astarte_fmt::format_to(out, ", explicit_timestamp: {}", *val);
    }
    if (auto val = mapping.reliability()) {
      astarte_fmt::format_to(out, ", reliability: {}", *val);
    }
    if (auto val = mapping.retention()) {
      astarte_fmt::format_to(out, ", retention: {}", *val);
    }
    if (auto val = mapping.expiry()) {
      astarte_fmt::format_to(out, ", expiry: {}", *val);
    }
    if (auto val = mapping.database_retention_policy()) {
      astarte_fmt::format_to(out, ", database_retention_policy: {}", *val);
    }
    if (auto val = mapping.database_retention_ttl()) {
      astarte_fmt::format_to(out, ", database_retention_ttl: {}", *val);
    }
    if (auto val = mapping.allow_unset()) {
      astarte_fmt::format_to(out, ", allow_unset: {}", *val);
    }
    if (auto val = mapping.description()) {
      astarte_fmt::format_to(out, ", description: {}", *val);
    }
    if (auto val = mapping.doc()) {
      astarte_fmt::format_to(out, ", doc: {}", *val);
    }
    astarte_fmt::format_to(out, "}}");

    return out;
  }
};

inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::Mapping& mapping)
    -> std::ostream& {
  out << astarte_fmt::format("{}", mapping);
  return out;
}

#endif  // ASTARTE_MAPPING_H
