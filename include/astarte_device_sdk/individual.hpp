// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_INDIVIDUAL_H
#define ASTARTE_DEVICE_SDK_INDIVIDUAL_H

/**
 * @file astarte_device_sdk/individual.hpp
 * @brief Astarte individual datastream class and its related methods.
 *
 * @details This file defines the container for individual datastream data used
 * within the Astarte device library to encapsulate single data points.
 */

#include <ostream>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/formatter.hpp"

namespace astarte::device {

/**
 * @brief Represents the Astarte individual datastream data.
 *
 * @details This class acts as a wrapper around the Astarte `Data` class, specifically
 * intended for use with interfaces using individual datastream aggregation.
 */
class DatastreamIndividual {
 public:
  /**
   * @brief Constructor for the DatastreamIndividual class.
   *
   * @param[in] data The wrapped Astarte data type.
   */
  explicit DatastreamIndividual(Data data);

  /**
   * @brief Gets the value contained within the object.
   *
   * @return A constant reference to the underlying data.
   */
  [[nodiscard]] auto get_value() const -> const Data&;

  /**
   * @brief Overloader for the comparison operator ==.
   *
   * @param[in] other The object to compare to.
   * @return True if the objects are equal, false otherwise.
   */
  [[nodiscard]] auto operator==(const DatastreamIndividual& other) const -> bool;

  /**
   * @brief Overloader for the comparison operator !=.
   *
   * @param[in] other The object to compare to.
   * @return True if the objects are different, false otherwise.
   */
  [[nodiscard]] auto operator!=(const DatastreamIndividual& other) const -> bool;

 private:
  Data data_;
};

}  // namespace astarte::device

/// @brief astarte_fmt::formatter specialization for astarte::device::DatastreamIndividual.
template <>
struct astarte_fmt::formatter<astarte::device::DatastreamIndividual> {
  /**
   * @brief Parses the format string.
   *
   * @param[in,out] ctx The parse context.
   * @return An iterator to the end of the parsed range.
   */
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) const {
    return ctx.begin();
  }

  /**
   * @brief Formats the DatastreamIndividual object.
   *
   * @param[in] data The DatastreamIndividual instance to format.
   * @param[in,out] ctx The format context.
   * @return An iterator to the end of the output context.
   */
  template <typename FormatContext>
  auto format(const astarte::device::DatastreamIndividual& data, FormatContext& ctx) const {
    return astarte_fmt::format_to(ctx.out(), "{}", data.get_value());
  }
};

/**
 * @brief Stream insertion operator for DatastreamIndividual.
 *
 * @param[in,out] out The output stream.
 * @param[in] data The DatastreamIndividual object to output.
 * @return A reference to the output stream.
 */
inline auto operator<<(std::ostream& out, const astarte::device::DatastreamIndividual& data)
    -> std::ostream& {
  out << astarte_fmt::format("{}", data);
  return out;
}

#endif  // ASTARTE_DEVICE_SDK_INDIVIDUAL_H
