// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_INTERFACE_H
#define ASTARTE_INTERFACE_H

/**
 * @file private/mqtt/interface.hpp
 * @brief Definition of Astarte Interface and related types.
 *
 * @details This file defines the `Interface` class which represents a full Astarte Interface
 * definition (datastream or property), including its versioning, ownership, aggregation,
 * and list of mappings.
 */

#include <spdlog/spdlog.h>

#include <chrono>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/errors.hpp"
#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/type.hpp"
#include "mqtt/mapping.hpp"

namespace astarte::device::mqtt {

/// @brief Alias for nlohmann json.
using json = nlohmann::json;

/// @brief Represents the type of an Astarte Interface.
class InterfaceType {
 public:
  /// @brief Underlying values for the interface type.
  enum class Value : uint8_t {
    /// @brief A datastream interface, used for time-series data.
    kDatastream,
    /// @brief A properties interface, used for state tracking.
    kProperty
  };

  /**
   * @brief Constructs an InterfaceType from a specific Value.
   * @param[in] val The interface type value.
   */
  explicit InterfaceType(Value val) : value_(val) {}

  /// @brief Default equality operator for comparing two InterfaceType objects.
  constexpr auto operator==(const InterfaceType& other) const -> bool = default;
  /// @brief Equality operator for comparing with a raw Value.
  constexpr auto operator==(Value val) const -> bool { return value_ == val; }

  /**
   * @brief Converts the interface type to its string representation.
   * @return A string_view containing "datastream" or "property".
   */
  [[nodiscard]] constexpr auto to_string() const -> std::string_view {
    switch (value_) {
      case Value::kDatastream:
        return "datastream";
      case Value::kProperty:
        return "property";
    }
    return "unreachable";
  }

  /**
   * @brief Attempts to create an InterfaceType from a string.
   * @param[in] str The string representation to parse.
   * @return An expected containing the InterfaceType on success or Error on failure.
   */
  static auto try_from_str(std::string_view str) -> astarte_tl::expected<InterfaceType, Error> {
    if (str == "datastream") {
      return InterfaceType(Value::kDatastream);
    }
    if (str == "properties") {
      return InterfaceType(Value::kProperty);
    }
    return astarte_tl::unexpected(
        InvalidInterfaceTypeError(astarte_fmt::format("interface type not valid: {}", str)));
  }

 private:
  Value value_;
};

/// @brief Represents the aggregation of an Astarte Interface.
class InterfaceAggregation {
 public:
  /// @brief Underlying values for the interface aggregation.
  enum class Value : uint8_t {
    /// @brief Data is collected as individual, distinct values.
    kIndividual,
    /// @brief Data is collected as a single object or document.
    kObject
  };

  /**
   * @brief Constructs an InterfaceAggregation from a specific Value.
   * @param[in] val The interface aggregation value.
   */
  explicit InterfaceAggregation(Value val) : value_(val) {}

  /// @brief Default equality operator for comparing two InterfaceAggregation objects.
  constexpr auto operator==(const InterfaceAggregation& other) const -> bool = default;
  /// @brief Equality operator for comparing with a raw Value.
  constexpr auto operator==(Value val) const -> bool { return value_ == val; }

  /**
   * @brief Converts the interface aggregation to its string representation.
   * @return A string_view containing "individual" or "object".
   */
  [[nodiscard]] constexpr auto to_string() const -> std::string_view {
    switch (value_) {
      case Value::kIndividual:
        return "individual";
      case Value::kObject:
        return "object";
    }
    return "unreachable";
  }

  /**
   * @brief Attempts to create an InterfaceAggregation from a string.
   * @param[in] str The string representation to parse.
   * @return An expected containing the InterfaceAggregation on success or Error on failure.
   */
  static auto try_from_str(std::string_view str)
      -> astarte_tl::expected<InterfaceAggregation, Error> {
    if (str == "individual") {
      return InterfaceAggregation(Value::kIndividual);
    }
    if (str == "object") {
      return InterfaceAggregation(Value::kObject);
    }
    return astarte_tl::unexpected(InvalidInterfaceAggregationError(
        astarte_fmt::format("interface aggregation not valid: {}", str)));
  }

  /**
   * @brief Checks if the aggregation is of type Individual.
   * @return True if the aggregation is individual, false otherwise.
   */
  [[nodiscard]] auto is_individual() const -> bool { return value_ == Value::kIndividual; }

