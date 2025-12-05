// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#if !defined(ASTARTE_TRANSPORT_GRPC)
#include "astarte_device_sdk/mqtt/pairing.hpp"

using AstarteDeviceSdk::create_deterministic_device_id;
using AstarteDeviceSdk::create_random_device_id;

TEST(AstarteTestDeviceId, GenerateRandom) {
  auto device_id = create_random_device_id();

  // A device id randomly generate (from a UUIDv4) is 16-bytes long
  ASSERT_EQ(device_id.size(), 16);
}

TEST(AstarteTestDeviceId, GenerateDeterministic) {
  //   auto key = PsaKey::create();
  //   ASSERT_TRUE(key);
  //   ASSERT_TRUE(key.value().generate());
  //   psa_key_attributes_t attr = psa_key_attributes_init();

  //   auto ret = psa_get_key_attributes(key.value().get(), &attr);
  //   ASSERT_EQ(ret, 0);

  //   auto algorithm = psa_get_key_algorithm(&attr);
  //   ASSERT_EQ(algorithm, PSA_ECC_FAMILY_SECP_R1);
  //   auto flags = psa_get_key_usage_flags(&attr);
  //   ASSERT_EQ(flags, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_EXPORT);
  //   auto key_type = psa_get_key_type(&attr);
  //   ASSERT_EQ(key_type, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
  //   auto key_bits = psa_get_key_bits(&attr);
  //   ASSERT_EQ(key_bits, 256);

  //   psa_reset_key_attributes(&attr);
}

#endif
