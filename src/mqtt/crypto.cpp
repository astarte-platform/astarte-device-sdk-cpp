// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/mqtt/crypto.hpp"

#include <cstring>
#include <format>
#include <stdexcept>
#include <vector>

#include "astarte_device_sdk/mqtt/exceptions.hpp"

// Mbed TLS Headers
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecp.h"
#include "mbedtls/entropy.h"
#include "mbedtls/error.h"
#include "mbedtls/oid.h"
#include "mbedtls/pk.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/x509_csr.h"

namespace AstarteDeviceSdk {

// Define wrappers for Mbed TLS Resources and exploit RAII principle by ensuring that
// mbedtls_*_free is always called automatically.
class MbedPk {
 public:
  mbedtls_pk_context ctx;
  MbedPk() { mbedtls_pk_init(&ctx); }
  ~MbedPk() { mbedtls_pk_free(&ctx); }
};

class MbedX509WriteCsr {
 public:
  mbedtls_x509write_csr ctx;
  MbedX509WriteCsr() { mbedtls_x509write_csr_init(&ctx); }
  ~MbedX509WriteCsr() { mbedtls_x509write_csr_free(&ctx); }
};

class MbedX509Crt {
 public:
  mbedtls_x509_crt ctx;
  MbedX509Crt() { mbedtls_x509_crt_init(&ctx); }
  ~MbedX509Crt() { mbedtls_x509_crt_free(&ctx); }
};

class MbedEntropy {
 public:
  mbedtls_entropy_context ctx;
  MbedEntropy() { mbedtls_entropy_init(&ctx); }
  ~MbedEntropy() { mbedtls_entropy_free(&ctx); }
};

class MbedCtrDrbg {
 public:
  mbedtls_ctr_drbg_context ctx;
  MbedCtrDrbg() { mbedtls_ctr_drbg_init(&ctx); }
  ~MbedCtrDrbg() { mbedtls_ctr_drbg_free(&ctx); }
};

// Helper to convert Mbed TLS errors into C++ exceptions
void mbedtls_check(int ret, const std::string& function_name) {
  if (ret != 0) {
    char error_buf[100];
    mbedtls_strerror(ret, error_buf, sizeof(error_buf));
    throw CryptoException(std::format("{} failed: {}", function_name, error_buf));
  }
}

auto Crypto::create_key() -> std::string {
  MbedPk key;
  MbedEntropy entropy;
  MbedCtrDrbg ctr_drbg;

  const std::string pers = "astarte_credentials_create_key";

  // seed the RNG
  mbedtls_check(
      mbedtls_ctr_drbg_seed(&ctr_drbg.ctx, mbedtls_entropy_func, &entropy.ctx,
                            reinterpret_cast<const unsigned char*>(pers.c_str()), pers.length()),
      "mbedtls_ctr_drbg_seed");

  // setup and generate the EC key
  mbedtls_check(mbedtls_pk_setup(&key.ctx, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY)),
                "mbedtls_pk_setup");

  mbedtls_check(mbedtls_ecp_gen_key(MBEDTLS_ECP_DP_SECP256R1, mbedtls_pk_ec(key.ctx),
                                    mbedtls_ctr_drbg_random, &ctr_drbg.ctx),
                "mbedtls_ecp_gen_key");

  // write the key to a PEM string
  std::vector<unsigned char> buf(1024, 0);
  mbedtls_check(mbedtls_pk_write_key_pem(&key.ctx, buf.data(), buf.size()),
                "mbedtls_pk_write_key_pem");

  return std::string(reinterpret_cast<const char*>(buf.data()));
}

auto Crypto::create_csr(std::string_view privkey_pem) -> std::string {
  MbedX509WriteCsr req;
  MbedPk key;
  MbedEntropy entropy;
  MbedCtrDrbg ctr_drbg;

  const std::string pers = "astarte_credentials_create_csr";

  // seed the RNG
  mbedtls_check(
      mbedtls_ctr_drbg_seed(&ctr_drbg.ctx, mbedtls_entropy_func, &entropy.ctx,
                            reinterpret_cast<const unsigned char*>(pers.c_str()), pers.length()),
      "mbedtls_ctr_drbg_seed");

  // parse the private key (+1 for null terminator)
  mbedtls_check(mbedtls_pk_parse_key(
                    &key.ctx, reinterpret_cast<const unsigned char*>(privkey_pem.data()),
                    privkey_pem.length() + 1, nullptr, 0, mbedtls_ctr_drbg_random, &ctr_drbg.ctx),
                "mbedtls_pk_parse_key");

  // configure the CSR
  mbedtls_x509write_csr_set_key(&req.ctx, &key.ctx);
  mbedtls_x509write_csr_set_md_alg(&req.ctx, MBEDTLS_MD_SHA256);
  mbedtls_check(mbedtls_x509write_csr_set_subject_name(&req.ctx, "CN=temporary"),
                "mbedtls_x509write_csr_set_subject_name");

  // write the CSR to a PEM string
  std::vector<unsigned char> buf(2048, 0);
  mbedtls_check(mbedtls_x509write_csr_pem(&req.ctx, buf.data(), buf.size(), mbedtls_ctr_drbg_random,
                                          &ctr_drbg.ctx),
                "mbedtls_x509write_csr_pem");

  return std::string(reinterpret_cast<const char*>(buf.data()));
}
}  // namespace AstarteDeviceSdk
