// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "mqtt/crypto.hpp"

#include <spdlog/spdlog.h>

#include <array>
#include <cstdint>
#include <cstring>
#include <format>
#include <string>
#include <utility>
#include <vector>

#include "astarte_device_sdk/mqtt/exceptions.hpp"
#include "mbedtls/build_info.h"
#include "mbedtls/error.h"
#include "mbedtls/md.h"
#include "mbedtls/pk.h"
#include "mbedtls/x509_csr.h"
#if MBEDTLS_VERSION_MAJOR < 0x04
#include "mbedtls/ctr_drbg.h"
#endif
#include "psa/crypto.h"
#include "psa/crypto_struct.h"
#include "psa/crypto_types.h"
#include "psa/crypto_values.h"

namespace AstarteDeviceSdk {

constexpr size_t ERROR_BUF_LEN = 100;
constexpr size_t PSA_KEY_LEN = 1024;
constexpr size_t PSA_KEY_BITS = 256;
constexpr size_t PSA_CSR_LEN = 2048;

// Helper to convert Mbed TLS errors into C++ exceptions
namespace {
void mbedtls_check(int ret, const std::string& function_name) {
  if (ret != 0) {
    auto error_buf = std::array<char, ERROR_BUF_LEN>();
    mbedtls_strerror(ret, error_buf.data(), error_buf.size());
    throw CryptoException(std::format("{} failed: {}", function_name, error_buf.data()));
  }
}
}  // namespace

PsaKey::PsaKey() : key_id_(PSA_KEY_ID_NULL) { mbedtls_check(psa_crypto_init(), "psa_crypto_init"); }
PsaKey::~PsaKey() { destroy_key_nothrow(); }

PsaKey::PsaKey(PsaKey&& other) noexcept : key_id_(other.key_id_) {
  other.key_id_ = PSA_KEY_ID_NULL;
}

auto PsaKey::operator=(PsaKey&& other) noexcept -> PsaKey& {
  if (this != &other) {
    std::swap(key_id_, other.key_id_);
  }
  return *this;
}

auto PsaKey::get() const -> const mbedtls_svc_key_id_t& { return key_id_; }

auto PsaKey::to_pem() const -> std::string {
  auto key = MbedPk(*this);

  std::vector<unsigned char> buf(PSA_KEY_LEN, 0);
  mbedtls_check(mbedtls_pk_write_key_pem(&key.ctx(), buf.data(), buf.size()),
                "mbedtls_pk_write_key_pem");

  return std::string(reinterpret_cast<const char*>(buf.data()));  // {buf.begin(), buf.size()};
}

void PsaKey::generate() {
  // generate the PSA EC key
  psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
  psa_set_key_algorithm(&attributes, PSA_ECC_FAMILY_SECP_R1);
  psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_EXPORT);
  psa_set_key_type(&attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
  psa_set_key_bits(&attributes, PSA_KEY_BITS);

  mbedtls_check(psa_generate_key(&attributes, &key_id_), "psa_generate_key");
}

void PsaKey::destroy_key_nothrow() noexcept {
  if (key_id_ != PSA_KEY_ID_NULL) {
    const int32_t ret = psa_destroy_key(key_id_);

    if (ret != 0) {
      auto error_buf = std::array<char, ERROR_BUF_LEN>();
      mbedtls_strerror(ret, error_buf.data(), error_buf.size());
      spdlog::critical("PsaKey failed to destroy key {}: {}", key_id_, error_buf.data());
    }

    key_id_ = PSA_KEY_ID_NULL;
  }
}

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
MbedPk::MbedPk(const PsaKey& psa_key) {
  mbedtls_pk_init(&ctx_);
  mbedtls_check(mbedtls_pk_copy_from_psa(psa_key.get(), &ctx_), "mbedtls_pk_copy_from_psa");
}

MbedPk::~MbedPk() { mbedtls_pk_free(&ctx_); }

auto MbedPk::ctx() -> mbedtls_pk_context& { return ctx_; }

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
MbedX509WriteCsr::MbedX509WriteCsr() { mbedtls_x509write_csr_init(&ctx_); }
MbedX509WriteCsr::~MbedX509WriteCsr() { mbedtls_x509write_csr_free(&ctx_); }
auto MbedX509WriteCsr::ctx() -> mbedtls_x509write_csr& { return ctx_; }

auto MbedX509WriteCsr::generate(MbedPk& key) -> std::vector<unsigned char> {
  // configure the CSR
  mbedtls_x509write_csr_set_key(&ctx_, &key.ctx());
  mbedtls_x509write_csr_set_md_alg(&ctx_, MBEDTLS_MD_SHA256);
  mbedtls_check(mbedtls_x509write_csr_set_subject_name(&ctx_, "CN=temporary"),
                "mbedtls_x509write_csr_set_subject_name");

  // write the CSR to a PEM string
  std::vector<unsigned char> buf(PSA_CSR_LEN, 0);

#if MBEDTLS_VERSION_MAJOR < 0x04
  mbedtls_ctr_drbg_context ctr_drbg_ctx;
  mbedtls_ctr_drbg_init(&ctr_drbg_ctx);
  mbedtls_entropy_context entropy_ctx;
  mbedtls_entropy_init(&entropy_ctx);

  // seed the RNG
  std::string pers = "astarte_credentials_create_key";
  mbedtls_check(
      mbedtls_ctr_drbg_seed(&ctr_drbg_ctx, mbedtls_entropy_func, &entropy_ctx,
                            reinterpret_cast<const unsigned char*>(pers.c_str()), pers.length()),
      "mbedtls_ctr_drbg_seed");

  mbedtls_check(mbedtls_x509write_csr_pem(&ctx_, buf.data(), buf.size(), mbedtls_ctr_drbg_random,
                                          &ctr_drbg_ctx),
                "mbedtls_x509write_csr_pem");

  mbedtls_ctr_drbg_free(&ctr_drbg_ctx);
  mbedtls_entropy_free(&entropy_ctx);
#else
  mbedtls_check(mbedtls_x509write_csr_pem(&ctx_, buf.data(), buf.size()),
                "mbedtls_x509write_csr_pem");
#endif

  return buf;
}

auto Crypto::create_csr(const PsaKey& priv_key) -> std::string {
  auto key = MbedPk(priv_key);
  auto csr = MbedX509WriteCsr();
  auto buf = csr.generate(key);
  return std::string(reinterpret_cast<const char*>(buf.data()));  // {buf.begin(), buf.size()};
}

}  // namespace AstarteDeviceSdk
