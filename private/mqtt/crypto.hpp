// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_CRYPTO_HPP
#define ASTARTE_CRYPTO_HPP

#include <string>
#include <string_view>
#include <vector>

#include "mbedtls/error.h"
#include "mbedtls/pem.h"
#include "mbedtls/pk.h"
#if MBEDTLS_VERSION_MAJOR < 0x04
#include "mbedtls/ctr_drbg.h"
#endif
#include "astarte_device_sdk/mqtt/errors.hpp"
#include "mbedtls/x509_crt.h"
#include "mbedtls/x509_csr.h"
#include "psa/crypto.h"

namespace AstarteDeviceSdk {

/**
 * @brief A C++ RAII wrapper for a PSA mbedtls_svc_key_id_t.
 */
class PsaKey {
 public:
  /**
   @brief Constructs an empty PsaKey, holding no key.
   */
  static auto create() -> astarte_tl::expected<PsaKey, AstarteError>;
  /**
   * @brief Destroys the PsaKey, releasing the managed PSA key (if any).
   */
  ~PsaKey();
  /**
   * @brief PsaKey is not copy-constructible.
   */
  PsaKey(const PsaKey&) = delete;
  /**
   * @brief PsaKey is not copy-assignable.
   */
  PsaKey& operator=(const PsaKey&) = delete;
  /**
   * @brief Move-constructs a PsaKey.
   * @param other The PsaKey to move from.
   */
  PsaKey(PsaKey&& other) noexcept;
  /**
   * @brief PsaKey is not move-assignable.
   */
  PsaKey& operator=(PsaKey&&) = delete;
  /**
   * @brief Get a reference to the underlying key ID
   * @return The managed mbedtls_svc_key_id_t. Returns PSA_KEY_ID_NULL if this object is empty.
   */
  auto get() const -> const mbedtls_svc_key_id_t&;
  /**
   * @brief Get the PEM representation of the underlying key ID
   * @return The PEM representation of the underlying key ID, an error otherwise.
   */
  auto to_pem() const -> astarte_tl::expected<const std::string, AstarteError>;
  /**
   * @brief Creates a new ECDSA (secp256r1) private key.
   * @return An error on failure.
   */
  auto generate() -> astarte_tl::expected<void, AstarteError>;

 private:
  /**
   @brief Constructs an empty PsaKey, holding no key.
   */
  PsaKey();
  /**
   * @brief The managed PSA key identifier.
   */
  mbedtls_svc_key_id_t key_id_;
};

/**
 * @brief A utility class for Astarte cryptographic operations.
 */
class Crypto {
 public:
  /**
   * @brief Creates a Certificate Signing Request (CSR) from a private key.
   *
   * @param priv_key A reference to the PsaKey holding the private key.
   * @return A string containing the CSR in PEM format on success, an error otherwise.
   */
  static auto create_csr(const PsaKey& priv_key) -> astarte_tl::expected<std::string, AstarteError>;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_CRYPTO_HPP
