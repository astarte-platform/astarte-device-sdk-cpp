// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/msg.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using astarte::device::AstarteDatastreamObject;
using astarte::device::AstartePropertyIndividual;
using astarte::device::Data;
using astarte::device::DatastreamIndividual;
using astarte::device::Message;

TEST(AstarteTestMessage, InstantiationDatastreamIndividual) {
  std::string interface("some.interface.Name");
  std::string endpoint("/some_endpoint");
  auto data = DatastreamIndividual(Data((int32_t)43));
  auto msg = Message(interface, endpoint, data);

  EXPECT_EQ(msg.get_interface(), interface);
  EXPECT_EQ(msg.get_path(), endpoint);
  EXPECT_TRUE(msg.is_datastream());
  EXPECT_TRUE(msg.is_individual());
  EXPECT_EQ(msg.into<DatastreamIndividual>(), data);
  EXPECT_EQ(msg.try_into<DatastreamIndividual>(), std::optional<DatastreamIndividual>{data});
  EXPECT_EQ(msg.try_into<AstarteDatastreamObject>(), std::nullopt);
  EXPECT_EQ(msg.try_into<AstartePropertyIndividual>(), std::nullopt);
}

TEST(AstarteTestMessage, InstantiationDatastreamObject) {
  std::string interface("some.interface.Name");
  std::string endpoint_common("/some_base_endpoint");
  std::string endpoint1("/some_endpoint");
  std::string endpoint2("/some_other_endpoint");
  AstarteDatastreamObject data = {{endpoint1, Data(43)}, {endpoint2, Data(43.5)}};
  auto msg = Message(interface, endpoint_common, data);

  EXPECT_EQ(msg.get_interface(), interface);
  EXPECT_EQ(msg.get_path(), endpoint_common);
  EXPECT_TRUE(msg.is_datastream());
  EXPECT_FALSE(msg.is_individual());
  EXPECT_EQ(msg.into<AstarteDatastreamObject>(), data);
  EXPECT_EQ(msg.try_into<DatastreamIndividual>(), std::nullopt);
  EXPECT_EQ(msg.try_into<AstarteDatastreamObject>(), std::optional<AstarteDatastreamObject>{data});
  EXPECT_EQ(msg.try_into<AstartePropertyIndividual>(), std::nullopt);
}

TEST(AstarteTestMessage, InstantiationPropertyIndividual) {
  std::string interface("some.interface.Name");
  std::string endpoint("/some_endpoint");
  auto data = AstartePropertyIndividual(Data((int32_t)43));
  auto msg = Message(interface, endpoint, data);

  EXPECT_EQ(msg.get_interface(), interface);
  EXPECT_EQ(msg.get_path(), endpoint);
  EXPECT_FALSE(msg.is_datastream());
  EXPECT_TRUE(msg.is_individual());
  EXPECT_EQ(msg.into<AstartePropertyIndividual>(), data);
  EXPECT_EQ(msg.try_into<DatastreamIndividual>(), std::nullopt);
  EXPECT_EQ(msg.try_into<AstarteDatastreamObject>(), std::nullopt);
  EXPECT_EQ(msg.try_into<AstartePropertyIndividual>(),
            std::optional<AstartePropertyIndividual>{data});
}
