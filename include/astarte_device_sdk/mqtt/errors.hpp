// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DEVICE_SDK_MQTT_ERRORS_H
#define ASTARTE_DEVICE_SDK_MQTT_ERRORS_H

#include <memory>
#include <optional>
#include <string>
#include <variant>

// We accept this circular inclusion as it's required for this file to be included without the
// generic errors header file (and vice-versa).
// NOLINTNEXTLINE(misc-header-include-cycle)
#include "astarte_device_sdk/errors.hpp"

namespace astarte::device::mqtt {

/**
 * @brief Specific error for when an operation failed due to an internal error.
 */
class MqttError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit MqttError(std::string_view message);
  /**
   * @brief Error constructor with no message.
   */
  explicit MqttError();
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit MqttError(std::string_view message, const Error& other);
  /**
   * @brief Nested error constructor with no message.
   * @param other The error to nest.
   */
  explicit MqttError(const Error& other);

 private:
  static constexpr std::string_view k_type_ = "MqttError";
};

/**
 * @brief Specific error for when an operation failed due to an internal error.
 */
class PairingApiError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit PairingApiError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit PairingApiError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "PairingApiError";
};

/**
 * @brief Specific error for when an operation failed due to an internal error.
 */
class InvalidUrlError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit InvalidUrlError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit InvalidUrlError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "InvalidUrlError";
};

/**
 * @brief Specific error for when an operation failed due to an internal error.
 */
class DeviceRegistrationError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit DeviceRegistrationError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit DeviceRegistrationError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "DeviceRegistrationError";
};

/**
 * @brief Specific error for when an operation failed due to an internal error.
 */
class JsonParsingError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit JsonParsingError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit JsonParsingError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "JsonParsingError";
};

/**
 * @brief Specific error for when an operation failed due to an internal error.
 */
class RetrieveBrokerUrlError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit RetrieveBrokerUrlError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit RetrieveBrokerUrlError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "RetrieveBrokerUrlError";
};

/**
 * @brief Specific error for when reading from a file.
 */
class ReadCredentialError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit ReadCredentialError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit ReadCredentialError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "ReadCredentialError";
};

/**
 * @brief Specific error for when writing to a file.
 */
class WriteCredentialError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit WriteCredentialError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit WriteCredentialError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "WriteCredentialError";
};

/**
 * @brief Specific error for when the configuration to connect the devite to Astarte is invalid.
 */
class PairingConfigError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit PairingConfigError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit PairingConfigError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "PairingConfigError";
};

/**
 * @brief Specific error for when an operation failed due to an internal error.
 */
class CryptoError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit CryptoError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit CryptoError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "CryptoError";
};

/**
 * @brief Specific error for when a UUID operation failed.
 */
class UuidError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit UuidError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit UuidError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "UuidError";
};

/**
 * @brief Specific error for when an operation failed due to an internal error.
 */
class HttpError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit HttpError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit HttpError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "HttpError";
};

/**
 * @brief Specific mqtt conncetion error.
 */
class MqttConnectionError : public ErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit MqttConnectionError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit MqttConnectionError(std::string_view message, const Error& other);

 private:
  static constexpr std::string_view k_type_ = "MqttConnectionError";
};

}  // namespace astarte::device::mqtt

#endif  // ASTARTE_DEVICE_SDK_MQTT_ERRORS_H
