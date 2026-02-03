// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_INTERFACE_H
#define ASTARTE_INTERFACE_H

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

namespace astarte::device {

/// @brief alias for nlohmann json
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
   * @brief Construct an InterfaceType from a specific Value.
   * @param val The interface type value.
   */
  explicit InterfaceType(Value val) : value_(val) {}

  /// @brief Default equality operator for comparing two InterfaceType objects.
  constexpr auto operator==(const InterfaceType& other) const -> bool = default;
  /// @brief Equality operator for comparing with a raw Value.
  constexpr auto operator==(Value val) const -> bool { return value_ == val; }

  /**
   * @brief Convert the interface type to its string representation.
   * @return A string_view containing "datastream", "property".
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
   * @brief Attempt to create an InterfaceType from a string.
   * @param str The string representation to parse.
   * @return An expected containing the InterfaceType on success, or an AstarteError on failure.
   */
  static auto try_from_str(std::string_view str)
      -> astarte_tl::expected<InterfaceType, AstarteError> {
    if (str == "datastream") {
      return InterfaceType(Value::kDatastream);
    }
    if (str == "properties") {
      return InterfaceType(Value::kProperty);
    }
    return astarte_tl::unexpected(
        AstarteInvalidInterfaceTypeError(astarte_fmt::format("interface type not valid: {}", str)));
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
   * @brief Construct an InterfaceAggregation from a specific Value.
   * @param val The interface aggregation value.
   */
  explicit InterfaceAggregation(Value val) : value_(val) {}

  /// @brief Default equality operator for comparing two InterfaceAggregation objects.
  constexpr auto operator==(const InterfaceAggregation& other) const -> bool = default;
  /// @brief Equality operator for comparing with a raw Value.
  constexpr auto operator==(Value val) const -> bool { return value_ == val; }

  /**
   * @brief Convert the interface aggregation to its string representation.
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
   * @brief Attempt to create an InterfaceAggregation from a string.
   * @param str The string representation to parse.
   * @return An expected containing the InterfaceAggregation on success, or an AstarteError on
   * failure.
   */
  static auto try_from_str(std::string_view str)
      -> astarte_tl::expected<InterfaceAggregation, AstarteError> {
    if (str == "individual") {
      return InterfaceAggregation(Value::kIndividual);
    }
    if (str == "object") {
      return InterfaceAggregation(Value::kObject);
    }
    return astarte_tl::unexpected(AstarteInvalidInterfaceAggregationError(
        astarte_fmt::format("interface aggregation not valid: {}", str)));
  }

  /**
   * @brief Check if the aggregation is of type Individual.
   * @return true if the aggregation is individual, false otherwise.
   */
  [[nodiscard]] auto is_individual() const -> bool { return value_ == Value::kIndividual; }

 private:
  Value value_;
};

/// @brief Represents a parsed Astarte interface.
class Interface {
 public:
  /**
   * @brief Try to convert a json into an Interface object.
   * @param interface json representation of the Astarte interface.
   * @return An Interface object containg all the parsed Astarte interface information.
   */
  static auto try_from_json(const json& interface) -> astarte_tl::expected<Interface, AstarteError>;

  /**
   * @brief Move constructor.
   *
   * @param other The Interface object to move from.
   */
  Interface(Interface&& other) noexcept = default;

  /**
   * @brief Move assignment operator.
   *
   * @param other The Interface object to move from.
   * @return A reference to this Interface object.
   */
  auto operator=(Interface&& other) noexcept -> Interface& = default;

  /// @brief Deleted copy constructor.
  Interface(const Interface&) = delete;

  /// @brief Deleted copy assignment operator.
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

  /// @return The quality of the interface.
  [[nodiscard]] auto ownership() const -> AstarteOwnership { return ownership_; }

  /// @return The aggregation of the mappings (Individual or Object), if present.
  [[nodiscard]] auto aggregation() const -> const std::optional<InterfaceAggregation>& {
    return aggregation_;
  }

  /// @return The optional description.
  [[nodiscard]] auto description() const -> const std::optional<std::string>& {
    return description_;
  }

