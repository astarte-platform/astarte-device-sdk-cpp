// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_PROPERTY_H
#define ASTARTE_DEVICE_SDK_PROPERTY_H

/**
 * @file astarte_device_sdk/individual.hpp
 * @brief Astarte individual datastream class and its related methods.
 */

#include <optional>
#include <ostream>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/formatter.hpp"

namespace astarte::device {

/** @brief Representing the Astarte individual datastream data. */
class AstartePropertyIndividual {
 public:
  /**
   * @brief Constructor for the AstarteDatastreamIndividual class.
   * @param data The wrapped Astarte data type.
   */
  explicit AstartePropertyIndividual(const std::optional<AstarteData>& data);
  /**
   * @brief Get the value contained within the object.
   * @return A constant reference to the data, if any.
   */
  [[nodiscard]] auto get_value() const -> const std::optional<AstarteData>&;
  /**
   * @brief Overloader for the comparison operator ==.
   * @param other The object to compare to.
   * @return True when equal, false otherwise.
   */
  [[nodiscard]] auto operator==(const AstartePropertyIndividual& other) const -> bool;
  /**
   * @brief Overloader for the comparison operator !=.
   * @param other The object to compare to.
   * @return True when different, false otherwise.
   */
  [[nodiscard]] auto operator!=(const AstartePropertyIndividual& other) const -> bool;

 private:
  std::optional<AstarteData> data_;
};

}  // namespace astarte::device

/**
 * @brief astarte_fmt::formatter specialization for
 * astarte::device::AstartePropertyIndividual.
 */
template <>
struct astarte_fmt::formatter<astarte::device::AstartePropertyIndividual> {
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
   * @brief Format the AstartePropertyIndividual object.
   * @param data The AstartePropertyIndividual to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const astarte::device::AstartePropertyIndividual& data, FormatContext& ctx) const {
    const auto& opt_data = data.get_value();
    if (opt_data.has_value()) {
      return astarte_fmt::format_to(ctx.out(), "{}", opt_data.value());
    }

    return ctx.out();
  }
};

/**
 * @brief Stream insertion operator for AstartePropertyIndividual.
 * @param out The output stream.
 * @param data The AstartePropertyIndividual object to output.
 * @return Reference to the output stream.
 */
inline auto operator<<(std::ostream& out, const astarte::device::AstartePropertyIndividual& data)
    -> std::ostream& {
  out << astarte_fmt::format("{}", data);
  return out;
}

#endif  // ASTARTE_DEVICE_SDK_PROPERTY_H
