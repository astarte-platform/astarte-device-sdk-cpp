// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_OWNERSHIP_H
#define ASTARTE_DEVICE_SDK_OWNERSHIP_H

/**
 * @file astarte_device_sdk/ownership.hpp
 * @brief Ownership definitions for communication with Astarte.
 */

#include <astarte_device_sdk/exceptions.hpp>
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

// TODO: use static or move function definition to .cpp file
inline auto ownership_from_str(std::string ownership) -> AstarteOwnership {
  if (ownership == "device") {
    return AstarteOwnership::kDevice;
  } else if (ownership == "server") {
    return AstarteOwnership::kServer;
  } else {
    throw InvalidInterfaceOwnershipeException(
        std::format("interface ownershipe not valid: {}", ownership));
  }
}

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_OWNERSHIP_H
