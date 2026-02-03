// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_OWNERSHIP_H
#define ASTARTE_DEVICE_SDK_OWNERSHIP_H

/**
 * @file astarte_device_sdk/ownership.hpp
 * @brief Ownership definitions for communication with Astarte.
 */

#include <cstdint>
#include <ostream>
#include <string>

#include "astarte_device_sdk/errors.hpp"
#include "astarte_device_sdk/formatter.hpp"

namespace astarte::device {

/** @brief Possible Astarte ownership. */
enum Ownership : int8_t {
  /** @brief Ownership is retained by the device. */
  kDevice,
  /** @brief Ownership is retained by the Astarte cloud server. */
  kServer
};

/**
 * @brief Convert a string to an Ownership enum.
 *
 * @param ownership The string representation of the interface ownership.
 * @return The corresponding Ownership enum value, an error if the string is not a valid
 * ownership.
 */
inline auto ownership_from_str(const std::string& ownership)
    -> astarte_tl::expected<Ownership, Error> {
  if (ownership == "device") {
    return Ownership::kDevice;
  }
  if (ownership == "server") {
    return Ownership::kServer;
  }
  return astarte_tl::unexpected(
      InvalidInterfaceOwnershipeError("interface ownershipe not valid: " + ownership));
}

}  // namespace astarte::device

/**
 * @brief astarte_fmt::formatter specialization for astarte::device::AstarteStoredProperty.
 */
template <>
struct astarte_fmt::formatter<astarte::device::Ownership> {
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
   * @brief Format the astarte::device:: object.
   * @param ownership The astarte::device:: to format.
   * @param ctx The format context.
   * @return An iterator to the end of the output.
   */
  template <typename FormatContext>
  auto format(const astarte::device::Ownership& ownership, FormatContext& ctx) const {
    auto out = ctx.out();

    switch (ownership) {
      case astarte::device::Ownership::kDevice:
        astarte_fmt::format_to(out, "device");
        break;
      case astarte::device::Ownership::kServer:
        astarte_fmt::format_to(out, "server");
        break;
    }

    return out;
  }
};

/**
 * @brief Stream insertion operator for Ownership.
 * @param out The output stream.
 * @param ownership The Ownership enum to output.
 * @return Reference to the output stream.
 */
inline auto operator<<(std::ostream& out, const astarte::device::Ownership ownership)
    -> std::ostream& {
  out << astarte_fmt::format("{}", ownership);
  return out;
}

#endif  // ASTARTE_DEVICE_SDK_OWNERSHIP_H
