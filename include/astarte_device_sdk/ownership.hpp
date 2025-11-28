// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_OWNERSHIP_H
#define ASTARTE_DEVICE_SDK_OWNERSHIP_H

/**
 * @file astarte_device_sdk/ownership.hpp
 * @brief Ownership definitions for communication with Astarte.
 */

#include <astarte_device_sdk/errors.hpp>
#include <cstdint>
#include <format>
#include <string>

namespace AstarteDeviceSdk {

/** @brief Possible Astarte ownership. */
enum AstarteOwnership : int8_t {
  /** @brief Ownership is retained by the device. */
  kDevice,
  /** @brief Ownership is retained by the Astarte cloud server. */
  kServer
};

/**
 * @brief Convert a string to an AstarteOwnership enum.
 *
 * @param ownership The string representation of the interface ownership.
 * @return The corresponding AstarteOwnership enum value, an error if the string is not a valid
 * ownership.
 */
// TODO: use static or move function definition to .cpp file
inline auto ownership_from_str(std::string ownership)
    -> astarte_tl::expected<AstarteOwnership, AstarteError> {
  if (ownership == "device") {
    return AstarteOwnership::kDevice;
  } else if (ownership == "server") {
    return AstarteOwnership::kServer;
  } else {
    return astarte_tl::unexpected(
        AstarteInvalidInterfaceOwnershipeError("interface ownershipe not valid: " + ownership));
  }
}

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_OWNERSHIP_H
