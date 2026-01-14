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

namespace AstarteDeviceSdk {

/**
 * @brief Specific error for when an operation failed due to an internal error.
 */
class AstarteMqttError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteMqttError(std::string_view message);
  /**
   * @brief Error constructor with no message.
   */
  explicit AstarteMqttError();
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteMqttError(std::string_view message, const AstarteError& other);
  /**
   * @brief Nested error constructor with no message.
   * @param other The error to nest.
   */
  explicit AstarteMqttError(const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteMqttError";
};

/**
 * @brief Specific error for when an operation failed due to an internal error.
 */
class AstartePairingApiError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstartePairingApiError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstartePairingApiError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstartePairingApiError";
};

/**
 * @brief Specific error for when an operation failed due to an internal error.
 */
class AstarteInvalidUrlError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteInvalidUrlError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteInvalidUrlError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteInvalidUrlError";
};

/**
 * @brief Specific error for when an operation failed due to an internal error.
 */
class AstarteDeviceRegistrationError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteDeviceRegistrationError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteDeviceRegistrationError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteDeviceRegistrationError";
};

/**
 * @brief Specific error for when an operation failed due to an internal error.
 */
class AstarteJsonParsingError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteJsonParsingError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteJsonParsingError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteJsonParsingError";
};

/**
 * @brief Specific error for when an operation failed due to an internal error.
 */
class AstarteRetrieveBrokerUrlError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteRetrieveBrokerUrlError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteRetrieveBrokerUrlError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteRetrieveBrokerUrlError";
};

/**
 * @brief Specific error for when reading from a file.
 */
class AstarteReadCredentialError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteReadCredentialError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteReadCredentialError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteReadCredentialError";
};

/**
 * @brief Specific error for when writing to a file.
 */
class AstarteWriteCredentialError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteWriteCredentialError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteWriteCredentialError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteWriteCredentialError";
};

/**
 * @brief Specific error for when the configuration to connect the devite to Astarte is invalid.
 */
class AstartePairingConfigError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstartePairingConfigError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstartePairingConfigError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstartePairingConfigError";
};

/**
 * @brief Specific error for when an operation failed due to an internal error.
 */
class AstarteCryptoError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteCryptoError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteCryptoError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteCryptoError";
};

/**
 * @brief Specific error for when a UUID operation failed.
 */
class AstarteUuidError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteUuidError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteUuidError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteUuidError";
};

/**
 * @brief Specific error for when an operation failed due to an internal error.
 */
class AstarteHttpError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteHttpError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteHttpError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteHttpError";
};

/**
 * @brief Specific mqtt conncetion error.
 */
class AstarteMqttConnectionError : public AstarteErrorBase {
 public:
  /**
   * @brief Standard error constructor.
   * @param message The error message.
   */
  explicit AstarteMqttConnectionError(std::string_view message);
  /**
   * @brief Nested error constructor.
   * @param message The error message.
   * @param other The error to nest.
   */
  explicit AstarteMqttConnectionError(std::string_view message, const AstarteError& other);

 private:
  static constexpr std::string_view k_type_ = "AstarteMqttConnectionError";
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_DEVICE_SDK_MQTT_ERRORS_H
