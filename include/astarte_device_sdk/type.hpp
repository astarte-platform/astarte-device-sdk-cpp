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

namespace AstarteDeviceSdk {

/** @brief Possible Astarte types. */
enum AstarteType : int8_t {
  /** @brief Binary blob Astarte type. */
  BINARYBLOB,
  /** @brief Boolean Astarte type. */
  BOOLEAN,
  /** @brief Date-time Astarte type. */
  DATETIME,
  /** @brief Double Astarte type. */
  DOUBLE,
  /** @brief Integer Astarte type. */
  INTEGER,
  /** @brief Long integer Astarte type. */
  LONGINTEGER,
  /** @brief String Astarte type. */
  STRING,
  /** @brief Binary blob array Astarte type. */
  BINARYBLOBARRAY,
  /** @brief Boolean array Astarte type. */
  BOOLEANARRAY,
  /** @brief Datetime array Astarte type. */
  DATETIMEARRAY,
  /** @brief Double array Astarte type. */
  DOUBLEARRAY,
  /** @brief Integer array Astarte type. */
  INTEGERARRAY,
  /** @brief Long integer array Astarte type. */
  LONGINTEGERARRAY,
  /** @brief String array Astarte type. */
  STRINGARRAY
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_TYPE_H
