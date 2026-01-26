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

/**
 * @brief Utility class for file system persistence operations.
 *
 * This class provides static helper methods to handle reading, writing, checking existence,
 * and securely deleting files used by the SDK for persistence.
 */
class Persistence {
 public:
  /**
   * @brief Checks if a file exists at the specified path.
   *
   * @param file_path The path to the file to check.
   * @return True if the file exists, false otherwise.
   */
  static auto exists(const std::filesystem::path& file_path) -> bool;

  /**
   * @brief Reads the content of a file into a string.
   *
   * @param file_path The path to the file to read.
   * @return The content of the file as a string, or an error if the operation fails.
   */
  static auto read_from_file(const std::filesystem::path& file_path)
      -> astarte_tl::expected<std::string, AstarteError>;

  /**
   * @brief Writes string data to a specific file.
   *
   * @param file_path The path where the file will be written.
   * @param data The string data to write.
   * @return An error if the write operation fails.
   */
  static auto write_to_file(const std::filesystem::path& file_path, std::string_view data)
      -> astarte_tl::expected<void, AstarteError>;

  /**
   * @brief Securely removes a file from the filesystem.
   *
   * @param path The path of the file to remove.
   * @return An error if the operation fails.
   */
  static auto secure_shred_file(const std::string& path)
      -> astarte_tl::expected<void, AstarteError>;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_MQTT_PERSISTENCE_H
