// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_CREDENTIALS_H
#define ASTARTE_MQTT_CREDENTIALS_H

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
 * @brief A type-safe wrapper for Astarte credentials, distinguishing between a credential secret
 * and a pairing token.
 */
class Credential {
 public:
  /**
   * @brief Creates a Credential instance from a pairing token.
   * @param credential The pairing token string.
   * @param store_dir The directory to use for storage.
   * @return A new Credential instance of type PAIRING_TOKEN.
   */
  static auto pairing_token(std::string_view credential, std::string_view store_dir) -> Credential {
    return Credential{CredentialType::PAIRING_TOKEN, std::string(credential),
                      std::string(store_dir)};
  }

  /**
   * @brief Creates a Credential instance from a credential secret.
   * @param credential The credential secret string.
   * @param store_dir The directory to use for storage.
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
  [[nodiscard]] auto is_pairing_token() const -> bool {
    return type_ == CredentialType::PAIRING_TOKEN;
  }

  /**
   * @brief Check if this credential is a credential secret.
   * @return true if the credential is a CREDENTIAL_SECRET, false otherwise.
   */
  [[nodiscard]] auto is_credential_secret() const -> bool {
    return type_ == CredentialType::CREDENTIAL_SECRET;
  }

  /**
   * @brief Get the string value of the credential.
   * @return The stored credential (token or secret) as a string.
   */
  [[nodiscard]] auto value() const -> std::string { return credential_; }

  /**
   * @brief Get the path for the device certificate PEM file.
   * @param store_dir The directory to used for storage.
   * @return The path to the file.
   */
  static auto get_device_certificate_path(std::string_view store_dir) -> std::string;

  /**
   * @brief Get the path for the device private key PEM file.
   * @param store_dir The directory to used for storage.
   * @return The path to the file.
   */
  static auto get_device_key_path(std::string_view store_dir) -> std::string;

  /**
   * @brief Retrieve and persist device crypto credentials.
   *
   * @param client_priv_key The device private key as a PEM string.
   * @param client_cert The device certificate as a PEM string.
   * @param store_dir The directory path where the certificate and private key files can be stored.
   * @return an error if the API request fails or file writing errors occur.
   */
  static auto store_device_key_and_certificate(std::string_view client_priv_key,
                                               std::string_view client_cert,
                                               std::string_view store_dir)
      -> astarte_tl::expected<void, Error>;

  /**
   * @brief Retrieve and persist device crypto credentials.
   *
   * @param api The PairingApi instance used to request the credentials.
   * @param secret The credential secret used to authenticate the request.
   * @param store_dir The directory path where the certificate and private key files are stored.
   * @return an error if the API request fails or file writing errors occur.
   */
  static auto validate_client_certificate(PairingApi& api, std::string_view secret,
                                          std::string_view store_dir)
      -> astarte_tl::expected<bool, Error>;

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
