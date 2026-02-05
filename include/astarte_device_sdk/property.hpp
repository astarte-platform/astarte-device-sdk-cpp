// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_PROPERTY_H
#define ASTARTE_DEVICE_SDK_PROPERTY_H

/**
 * @file astarte_device_sdk/property.hpp
 * @brief Astarte individual property class and its related methods.
 */

#include <optional>
#include <ostream>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/formatter.hpp"

namespace astarte::device {

/// @brief Representing the Astarte individual property data.
class PropertyIndividual {
 public:
  /**
   * @brief Constructor for the PropertyIndividual class.
   *
   * @param[in] data The wrapped Astarte data type.
   */
  explicit PropertyIndividual(const std::optional<Data>& data);

  /**
   * @brief Gets the value contained within the object.
   *
   * @return A constant reference to the data, if any.
   */
  [[nodiscard]] auto get_value() const -> const std::optional<Data>&;

  /**
   * @brief Overloader for the comparison operator ==.
   *
   * @param[in] other The object to compare to.
   * @return True when equal, false otherwise.
   */
  [[nodiscard]] auto operator==(const PropertyIndividual& other) const -> bool;

  /**
   * @brief Overloader for the comparison operator !=.
   *
   * @param[in] other The object to compare to.
   * @return True when different, false otherwise.
   */
  [[nodiscard]] auto operator!=(const PropertyIndividual& other) const -> bool;

 private:
  std::optional<Data> data_;
};

}  // namespace astarte::device

/// @brief astarte_fmt::formatter specialization for astarte::device::PropertyIndividual.
template <>
struct astarte_fmt::formatter<astarte::device::PropertyIndividual> {
  /**
   * @brief Parses the format string. Default implementation.
   *
   * @param[in,out] ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Formats the PropertyIndividual object.
   *
   * @param[in] data The PropertyIndividual to format.
   * @param[in,out] ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const astarte::device::PropertyIndividual& data, FormatContext& ctx) const {
    const auto& opt_data = data.get_value();
    if (opt_data.has_value()) {
      return astarte_fmt::format_to(ctx.out(), "{}", opt_data.value());
    }

    return ctx.out();
  }
};

/**
 * @brief Stream insertion operator for PropertyIndividual.
 *
 * @param[in,out] out The output stream.
 * @param[in] data The PropertyIndividual object to output.
 * @return Reference to the output stream.
 */
inline auto operator<<(std::ostream& out, const astarte::device::PropertyIndividual& data)
    -> std::ostream& {
  out << astarte_fmt::format("{}", data);
  return out;
}

#endif  // ASTARTE_DEVICE_SDK_PROPERTY_H