 private:
  Value value_;
};

/**
 * @brief Represents a parsed Astarte interface.
 *
 * @details An Interface object contains metadata (name, version, type) and a collection
 * of mappings that define the structure of data exchanged with Astarte.
 */
class Interface {
 public:
  /**
   * @brief Tries to convert a JSON object into an Interface object.
   * @param[in] interface The JSON representation of the Astarte interface.
   * @return An expected containing the Interface on success or Error on failure.
   */
  static auto try_from_json(const json& interface) -> astarte_tl::expected<Interface, Error>;

  /**
   * @brief Move constructor.
   * @param[in,out] other The Interface object to move from.
   */
  Interface(Interface&& other) noexcept = default;

  /**
   * @brief Move assignment operator.
   * @param[in,out] other The Interface object to move from.
   * @return A reference to this Interface object.
   */
  auto operator=(Interface&& other) noexcept -> Interface& = default;

  /// @brief Interface is non-copyable.
  Interface(const Interface&) = delete;

  /// @brief Interface is non-copyable.
  auto operator=(const Interface&) -> Interface& = delete;

  /// @brief Destructor.
  ~Interface() = default;

  /// @return The name of the interface.
  [[nodiscard]] auto interface_name() const -> const std::string& { return interface_name_; }

  /// @return The Major version qualifier.
  [[nodiscard]] auto version_major() const -> uint32_t { return version_major_; }

  /// @return The Minor version qualifier.
  [[nodiscard]] auto version_minor() const -> uint32_t { return version_minor_; }

  /// @return The type of this Interface (Datastream or Property).
  [[nodiscard]] auto interface_type() const -> InterfaceType { return interface_type_; }

  /// @return The ownership of the interface (Device or Server).
  [[nodiscard]] auto ownership() const -> Ownership { return ownership_; }

  /// @return The aggregation of the mappings (Individual or Object), if present.
  [[nodiscard]] auto aggregation() const -> const std::optional<InterfaceAggregation>& {
    return aggregation_;
  }

  /// @return The optional description of the interface.
  [[nodiscard]] auto description() const -> const std::optional<std::string>& {
    return description_;
  }

  /// @return The documentation string.
  [[nodiscard]] auto doc() const -> const std::optional<std::string>& { return doc_; }

  /// @return The vector of mappings defined for this interface.
  [[nodiscard]] auto mappings() const -> const std::vector<Mapping>& { return mappings_; }

  /**
   * @brief Retrieves the mapping associated with a given path if it exists.
   *
   * @param[in] path The Astarte interface path.
   * @return An expected containing the reference to the Mapping on success or Error on failure.
   */
  [[nodiscard]] auto get_mapping(std::string_view path) const
      -> astarte_tl::expected<const Mapping*, Error>;

  /**
   * @brief Validates an Astarte individual data point against this interface.
   *
   * @param[in] path The Astarte interface path.
   * @param[in] data The value to validate.
   * @param[in] timestamp A pointer to the timestamp, if provided.
   * @return An expected containing void on success or Error on failure.
   */
  auto validate_individual(std::string_view path, const Data& data,
                           const std::chrono::system_clock::time_point* timestamp) const
      -> astarte_tl::expected<void, Error>;

  /**
   * @brief Validates an Astarte object against this interface.
   *
   * @param[in] common_path The common base path of the Astarte interface endpoints.
   * @param[in] object The Astarte object data to validate.
   * @param[in] timestamp A pointer to the timestamp, if provided.
   * @return An expected containing void on success or Error on failure.
   */
  auto validate_object(std::string_view common_path, const DatastreamObject& object,
                       const std::chrono::system_clock::time_point* timestamp) const
      -> astarte_tl::expected<void, Error>;

  /**
   * @brief Gets the MQTT QoS level from a certain mapping endpoint.
   *
   * @param[in] path The Astarte interface path.
   * @return An expected containing the QoS value on success or Error on failure.
   */
  [[nodiscard]] auto get_qos(std::string_view path) const -> astarte_tl::expected<uint8_t, Error>;

