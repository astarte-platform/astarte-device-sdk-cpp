// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_PAIRING_H
#define ASTARTE_MQTT_PAIRING_H

#include <chrono>
#include <format>
#include <string>
#include <string_view>

#include "ada.h"
#include "astarte_device_sdk/mqtt/errors.hpp"

namespace AstarteDeviceSdk {

/// using namespace for timeout values
using namespace std::chrono_literals;

/** @brief Class providing the methods to perform the Astarte pairing operations */
class PairingApi {
 public:
  /**
   * @brief Create an instance of the PairingApi class.
   * @param realm The Astarte realm name.
   * @param device_id The Astarte device id.
   * @param astarte_base_url string containing the Astarte pairing API URL.
   * @return The generate class instance, or an error on failure.
   */
  static auto create(std::string_view realm, std::string_view device_id,
                     std::string_view astarte_base_url)
      -> astarte_tl::expected<PairingApi, AstarteError>;

  /**
   * @brief Register a device.
   * @param pairing_token The Astarte pairing token.
   * @param timeout_ms A timeout value to perform the HTTP request.
   * @return The credentials secret if successful, otherwise an error.
   */
  auto register_device(std::string_view pairing_token,
                       std::chrono::milliseconds timeout_ms = 0ms) const
      -> astarte_tl::expected<std::string, AstarteError>;

 private:
  /**
   * @brief Constructor for the PairingApi class.
   * @param realm The Astarte realm name.
   * @param device_id The Astarte device id.
   * @param pairing_url string containing the Astarte pairing API URL.
   */
  PairingApi(std::string_view realm, std::string_view device_id, ada::url_aggregator pairing_url);
  /**
   * @brief Retrieve the URL of the Astarte MQTT broker.
   * @param credential_secret The Astarte device credential necessary to authenticate to the broker.
   * @param timeout_ms A timeout value to perform the HTTP request.
   * @return The broker URL on success, an error otherwise.
   */
  auto get_broker_url(std::string_view credential_secret, int timeout_ms = 0) const
      -> astarte_tl::expected<std::string, AstarteError>;

  /**
   * @brief Retrieve the Astarte device certificate.
   * @param credential_secret The Astarte device credential necessary to authenticate to the broker.
   * @param timeout_ms A timeout value to perform the HTTP request.
   * @return The device certificate on success, an error otherwise.
   */
  auto get_device_cert(std::string_view credential_secret, int timeout_ms = 0) const
      -> astarte_tl::expected<std::string, AstarteError>;

  /**
   * @brief Check if the Astarte device certificate is valid.
   * @param certificate The Astarte device certificate.
   * @param credential_secret The Astarte device credential necessary to authenticate to the broker.
   * @param timeout_ms A timeout value to perform the HTTP request.
   * @return true if the cerficate is valid, false if the certificate is not valid, an error when
   * the validation failed.
   */
  auto device_cert_valid(std::string_view certificate, std::string_view credential_secret,
                         int timeout_ms = 0) const -> astarte_tl::expected<bool, AstarteError>;

  /** @brief The Astarte realm name. */
  const std::string realm_;
  /** @brief The Astarte device id. */
  const std::string device_id_;
  /** @brief The Astarte pairing URL as a string. */
  const ada::url_aggregator pairing_url_;
};

/**
 * @brief Create a random Astarte device id starting from a UUIDv4.
 *
 * Generate a random Astarte device id follow Astarte specifications as described here
 * https://docs.astarte-platform.org/astarte/latest/010-design_principles.html#device-id
 *
 * @return A string containing the device id.
 */
auto create_random_device_id() -> std::string;

/**
 * @brief Create a deterministic Astarte device id using a UUIDv5.
 *
 * Generate a random Astarte device id follow Astarte specifications as described here
 * https://docs.astarte-platform.org/astarte/latest/010-design_principles.html#device-id
 *
 * @param namespc namespace necessary to generate a UUIDv5.
 * @param unique_data unique necessary to generate a UUIDv5.
 * @return A string containing the device id, or an error on failure.
 */
auto create_deterministic_device_id(std::string_view namespc, std::string_view unique_data)
    -> astarte_tl::expected<std::string, AstarteError>;

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_MQTT_PAIRING_H
