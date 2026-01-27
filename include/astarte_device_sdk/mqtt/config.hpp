// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_CONFIG_H
#define ASTARTE_MQTT_CONFIG_H

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

/**
 * @brief Namespace for configuration classes definition.
 */
namespace AstarteDeviceSdk::config {

/// @cond DO_NOT_DOCUMENT
using namespace std::chrono_literals;
/// @endcond

// Forward declaration, needed to make Credential private.
class Credential;

/** @brief Default keep alive interval in seconds for the MQTT connection. */
constexpr uint32_t DEFAULT_KEEP_ALIVE = 30;

/** @brief Default connection timeout in seconds for the MQTT connection. */
constexpr uint32_t DEFAULT_CONNECTION_TIMEOUT = 5;

/** @brief Default disconnection timeout in seconds for the MQTT connection. */
constexpr auto DEFAULT_DISCONNECTION_TIMEOUT = 1s;

/**
 * @brief Configuration for the Astarte MQTT connection.
 * @details This class holds all necessary settings for connecting a device to Astarte via MQTT.
 * It uses a builder pattern, allowing for fluent and readable configuration.
 */
class MqttConfig {
 public:
  /** @brief Move constructor for the MqttConfig class. */
  MqttConfig(MqttConfig&&) noexcept;
  /**
   * @brief Move assignment operator for the MqttConfig class.
   * @return A reference to this MqttConfig object.
   */
  auto operator=(MqttConfig&&) noexcept -> MqttConfig&;

  /** @brief Copy constructor is deleted. */
  MqttConfig(const MqttConfig&) = delete;
  /** @brief Copy assignment operator is deleted. */
  auto operator=(const MqttConfig&) -> MqttConfig& = delete;

  ~MqttConfig();

  /**
   * @brief Create a new MqttConfig instance using a credential secret.
   * @param realm The Astarte realm.
   * @param device_id The device's unique ID.
   * @param credential The credential secret string.
   * @param pairing_url The base URL of the Astarte pairing API.
   * @param store_dir Path to a local directory for persisting state.
   * @return A new MqttConfig object.
   */
  [[nodiscard]] static auto with_credential_secret(std::string_view realm,
                                                   std::string_view device_id,
                                                   std::string_view credential,
                                                   std::string_view pairing_url,
                                                   std::string_view store_dir) -> MqttConfig;

  /**
   * @brief Get the configured realm.
   * @return a string containing the realm
   */
  [[nodiscard]] auto realm() -> std::string_view { return realm_; }

  /**
   * @brief Get the configured device ID.
   * @return a string containing the device ID
   */
  [[nodiscard]] auto device_id() -> std::string_view { return device_id_; }

  /**
   * @brief Get the configured Pairing API URL.
   * @return a string containing the pairing URL
   */
  [[nodiscard]] auto pairing_url() -> std::string_view { return pairing_url_; }

  /**
   * @brief Check if the credential is of type credential secret.
   * @return a boolean stating if the stored credential is a credential secret or not.
   */
  [[nodiscard]] auto cred_is_credential_secret() -> bool;

  /**
   * @brief Retrieve the credential value
   * @return a string representing the credential value.
   */
  [[nodiscard]] auto credential_secret() -> std::optional<std::string>;

  /**
   * @brief Get the configured store directory.
   * @return a string containing the store directory.
   */
  [[nodiscard]] auto store_dir() -> std::string_view { return store_dir_; }

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
   * @brief Set the MQTT connection timeout.
   * @param duration The timeout duration in milliseconds.
   * @return A reference to the MqttConfig object for chaining.
   */
  auto disconnection_timeout(std::chrono::milliseconds duration) -> MqttConfig& {
    this->disconn_timeout_ = duration;
    return *this;
  }

  /**
   * @brief Get the MQTT keep-alive interval.
   * @return The connection keepalive value..
   */
  [[nodiscard]] auto keepalive() const -> uint32_t { return keepalive_; }

  /**
   * @brief Get the MQTT connection timeout.
   * @return The connection timeout value.
   */
  [[nodiscard]] auto connection_timeout() const -> uint32_t { return conn_timeout_; }

  /**
   * @brief Get the MQTT connection timeout.
   * @return The connection timeout value.
   */
  [[nodiscard]] auto disconnection_timeout() const -> std::chrono::milliseconds {
    return disconn_timeout_;
  }

 private:
  /**
   * @brief Create a new instance of MqttConfig.
   *
   * This constructor is private to enforce instance creation through the provided static factory
   * methods, ensuring that a valid credential type is always supplied.
   */
  MqttConfig(std::string_view realm, std::string_view device_id,
             std::unique_ptr<Credential> credential, std::string_view pairing_url,
             std::string_view store_dir);

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

}  // namespace AstarteDeviceSdk::config

#endif  // ASTARTE_MQTT_CONFIG_H
