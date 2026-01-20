// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#if !defined(ASTARTE_TRANSPORT_GRPC)
#include <nlohmann/json.hpp>
#include <string_view>

#include "mqtt/introspection.hpp"

using AstarteDeviceSdk::aggregation_from_str;
using AstarteDeviceSdk::astarte_type_from_str;
using AstarteDeviceSdk::AstarteData;
using AstarteDeviceSdk::Interface;
using AstarteDeviceSdk::interface_type_from_str;
using AstarteDeviceSdk::InterfaceAggregation;
using AstarteDeviceSdk::InterfaceType;
using AstarteDeviceSdk::Introspection;
using AstarteDeviceSdk::Mapping;

using nlohmann::json;

// helper matchers
MATCHER(IsExpected, "") { return arg.has_value(); }
MATCHER_P(IsExpected, value, "") { return arg.has_value() && (arg.value() == value); }
MATCHER(IsUnexpected, "") { return !arg.has_value(); }

TEST(AstarteTestInterfaceType, ConvertFromString) {
  ASSERT_THAT(interface_type_from_str("datastream"), IsExpected(InterfaceType::kDatastream));
  ASSERT_THAT(interface_type_from_str("properties"), IsExpected(InterfaceType::kProperty));
  ASSERT_THAT(interface_type_from_str("test"), IsUnexpected());
}

TEST(AstarteTestInterfaceAggregate, ConvertFromString) {
  ASSERT_THAT(aggregation_from_str("individual"), IsExpected(InterfaceAggregation::kIndividual));
  ASSERT_THAT(aggregation_from_str("object"), IsExpected(InterfaceAggregation::kObject));
  ASSERT_THAT(aggregation_from_str("test"), IsUnexpected());
}

TEST(AstarteTestInterface, ConvertFromJsonMappings) {
  // start with a valid base JSON object
  json base_interface = {
      {"interface_name", "test.Test"}, {"version_major", 0},    {"version_minor", 1},
      {"type", "datastream"},          {"ownership", "device"}, {"mappings", json::array()}};

  // valid mapping with only required fields
  auto valid_mappings = base_interface;
  valid_mappings["mappings"] = json::array({{{"endpoint", "/test/path"}, {"type", "double"}}});
  auto res = Interface::try_from_json(valid_mappings);
  ASSERT_THAT(res, IsExpected());
  ASSERT_EQ(res.value().mappings().size(), 1);

  // valid mapping with all optional fields
  auto full_mappings = base_interface;
  full_mappings["mappings"] = json::array({{{"endpoint", "/full/path"},
                                            {"type", "integer"},
                                            {"explicit_timestamp", true},
                                            {"reliability", "reliable"},
                                            {"retention", "stored"},
                                            {"expiry", 3600},
                                            {"database_retention_policy", "use_ttl"},
                                            {"database_retention_ttl", 86400},
                                            {"allow_unset", true},
                                            {"description", "test description"},
                                            {"doc", "test doc"}}});
  res = Interface::try_from_json(full_mappings);
  ASSERT_THAT(res, IsExpected());

  // mapping missing required endpoint
  auto missing_endpoint = base_interface;
  missing_endpoint["mappings"] = json::array({{{"type", "double"}}});
  ASSERT_THAT(Interface::try_from_json(missing_endpoint), IsUnexpected());

  // mapping missing required type
  auto missing_type = base_interface;
  missing_type["mappings"] = json::array({{{"endpoint", "/test"}}});
  ASSERT_THAT(Interface::try_from_json(missing_type), IsUnexpected());

  // mapping with invalid type value
  auto invalid_type_val = base_interface;
  invalid_type_val["mappings"] =
      json::array({{{"endpoint", "/test"}, {"type", "not_a_valid_astarte_type"}}});
  ASSERT_THAT(Interface::try_from_json(invalid_type_val), IsUnexpected());

  // endpoint is not a string
  auto wrong_endpoint_type = base_interface;
  wrong_endpoint_type["mappings"] = json::array({{{"endpoint", 123}, {"type", "double"}}});
  ASSERT_THAT(Interface::try_from_json(wrong_endpoint_type), IsUnexpected());

  // mappings array contains a non-object
  auto malformed_array = base_interface;
  malformed_array["mappings"] = json::array(
      {json::object({{"endpoint", "/ok"}, {"type", "double"}}), "this_should_be_an_object"});
  ASSERT_THAT(Interface::try_from_json(malformed_array), IsUnexpected());

  // optional fields type safety
  auto wrong_optional_type = base_interface;
  wrong_optional_type["mappings"] = json::array(
      {{{"endpoint", "/test"}, {"type", "double"}, {"explicit_timestamp", "not_a_boolean"}}});
  res = Interface::try_from_json(wrong_optional_type);
  // because our new optional helper returns nullopt on type mismatch,
  // the interface is still valid, but the value is ignored/defaulted.
  ASSERT_THAT(res, IsExpected());
  ASSERT_FALSE(res.value().mappings()[0].explicit_timestamp_.has_value());

  // multiple mappings
  auto multi_mappings = base_interface;
  multi_mappings["mappings"] = json::array({{{"endpoint", "/path1"}, {"type", "boolean"}},
                                            {{"endpoint", "/path2"}, {"type", "string"}}});
  res = Interface::try_from_json(multi_mappings);
  ASSERT_THAT(res, IsExpected());
  ASSERT_EQ(res.value().mappings().size(), 2);
}

