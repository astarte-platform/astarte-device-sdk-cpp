// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_CREDENTIALS_H
#define ASTARTE_MQTT_CREDENTIALS_H

/**
 * @file private/mqtt/credentials.hpp
 * @brief Credential management for Astarte MQTT connection.
 *
 * @details This file defines the `Credential` class, which abstracts authentication secrets
 * (pairing tokens and credential secrets) and handles the secure storage and validation
 * of client certificates.
 */

#include <ada.h>
#include <spdlog/spdlog.h>
#include <unistd.h>

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>

#include "astarte_device_sdk/mqtt/errors.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "mqtt/connect_options.h"

namespace astarte::device::mqtt {

/**
 * @brief A type-safe wrapper for Astarte credentials.
 * @details Distinguishes between a pairing token (used to obtain a certificate) and a
 * credential secret (used for authentication).
 */
class Credential {
 public:
  /**
   * @brief Creates a Credential instance from a pairing token.
   * @param[in] credential The pairing token string.
   * @param[in] store_dir The directory to use for storage.
   * @return A new Credential instance of type PAIRING_TOKEN.
   */
  static auto pairing_token(std::string_view credential, std::string_view store_dir) -> Credential {
    return Credential{CredentialType::PAIRING_TOKEN, std::string(credential),
                      std::string(store_dir)};
  }

  /**
   * @brief Creates a Credential instance from a credential secret.
   * @param[in] credential The credential secret string.
   * @param[in] store_dir The directory to use for storage.
   * @return A new Credential instance of type CREDENTIAL_SECRET.
   */
  static auto secret(std::string_view credential, std::string_view store_dir) -> Credential {
    return Credential{CredentialType::CREDENTIAL_SECRET, std::string(credential),
                      std::string(store_dir)};
  }

  /**
   * @brief Checks if this credential is a pairing token.
   * @return True if the credential is a PAIRING_TOKEN, false otherwise.
   */
  [[nodiscard]] auto is_pairing_token() const -> bool {
    return type_ == CredentialType::PAIRING_TOKEN;
  }

  /**
   * @brief Checks if this credential is a credential secret.
   * @return True if the credential is a CREDENTIAL_SECRET, false otherwise.
   */
  [[nodiscard]] auto is_credential_secret() const -> bool {
    return type_ == CredentialType::CREDENTIAL_SECRET;
  }

  /**
   * @brief Gets the string value of the credential.
   * @return The stored credential (token or secret) as a string.
   */
  [[nodiscard]] auto value() const -> std::string { return credential_; }

  /**
   * @brief Gets the path for the device certificate PEM file.
   * @param[in] store_dir The directory used for storage.
   * @return The full filesystem path to the certificate file.
   */
  static auto get_device_certificate_path(std::string_view store_dir) -> std::string;

  /**
   * @brief Gets the path for the device private key PEM file.
   * @param[in] store_dir The directory used for storage.
   * @return The full filesystem path to the key file.
   */
  static auto get_device_key_path(std::string_view store_dir) -> std::string;

  /**
   * @brief Retrieves and persists device crypto credentials.
   *
   * @param[in] client_priv_key The device private key as a PEM string.
   * @param[in] client_cert The device certificate as a PEM string.
   * @param[in] store_dir The directory path where the certificate and private key files can be
   * stored.
   * @return An expected containing void on success or Error on failure.
   */
  static auto store_device_key_and_certificate(std::string_view client_priv_key,
                                               std::string_view client_cert,
                                               std::string_view store_dir)
      -> astarte_tl::expected<void, Error>;

  /**
   * @brief Validates the existing client certificate against the server.
   *
   * @param[in] api The PairingApi instance used to request the credentials.
   * @param[in] secret The credential secret used to authenticate the request.
   * @param[in] store_dir The directory path where the certificate and private key files are stored.
   * @return An expected containing the certificate validity on success or Error on failure.
   */
  static auto validate_client_certificate(PairingApi& api, std::string_view secret,
                                          std::string_view store_dir)
      -> astarte_tl::expected<bool, Error>;

  /**
   * @brief Deletes the stored client certificate and private key.
   *
   * @param[in] store_dir The directory where the files are stored.
   * @return An expected containing void on success or Error on failure.
   */
  static auto delete_client_certificate_and_key(std::string_view store_dir)
      -> astarte_tl::expected<void, Error>;

 private:
  /// @brief Enum to differentiate credential types.
  enum CredentialType : std::uint8_t {
    CREDENTIAL_SECRET,
    PAIRING_TOKEN,
  };

  /// @brief Private constructor to enforce creation through static factory methods.
  Credential(CredentialType type, std::string cred, std::string store_dir)
      : type_(type), credential_(std::move(cred)), store_dir_(std::move(store_dir)) {}

  CredentialType type_;
  std::string credential_;
  std::string store_dir_;
};

}  // namespace astarte::device::mqtt

#endif  // ASTARTE_MQTT_CREDENTIALS_H
