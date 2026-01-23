// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_PERSISTENCE_H
#define ASTARTE_MQTT_PERSISTENCE_H

#include <spdlog/spdlog.h>

#include <filesystem>
#include <string>
#include <string_view>

#include "astarte_device_sdk/mqtt/errors.hpp"

namespace AstarteDeviceSdk {

class Persistence {
 public:
  static auto exists(const std::filesystem::path& file_path) -> bool;

  static auto read_from_file(const std::filesystem::path& file_path)
      -> astarte_tl::expected<std::string, AstarteError>;

  static auto write_to_file(const std::filesystem::path& file_path, std::string_view data)
      -> astarte_tl::expected<void, AstarteError>;

  static auto secure_shred_file(const std::string& path)
      -> astarte_tl::expected<void, AstarteError>;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_MQTT_PERSISTENCE_H
