// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_PAIRING_H
#define ASTARTE_MQTT_PAIRING_H

#include <chrono>
#include <format>
#include <string>
#include <string_view>
#include <tuple>

#include "ada.h"
#include "exceptions.hpp"

namespace AstarteDeviceSdk {

/// using namespace for timeout values
using namespace std::chrono_literals;

/** @brief Class providing the methods to perform the Astarte pairing operations */
class PairingApi {
 public:
  /**
   * @brief Constructor for the PairingApi class.
   * @param realm The Astarte realm name.
   * @param device_id The Astarte device id.
   * @param astarte_base_url string containing the Astarte pairing API URL.
   */
  PairingApi(std::string_view realm, std::string_view device_id, std::string_view astarte_base_url)
      : realm(realm), device_id(device_id), pairing_url(create_pairing_url(astarte_base_url)) {}

  /**
   * @brief Register a device.
   * @param pairing_token The Astarte pairing token.
   * @param timeout_ms A timeout value to perform the HTTP request.
   * @return The credentials secret if successful, otherwise std::nullopt.
   */
  auto register_device(std::string_view pairing_token,
                       std::chrono::milliseconds timeout_ms = 0ms) const -> std::string;

  /**
   * @brief Retrieve the URL of the Astarte MQTT broker.
   * @param credential_secret The Astarte device credential necessary to authenticate to the broker.
   * @param timeout_ms A timeout value to perform the HTTP request.
   * @return The broker URL.
   */
  auto get_broker_url(std::string_view credential_secret, int timeout_ms = 0) const -> std::string;

  /**
   * @brief Retrieve the Astarte device private key and certificate.
   * @param credential_secret The Astarte device credential necessary to authenticate to the broker.
   * @param timeout_ms A timeout value to perform the HTTP request.
   * @return The device private key in PEM format.
   * @return The device certificate in PEM format.
   */
  auto get_device_key_and_cert(std::string_view credential_secret, int timeout_ms = 0) const
      -> std::tuple<std::string, std::string>;

 private:
  /**
   * @brief Check if the Astarte device certificate is valid.
   * @param certificate The Astarte device certificate.
   * @param credential_secret The Astarte device credential necessary to authenticate to the broker.
   * @param timeout_ms A timeout value to perform the HTTP request.
   * @return true if the cerficate is valid, false otherwise.
   */
  auto device_cert_valid(std::string_view certificate, std::string_view credential_secret,
                         int timeout_ms = 0) const -> bool;

  /**
   * @brief Helper function to parse astarte base URL and create the pairing URL.
   * @param astarte_base_url The Astarte base URL.
   * @return The Astarte Pairing API broker URL.
   */
  static auto create_pairing_url(std::string_view astarte_base_url) -> ada::url_aggregator;

  /**
   * @brief Retrieve the Astarte device private key and CSR.
   * @return The device private key.
   * @return The device CSR.
   */
  auto get_device_key_and_csr() const -> std::tuple<std::string, std::string>;

  /** @brief The Astarte realm name. */
  const std::string realm;
  /** @brief The Astarte device id. */
  const std::string device_id;
  /** @brief The Astarte pairing URL as a string. */
  const ada::url_aggregator pairing_url;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_MQTT_PAIRING_H
