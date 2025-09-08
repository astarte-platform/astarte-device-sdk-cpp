// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_CRYPTO_HPP
#define ASTARTE_CRYPTO_HPP

#include <string>
#include <string_view>

namespace AstarteDeviceSdk {

/**
 * @brief Handle Astarte cryptographic operations.
 */
class Crypto {
 public:
  /**
   * @brief Creates a new ECDSA (secp256r1) private key.
   * @return A string containing the private key in PEM format.
   * @throws CryptoException on failure.
   */
  static auto create_key() -> std::string;

  /**
   * @brief Creates a Certificate Signing Request (CSR) from a private key.
   * @param privkey_pem The private key in PEM format.
   * @return A string containing the CSR in PEM format.
   * @throws CryptoException on failure.
   */
  static auto create_csr(std::string_view privkey_pem) -> std::string;

  /**
   * @brief Creates a random Astarte device id starting from a UUIDv4.
   * @return A string containing the device id.
   */
  static auto create_random_device_id() -> std::string;

  /**
   * @brief Creates a deterministic Astarte device id using a UUIDv5.
   * @param namespc namespace necessary to generate a UUIDv5.
   * @param unique_data unique necessary to generate a UUIDv5.
   * @return A string containing the device id.
   */
  static auto create_deterministic_device_id(std::string_view namespc, std::string_view unique_data)
      -> std::string;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_CRYPTO_HPP
