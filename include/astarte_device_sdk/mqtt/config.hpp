// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_CONFIG_H
#define ASTARTE_MQTT_CONFIG_H

#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>

#include "ada.h"
#include "astarte_device_sdk/mqtt/errors.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "mqtt/connect_options.h"

namespace AstarteDeviceSdk {

/** @brief Default keep alive interval in seconds for the MQTT connection. */
constexpr uint32_t DEFAULT_KEEP_ALIVE = 30;

/** @brief Default connection timeout in seconds for the MQTT connection. */
constexpr uint32_t DEFAULT_CONNECTION_TIMEOUT = 5;

/** @brief Default file name inside the store directory where the certificate is stored in PEM
 * format. */
constexpr std::string_view CLIENT_CERTIFICATE_FILE = "client-certificate.pem";

/** @brief Default file name inside the store directory where the private key is stored in PEM
 * format. */
constexpr std::string_view PRIVATE_KEY_FILE = "client-priv-key.pem";

/**
 * @brief A type-safe wrapper for Astarte credentials, distinguishing between a credential secret
 * and a pairing token.
 */
class Credential {
 public:
  /**
   * @brief Creates a Credential instance from a pairing token.
   * @param credential The pairing token string.
   * @return A new Credential instance of type PAIRING_TOKEN.
   */
  static auto pairing_token(std::string_view credential) -> Credential {
    return Credential{CredentialType::PAIRING_TOKEN, std::string(credential)};
  }

  /**
   * @brief Creates a Credential instance from a credential secret.
   * @param credential The credential secret string.
   * @return A new Credential instance of type CREDENTIAL_SECRET.
   */
  static auto secret(std::string_view credential) -> Credential {
    return Credential{CredentialType::CREDENTIAL_SECRET, std::string(credential)};
  }

  /**
   * @brief Check if this credential is a pairing token.
   * @return true if the credential is a PAIRING_TOKEN, false otherwise.
   */
  auto is_pairing_token() const -> bool { return typ_ == CredentialType::PAIRING_TOKEN; }

  /**
   * @brief Check if this credential is a credential secret.
   * @return true if the credential is a CREDENTIAL_SECRET, false otherwise.
   */
  auto is_credential_secret() const -> bool { return typ_ == CredentialType::CREDENTIAL_SECRET; }

  /**
   * @brief Get the string value of the credential.
   * @return The stored credential (token or secret) as a string.
   */
  auto value() const -> std::string { return credential_; }

 private:
  /// @brief Enum to differentiate credential types.
  enum CredentialType {
    CREDENTIAL_SECRET,
    PAIRING_TOKEN,
  };

  CredentialType typ_;
  std::string credential_;

  /// @brief Private constructor to enforce creation through static factory methods.
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
   * @brief Create a new MqttConfig instance using a credential secret.
   * @param realm The Astarte realm.
   * @param device_id The device's unique ID.
   * @param credential The credential secret string.
   * @param pairing_url The base URL of the Astarte pairing API.
   * @param store_dir Path to a local directory for persisting state.
   * @return A new MqttConfig object.
   */
  static auto with_credential_secret(std::string_view realm, std::string_view device_id,
                                     std::string_view credential, std::string_view pairing_url,
                                     std::string_view store_dir) -> MqttConfig {
    return MqttConfig{realm, device_id, Credential::secret(credential), pairing_url, store_dir};
  };

  /**
   * @brief Create a new MqttConfig instance using a pairing token.
   * @param realm The Astarte realm.
   * @param device_id The device's unique ID.
   * @param credential The pairing token string.
   * @param pairing_url The base URL of the Astarte pairing API.
   * @param store_dir Path to a local directory for persisting state.
   * @return A new MqttConfig object.
   */
  static auto with_pairing_token(std::string_view realm, std::string_view device_id,
                                 std::string_view credential, std::string_view pairing_url,
                                 std::string_view store_dir) -> MqttConfig {
    return MqttConfig{realm, device_id, Credential::pairing_token(credential), pairing_url,
                      store_dir};
  };

  /**
   * @brief Get the configured realm.
   * @return a string containing the realm
   */
  auto realm() -> std::string_view { return realm_; }

  /**
   * @brief Get the configured device ID.
   * @return a string containing the device ID
   */
  auto device_id() -> std::string_view { return device_id_; }

  /**
   * @brief Get the configured Pairing API URL.
   * @return a string containing the pairing URL
   */
  auto pairing_url() -> std::string_view { return pairing_url_; }

  /**
   * @brief Check if the credential is of type pairing token.
   * @return a boolean stating if the stored credential is a pairing token or not.
   */
  auto cred_is_pairing_token() -> bool { return credential_.is_pairing_token(); }

  /**
   * @brief Check if the credential is of type credential secret.
   * @return a boolean stating if the stored credential is a credential secret or not.
   */
  auto cred_is_credential_secret() -> bool { return credential_.is_credential_secret(); }

  /**
   * @brief Retrieve the credential value
   * @return a string representing the credential value.
   */
  auto cred_value() -> std::string { return credential_.value(); }

  /**
   * @brief Get the configured store directory.
   * @return a string containing the store directory.
   */
  auto store_dir() -> std::string_view { return store_dir_; }

  /**
   * @brief Set the MQTT keep-alive interval.
   * @param duration The keep-alive duration in seconds.
   * @return A reference to the MqttConfig object for chaining.
   */
  auto keepalive(uint32_t duration) -> MqttConfig& {
    this->keepalive_ = duration;
    return *this;
  }

  /**
   * @brief Configure the client to ignore TLS/SSL certificate validation errors.
   * @return A reference to the updated MqttConfig object.
   */
  auto ignore_ssl_errors() -> MqttConfig& {
    this->ignore_ssl_ = true;
    return *this;
  }

  /**
   * @brief Set the MQTT connection timeout.
   * @param duration The timeout duration in seconds.
   * @return A reference to the MqttConfig object for chaining.
   */
  auto connection_timeout(uint32_t duration) -> MqttConfig& {
    this->conn_timeout_ = duration;
    return *this;
  }

  /**
   * @brief Build method to retrieve the Paho MQTT connection options.
   * @return Paho MQTT connection options object, an error otherwise.
   */
  auto build_mqtt_options() -> astarte_tl::expected<mqtt::connect_options, AstarteError>;

 private:
  /**
   * @brief Create a new instance of MqttConfig.
   *
   * This constructor is private to enforce instance creation through the provided static factory
   * methods, ensuring that a valid credential type is always supplied.
   */
  MqttConfig(std::string_view realm, std::string_view device_id, Credential credential,
             std::string_view pairing_url, std::string_view store_dir)
      : realm_(realm),
        device_id_(device_id),
        credential_(credential),
        pairing_url_(pairing_url),
        store_dir_(store_dir),
        // default does not ignore SSL errors
        ignore_ssl_(false),
        // default has a keepalive of 30 seconds
        keepalive_(DEFAULT_KEEP_ALIVE),
        // default has a connection timeout of 5 seconds
        conn_timeout_(DEFAULT_CONNECTION_TIMEOUT) {}

  std::string realm_;
  std::string device_id_;
  std::string pairing_url_;
  Credential credential_;
  std::string store_dir_;
  bool ignore_ssl_;
  uint32_t keepalive_;
  uint32_t conn_timeout_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_MQTT_CONFIG_H
