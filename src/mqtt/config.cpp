// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/mqtt/config.hpp"

#include <optional>
#include <string>
#include <string_view>

namespace AstarteDeviceSdk {

auto MqttConfig::realm() -> std::string_view { return realm_; }

/**
 * @brief Gets the configured device ID.
 * @return a string containing the device ID
 */
auto MqttConfig::device_id() -> std::string_view { return device_id_; }

/**
 * @brief Gets the configured Pairing API URL.
 * @return a string containing the pairing URL
 */
auto MqttConfig::pairing_url() -> std::string_view { return pairing_url_; }

/**
 * @brief Gets the configured local storage directory.
 * @return a string containing the store directory
 */
auto MqttConfig::store_dir() -> std::string_view { return store_dir_; }

// TODO: this method is only used for testing. Remove it once the connection to the MQTT will be
// implemented.
/**
 * @brief Gets the pairing token.
 * @return a string containing the pairing token, nullopt if the Credential is the
 * credential_secret
 */
auto MqttConfig::pairing_token() -> std::optional<std::string> {
  return credential_.is_pairing_token() ? std::make_optional(credential_.credential())
                                        : std::nullopt;
}

}  // namespace AstarteDeviceSdk
