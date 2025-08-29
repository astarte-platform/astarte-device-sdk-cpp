// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

// TODO(sorru94) Consider disabling this checker fully or disable the MissingInclude setting
// NOLINTBEGIN(misc-include-cleaner) PSA structures are not intended to be included directly

#include "mqtt/crypto.hpp"

#include <mbedtls/build_info.h>
#include <psa/crypto.h>
#include <spdlog/spdlog.h>

#include <array>
#include <cstring>
#include <format>
#include <string>
#include <vector>
#if MBEDTLS_VERSION_MAJOR < 0x04
#include <mbedtls/ctr_drbg.h>
#endif
#include <mbedtls/error.h>
#include <mbedtls/md.h>
#include <mbedtls/pk.h>
#include <mbedtls/x509_csr.h>

#include "astarte_device_sdk/errors.hpp"

namespace AstarteDeviceSdk {

namespace {

auto mbedtls_ret_to_astarte_errors(int ret, const std::string& function_name)
    -> astarte_tl::expected<void, AstarteError> {
  if (ret != 0) {
    constexpr std::size_t error_buf_size = 100;
    std::array<char, error_buf_size> error_buf{};
    mbedtls_strerror(ret, error_buf.data(), error_buf.size());
    return astarte_tl::unexpected(AstarteCryptoError(
        astarte_fmt::format("{} failed: {:#x} {}", function_name, ret, error_buf.data())));
  }
  return {};
}

class MbedPk {
 public:
  static auto create(const PsaKey& psa_key) -> astarte_tl::expected<MbedPk, AstarteError> {
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
struct MbedDrbgGuard {
  mbedtls_ctr_drbg_context ctx;
  MbedDrbgGuard() { mbedtls_ctr_drbg_init(&ctx); }
  ~MbedDrbgGuard() { mbedtls_ctr_drbg_free(&ctx); }
  MbedDrbgGuard(const MbedDrbgGuard&) = delete;
  auto operator=(const MbedDrbgGuard&) -> MbedDrbgGuard& = delete;
  MbedDrbgGuard(MbedDrbgGuard&&) = delete;
  auto operator=(MbedDrbgGuard&&) -> MbedDrbgGuard& = delete;
};

struct MbedEntropyGuard {
  mbedtls_entropy_context ctx;
  MbedEntropyGuard() { mbedtls_entropy_init(&ctx); }
  ~MbedEntropyGuard() { mbedtls_entropy_free(&ctx); }
  MbedEntropyGuard(const MbedEntropyGuard&) = delete;
  auto operator=(const MbedEntropyGuard&) -> MbedEntropyGuard& = delete;
  MbedEntropyGuard(MbedEntropyGuard&&) = delete;
  auto operator=(MbedEntropyGuard&&) -> MbedEntropyGuard& = delete;
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
  auto generate(MbedPk& key) -> astarte_tl::expected<std::vector<unsigned char>, AstarteError> {
    // configure the CSR
    mbedtls_x509write_csr_set_key(&ctx_, &key.ctx());
    mbedtls_x509write_csr_set_md_alg(&ctx_, MBEDTLS_MD_SHA256);

    // write the CSR to a PEM string
    constexpr std::size_t csr_buf_size{2048};
    std::vector<unsigned char> csr_buf(csr_buf_size, 0);

    auto res =
        mbedtls_ret_to_astarte_errors(mbedtls_x509write_csr_set_subject_name(&ctx_, "CN=temporary"),
                                      "mbedtls_x509write_csr_set_subject_name");

#if MBEDTLS_VERSION_MAJOR < 0x04
    MbedEntropyGuard entropy;
    MbedDrbgGuard drbg;

    std::string pers = "astarte_credentials_create_key";

    return res
        .and_then([&]() {
          return mbedtls_ret_to_astarte_errors(
              mbedtls_ctr_drbg_seed(&drbg.ctx, mbedtls_entropy_func, &entropy.ctx,
                                    reinterpret_cast<const unsigned char*>(pers.c_str()),
                                    pers.length()),
              "mbedtls_ctr_drbg_seed");
        })
        .and_then([&]() {
          return mbedtls_ret_to_astarte_errors(
              mbedtls_x509write_csr_pem(&ctx_, csr_buf.data(), csr_buf.size(),
                                        mbedtls_ctr_drbg_random, &drbg.ctx),
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

auto PsaKey::create() -> astarte_tl::expected<PsaKey, AstarteError> {
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

auto PsaKey::to_pem() const -> astarte_tl::expected<const std::string, AstarteError> {
  auto key = MbedPk::create(*this);
  if (!key) {
    spdlog::error("Failed to create MBedPk key from PsaKey. Error: {}", key.error());
  }

  std::vector<unsigned char> buf(1024, 0);
  auto res = mbedtls_ret_to_astarte_errors(
      mbedtls_pk_write_key_pem(&key.value().ctx(), buf.data(), buf.size()),
      "mbedtls_pk_write_key_pem");
  if (!res) {
    spdlog::error("PsaKey failed to write the key. Key ID {} may be leaked. Error: {}", key_id_,
                  res.error());
    return astarte_tl::unexpected(res.error());
  }

  return std::string(reinterpret_cast<const char*>(buf.data()));
}

auto PsaKey::generate() -> astarte_tl::expected<void, AstarteError> {
  // generate the PSA EC key
  psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
  psa_set_key_algorithm(&attributes, PSA_ECC_FAMILY_SECP_R1);
  psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_EXPORT);
  psa_set_key_type(&attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
  constexpr std::size_t key_size{256};
  psa_set_key_bits(&attributes, key_size);

  return mbedtls_ret_to_astarte_errors(psa_generate_key(&attributes, &key_id_), "psa_generate_key");
}

auto Crypto::create_csr(const PsaKey& priv_key) -> astarte_tl::expected<std::string, AstarteError> {
  return MbedPk::create(priv_key)
      .and_then([&](auto&& key_value) {
        auto csr = MbedX509WriteCsr();
        return csr.generate(key_value);
      })
      .transform([&](auto&& buf_value) { return std::string(buf_value.begin(), buf_value.end()); });
}

}  // namespace AstarteDeviceSdk

// NOLINTEND(misc-include-cleaner)