 private:
  Interface(std::string interface_name, uint32_t version_major, uint32_t version_minor,
            InterfaceType interface_type, Ownership ownership,
            std::optional<InterfaceAggregation> aggregation, std::optional<std::string> description,
            std::optional<std::string> doc, std::vector<Mapping> mappings)
      : interface_name_(std::move(interface_name)),
        version_major_(version_major),
        version_minor_(version_minor),
        interface_type_(interface_type),
        ownership_(ownership),
        aggregation_(aggregation),
        description_(std::move(description)),
        doc_(std::move(doc)),
        mappings_(std::move(mappings)) {}

  std::string interface_name_;
  uint32_t version_major_;
  uint32_t version_minor_;
  InterfaceType interface_type_;
  Ownership ownership_;
  std::optional<InterfaceAggregation> aggregation_;
  std::optional<std::string> description_;
  std::optional<std::string> doc_;
  std::vector<Mapping> mappings_;
};

}  // namespace astarte::device::mqtt

// ------------------------------------------------------------------------------------------------
// FORMATTING
// ------------------------------------------------------------------------------------------------

/// @brief astarte_fmt::formatter specialization for InterfaceType.
template <>
struct astarte_fmt::formatter<astarte::device::mqtt::InterfaceType> {
  /**
   * @brief Parses the format string. Default implementation.
   * @param[in,out] ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Formats the InterfaceType enum.
   * @param[in] typ The InterfaceType to format.
   * @param[in,out] ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const astarte::device::mqtt::InterfaceType& typ, FormatContext& ctx) const {
    return astarte_fmt::format_to(ctx.out(), "{}", typ.to_string());
  }
};

inline auto operator<<(std::ostream& out, const astarte::device::mqtt::InterfaceType typ)
    -> std::ostream& {
  out << astarte_fmt::format("{}", typ);
  return out;
}

/// @brief astarte_fmt::formatter specialization for InterfaceAggregation.
template <>
struct astarte_fmt::formatter<astarte::device::mqtt::InterfaceAggregation> {
  /**
   * @brief Parses the format string. Default implementation.
   * @param[in,out] ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Formats the InterfaceAggregation enum.
   * @param[in] aggr The InterfaceAggregation to format.
   * @param[in,out] ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const astarte::device::mqtt::InterfaceAggregation& aggr, FormatContext& ctx) const {
    return astarte_fmt::format_to(ctx.out(), "{}", aggr.to_string());
  }
};

inline auto operator<<(std::ostream& out, const astarte::device::mqtt::InterfaceAggregation aggr)
    -> std::ostream& {
  out << astarte_fmt::format("{}", aggr);
  return out;
}

/// @brief astarte_fmt::formatter specialization for Interface.
template <>
struct astarte_fmt::formatter<astarte::device::mqtt::Interface> {
  /**
   * @brief Parses the format string. Default implementation.
   * @param[in,out] ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Formats the Interface object.
   * @param[in] interface The Interface to format.
   * @param[in,out] ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const astarte::device::mqtt::Interface& interface, FormatContext& ctx) const {
    auto out = ctx.out();

    astarte_fmt::format_to(out, "Interface {{\n");
    astarte_fmt::format_to(out, "  interface name: {}\n", interface.interface_name());
    astarte_fmt::format_to(out, "  major version: {}\n", interface.version_major());
    astarte_fmt::format_to(out, "  minor version: {}\n", interface.version_minor());
    astarte_fmt::format_to(out, "  interface type: {}\n", interface.interface_type());
    astarte_fmt::format_to(out, "  ownership: {}\n", interface.ownership());
    auto aggr = interface.aggregation();
    if (aggr) {
      astarte_fmt::format_to(out, "  aggregation: {}\n", *aggr);
    }
    auto desc = interface.description();
    if (desc) {
      astarte_fmt::format_to(out, "  description: {}\n", *desc);
    }
    auto doc = interface.doc();
    if (doc) {
      astarte_fmt::format_to(out, "  doc: {}\n", *doc);
    }
    astarte_fmt::format_to(out, "  mappings: ");
    astarte::device::utils::format_vector(out, interface.mappings());
    astarte_fmt::format_to(out, "\n");

    astarte_fmt::format_to(out, "}}\n");

    return out;
  }
};

inline auto operator<<(std::ostream& out, const astarte::device::mqtt::Interface& interface)
    -> std::ostream& {
  out << astarte_fmt::format("{}", interface);
  return out;
}

#endif  // ASTARTE_INTERFACE_H
