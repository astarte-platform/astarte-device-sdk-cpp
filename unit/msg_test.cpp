// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/msg.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using astarte::device::AstarteData;
using astarte::device::AstarteDatastreamIndividual;
using astarte::device::AstarteDatastreamObject;
using astarte::device::AstarteMessage;
using astarte::device::AstartePropertyIndividual;

TEST(AstarteTestMessage, InstantiationDatastreamIndividual) {
  std::string interface("some.interface.Name");
  std::string endpoint("/some_endpoint");
  auto data = AstarteDatastreamIndividual(AstarteData((int32_t)43));
  auto msg = AstarteMessage(interface, endpoint, data);

  EXPECT_EQ(msg.get_interface(), interface);
  EXPECT_EQ(msg.get_path(), endpoint);
  EXPECT_TRUE(msg.is_datastream());
  EXPECT_TRUE(msg.is_individual());
  EXPECT_EQ(msg.into<AstarteDatastreamIndividual>(), data);
  EXPECT_EQ(msg.try_into<AstarteDatastreamIndividual>(),
            std::optional<AstarteDatastreamIndividual>{data});
  EXPECT_EQ(msg.try_into<AstarteDatastreamObject>(), std::nullopt);
  EXPECT_EQ(msg.try_into<AstartePropertyIndividual>(), std::nullopt);
}

TEST(AstarteTestMessage, InstantiationDatastreamObject) {
  std::string interface("some.interface.Name");
  std::string endpoint_common("/some_base_endpoint");
  std::string endpoint1("/some_endpoint");
  std::string endpoint2("/some_other_endpoint");
  AstarteDatastreamObject data = {{endpoint1, AstarteData(43)}, {endpoint2, AstarteData(43.5)}};
  auto msg = AstarteMessage(interface, endpoint_common, data);

  EXPECT_EQ(msg.get_interface(), interface);
  EXPECT_EQ(msg.get_path(), endpoint_common);
  EXPECT_TRUE(msg.is_datastream());
  EXPECT_FALSE(msg.is_individual());
  EXPECT_EQ(msg.into<AstarteDatastreamObject>(), data);
  EXPECT_EQ(msg.try_into<AstarteDatastreamIndividual>(), std::nullopt);
  EXPECT_EQ(msg.try_into<AstarteDatastreamObject>(), std::optional<AstarteDatastreamObject>{data});
  EXPECT_EQ(msg.try_into<AstartePropertyIndividual>(), std::nullopt);
}

TEST(AstarteTestMessage, InstantiationPropertyIndividual) {
  std::string interface("some.interface.Name");
  std::string endpoint("/some_endpoint");
  auto data = AstartePropertyIndividual(AstarteData((int32_t)43));
  auto msg = AstarteMessage(interface, endpoint, data);

  EXPECT_EQ(msg.get_interface(), interface);
  EXPECT_EQ(msg.get_path(), endpoint);
  EXPECT_FALSE(msg.is_datastream());
  EXPECT_TRUE(msg.is_individual());
  EXPECT_EQ(msg.into<AstartePropertyIndividual>(), data);
  EXPECT_EQ(msg.try_into<AstarteDatastreamIndividual>(), std::nullopt);
  EXPECT_EQ(msg.try_into<AstarteDatastreamObject>(), std::nullopt);
  EXPECT_EQ(msg.try_into<AstartePropertyIndividual>(),
            std::optional<AstartePropertyIndividual>{data});
}
