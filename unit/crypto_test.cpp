// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "astarte_device_sdk/formatter.hpp"

#if !defined(ASTARTE_TRANSPORT_GRPC)
#include "mqtt/crypto.hpp"

using AstarteDeviceSdk::Crypto;
using AstarteDeviceSdk::PsaKey;

TEST(AstarteTestCrypto, ParseNullPsaKey) {
  auto key = PsaKey::create();
  ASSERT_TRUE(key);
  ASSERT_EQ(key.value().get(), PSA_KEY_ID_NULL);
}

TEST(AstarteTestCrypto, ParsePsaKey) {
  auto key = PsaKey::create();
  ASSERT_TRUE(key);
  ASSERT_TRUE(key.value().generate());
  psa_key_attributes_t attr = psa_key_attributes_init();

  auto ret = psa_get_key_attributes(key.value().get(), &attr);
  ASSERT_EQ(ret, 0);

  auto algorithm = psa_get_key_algorithm(&attr);
  ASSERT_EQ(algorithm, PSA_ECC_FAMILY_SECP_R1);
  auto flags = psa_get_key_usage_flags(&attr);
  ASSERT_EQ(flags, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_EXPORT);
  auto key_type = psa_get_key_type(&attr);
  ASSERT_EQ(key_type, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
  auto key_bits = psa_get_key_bits(&attr);
  ASSERT_EQ(key_bits, 256);

  psa_reset_key_attributes(&attr);
}

TEST(AstarteTestCrypto, ParseCsr) {
  auto psa_key = PsaKey::create();
  ASSERT_TRUE(psa_key);
  auto gen_res = psa_key.value().generate();
  ASSERT_TRUE(gen_res) << astarte_fmt::format("{}", gen_res.error());

  auto csr = Crypto::create_csr(psa_key.value());
  ASSERT_TRUE(csr) << astarte_fmt::format("{}", csr.error());

  mbedtls_x509_csr csr_n;
  mbedtls_x509_csr_init(&csr_n);

  auto ret = mbedtls_x509_csr_parse(
      &csr_n, reinterpret_cast<const unsigned char*>(csr.value().data()), csr.value().length() + 1);
  ASSERT_EQ(ret, 0);

  mbedtls_x509_csr_free(&csr_n);
}
#endif
