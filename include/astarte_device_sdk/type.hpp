// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_TYPE_H
#define ASTARTE_DEVICE_SDK_TYPE_H

/**
 * @file astarte_device_sdk/type.hpp
 * @brief Types definitions for communication with Astarte.
 */

#include <cstdint>
#include <string_view>

namespace AstarteDeviceSdk {

/** @brief Possible Astarte types. */
enum AstarteType : int8_t {
  /** @brief Binary blob Astarte type. */
  kBinaryBlob,
  /** @brief Boolean Astarte type. */
  kBoolean,
  /** @brief Date-time Astarte type. */
  kDatetime,
  /** @brief Double Astarte type. */
  kDouble,
  /** @brief Integer Astarte type. */
  kInteger,
  /** @brief Long integer Astarte type. */
  kLongInteger,
  /** @brief String Astarte type. */
  kString,
  /** @brief Binary blob array Astarte type. */
  kBinaryBlobArray,
  /** @brief Boolean array Astarte type. */
  kBooleanArray,
  /** @brief Datetime array Astarte type. */
  kDatetimeArray,
  /** @brief Double array Astarte type. */
  kDoubleArray,
  /** @brief Integer array Astarte type. */
  kIntegerArray,
  /** @brief Long integer array Astarte type. */
  kLongIntegerArray,
  /** @brief String array Astarte type. */
  kStringArray
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_TYPE_H