TEST(AstarteTestInterface, ConvertFromJson) {
  auto json = json::parse("{}");

  // an intrerface must contains at least interface_name, version_major, version_minor,
  // interface_type, ownership, mappings.
  // the absense of fields InterfaceAggregation, description and doc does not prevent to build a
  // valid Interface object

  auto res = Interface::try_from_json(json);
  ASSERT_THAT(res, IsUnexpected());

  json.update({{"interface_name", "test.Test"}});
  res = Interface::try_from_json(json);
  ASSERT_THAT(res, IsUnexpected());

  json.update({{"version_major", 0}});
  res = Interface::try_from_json(json);
  ASSERT_THAT(res, IsUnexpected());

  json.update({{"version_minor", 1}});
  res = Interface::try_from_json(json);
  ASSERT_THAT(res, IsUnexpected());

  json.update({{"type", "datastream"}});
  res = Interface::try_from_json(json);
  ASSERT_THAT(res, IsUnexpected());

  json.update({{"ownership", "device"}});
  res = Interface::try_from_json(json);
  ASSERT_THAT(res, IsUnexpected());

  json.update({{"mappings", json::array()}});
  res = Interface::try_from_json(json);
  ASSERT_THAT(res, IsUnexpected());

  json.update({{"mappings", json::array({{{"endpoint", "/test"},
                                          {"type", "boolean"},
                                          {"explicit_timestamp", true}}})}});
  res = Interface::try_from_json(json);
  ASSERT_THAT(res, IsExpected());

  // check the parsing of the optional fields

  json.update({{"aggregation", "individual"}});
  res = Interface::try_from_json(json);
  ASSERT_THAT(res, IsExpected());

  json.update({{"description", "test"}});
  res = Interface::try_from_json(json);
  ASSERT_THAT(res, IsExpected());

  json.update({{"doc", "test"}});
  res = Interface::try_from_json(json);
  ASSERT_THAT(res, IsExpected());

  // checking type safety

  auto wrong_name = json;
  wrong_name["interface_name"] = nullptr;
  ASSERT_THAT(Interface::try_from_json(wrong_name), IsUnexpected());

  auto wrong_major = json;
  wrong_major.update({{"version_major", -1}});
  ASSERT_THAT(Interface::try_from_json(wrong_major), IsUnexpected());

  wrong_major["version_major"] = "not_a_number";
  ASSERT_THAT(Interface::try_from_json(wrong_major), IsUnexpected());

  auto wrong_minor = json;
  wrong_minor.update({{"version_minor", -1}});
  ASSERT_THAT(Interface::try_from_json(wrong_minor), IsUnexpected());

  auto wrong_type = json;
  wrong_type.update({{"type", "wrong"}});
  ASSERT_THAT(Interface::try_from_json(wrong_type), IsUnexpected());

  auto wrong_ownership = json;
  wrong_ownership.update({{"ownership", "wrong"}});
  ASSERT_THAT(Interface::try_from_json(wrong_ownership), IsUnexpected());

  auto wrong_aggregation = json;
  wrong_aggregation.update({{"aggregation", "wrong"}});
  ASSERT_THAT(Interface::try_from_json(wrong_aggregation), IsUnexpected());

  // mappings can only be an array, not an object
  auto wrong_mappings = json;
  wrong_mappings["mappings"] = json::object();
  ASSERT_THAT(Interface::try_from_json(wrong_mappings), IsUnexpected());
}

class AstarteTestInterfaceValidation : public testing::Test {
 protected:
  auto create_test_interface(std::string_view path, std::string_view type, bool explicit_ts)
      -> Interface {
    json j = {{"interface_name", "test.Test"},
              {"version_major", 1},
              {"version_minor", 0},
              {"type", "datastream"},
              {"ownership", "device"},
              {"mappings", json::array({{{"endpoint", std::string(path)},
                                         {"type", std::string(type)},
                                         {"explicit_timestamp", explicit_ts}}})}};
    return Interface::try_from_json(j).value();
  }
};

