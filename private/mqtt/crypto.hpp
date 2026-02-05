// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_CRYPTO_HPP
#define ASTARTE_CRYPTO_HPP

/**
 * @file private/mqtt/crypto.hpp
 * @brief Cryptographic utilities for Astarte device SDK.
 *
 * @details This file defines wrappers around Mbed TLS and PSA Crypto APIs to handle
 * key generation and Certificate Signing Request (CSR) creation.
 */

#include <mbedtls/error.h>
#include <mbedtls/pem.h>
#include <mbedtls/pk.h>
#if MBEDTLS_VERSION_MAJOR < 0x04
#include <mbedtls/ctr_drbg.h>
#endif
#include <mbedtls/x509_crt.h>
#include <mbedtls/x509_csr.h>
#include <psa/crypto.h>

#include <string>
#include <string_view>
#include <vector>

#include "astarte_device_sdk/mqtt/errors.hpp"

namespace astarte::device::mqtt {

/**
 * @brief A C++ RAII wrapper for a PSA mbedtls_svc_key_id_t.
 * @details Manages the lifecycle of a PSA cryptographic key, ensuring proper cleanup.
 */
class PsaKey {
 public:
  /**
   * @brief Constructs an empty PsaKey, holding no key.
   * @return An expected containing the PsaKey instance on success, or an Error on failure.
   */
  static auto create() -> astarte_tl::expected<PsaKey, Error>;

  /// @brief Destroys the PsaKey, releasing the managed PSA key (if any).
  ~PsaKey();

  /// @brief PsaKey is not copy-constructible.
  PsaKey(const PsaKey&) = delete;

  /// @brief PsaKey is not copy-assignable.
  auto operator=(const PsaKey&) -> PsaKey& = delete;

  /**
   * @brief Move-constructs a PsaKey.
   * @param[in,out] other The PsaKey to move from.
   */
  PsaKey(PsaKey&& other) noexcept;

  /// @brief PsaKey is not move-assignable.
  auto operator=(PsaKey&&) -> PsaKey& = delete;

  /**
   * @brief Gets a reference to the underlying key ID.
   * @return The managed mbedtls_svc_key_id_t. Returns PSA_KEY_ID_NULL if this object is empty.
   */
  [[nodiscard]] auto get() const -> const mbedtls_svc_key_id_t&;

  /**
   * @brief Gets the PEM representation of the underlying key ID.
   * @return An expected string containing the PEM formatted key on success, or an Error on failure.
   */
  [[nodiscard]] auto to_pem() const -> astarte_tl::expected<const std::string, Error>;

  /**
   * @brief Creates a new ECDSA (secp256r1) private key.
   * @return An expected void on success, or an Error on failure.
   */
  auto generate() -> astarte_tl::expected<void, Error>;

 private:
  /// @brief Constructs an empty PsaKey, holding no key. */
  PsaKey();

  /// @brief The managed PSA key identifier. */
  mbedtls_svc_key_id_t key_id_;
};

/// @brief A utility class for Astarte cryptographic operations. */
class Crypto {
 public:
  /**
   * @brief Creates a Certificate Signing Request (CSR) from a private key.
   *
   * @param[in] priv_key A reference to the PsaKey holding the private key.
   * @return An expected string containing the CSR in PEM format on success, or an Error on failure.
   */
  static auto create_csr(const PsaKey& priv_key) -> astarte_tl::expected<std::string, Error>;
};

}  // namespace astarte::device::mqtt

#endif  // ASTARTE_CRYPTO_HPP
