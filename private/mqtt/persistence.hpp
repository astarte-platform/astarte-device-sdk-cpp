// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_PERSISTENCE_H
#define ASTARTE_MQTT_PERSISTENCE_H

/**
 * @file private/mqtt/persistence.hpp
 * @brief Persistence layer utilities.
 *
 * @details This file defines the `Persistence` class, offering static methods for basic
 * filesystem operations such as reading, writing, and securely deleting files, used
 * for storing configuration and credentials.
 */

#include <spdlog/spdlog.h>

#include <filesystem>
#include <string>
#include <string_view>

#include "astarte_device_sdk/mqtt/errors.hpp"

namespace astarte::device::mqtt {

/**
 * @brief Utility class for file system persistence operations.
 *
 * @details This class provides static helper methods to handle reading, writing, checking
 * existence, and securely deleting files used by the SDK for persistence.
 */
class Persistence {
 public:
  /**
   * @brief Checks if a file exists at the specified path.
   *
   * @param[in] file_path The path to the file to check.
   * @return True if the file exists, false otherwise.
   */
  static auto exists(const std::filesystem::path& file_path) -> bool;

  /**
   * @brief Reads the content of a file into a string.
   *
   * @param[in] file_path The path to the file to read.
   * @return An expected containing the file content on success or Error on failure.
   */
  static auto read_from_file(const std::filesystem::path& file_path)
      -> astarte_tl::expected<std::string, Error>;

  /**
   * @brief Writes string data to a specific file.
   *
   * @param[in] file_path The path where the file will be written.
   * @param[in] data The string data to write.
   * @return An expected containing void on success or Error on failure.
   */
  static auto write_to_file(const std::filesystem::path& file_path, std::string_view data)
      -> astarte_tl::expected<void, Error>;

  /**
   * @brief Securely removes a file from the filesystem.
   *
   * @param[in] path The path of the file to remove.
   * @return An expected containing void on success or Error on failure.
   */
  static auto secure_shred_file(const std::string& path) -> astarte_tl::expected<void, Error>;
};

}  // namespace astarte::device::mqtt

#endif  // ASTARTE_MQTT_PERSISTENCE_H
