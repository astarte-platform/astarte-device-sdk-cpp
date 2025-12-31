// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_CREDENTIALS_H
#define ASTARTE_MQTT_CREDENTIALS_H

#include <spdlog/spdlog.h>
#include <unistd.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#ifdef _WIN32
#include <io.h>
#define fsync _commit
#define fileno _fileno
#endif

#include "ada.h"
#include "astarte_device_sdk/mqtt/errors.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "mqtt/connect_options.h"

/**
 * @brief File read and write utilities
 */
namespace config {

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
 * @brief Reads the entire content of a file into a string.
 * @param file_path The path to the file to be read.
 * @return the file content as a string, or an error if the file cannot be opened.
 */
auto read_from_file(const std::filesystem::path& file_path)
    -> astarte_tl::expected<std::string, AstarteError>;

/**
 * @brief Writes a string to a file, overwriting any existing content.
 * @param file_path The path to the file to be written.
 * @param data The string content to write to the file.
 * @return an error in case the write operation failed, nothing otherwise.
 */
auto write_to_file(const std::filesystem::path& file_path, std::string_view data)
    -> astarte_tl::expected<void, AstarteError>;

auto secure_shred_file(const std::string& path) -> astarte_tl::expected<void, AstarteError>;

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

}  // namespace config

#endif  // ASTARTE_MQTT_CREDENTIALS_H
