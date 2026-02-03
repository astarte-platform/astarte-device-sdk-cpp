// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#if !defined(ASTARTE_TRANSPORT_GRPC)
#include "astarte_device_sdk/mqtt/pairing.hpp"

using astarte::device::create_deterministic_device_id;
using astarte::device::create_random_device_id;

TEST(AstarteTestDeviceId, GenerateRandom) {
  auto device_id = create_random_device_id();

  // A device id randomly generate (from a UUIDv4) is 16-bytes long, which converted
  // in a URL safe string is 22-characters long (without padding).
  ASSERT_EQ(device_id.size(), 22);
}

TEST(AstarteTestDeviceId, GenerateDeterministic) {
  // fail to create deterministic ID due to incorrect namespace
  auto wrong_device_id = create_deterministic_device_id("incorrect namespace", "test");
  ASSERT_EQ(wrong_device_id.has_value(), false);

  auto device_id = create_deterministic_device_id("6ba7b811-9dad-11d1-80b4-00c04fd430c8", "test");
  // the reslting device_id from the cobination of namespace "6ba7b811-9dad-11d1-80b4-00c04fd430c8"
  // and name "test" will always be "2luIk9bKXByanJH0Cio2SQ"
  ASSERT_EQ(device_id.value().size(), 22);
  ASSERT_EQ(device_id.value(), "2luIk9bKXByanJH0Cio2SQ");

  // verify that we can deterministically generate the same id
  auto new_device_id =
      create_deterministic_device_id("6ba7b811-9dad-11d1-80b4-00c04fd430c8", "test");
  ASSERT_EQ(device_id.value(), new_device_id.value());
}

#endif
