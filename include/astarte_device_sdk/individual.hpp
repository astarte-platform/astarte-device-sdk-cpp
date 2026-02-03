// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_INDIVIDUAL_H
#define ASTARTE_DEVICE_SDK_INDIVIDUAL_H

/**
 * @file astarte_device_sdk/individual.hpp
 * @brief Astarte individual datastream class and its related methods.
 */

#include <ostream>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/formatter.hpp"

namespace astarte::device {

/** @brief Representing the Astarte individual datastream data. */
class AstarteDatastreamIndividual {
 public:
  /**
   * @brief Constructor for the AstarteDatastreamIndividual class.
   * @param data The wrapped Astarte data type.
   */
  explicit AstarteDatastreamIndividual(Data data);
  /**
   * @brief Get the value contained within the object.
   * @return A constant reference to the data.
   */
  [[nodiscard]] auto get_value() const -> const Data&;
  /**
   * @brief Overloader for the comparison operator ==.
   * @param other The object to compare to.
   * @return True when equal, false otherwise.
   */
  [[nodiscard]] auto operator==(const AstarteDatastreamIndividual& other) const -> bool;
  /**
   * @brief Overloader for the comparison operator !=.
   * @param other The object to compare to.
   * @return True when different, false otherwise.
   */
  [[nodiscard]] auto operator!=(const AstarteDatastreamIndividual& other) const -> bool;

 private:
  Data data_;
};

}  // namespace astarte::device

/**
 * @brief astarte_fmt::formatter specialization for
 * astarte::device::AstarteDatastreamIndividual.
 */
template <>
struct astarte_fmt::formatter<astarte::device::AstarteDatastreamIndividual> {
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
   * @brief Format the AstarteDatastreamIndividual object.
   * @param data The AstarteDatastreamIndividual to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const astarte::device::AstarteDatastreamIndividual& data, FormatContext& ctx) const {
    return astarte_fmt::format_to(ctx.out(), "{}", data.get_value());
  }
};

/**
 * @brief Stream insertion operator for AstarteDatastreamIndividual.
 * @param out The output stream.
 * @param data The AstarteDatastreamIndividual object to output.
 * @return Reference to the output stream.
 */
inline auto operator<<(std::ostream& out, const astarte::device::AstarteDatastreamIndividual& data)
    -> std::ostream& {
  out << astarte_fmt::format("{}", data);
  return out;
}

#endif  // ASTARTE_DEVICE_SDK_INDIVIDUAL_H
