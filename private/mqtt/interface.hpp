// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_INTERFACE_H
#define ASTARTE_INTERFACE_H

#include <spdlog/spdlog.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <format>
#include <functional>
#include <map>
#include <nlohmann/json.hpp>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/errors.hpp"
#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/object.hpp"
#include "astarte_device_sdk/ownership.hpp"
#include "astarte_device_sdk/type.hpp"
#include "mqtt/mapping.hpp"

namespace AstarteDeviceSdk {

/**
 * @brief alias for nlohmann json
 */
using json = nlohmann::json;

/**
 * @brief Define the type of an Astarte interface.
 */
enum InterfaceType : uint8_t {
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
enum InterfaceAggregation : uint8_t {
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
 * @brief Convert a string to an InterfaceAggregation enum.
 *
 * @param aggr The string representation of the aggregation (e.g., "individual", "object").
 * @return The corresponding InterfaceAggregation enum value, an error if the string is not a valid
 * aggregation type.
 */
auto aggregation_from_str(std::string aggr)
    -> astarte_tl::expected<InterfaceAggregation, AstarteError>;

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

  /**
   * @brief Deleted copy constructor.
   */
  Interface(const Interface&) = delete;

  /**
   * @brief Deleted copy assignment operator.
   */
  auto operator=(const Interface&) -> Interface& = delete;

  /**
   * @brief Destructor.
   */
  ~Interface() = default;

  /**
   * @return The name of the interface.
   */
  [[nodiscard]] auto interface_name() const -> const std::string& { return interface_name_; }

  /**
   * @return The Major version qualifier.
   */
  [[nodiscard]] auto version_major() const -> uint32_t { return version_major_; }

  /**
   * @return The Minor version qualifier.
   */
  [[nodiscard]] auto version_minor() const -> uint32_t { return version_minor_; }

  /**
   * @return The type of this Interface (Datastream or Property).
   */
  [[nodiscard]] auto interface_type() const -> InterfaceType { return interface_type_; }

  /**
   * @return The quality of the interface.
   */
  [[nodiscard]] auto ownership() const -> AstarteOwnership { return ownership_; }

  /**
   * @return The aggregation of the mappings (Individual or Object), if present.
   */
  [[nodiscard]] auto aggregation() const -> const std::optional<InterfaceAggregation>& {
    return aggregation_;
  }

  /**
   * @return The optional description.
   */
  [[nodiscard]] auto description() const -> const std::optional<std::string>& {
    return description_;
  }

  /**
   * @return The documentation string.
   */
  [[nodiscard]] auto doc() const -> const std::optional<std::string>& { return doc_; }

  /**
   * @return The vector of mappings defined for this interface.
   */
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

}  // namespace AstarteDeviceSdk

// ------------------------------------------------------------------------------------------------
// FORAMATTING
// ------------------------------------------------------------------------------------------------

/**
 * @brief astarte_fmt::formatter specialization for InterfaceType.
 */

template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::InterfaceType> {
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
  auto format(const AstarteDeviceSdk::InterfaceType& typ, FormatContext& ctx) const {
    auto out = ctx.out();

    switch (typ) {
      case AstarteDeviceSdk::InterfaceType::kDatastream:
        astarte_fmt::format_to(out, "datastream");
        break;

      case AstarteDeviceSdk::InterfaceType::kProperty:
        astarte_fmt::format_to(out, "property");
        break;
    }

    return out;
  }
};

inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::InterfaceType typ)
    -> std::ostream& {
  out << astarte_fmt::format("{}", typ);
  return out;
}

/**
 * @brief astarte_fmt::formatter specialization for InterfaceAggregation.
 */
template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::InterfaceAggregation> {
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
  auto format(const AstarteDeviceSdk::InterfaceAggregation& aggr, FormatContext& ctx) const {
    auto out = ctx.out();

    switch (aggr) {
      case AstarteDeviceSdk::InterfaceAggregation::kIndividual:
        astarte_fmt::format_to(out, "individual");
        break;

      case AstarteDeviceSdk::InterfaceAggregation::kObject:
        astarte_fmt::format_to(out, "object");
        break;
    }

    return out;
  }
};

inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::InterfaceAggregation aggr)
    -> std::ostream& {
  out << astarte_fmt::format("{}", aggr);
  return out;
}

/**
 * @brief astarte_fmt::formatter specialization for Interface.
 */
template <>
struct astarte_fmt::formatter<AstarteDeviceSdk::Interface> {
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
  auto format(const AstarteDeviceSdk::Interface& interface, FormatContext& ctx) const {
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
    AstarteDeviceSdk::utils::format_vector(out, interface.mappings());
    astarte_fmt::format_to(out, "\n");

    astarte_fmt::format_to(out, "}}\n");

    return out;
  }
};

inline auto operator<<(std::ostream& out, const AstarteDeviceSdk::Interface& interface)
    -> std::ostream& {
  out << astarte_fmt::format("{}", interface);
  return out;
}

#endif  // ASTARTE_INTERFACE_H
