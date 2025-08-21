// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

/**
 * @file exceptions.hpp
 * @brief Defines a hierarchy of custom exceptions used throughout the Astarte Device SDK.
 */

#pragma once

#include <exception>
#include <string>

/**
 * @brief A base exception class for all SDK-related errors.
 * @details This is the root exception from which all other custom exceptions in the SDK inherit.
 * It extends std::exception to provide a consistent error handling mechanism.
 */
class MqttException : public std::exception {
 public:
  /**
   * @brief Constructs the MqttException object.
   * @param message A descriptive error message explaining the reason for the exception.
   */
  explicit MqttException(std::string message) : message_(std::move(message)) {}

  /**
   * @brief Returns the exception message.
   * @details Overrides the base std::exception::what() method.
   * @return A constant C-style string containing the error message.
   */
  [[nodiscard]] auto what() const noexcept -> const char* override { return message_.c_str(); }

 private:
  /// @brief The error message associated with the exception.
  std::string message_;
};

/**
 * @brief Base exception for errors related to the Astarte API client.
 * @details This class serves as a parent for more specific API-level exceptions.
 */
class ApiClientException : public MqttException {
 public:
  /**
   * @brief Constructs the ApiClientException object.
   * @param err_message A descriptive error message.
   */
  explicit ApiClientException(const std::string& err_message) : MqttException(err_message) {}
};

/**
 * @brief Exception thrown for an invalid Astarte pairing URL.
 * @details This typically occurs during the ApiClient's construction if the pairing URL string
 * cannot be parsed correctly.
 */
class InvalidUrlException : public ApiClientException {
 public:
  /**
   * @brief Constructs the InvalidUrlException object.
   * @param err_message A descriptive error message.
   */
  explicit InvalidUrlException(const std::string& err_message) : ApiClientException(err_message) {}
};

/**
 * @brief Exception thrown when an Astarte device fails to register.
 * @details This can be caused by network errors, an invalid pairing token, or other server-side
 * issues during the pairing process.
 */
class DeviceRegistrationException : public ApiClientException {
 public:
  /**
   * @brief Constructs the DeviceRegistrationException object.
   * @param err_message A descriptive error message.
   */
  explicit DeviceRegistrationException(const std::string& err_message)
      : ApiClientException(err_message) {}
};

/**
 * @brief Exception thrown when parsing a JSON response from Astarte APIs fails.
 * @details This occurs if the server's JSON response is malformed or if expected keys
 * are missing from the JSON object.
 */
class JsonAccessErrorException : public ApiClientException {
 public:
  /**
   * @brief Constructs the JsonAccessErrorException object.
   * @param err_message A descriptive error message.
   */
  explicit JsonAccessErrorException(const std::string& err_message)
      : ApiClientException(err_message) {}
};

/** @brief Exception thrown when failing to retrieve the Astarte MQTT broker URL. */
class RetrieveBrokerUrlException : public ApiClientException {
 public:
  /**
   * @brief Constructs the RetrieveBrokerUrlException object.
   * @param err_message A descriptive error message.
   */
  explicit RetrieveBrokerUrlException(const std::string& err_message)
      : ApiClientException(err_message) {}
};
