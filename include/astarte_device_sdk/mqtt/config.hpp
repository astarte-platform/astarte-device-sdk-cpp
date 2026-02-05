// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_CONFIG_H
#define ASTARTE_MQTT_CONFIG_H

/**
 * @file astarte_device_sdk/mqtt/config.hpp
 * @brief Configuration for the Astarte MQTT transport.
 *
 * @details This file defines the configuration class used to set up the MQTT
 * connection to the Astarte platform, including credentials, timeouts, and
 * pairing information.
 */

#include <ada.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "astarte_device_sdk/mqtt/errors.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"

namespace astarte::device::mqtt {

/// @cond DO_NOT_DOCUMENT
using namespace std::chrono_literals;
/// @endcond

// Forward declaration, needed to make Credential private.
class Credential;

/// @brief Default keep alive interval in seconds for the MQTT connection.
constexpr uint32_t DEFAULT_KEEP_ALIVE = 30;

/// @brief Default connection timeout in seconds for the MQTT connection.
constexpr uint32_t DEFAULT_CONNECTION_TIMEOUT = 5;

/// @brief Default disconnection timeout in seconds for the MQTT connection.
constexpr auto DEFAULT_DISCONNECTION_TIMEOUT = 1s;

/**
 * @brief Configuration for the Astarte MQTT connection.
 *
 * @details This class holds all necessary settings for connecting a device to Astarte via MQTT.
 * It uses a builder pattern, allowing for fluent and readable configuration.
 */
class Config {
 public:
  /**
   * @brief Move constructor.
   * @param[in,out] other The config instance to move data from.
   */
  Config(Config&& other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param[in,out] other The config instance to move data from.
   * @return A reference to this config.
   */
  auto operator=(Config&& other) noexcept -> Config&;

  /// @brief Config is non-copyable.
  Config(const Config&) = delete;

  /// @brief Config is non-copyable.
  auto operator=(const Config&) -> Config& = delete;

  /// @brief Destructor.
  ~Config();

  /**
   * @brief Creates a new Config instance using a credential secret.
   *
   * @param[in] realm The Astarte realm.
   * @param[in] device_id The device's unique ID.
   * @param[in] credential The credential secret string.
   * @param[in] pairing_url The base URL of the Astarte pairing API.
   * @param[in] store_dir Path to a local directory for persisting state.
   * @return A new Config object.
   */
  [[nodiscard]] static auto with_credential_secret(std::string_view realm,
                                                   std::string_view device_id,
                                                   std::string_view credential,
                                                   std::string_view pairing_url,
                                                   std::string_view store_dir) -> Config;

  /**
   * @brief Gets the configured realm.
   * @return A string view containing the realm.
   */
  [[nodiscard]] auto realm() -> std::string_view { return realm_; }

  /**
   * @brief Gets the configured device ID.
   * @return A string view containing the device ID.
   */
  [[nodiscard]] auto device_id() -> std::string_view { return device_id_; }

  /**
   * @brief Gets the configured Pairing API URL.
   * @return A string view containing the pairing URL.
   */
  [[nodiscard]] auto pairing_url() -> std::string_view { return pairing_url_; }

  /**
   * @brief Checks if the credential is of type credential secret.
   * @return True if the stored credential is a credential secret, false otherwise.
   */
  [[nodiscard]] auto cred_is_credential_secret() -> bool;

  /**
   * @brief Retrieves the credential value.
   * @return An optional string containing the credential value if available.
   */
  [[nodiscard]] auto credential_secret() -> std::optional<std::string>;

  /**
   * @brief Gets the configured store directory.
   * @return A string view containing the store directory.
   */
  [[nodiscard]] auto store_dir() -> std::string_view { return store_dir_; }

  /**
   * @brief Sets the MQTT keep-alive interval.
   *
   * @param[in] duration The keep-alive duration in seconds.
   * @return A reference to the Config object for chaining.
   */
  auto keepalive(uint32_t duration) -> Config& {
    this->keepalive_ = duration;
    return *this;
  }

  /**
   * @brief Configures the client to ignore TLS/SSL certificate validation errors.
   * @return A reference to the updated Config object.
   */
  auto ignore_ssl_errors() -> Config& {
    this->ignore_ssl_ = true;
    return *this;
  }

  /**
   * @brief Sets the MQTT connection timeout.
   *
   * @param[in] duration The timeout duration in seconds.
   * @return A reference to the Config object for chaining.
   */
  auto connection_timeout(uint32_t duration) -> Config& {
    this->conn_timeout_ = duration;
    return *this;
  }

  /**
   * @brief Sets the MQTT disconnection timeout.
   *
   * @param[in] duration The timeout duration in milliseconds.
   * @return A reference to the Config object for chaining.
   */
  auto disconnection_timeout(std::chrono::milliseconds duration) -> Config& {
    this->disconn_timeout_ = duration;
    return *this;
  }

  /**
   * @brief Gets the MQTT keep-alive interval.
   * @return The connection keepalive value.
   */
  [[nodiscard]] auto keepalive() const -> uint32_t { return keepalive_; }

  /**
   * @brief Gets the MQTT connection timeout.
   * @return The connection timeout value.
   */
  [[nodiscard]] auto connection_timeout() const -> uint32_t { return conn_timeout_; }

  /**
   * @brief Gets the MQTT disconnection timeout.
   * @return The disconnection timeout value.
   */
  [[nodiscard]] auto disconnection_timeout() const -> std::chrono::milliseconds {
    return disconn_timeout_;
  }

 private:
  /**
   * @brief Private constructor.
   *
   * @details This constructor is private to enforce instance creation through the provided static
   * factory methods, ensuring that a valid credential type is always supplied.
   *
   * @param[in] realm The Astarte realm.
   * @param[in] device_id The device's unique ID.
   * @param[in] credential The unique pointer to the credential object.
   * @param[in] pairing_url The base URL of the Astarte pairing API.
   * @param[in] store_dir Path to a local directory for persisting state.
   */
  Config(std::string_view realm, std::string_view device_id, std::unique_ptr<Credential> credential,
         std::string_view pairing_url, std::string_view store_dir);

  std::string realm_;
  std::string device_id_;
  std::string pairing_url_;
  std::unique_ptr<Credential> credential_;
  std::string store_dir_;
  bool ignore_ssl_{false};
  uint32_t keepalive_;
  uint32_t conn_timeout_;
  std::chrono::milliseconds disconn_timeout_;
};

}  // namespace astarte::device::mqtt

#endif  // ASTARTE_MQTT_CONFIG_H
