// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_CONFIG_H
#define ASTARTE_MQTT_CONFIG_H

/**
 * @file config.hpp
 * @brief Defines classes and parameters for configuring an MQTT connection to Astarte.
 *
 * This file provides the MqttConfig and Credential classes, which are used to set up all
 * necessary parameters for device authentication and communication with the Astarte
 * platform over the MQTT transport.
 */

/** @brief Default keep alive interval in seconds for the MQTT connection. */
#ifndef DEFAULT_KEEP_ALIVE
#define DEFAULT_KEEP_ALIVE 30
#endif

/** @brief Default connection timeout in seconds for the MQTT connection. */
#ifndef DEFAULT_CONNECTION_TIMEOUT
#define DEFAULT_CONNECTION_TIMEOUT 5
#endif

#include <optional>
#include <string>
#include <string_view>

/** @brief Umbrella namespace for the Astarte C++ Device SDK. */
namespace AstarteDeviceSdk {

/**
 * @brief A factory class for creating device authentication credentials.
 * @details This class provides a type-safe way to handle the two different authentication
 * methods in Astarte: a credential secret for registered devices, and a
 * one-time pairing token for new devices.
 */
class Credential {
 public:
  /**
   * @brief Creates a Credential object from a one-time pairing token.
   * @param credential The pairing token string used to register a new device.
   * @return A new Credential instance configured for pairing.
   */
  static auto pairing_token(std::string_view credential) -> Credential {
    return Credential{CredentialType::PAIRING_TOKEN, std::string(credential)};
  }

  /**
   * @brief Creates a Credential object from a credential secret.
   * @param credential The credential secret string used for authenticating an existing device.
   * @return A new Credential instance configured for standard authentication.
   */
  static auto secret(std::string_view credential) -> Credential {
    return Credential{CredentialType::CREDENTIAL_SECRET, std::string(credential)};
  }

  /**
   * @brief Check if the credential stored is the pairing token.
   * @return true if the credential stored is the pairing token.
   */
  auto is_pairing_token() const -> bool { return typ_ == CredentialType::PAIRING_TOKEN; }

  /**
   * @brief Check if the credential stored is the credential secret.
   * @return true if the credential stored is the credential secret.
   */
  auto is_credential_secret() const -> bool { return typ_ == CredentialType::CREDENTIAL_SECRET; }

  // TODO: this method is only used for testing. Remove it once the connection to the MQTT will be
  // implemented.
  /**
   * @brief Gets the credential.
   * @return a string containing the credential
   */
  auto credential() const -> std::string { return credential_; }

 private:
  // The type of credential being stored.
  enum CredentialType {
    CREDENTIAL_SECRET,
    PAIRING_TOKEN,
  };

  // The internal type of the credential.
  CredentialType typ_;
  // The credential payload (either the secret or the token).
  std::string credential_;

  /**
   * @brief Private constructor to enforce object creation via static factory methods.
   * @param t The type of the credential.
   * @param cred The credential string.
   */
  Credential(CredentialType t, std::string cred) : typ_(t), credential_(std::move(cred)) {}
};

/**
 * @brief Configuration for the Astarte MQTT connection.
 * @details This class holds all necessary settings for connecting a device to Astarte via MQTT.
 * It uses a builder pattern, allowing for fluent and readable configuration.
 */
class MqttConfig {
 public:
  /**
   * @brief Create a new instance with a credential secret for authentication.
   * @param realm The Astarte realm the device belongs to.
   * @param device_id The unique identifier for the device.
   * @param credential The credential secret for this device.
   * @param pairing_url The base URL of the Astarte Pairing API.
   * @param store_dir Path to a local directory for persisting certificates and state.
   * @return A new MqttConfig instance.
   */
  static auto with_credential_secret(std::string_view realm, std::string_view device_id,
                                     std::string_view credential, std::string_view pairing_url,
                                     std::string_view store_dir) -> MqttConfig {
    return MqttConfig{realm, device_id, Credential::secret(credential), pairing_url, store_dir};
  };

  /**
   * @brief Create a new instance with a pairing token to register the device.
   * @param realm The Astarte realm the device will be registered in.
   * @param device_id The unique identifier for the device.
   * @param credential The one-time pairing token.
   * @param pairing_url The base URL of the Astarte Pairing API.
   * @param store_dir Path to a local directory for persisting certificates and state.
   * @return A new MqttConfig instance.
   */
  static auto with_pairing_token(std::string_view realm, std::string_view device_id,
                                 std::string_view credential, std::string_view pairing_url,
                                 std::string_view store_dir) -> MqttConfig {
    return MqttConfig{realm, device_id, Credential::pairing_token(credential), pairing_url,
                      store_dir};
  };

  /**
   * @brief Gets the configured realm.
   * @return a string containing the realm
   */
  auto realm() -> std::string_view;

  /**
   * @brief Gets the configured device ID.
   * @return a string containing the device ID
   */
  auto device_id() -> std::string_view;

  /**
   * @brief Gets the configured Pairing API URL.
   * @return a string containing the pairing URL
   */
  auto pairing_url() -> std::string_view;

  /**
   * @brief Gets the configured local storage directory.
   * @return a string containing the store directory
   */
  auto store_dir() -> std::string_view;

  // TODO: this method is only used for testing. Remove it once the connection to the MQTT will be
  // implemented.
  /**
   * @brief Gets the pairing token.
   * @return a string containing the pairing token, nullopt if the Credential is the
   * credential_secret
   */
  auto pairing_token() -> std::optional<std::string>;

  /**
   * @brief Sets the MQTT keep-alive interval.
   * @param duration The keep-alive duration in seconds.
   * @return A reference to the MqttConfig object for chaining.
   */
  auto keepalive(int duration) -> MqttConfig&;

  /**
   * @brief Configure the client to ignore TLS/SSL certificate validation errors.
   * @return A reference to the MqttConfig object for chaining.
   */
  auto ignore_ssl_errors() -> MqttConfig&;

  /**
   * @brief Sets the MQTT connection timeout.
   * @param duration The timeout duration in seconds.
   * @return A reference to the MqttConfig object for chaining.
   */
  auto connection_timeout(int duration) -> MqttConfig&;

 private:
  /**
   * @brief Private constructor to enforce object creation via static factory methods.
   */
  MqttConfig(std::string_view realm, std::string_view device_id, Credential credential,
             std::string_view pairing_url, std::string_view store_dir)
      : realm_(realm),
        device_id_(device_id),
        credential_(credential),
        pairing_url_(pairing_url),
        store_dir_(store_dir),
        ignore_ssl_(false),
        keepalive_(DEFAULT_KEEP_ALIVE),
        conn_timeout_(DEFAULT_CONNECTION_TIMEOUT) {}

  // The Astarte realm.
  std::string realm_;
  // The unique device identifier.
  std::string device_id_;
  // The base URL for the Pairing API.
  std::string pairing_url_;
  // The device's authentication credential.
  Credential credential_;
  // The local directory for storing persistent data.
  std::string store_dir_;
  // Flag to ignore SSL/TLS certificate errors.
  bool ignore_ssl_;
  // Keep-alive interval in seconds.
  int keepalive_;
  // Connection timeout in seconds.
  int conn_timeout_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_MQTT_CONFIG_H