TEST_F(AstarteTestInterfaceValidation, DataTypeCompatibilityMatrix) {
  struct TypeMatch {
    std::string astarte_type;
    AstarteData data;
  };

  std::vector<TypeMatch> matrix = {
      {"double", AstarteData(1.1)},
      {"integer", AstarteData(int32_t(42))},
      {"longinteger", AstarteData(int64_t(9000000000LL))},
      {"boolean", AstarteData(true)},
      {"string", AstarteData(std::string("hello"))},
      {"binaryblob", AstarteData(std::vector<uint8_t>{0xDE, 0xAD})},
      {"datetime", AstarteData(std::chrono::system_clock::now())},
      {"doublearray", AstarteData(std::vector<double>{1.1, 2.2})},
      {"integerarray", AstarteData(std::vector<int32_t>{1, 2})},
      {"longintegerarray", AstarteData(std::vector<int64_t>{1, 2})},
      {"booleanarray", AstarteData(std::vector<bool>{true, false})},
      {"stringarray", AstarteData(std::vector<std::string>{"a", "b"})},
      {"binaryblobarray", AstarteData(std::vector<std::vector<uint8_t>>{{0x01}, {0x02}})},
      {"datetimearray", AstarteData(std::vector<std::chrono::system_clock::time_point>{})}};

  for (const auto& entry : matrix) {
    auto iface = create_test_interface("/path", entry.astarte_type, false);

    // test correct match
    EXPECT_THAT(iface.validate_individual("/path", entry.data, nullptr), IsExpected());

    // test incorrect match (for simplicity, we try to send a boolean to every non-boolean type)
    if (entry.astarte_type != "boolean") {
      EXPECT_THAT(iface.validate_individual("/path", AstarteData(false), nullptr), IsUnexpected());
    }
  }
}

TEST_F(AstarteTestInterfaceValidation, TimestampCheck) {
  auto timestamp = std::chrono::system_clock::now();

  // interface requires explicit timestamp
  auto iface_req = create_test_interface("/test", "double", true);

  EXPECT_THAT(iface_req.validate_individual("/test", AstarteData(1.1), &timestamp), IsExpected());
  // timestamp required but not provided
  EXPECT_THAT(iface_req.validate_individual("/test", AstarteData(1.1), nullptr), IsUnexpected());

  // interface doesn't require explicit timestamp
  auto iface_forbid = create_test_interface("/test", "double", false);

  EXPECT_THAT(iface_forbid.validate_individual("/test", AstarteData(1.1), nullptr), IsExpected());
  // timestamp provided even if not requested
  EXPECT_THAT(iface_forbid.validate_individual("/test", AstarteData(1.1), &timestamp),
              IsUnexpected());
}

TEST_F(AstarteTestInterfaceValidation, PathResolution) {
  auto iface = create_test_interface("/sensors/%{sensor_id}/value", "double", false);

  // parameterized path matching
  EXPECT_THAT(iface.validate_individual("/sensors/123/value", AstarteData(1.1), nullptr),
              IsExpected());

  // incomplete path
  EXPECT_THAT(iface.validate_individual("/sensors/123", AstarteData(1.1), nullptr), IsUnexpected());

  // path prefix mismatch
  EXPECT_THAT(iface.validate_individual("/actuators/1/value", AstarteData(1.1), nullptr),
              IsUnexpected());
}

TEST_F(AstarteTestInterfaceValidation, CheckDatatType) {
  auto check = [](std::string astarte_type_str, auto value) -> bool {
    Mapping m;
    auto type_res = astarte_type_from_str(astarte_type_str);
    m.type_ = type_res.value();
    return m.check_data_type(AstarteData(value)).has_value();
  };

  EXPECT_TRUE(check("double", 1.1));
  EXPECT_TRUE(check("integer", int32_t(1)));
  EXPECT_TRUE(check("longinteger", int64_t(1)));
  EXPECT_TRUE(check("boolean", true));
  EXPECT_TRUE(check("string", std::string("test")));
  EXPECT_TRUE(check("binaryblob", std::vector<uint8_t>{0x01}));
  EXPECT_TRUE(check("datetime", std::chrono::system_clock::now()));
  EXPECT_TRUE(check("doublearray", std::vector<double>{1.1, 2.2}));
  EXPECT_TRUE(check("integerarray", std::vector<int32_t>{1, 2}));
  EXPECT_TRUE(check("longintegerarray", std::vector<int64_t>{1, 2}));
  EXPECT_TRUE(check("booleanarray", std::vector<bool>{true, false}));
  EXPECT_TRUE(check("stringarray", std::vector<std::string>{"a", "b"}));
  EXPECT_TRUE(check("binaryblobarray", std::vector<std::vector<uint8_t>>{{0x01}, {0x02}}));
  EXPECT_TRUE(check("datetimearray", std::vector<std::chrono::system_clock::time_point>{}));

  // safety
  EXPECT_FALSE(check("double", int32_t(1)));
  EXPECT_FALSE(check("integer", 1.1));
  EXPECT_FALSE(check("boolean", std::string("true")));
}

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
    auto interface_res = Interface::try_from_json(base_json_);
    ASSERT_TRUE(interface_res.has_value());
    ASSERT_THAT(introspection_.checked_insert(std::move(interface_res.value())), IsExpected());
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
