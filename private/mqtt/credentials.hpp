// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_CREDENTIALS_H
#define ASTARTE_MQTT_CREDENTIALS_H

#include <spdlog/spdlog.h>
#include <unistd.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>

#include "ada.h"
#include "astarte_device_sdk/mqtt/errors.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "mqtt/connect_options.h"

namespace AstarteDeviceSdk::config {

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
  static auto pairing_token(std::string_view credential, std::string_view store_dir) -> Credential {
    return Credential{CredentialType::PAIRING_TOKEN, std::string(credential),
                      std::string(store_dir)};
  }

  /**
   * @brief Creates a Credential instance from a credential secret.
   * @param credential The credential secret string.
   * @return A new Credential instance of type CREDENTIAL_SECRET.
   */
  static auto secret(std::string_view credential, std::string_view store_dir) -> Credential {
    return Credential{CredentialType::CREDENTIAL_SECRET, std::string(credential),
                      std::string(store_dir)};
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

  static auto get_device_certificate_path(const std::string_view store_dir) -> std::string;

  static auto get_device_key_path(const std::string_view store_dir) -> std::string;

  /**
   * @brief Retrieve and persist device crypto credentials.
  .
  * @param api The PairingApi instance used to request the credentials.
  * @param secret The credential secret used to authenticate the request.
  * @param store_dir The directory path where the certificate and private key files will be created.
  * @return an error if the API request fails or file writing errors occur.
  */
  static auto store_device_key_and_certificate(const std::string_view client_priv_key,
                                               const std::string_view client_cert,
                                               const std::string_view store_dir)
      -> astarte_tl::expected<void, AstarteError>;

  /**
   * @brief Retrieve and persist device crypto credentials.
  .
   * @param api The PairingApi instance used to request the credentials.
   * @param secret The credential secret used to authenticate the request.
   * @param store_dir The directory path where the certificate and private key files will be
  created.
   * @return an error if the API request fails or file writing errors occur.
  */
  static auto validate_client_certificate(PairingApi& api, const std::string_view secret,
                                          const std::string_view store_dir)
      -> astarte_tl::expected<bool, AstarteError>;

  static auto delete_client_certificate_and_key(const std::string_view store_dir)
      -> astarte_tl::expected<void, AstarteError>;

 private:
  /// @brief Enum to differentiate credential types.
  enum CredentialType {
    CREDENTIAL_SECRET,
    PAIRING_TOKEN,
  };

  CredentialType typ_;
  std::string credential_;
  std::string store_dir_;

  /// @brief Private constructor to enforce creation through static factory methods.
  Credential(CredentialType t, std::string cred, std::string store_dir)
      : typ_(t), credential_(std::move(cred)), store_dir_(std::move(store_dir)) {}
};

}  // namespace AstarteDeviceSdk::config

#endif  // ASTARTE_MQTT_CREDENTIALS_H
