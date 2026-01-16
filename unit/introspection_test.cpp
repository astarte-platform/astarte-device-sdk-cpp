// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#if !defined(ASTARTE_TRANSPORT_GRPC)
#include <nlohmann/json.hpp>
#include <string_view>

#include "mqtt/introspection.hpp"

using AstarteDeviceSdk::Interface;
using AstarteDeviceSdk::Introspection;
using nlohmann::json;

// helper matchers
MATCHER(IsExpected, "") { return arg.has_value(); }
MATCHER(IsUnexpected, "") { return !arg.has_value(); }

constexpr std::string_view interface_str = R"({
    "interface_name": "test.Test",
    "version_major": 0,
    "version_minor": 1,
    "type": "datastream",
    "aggregation": "object",
    "ownership": "device",
    "description": "Test aggregate interface.",
    "doc": "Test interface used to test aggregates.",
    "mappings": [
        {
            "endpoint": "/%{sensor_id}/double_endpoint",
            "type": "double",
            "explicit_timestamp": false
        },
        {
            "endpoint": "/%{sensor_id}/integer_endpoint",
            "type": "integer",
            "explicit_timestamp": false
        }
    ]
})";

class AstarteTestIntrospection : public testing::Test {
 protected:
  Introspection introspection_;
  json base_json_;

  void SetUp() override {
    base_json_ = json::parse(interface_str);
    auto interface = Interface::try_from_json(base_json_).value();
    auto res = introspection_.checked_insert(std::move(interface));
    ASSERT_THAT(res, IsExpected());
  }

  // helper to create a new Interface object based on the modification passed in input
  Interface update(const json& modifications) {
    json new_json = base_json_;
    // overwrite existing json fields
    new_json.update(modifications);
    return Interface::try_from_json(new_json).value();
  }
};

TEST_F(AstarteTestIntrospection, CheckInsertion) {
  // here AstarteTestIntrospection has already been initialized with 1 interface
  ASSERT_EQ(introspection_.values().size(), 1);

  // ownership mismatch (expected: device, got: server)
  {
    auto iface = update({{"ownership", "server"}});
    auto res = introspection_.checked_insert(std::move(iface));
    ASSERT_THAT(res, IsUnexpected());
  }

  // type mismatch (expected: datastream, got: properties)
  {
    // Note: properties cannot have aggregation: object, so we clear aggregation for validity
    auto iface = update({{"type", "properties"}});
    auto res = introspection_.checked_insert(std::move(iface));
    ASSERT_THAT(res, IsUnexpected());
  }

  // update to version 1.0 (valid major ppgrade)
  {
    auto iface = update({{"version_major", 1}, {"version_minor", 0}});
    auto res = introspection_.checked_insert(std::move(iface));
    ASSERT_THAT(res, IsExpected());
    // Internal state is now Version 1.0
  }

  // major version regression (current: 1.0, new: 0.9)
  {
    auto iface = update({{"version_major", 0}, {"version_minor", 9}});
    auto res = introspection_.checked_insert(std::move(iface));
    ASSERT_THAT(res, IsUnexpected());
  }

  // update to version 1.2 (valid minor upgrade)
  {
    auto iface = update({{"version_major", 1}, {"version_minor", 2}});
    auto res = introspection_.checked_insert(std::move(iface));
    ASSERT_THAT(res, IsExpected());
    // Internal state is now Version 1.2
  }

  // minor version regression (current: 1.2, new: 1.1)
  {
    auto iface = update({{"version_major", 1}, {"version_minor", 1}});
    auto res = introspection_.checked_insert(std::move(iface));
    ASSERT_THAT(res, IsUnexpected());
  }

  // idempotency (current: 1.2, new: 1.2)
  {
    auto iface = update({{"version_major", 1}, {"version_minor", 2}});
    auto res = introspection_.checked_insert(std::move(iface));
    ASSERT_THAT(res, IsExpected());
  }
}

TEST_F(AstarteTestIntrospection, CheckGetInterface) {
  // here AstarteTestIntrospection has already been initialized with 1 interface

  // retrieve the pointer to an existing interface insidwe the introspection
  {
    auto res = introspection_.get("test.Test");
    ASSERT_THAT(res, IsExpected());
  }

  // trying to retrieve an inexistent interface
  {
    auto res = introspection_.get("inexistent.Interface");
    ASSERT_THAT(res, IsUnexpected());
  }
}

#endif
