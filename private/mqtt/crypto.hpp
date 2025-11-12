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
  PsaKey();
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
   * @brief Move-assigns a PsaKey.
   * @param other The PsaKey to move from.
   * @return A reference to this object.
   * @throws CryptoException if destroying the *current* key fails.
   */
  auto operator=(PsaKey&& other) noexcept -> PsaKey&;
  /**
   * @brief Get a reference to the underlying key ID
   * @return The managed mbedtls_svc_key_id_t. Returns PSA_KEY_ID_NULL if this object is empty.
   */
  auto get() const -> const mbedtls_svc_key_id_t&;
  /**
   * @brief Get the PEM representation of the underlying key ID
   * @return The  PEM representation of the underlying key ID. returns an empty string if the key is PSA_KEY_ID_NULL.
   */
  auto to_pem() const -> std::string;
  /**
   * @brief Creates a new ECDSA (secp256r1) private key.
   * @throws CryptoException on failure.
   */
  void generate();

 private:
  /**
   * @brief Destroys the key, but does not throw on failure.
   */
  void destroy_key_nothrow() noexcept;

  /**
   * @brief The managed PSA key identifier.
   */
  mbedtls_svc_key_id_t key_id_;
};

/**
 * @brief A C++ RAII wrapper for an mbedtls_pk_context.
 */
class MbedPk {
 public:
  /**
   * @brief Constructs an MbedPk wrapper from a PsaKey.
   * @param psa_key The PsaKey containing the key to be wrapped.
   * @throws CryptoException if mbedtls_pk_setup_from_psa fails.
   */
  explicit MbedPk(const PsaKey& psa_key);
  /**
   * @brief Destroys the MbedPk, freeing the mbedtls_pk_context.
   */
  ~MbedPk();

  /**
   * @brief Gets a mutable reference to the underlying mbedtls_pk_context.
   * @return A reference to the managed context.
   */
  auto ctx() -> mbedtls_pk_context&;

 private:
  /**
   * @brief The managed mbedtls PK context.
   */
  mbedtls_pk_context ctx_;
};

/**
 * @brief A C++ RAII wrapper for an mbedtls_x509write_csr context.
 */
class MbedX509WriteCsr {
 public:
  /**
   * @brief Constructs and initializes the mbedtls_x509write_csr context.
   */
  MbedX509WriteCsr();
  /**
   * @brief Destroys the MbedX509WriteCsr, freeing the mbedtls_x509write_csr context.
   */
  ~MbedX509WriteCsr();
  /**
   * @brief Gets a mutable reference to the underlying mbedtls_x509write_csr context.
   * @return A reference to the managed context.
   */
  auto ctx() -> mbedtls_x509write_csr&;
  /**
   * @brief Generates the CSR in PEM format using the provided key.
   * @param key The private key (wrapped in MbedPk) to sign the CSR.
   * @return A std::vector<unsigned char> containing the CSR in PEM format.
   * @throws CryptoException if the CSR generation fails.
   */
  auto generate(MbedPk& key) -> std::vector<unsigned char>;

 private:
  /**
   * @brief The managed mbedtls CSR writer context.
   */
  mbedtls_x509write_csr ctx_;
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
   * @return A string containing the CSR in PEM format.
   * @throws CryptoException on failure.
   */
  static auto create_csr(const PsaKey& priv_key) -> std::string;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_CRYPTO_HPP
