// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_MQTT_PAIRING_H
#define ASTARTE_MQTT_PAIRING_H

#include <format>
#include <string>
#include <string_view>

#include "ada.h"
#include "exceptions.hpp"

namespace AstarteDeviceSdk {

/** @brief Class providing the methods to perform the Astarte pairing operations */
class ApiClient {
 public:
  /**
   * @brief Constructor for the ApiClient class.
   * @param realm The Astarte realm name.
   * @param device_id The Astarte device id.
   * @param pairing_url The Astarte pairing URL as a string.
   */
  ApiClient(std::string_view realm, std::string_view device_id, std::string_view pairing_url)
      : realm(realm), device_id(device_id), pairing_url(parse_and_validate_url(pairing_url)) {}

  // TODO: add a constructor without the device_id, it will generate randomply one.

  /**
   * @brief Register a device.
   * @param pairing_jwt The Astarte pairing token.
   * @param timeout_ms A timeout value to perform the HTTP request.
   * @return The credentials secret if successful, otherwise std::nullopt.
   */
  auto register_device(std::string_view pairing_jwt, int timeout_ms = 0) const -> std::string;

  /**
   * @brief Retrieve the URL of the Astarte MQTT broker.
   * @param credential_secret The Astarte device credential necessary to authenticate to the broker.
   * @param timeout_ms A timeout value to perform the HTTP request.
   * @return The broker URL.
   */
  auto get_broker_url(std::string_view credential_secret, int timeout_ms = 0) const -> std::string;

  /** @brief The Astarte realm name. */
  const std::string realm;
  /** @brief The Astarte device id. */
  const std::string device_id;
  /** @brief The Astarte pairing URL as a string. */
  const ada::url_aggregator pairing_url;

 private:
  // static helper function to handle URL parsing.
  static auto parse_and_validate_url(std::string_view url) -> ada::url_aggregator {
    auto parsed_url = ada::parse(url);
    if (!parsed_url) {
      throw InvalidUrlException(
          std::format("Failed to register device. Provided wrong pairing URL: {}", url));
    }
    return std::move(*parsed_url);
  }
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_MQTT_PAIRING_H
