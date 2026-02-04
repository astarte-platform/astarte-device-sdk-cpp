// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

// TODO(sorru94) Consider disabling this checker fully or disable the MissingInclude setting
// PSA structures are not intended to be included directly

#include "mqtt/crypto.hpp"

#include <spdlog/spdlog.h>

#include <array>
#include <cstdint>
#include <cstring>
#include <format>
#include <string>
#include <utility>
#include <vector>
#if MBEDTLS_VERSION_MAJOR < 0x04
#include <mbedtls/ctr_drbg.h>
#endif
#include <mbedtls/build_info.h>
#include <mbedtls/error.h>
#include <mbedtls/md.h>
#include <mbedtls/pk.h>
#include <mbedtls/x509_csr.h>

#include "astarte_device_sdk/errors.hpp"
#include "astarte_device_sdk/formatter.hpp"
#include "psa/crypto.h"
#include "psa/crypto_struct.h"
#include "psa/crypto_types.h"
#include "psa/crypto_values.h"

namespace astarte::device::mqtt {

constexpr size_t ERROR_BUF_LEN = 100;
constexpr size_t PSA_KEY_LEN = 1024;
constexpr size_t PSA_KEY_BITS = 256;
constexpr size_t PSA_CSR_LEN = 2048;

// Helper to convert Mbed TLS errors into C++ exceptions
namespace {

auto mbedtls_ret_to_astarte_errors(int ret, const std::string& function_name)
    -> astarte_tl::expected<void, Error> {
  if (ret != 0) {
    constexpr std::size_t error_buf_size = 100;
    std::array<char, error_buf_size> error_buf{};
    mbedtls_strerror(ret, error_buf.data(), error_buf.size());
    return astarte_tl::unexpected(CryptoError(
        astarte_fmt::format("{} failed: {:#x} {}", function_name, ret, error_buf.data())));
  }
  return {};
}

class MbedPk {
 public:
  static auto create(const PsaKey& psa_key) -> astarte_tl::expected<MbedPk, Error> {
    MbedPk key;
    auto res = mbedtls_ret_to_astarte_errors(mbedtls_pk_copy_from_psa(psa_key.get(), &key.ctx()),
                                             "mbedtls_pk_copy_from_psa");
    if (!res) {
      return astarte_tl::unexpected(res.error());
    }
    return key;
  }
  ~MbedPk() { mbedtls_pk_free(&ctx_); }
  MbedPk(const MbedPk&) = delete;
  auto operator=(const MbedPk&) -> MbedPk& = delete;
  MbedPk(MbedPk&& other) noexcept : ctx_(other.ctx_) { mbedtls_pk_init(&other.ctx_); }
  auto operator=(MbedPk&& other) noexcept -> MbedPk& {
    if (this != &other) {
      mbedtls_pk_free(&ctx_);
      ctx_ = other.ctx_;
      mbedtls_pk_init(&other.ctx_);
    }
    return *this;
  }
  auto ctx() -> mbedtls_pk_context& { return ctx_; }

 private:
  MbedPk() { mbedtls_pk_init(&ctx_); }
  mbedtls_pk_context ctx_{};
};

#if MBEDTLS_VERSION_MAJOR < 0x04
class MbedDrbgGuard {
 public:
  MbedDrbgGuard() { mbedtls_ctr_drbg_init(&ctx_); }
  ~MbedDrbgGuard() { mbedtls_ctr_drbg_free(&ctx_); }
  MbedDrbgGuard(const MbedDrbgGuard&) = delete;
  auto operator=(const MbedDrbgGuard&) -> MbedDrbgGuard& = delete;
  MbedDrbgGuard(MbedDrbgGuard&&) = delete;
  auto operator=(MbedDrbgGuard&&) -> MbedDrbgGuard& = delete;
  auto ctx() -> mbedtls_ctr_drbg_context& { return ctx_; }

 private:
  mbedtls_ctr_drbg_context ctx_{};
};

class MbedEntropyGuard {
 public:
  MbedEntropyGuard() { mbedtls_entropy_init(&ctx_); }
  ~MbedEntropyGuard() { mbedtls_entropy_free(&ctx_); }
  MbedEntropyGuard(const MbedEntropyGuard&) = delete;
  auto operator=(const MbedEntropyGuard&) -> MbedEntropyGuard& = delete;
  MbedEntropyGuard(MbedEntropyGuard&&) = delete;
  auto operator=(MbedEntropyGuard&&) -> MbedEntropyGuard& = delete;
  auto ctx() -> mbedtls_entropy_context& { return ctx_; }

