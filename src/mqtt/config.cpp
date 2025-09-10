// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/mqtt/config.hpp"

#include <optional>
#include <string>
#include <string_view>

namespace AstarteDeviceSdk {

auto MqttConfig::realm() -> std::string_view { return realm_; }
auto MqttConfig::device_id() -> std::string_view { return device_id_; }
auto MqttConfig::pairing_url() -> std::string_view { return pairing_url_; }
auto MqttConfig::store_dir() -> std::string_view { return store_dir_; }
auto MqttConfig::credential_secret() -> std::string_view { return credential_secret_; }

}  // namespace AstarteDeviceSdk
