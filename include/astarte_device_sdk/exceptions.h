// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_EXCEPTIONS_H
#define ASTARTE_DEVICE_SDK_EXCEPTIONS_H

/**
 * @file astarte_device_sdk/exceptions.h
 * @brief Exceptions raised by the Astarte device.
 */

#include <exception>
#include <string>

namespace AstarteDeviceSdk {

/**
 * @brief Base exception class for the Astarte device library.
 */
class AstarteException : public std::exception {
 public:
  /**
   * @brief Constructor for the Astarte exception.
   * @param message The exception message.
   */
  explicit AstarteException(std::string message);
  /**
   * @brief Implementation of the virtual method what from the partent std::exception class.
   * @return The exception message.
   */
  [[nodiscard]] auto what() const noexcept -> const char* override;

 private:
  std::string message_;
};

/**
 * @brief Specific exception for when a file cannot be opened.
 */
class AstarteFileOpenException : public AstarteException {
 public:
  /**
   * @brief Constructor for the Astarte file open exception.
   * @param filename The file which could not be opened.
   */
  explicit AstarteFileOpenException(const std::string& filename);
  /**
   * @brief Get the file name for the file which could not be opened.
   * @return The file name for the file whick could not be opened.
   */
  [[nodiscard]] auto get_filename() const -> const std::string&;

 private:
  std::string filename_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_EXCEPTIONS_H
