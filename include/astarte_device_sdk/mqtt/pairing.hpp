// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_PAIRING_H
#define ASTARTE_MQTT_PAIRING_H

/**
 * @file astarte_device_sdk/mqtt/pairing.hpp
 * @brief Astarte pairing API and utilities.
 *
 * @details This file defines the PairingApi class and utility functions for
 * managing device registration, credential retrieval, and device ID generation.
 */

#include <ada.h>

#include <chrono>
#include <format>
#include <string>
#include <string_view>
#include <tuple>

#include "astarte_device_sdk/mqtt/errors.hpp"

namespace astarte::device::mqtt {

/// @cond DO_NOT_DOCUMENT
using namespace std::chrono_literals;
/// @endcond

/**
 * @brief Class providing the methods to perform the Astarte pairing operations.
 *
 * @details This class handles interactions with the Astarte Pairing API, including
 * device registration and fetching MQTT broker credentials or certificates.
 */
class PairingApi {
 public:
  /**
   * @brief Creates an instance of the PairingApi class.
   *
   * @param[in] realm The Astarte realm name.
   * @param[in] device_id The Astarte device id.
   * @param[in] astarte_base_url string containing the Astarte pairing API URL.
   * @return An expected containing the class instance on success or Error on failure.
   */
  static auto create(std::string_view realm, std::string_view device_id,
                     std::string_view astarte_base_url) -> astarte_tl::expected<PairingApi, Error>;

  /**
   * @brief Registers a device.
   *
   * @param[in] pairing_token The Astarte pairing token.
   * @param[in] timeout_ms A timeout value to perform the HTTP request.
   * @return An expected containing the credentials secret on success or Error on failure.
   */
  [[nodiscard]] auto register_device(std::string_view pairing_token,
                                     std::chrono::milliseconds timeout_ms = 0ms) const
      -> astarte_tl::expected<std::string, Error>;

  /**
   * @brief Retrieves the URL of the Astarte MQTT broker.
   *
   * @param[in] credential_secret The Astarte device credential necessary to authenticate to the
   * broker.
   * @param[in] timeout_ms A timeout value to perform the HTTP request.
   * @return An expected containing the broker URL on success or Error on failure.
   */
  [[nodiscard]] auto get_broker_url(std::string_view credential_secret, int timeout_ms = 0) const
      -> astarte_tl::expected<std::string, Error>;

  /**
   * @brief Retrieves the Astarte device certificate and relative private key.
   *
   * @param[in] credential_secret The Astarte device credential necessary to authenticate to the
   * broker.
   * @param[in] timeout_ms A timeout value to perform the HTTP request.
   * @return An expected containing the tuple of key and certificate on success or Error on failure.
   */
  [[nodiscard]] auto get_device_key_and_certificate(std::string_view credential_secret,
                                                    int timeout_ms = 0) const
      -> astarte_tl::expected<std::tuple<std::string, std::string>, Error>;

  /**
   * @brief Checks if the Astarte device certificate is valid.
   *
   * @param[in] certificate The Astarte device certificate.
   * @param[in] credential_secret The Astarte device credential necessary to authenticate to the
   * broker.
   * @param[in] timeout_ms A timeout value to perform the HTTP request.
   * @return An expected containing the certificate validity on success or Error on failure.
   */
  [[nodiscard]] auto device_cert_valid(std::string_view certificate,
                                       std::string_view credential_secret, int timeout_ms = 0) const
      -> astarte_tl::expected<bool, Error>;

 private:
  /**
   * @brief Constructor for the PairingApi class.
   *
   * @param[in] realm The Astarte realm name.
   * @param[in] device_id The Astarte device id.
   * @param[in] pairing_url URL object for the Astarte pairing API.
   */
  PairingApi(std::string_view realm, std::string_view device_id, ada::url_aggregator pairing_url);

  /// @brief The Astarte realm name.
  std::string realm_;
  /// @brief The Astarte device id.
  std::string device_id_;
  /// @brief The Astarte pairing URL as a string.
  ada::url_aggregator pairing_url_;
};

/**
 * @brief Creates a random Astarte device id starting from a UUIDv4.
 *
 * @details Generates a random Astarte device id following Astarte specifications.
 * @return A string containing the device id.
 */
auto create_random_device_id() -> std::string;

/**
 * @brief Creates a deterministic Astarte device id using a UUIDv5.
 *
 * @details Generates a deterministic Astarte device id following Astarte specifications.
 *
 * @param[in] namespc namespace necessary to generate a UUIDv5.
 * @param[in] unique_data unique data necessary to generate a UUIDv5.
 * @return An expected containing the device id on success or Error on failure.
 */
auto create_deterministic_device_id(std::string_view namespc, std::string_view unique_data)
    -> astarte_tl::expected<std::string, Error>;

}  // namespace astarte::device::mqtt

#endif  // ASTARTE_MQTT_PAIRING_H