 private:
  mbedtls_entropy_context ctx_{};
};
#endif

class MbedX509WriteCsr {
 public:
  MbedX509WriteCsr() { mbedtls_x509write_csr_init(&ctx_); }
  ~MbedX509WriteCsr() { mbedtls_x509write_csr_free(&ctx_); }
  MbedX509WriteCsr(const MbedX509WriteCsr&) = delete;
  auto operator=(const MbedX509WriteCsr&) -> MbedX509WriteCsr& = delete;
  MbedX509WriteCsr(MbedX509WriteCsr&& other) = delete;
  auto operator=(MbedX509WriteCsr&& other) -> MbedX509WriteCsr& = delete;
  auto ctx() -> mbedtls_x509write_csr& { return ctx_; }
  auto generate(MbedPk& key) -> astarte_tl::expected<std::vector<unsigned char>, Error> {
    // configure the CSR
    mbedtls_x509write_csr_set_key(&ctx_, &key.ctx());
    mbedtls_x509write_csr_set_md_alg(&ctx_, MBEDTLS_MD_SHA256);

    // write the CSR to a PEM string
    constexpr std::size_t csr_buf_size{PSA_CSR_LEN};
    std::vector<unsigned char> csr_buf(csr_buf_size, 0);

    auto res =
        mbedtls_ret_to_astarte_errors(mbedtls_x509write_csr_set_subject_name(&ctx_, "CN=temporary"),
                                      "mbedtls_x509write_csr_set_subject_name");

#if MBEDTLS_VERSION_MAJOR < 0x04
    MbedEntropyGuard entropy;
    MbedDrbgGuard drbg;

    const std::string pers_str = "astarte_credentials_create_key";
    std::vector<unsigned char> pers(pers_str.begin(), pers_str.end());

    return res
        .and_then([&]() {
          return mbedtls_ret_to_astarte_errors(
              mbedtls_ctr_drbg_seed(&drbg.ctx(), mbedtls_entropy_func, &entropy.ctx(), pers.data(),
                                    pers.size()),
              "mbedtls_ctr_drbg_seed");
        })
        .and_then([&]() {
          return mbedtls_ret_to_astarte_errors(
              mbedtls_x509write_csr_pem(&ctx_, csr_buf.data(), csr_buf.size(),
                                        mbedtls_ctr_drbg_random, &drbg.ctx()),
              "mbedtls_x509write_csr_pem");
        })
        .transform([&]() { return csr_buf; });
#else
    return res
        .and_then([&]() {
          return mbedtls_ret_to_astarte_errors(
              mbedtls_x509write_csr_pem(&ctx_, csr_buf.data(), csr_buf.size()),
              "mbedtls_x509write_csr_pem");
        })
        .transform([&]() { return csr_buf; });
#endif
  }

 private:
  mbedtls_x509write_csr ctx_{};
};
}  // namespace

auto PsaKey::create() -> astarte_tl::expected<PsaKey, Error> {
  auto res = mbedtls_ret_to_astarte_errors(psa_crypto_init(), "psa_crypto_init");
  if (!res) {
    return astarte_tl::unexpected(res.error());
  }
  return PsaKey();
}

PsaKey::PsaKey() : key_id_(PSA_KEY_ID_NULL) {}

PsaKey::~PsaKey() {
  if (key_id_ != PSA_KEY_ID_NULL) {
    auto res = mbedtls_ret_to_astarte_errors(psa_destroy_key(key_id_), "psa_destroy_key");
    if (!res) {
      spdlog::error(
          "PsaKey destructor failed to destroy the key. Key ID {} may be leaked. Error: {}",
          key_id_, res.error());
    }
  }
}

PsaKey::PsaKey(PsaKey&& other) noexcept : key_id_(other.key_id_) {
  other.key_id_ = PSA_KEY_ID_NULL;
}

auto PsaKey::get() const -> const mbedtls_svc_key_id_t& { return key_id_; }

auto PsaKey::to_pem() const -> astarte_tl::expected<const std::string, Error> {
  auto key = MbedPk::create(*this);
  if (!key) {
    spdlog::error("Failed to create MBedPk key from PsaKey. Error: {}", key.error());
  }

  std::vector<unsigned char> buf(PSA_KEY_LEN, 0);
  auto res = mbedtls_ret_to_astarte_errors(
      mbedtls_pk_write_key_pem(&key.value().ctx(), buf.data(), buf.size()),
      "mbedtls_pk_write_key_pem");
  if (!res) {
    spdlog::error("PsaKey failed to write the key. Key ID {} may be leaked. Error: {}", key_id_,
                  res.error());
    return astarte_tl::unexpected(res.error());
  }

  return std::string(buf.begin(), buf.end());
}

auto PsaKey::generate() -> astarte_tl::expected<void, Error> {
  // generate the PSA EC key
  psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
  psa_set_key_algorithm(&attributes, PSA_ECC_FAMILY_SECP_R1);
  psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_EXPORT);
  psa_set_key_type(&attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
  psa_set_key_bits(&attributes, PSA_KEY_BITS);

  return mbedtls_ret_to_astarte_errors(psa_generate_key(&attributes, &key_id_), "psa_generate_key");
}

auto Crypto::create_csr(const PsaKey& priv_key) -> astarte_tl::expected<std::string, Error> {
  return MbedPk::create(priv_key)
      .and_then([&](auto&& key_value) {
        auto csr = MbedX509WriteCsr();
        return csr.generate(key_value);
      })
      .transform([&](auto&& buf_value) { return std::string(buf_value.begin(), buf_value.end()); });
}

}  // namespace astarte::device::mqtt