  /// @return  The documentation string.
  [[nodiscard]] auto doc() const -> const std::optional<std::string>& { return doc_; }

  /// @return The vector of mappings defined for this interface.
  [[nodiscard]] auto mappings() const -> const std::vector<Mapping>& { return mappings_; }

  /**
   * @brief Retrieve the mapping associated to a given path if it exists.
   *
   * @param path the Astarte interface path.
   * @return a pointer to the mapping associated with the path, an error otherwise.
   */
  [[nodiscard]] auto get_mapping(std::string_view path) const
      -> astarte_tl::expected<const Mapping*, AstarteError>;

  /**
   * @brief Validate an Astarte individual.
   *
   * @param path the Astarte interface path.
   * @param data the value to validate.
   * @param timestamp a pointer to the timestamp poiting out when the data is sent.
   * @return an error if the falidation fails, nothing otherwise.
   */
  auto validate_individual(std::string_view path, const AstarteData& data,
                           const std::chrono::system_clock::time_point* timestamp) const
      -> astarte_tl::expected<void, AstarteError>;

  /**
   * @brief Validate an Astarte object.
   *
   * @param common_path common path of the Astarte interface enpoints.
   * @param object the Astarte object data to validate.
   * @param timestamp a pointer to the timestamp pointing out when the data is sent.
   * @return an error if the falidation fails, nothing otherwise.
   */
  auto validate_object(std::string_view common_path, const AstarteDatastreamObject& object,
                       const std::chrono::system_clock::time_point* timestamp) const
      -> astarte_tl::expected<void, AstarteError>;

  /**
   * @brief Get the MQTT QoS from a certain mapping endpoint.
   *
   * @param path the Astarte interface path.
   * @return the QoS value, an error otherwise.
   */
  [[nodiscard]] auto get_qos(std::string_view path) const
      -> astarte_tl::expected<uint8_t, AstarteError>;

 private:
  Interface(std::string interface_name, uint32_t version_major, uint32_t version_minor,
            InterfaceType interface_type, AstarteOwnership ownership,
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
  AstarteOwnership ownership_;
  std::optional<InterfaceAggregation> aggregation_;
  std::optional<std::string> description_;
  std::optional<std::string> doc_;
  std::vector<Mapping> mappings_;
};

}  // namespace astarte::device

// ------------------------------------------------------------------------------------------------
// FORAMATTING
// ------------------------------------------------------------------------------------------------

/// @brief astarte_fmt::formatter specialization for InterfaceType.
template <>
struct astarte_fmt::formatter<astarte::device::InterfaceType> {
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
   * @brief Format the InterfaceType enum.
   * @param msg The InterfaceType to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const astarte::device::InterfaceType& typ, FormatContext& ctx) const {
    return astarte_fmt::format_to(ctx.out(), "{}", typ.to_string());
  }
};

inline auto operator<<(std::ostream& out, const astarte::device::InterfaceType typ)
    -> std::ostream& {
  out << astarte_fmt::format("{}", typ);
  return out;
}

/// @brief astarte_fmt::formatter specialization for InterfaceAggregation.
template <>
struct astarte_fmt::formatter<astarte::device::InterfaceAggregation> {
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
   * @brief Format the InterfaceAggregation enum.
   * @param msg The InterfaceAggregation to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const astarte::device::InterfaceAggregation& aggr, FormatContext& ctx) const {
    return astarte_fmt::format_to(ctx.out(), "{}", aggr.to_string());
  }
};

inline auto operator<<(std::ostream& out, const astarte::device::InterfaceAggregation aggr)
    -> std::ostream& {
  out << astarte_fmt::format("{}", aggr);
  return out;
}

/// @brief astarte_fmt::formatter specialization for Interface.
template <>
struct astarte_fmt::formatter<astarte::device::Interface> {
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
   * @brief Format the Interface object.
   * @param msg The Interface to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const astarte::device::Interface& interface, FormatContext& ctx) const {
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

inline auto operator<<(std::ostream& out, const astarte::device::Interface& interface)
    -> std::ostream& {
  out << astarte_fmt::format("{}", interface);
  return out;
}

#endif  // ASTARTE_INTERFACE_H
