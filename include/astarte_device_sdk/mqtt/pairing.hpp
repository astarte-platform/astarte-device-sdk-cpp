// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_PAIRING_H
#define ASTARTE_MQTT_PAIRING_H

/**
 * @file astarte_device_sdk/mqtt/pairing.hpp
 * @brief Methods to handle Astarte device pairing.
 */

#include <optional>
#include <string_view>

namespace AstarteDeviceSdk {
auto register_device(std::string_view pairing_jwt, std::string_view pairing_url,
                     std::string_view realm, std::string_view device_id, int timeout_ms = 0)
    -> std::optional<std::string>;
}

#endif  // ASTARTE_MQTT_PAIRING_H
