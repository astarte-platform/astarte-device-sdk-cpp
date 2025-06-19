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

namespace AstarteDeviceSdk {

/** @brief Possible Astarte ownership. */
enum AstarteOwnership : int8_t {
  /** @brief Ownership is retained by the device. */
  kDevice,
  /** @brief Ownership is retained by the Astarte cloud server. */
  kServer
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_OWNERSHIP_H
