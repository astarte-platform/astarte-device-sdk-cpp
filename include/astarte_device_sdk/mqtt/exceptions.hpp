// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_EXCEPTIONS_HPP
#define ASTARTE_MQTT_EXCEPTIONS_HPP

/**
 * @file exceptions.hpp
 * @brief Hierarchy of custom exceptions used throughout the Astarte device SDK.
 */

#include <exception>
#include <string>

#include "astarte_device_sdk/exceptions.hpp"

namespace AstarteDeviceSdk {

/**
 * @brief A base exception class for all SDK-related errors.
 * @details This is the root exception from which all other custom exceptions in the SDK inherit.
 * It extends std::exception to provide a consistent error handling mechanism.
 */
class MqttException : public AstarteException {
 public:
  /**
   * @brief Construct the MqttException object.
   * @param message A descriptive error message explaining the reason for the exception.
   */
  explicit MqttException(std::string message)
      : AstarteDeviceSdk::AstarteException("MqttException(" + std::move(message) + ")") {}
};

/**
 * @brief Base exception for errors related to the Astarte API client.
 * @details This class serves as a parent for more specific API-level exceptions.
 */
class PairingApiException : public MqttException {
 public:
  /**
   * @brief Construct the PairingApiException object.
   * @param err_message A descriptive error message.
   */
  explicit PairingApiException(const std::string& err_message)
      : MqttException("PairingApiException(" + err_message + ")") {}
};

/**
 * @brief Exception thrown for an invalid Astarte pairing URL.
 * @details This typically occurs during the PairingApi's construction if the pairing URL string
 * cannot be parsed correctly.
 */
class InvalidUrlException : public PairingApiException {
 public:
  /**
   * @brief Construct the InvalidUrlException object.
   * @param err_message A descriptive error message.
   */
  explicit InvalidUrlException(const std::string& err_message)
      : PairingApiException("InvalidUrlException(" + err_message + ")") {}
};

/**
 * @brief Exception thrown when an Astarte device fails to register.
 * @details This can be caused by network errors, an invalid pairing token, or other server-side
 * issues during the pairing process.
 */
class DeviceRegistrationException : public PairingApiException {
 public:
  /**
   * @brief Construct the DeviceRegistrationException object.
   * @param err_message A descriptive error message.
   */
  explicit DeviceRegistrationException(const std::string& err_message)
      : PairingApiException("DeviceRegistrationException(" + err_message + ")") {}
};

/**
 * @brief Exception thrown when parsing a JSON response from Astarte APIs fails.
 * @details This occurs if the server's JSON response is malformed or if expected keys
 * are missing from the JSON object.
 */
class JsonAccessErrorException : public PairingApiException {
 public:
  /**
   * @brief Construct the JsonAccessErrorException object.
   * @param err_message A descriptive error message.
   */
  explicit JsonAccessErrorException(const std::string& err_message)
      : PairingApiException("JsonAccessErrorException(" + err_message + ")") {}
};

/** @brief Exception thrown when failing to retrieve the Astarte MQTT broker URL. */
class RetrieveBrokerUrlException : public PairingApiException {
 public:
  /**
   * @brief Constructs the RetrieveBrokerUrlException object.
   * @param err_message A descriptive error message.
   */
  explicit RetrieveBrokerUrlException(const std::string& err_message)
      : PairingApiException("RetrieveBrokerUrlException(" + err_message + ")") {}
};

/** @brief Exception thrown when failing to read the device credential secret from file. */
class ReadCredentialException : public PairingApiException {
 public:
  /**
   * @brief Constructs the ReadCredentialException object.
   * @param err_message A descriptive error message.
   */
  explicit ReadCredentialException(const std::string& err_message)
      : PairingApiException("ReadCredentialException(" + err_message + ")") {}
};

/** @brief Exception thrown when failing to write the device credential secret to file. */
class WriteCredentialException : public PairingApiException {
 public:
  /**
   * @brief Constructs the WriteCredentialException object.
   * @param err_message A descriptive error message.
   */
  explicit WriteCredentialException(const std::string& err_message)
      : PairingApiException("WriteCredentialException(" + err_message + ")") {}
};

/** @brief Exception thrown when the configuration to connect the devite to Astarte is invalid. */
class PairingConfigException : public PairingApiException {
 public:
  /**
   * @brief Constructs the PairingConfigException object.
   * @param err_message A descriptive error message.
   */
  explicit PairingConfigException(const std::string& err_message)
      : PairingApiException("PairingConfigException(" + err_message + ")") {}
};

/**
 * @brief Base exception for errors related to the cryptographic operations.
 */
class CryptoException : public MqttException {
 public:
  /**
   * @brief Constructs the CryptoException object.
   * @param err_message A descriptive error message.
   */
  explicit CryptoException(const std::string& err_message)
      : MqttException("CryptoException(" + err_message + ")") {}
};

/**
 * @brief Base exception for errors related to the cryptographic operations.
 */
class MqttConnectionException : public MqttException {
 public:
  /**
   * @brief Constructs the MqttConnectionException object.
   * @param err_message A descriptive error message.
   */
  explicit MqttConnectionException(const std::string& err_message)
      : MqttException("MqttConnectionException(" + err_message + ")") {}
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_MQTT_EXCEPTIONS_HPP
