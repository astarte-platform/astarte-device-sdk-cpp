// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#if !defined(ASTARTE_TRANSPORT_GRPC)
#include <cmath>
#include <limits>
#include <nlohmann/json.hpp>
#include <string_view>
#include <variant>

#include "mqtt/introspection.hpp"

using astarte::device::astarte_type_from_str;
using astarte::device::Data;
using astarte::device::DatastreamObject;
using astarte::device::Introspection;
using astarte::device::Reliability;
using astarte::device::Type;
using astarte::device::mqtt::DatabaseRetentionPolicy;
using astarte::device::mqtt::Interface;
using astarte::device::mqtt::InterfaceAggregation;
using astarte::device::mqtt::InterfaceType;
using astarte::device::mqtt::Mapping;
using astarte::device::mqtt::Retention;

using nlohmann::json;

// helper matchers
MATCHER(IsExpected, "") { return arg.has_value(); }
MATCHER_P(IsExpected, value, "") { return arg.has_value() && (arg.value() == value); }
MATCHER_P(IsUnexpected, error_msg, "") {
  if (arg.has_value()) {
    return false;
  }
  // visit the error variant to access the actual error object stored inside
  std::string actual_msg = std::visit([](const auto& e) { return e.message(); }, arg.error());
  // check if the retrieved message contains the expected substring
  return actual_msg.find(error_msg) != std::string::npos;
}

TEST(AstarteTestInterfaceType, ConvertFromString) {
  ASSERT_THAT(InterfaceType::try_from_str("datastream"),
              IsExpected(InterfaceType::Value::kDatastream));
  ASSERT_THAT(InterfaceType::try_from_str("properties"),
              IsExpected(InterfaceType::Value::kProperty));
  ASSERT_THAT(InterfaceType::try_from_str("test"), IsUnexpected("interface type not valid"));
}

TEST(AstarteTestInterfaceAggregate, ConvertFromString) {
  ASSERT_THAT(InterfaceAggregation::try_from_str("individual"),
              IsExpected(InterfaceAggregation::Value::kIndividual));
  ASSERT_THAT(InterfaceAggregation::try_from_str("object"),
              IsExpected(InterfaceAggregation::Value::kObject));
  ASSERT_THAT(InterfaceAggregation::try_from_str("test"),
              IsUnexpected("interface aggregation not valid"));
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
                                            {"reliability", "guaranteed"},
                                            {"retention", "stored"},
                                            {"expiry", 3600},
                                            {"database_retention_policy", "use_ttl"},
                                            {"database_retention_ttl", 86400},
                                            {"allow_unset", true},
                                            {"description", "test description"},
                                            {"doc", "test doc"}}});
  res = Interface::try_from_json(full_mappings);
  ASSERT_THAT(res, IsExpected());

  // Verify parsed values for the complex mapping
  const auto& mapping = res.value().mappings()[0];
  ASSERT_THAT(mapping.reliability(),
              testing::Optional(Reliability(Reliability::Value::kGuaranteed)));
  ASSERT_THAT(mapping.retention(), testing::Optional(Retention(Retention::Value::kStored)));
  ASSERT_THAT(mapping.expiry(), testing::Optional(3600));

  // mapping missing required endpoint
  auto missing_endpoint = base_interface;
  missing_endpoint["mappings"] = json::array({{{"type", "double"}}});
  ASSERT_THAT(Interface::try_from_json(missing_endpoint),
              IsUnexpected("Missing required field: endpoint"));

  // mapping missing required type
  auto missing_type = base_interface;
  missing_type["mappings"] = json::array({{{"endpoint", "/test"}}});
  ASSERT_THAT(Interface::try_from_json(missing_type), IsUnexpected("Missing required field: type"));

  // mapping with invalid type value
  auto invalid_type_val = base_interface;
  invalid_type_val["mappings"] =
      json::array({{{"endpoint", "/test"}, {"type", "not_a_valid_astarte_type"}}});
  ASSERT_THAT(Interface::try_from_json(invalid_type_val),
              IsUnexpected("data type not valid: not_a_valid_astarte_type"));

  // endpoint is not a string
  auto wrong_endpoint_type = base_interface;
  wrong_endpoint_type["mappings"] = json::array({{{"endpoint", 123}, {"type", "double"}}});
  ASSERT_THAT(Interface::try_from_json(wrong_endpoint_type),
              IsUnexpected("Field endpoint has invalid type"));

  // mappings array contains a non-object
  auto malformed_array = base_interface;
  malformed_array["mappings"] = json::array(
      {json::object({{"endpoint", "/ok"}, {"type", "double"}}), "this_should_be_an_object"});
  ASSERT_THAT(Interface::try_from_json(malformed_array),
              IsUnexpected("Each element in 'mappings' must be an object"));

  // optional fields type safety
  auto wrong_optional_type = base_interface;
  wrong_optional_type["mappings"] = json::array(
      {{{"endpoint", "/test"}, {"type", "double"}, {"explicit_timestamp", "not_a_boolean"}}});
  res = Interface::try_from_json(wrong_optional_type);
  // because our new optional helper returns nullopt on type mismatch,
  // the interface is still valid, but the value is ignored/defaulted.
  ASSERT_THAT(res, IsExpected());
  ASSERT_FALSE(res.value().mappings()[0].explicit_timestamp().has_value());

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
  ASSERT_THAT(res, IsUnexpected("Missing required field: interface_name"));

  json.update({{"interface_name", "test.Test"}});
  res = Interface::try_from_json(json);
  ASSERT_THAT(res, IsUnexpected("Missing required field: version_major"));

  json.update({{"version_major", 0}});
  res = Interface::try_from_json(json);
  ASSERT_THAT(res, IsUnexpected("Missing required field: version_minor"));

  json.update({{"version_minor", 1}});
  res = Interface::try_from_json(json);
  ASSERT_THAT(res, IsUnexpected("Missing required field: type"));

  json.update({{"type", "datastream"}});
  res = Interface::try_from_json(json);
  ASSERT_THAT(res, IsUnexpected("Missing required field: ownership"));

  json.update({{"ownership", "device"}});
  res = Interface::try_from_json(json);
  ASSERT_THAT(res, IsUnexpected("Missing required field: mappings"));

  json.update({{"mappings", json::array()}});
  res = Interface::try_from_json(json);
  ASSERT_THAT(res, IsUnexpected("There must be at least one mapping"));

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
  ASSERT_THAT(Interface::try_from_json(wrong_name),
              IsUnexpected("Field interface_name has invalid type"));

  auto wrong_major = json;
  wrong_major.update({{"version_major", -1}});
  ASSERT_THAT(Interface::try_from_json(wrong_major),
              IsUnexpected("received negative major version value"));

  wrong_major["version_major"] = "not_a_number";
  ASSERT_THAT(Interface::try_from_json(wrong_major),
              IsUnexpected("Field version_major has invalid type"));

  auto wrong_minor = json;
  wrong_minor.update({{"version_minor", -1}});
  ASSERT_THAT(Interface::try_from_json(wrong_minor),
              IsUnexpected("received negative minor version value"));

  auto wrong_type = json;
  wrong_type.update({{"type", "wrong"}});
  ASSERT_THAT(Interface::try_from_json(wrong_type), IsUnexpected("interface type not valid"));

  auto wrong_ownership = json;
  wrong_ownership.update({{"ownership", "wrong"}});
  ASSERT_THAT(Interface::try_from_json(wrong_ownership),
              IsUnexpected("interface ownershipe not valid: wrong"));

  auto wrong_aggregation = json;
  wrong_aggregation.update({{"aggregation", "wrong"}});
  ASSERT_THAT(Interface::try_from_json(wrong_aggregation),
              IsUnexpected("interface aggregation not valid"));

  // mappings can only be an array, not an object
  auto wrong_mappings = json;
  wrong_mappings["mappings"] = json::object();
  ASSERT_THAT(Interface::try_from_json(wrong_mappings),
              IsUnexpected("Field mappings has invalid type"));
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
    Data data;
  };

  std::vector<TypeMatch> matrix = {
      {"double", Data(1.1)},
      {"integer", Data(int32_t(42))},
      {"longinteger", Data(int64_t(9000000000LL))},
      {"boolean", Data(true)},
      {"string", Data(std::string("hello"))},
      {"binaryblob", Data(std::vector<uint8_t>{0xDE, 0xAD})},
      {"datetime", Data(std::chrono::system_clock::now())},
      {"doublearray", Data(std::vector<double>{1.1, 2.2})},
      {"integerarray", Data(std::vector<int32_t>{1, 2})},
      {"longintegerarray", Data(std::vector<int64_t>{1, 2})},
      {"booleanarray", Data(std::vector<bool>{true, false})},
      {"stringarray", Data(std::vector<std::string>{"a", "b"})},
      {"binaryblobarray", Data(std::vector<std::vector<uint8_t>>{{0x01}, {0x02}})},
      {"datetimearray", Data(std::vector<std::chrono::system_clock::time_point>{})}};

  for (const auto& entry : matrix) {
    auto iface = create_test_interface("/path", entry.astarte_type, false);

    // test correct match
    EXPECT_THAT(iface.validate_individual("/path", entry.data, nullptr), IsExpected());

    // test incorrect match (for simplicity, we try to send a boolean to every non-boolean type)
    if (entry.astarte_type != "boolean") {
      EXPECT_THAT(iface.validate_individual("/path", Data(false), nullptr),
                  IsUnexpected("Astarte data type and mapping type do not match"));
    }
  }
}

TEST_F(AstarteTestInterfaceValidation, TimestampCheck) {
  auto timestamp = std::chrono::system_clock::now();

  // interface requires explicit timestamp
  auto iface_req = create_test_interface("/test", "double", true);

  EXPECT_THAT(iface_req.validate_individual("/test", Data(1.1), &timestamp), IsExpected());
  // timestamp required but not provided
  EXPECT_THAT(iface_req.validate_individual("/test", Data(1.1), nullptr),
              IsUnexpected("Explicit timestamp required"));

  // interface doesn't require explicit timestamp
  auto iface_forbid = create_test_interface("/test", "double", false);

  EXPECT_THAT(iface_forbid.validate_individual("/test", Data(1.1), nullptr), IsExpected());
  // timestamp provided even if not requested
  EXPECT_THAT(iface_forbid.validate_individual("/test", Data(1.1), &timestamp),
              IsUnexpected("Explicit timestamp not supported"));
}

TEST_F(AstarteTestInterfaceValidation, PathResolution) {
  auto iface = create_test_interface("/sensors/%{sensor_id}/value", "double", false);

  // parameterized path matching
  EXPECT_THAT(iface.validate_individual("/sensors/123/value", Data(1.1), nullptr), IsExpected());

  // incomplete path
  EXPECT_THAT(iface.validate_individual("/sensors/123", Data(1.1), nullptr),
              IsUnexpected("couldn't find mapping with path"));

  // path prefix mismatch
  EXPECT_THAT(iface.validate_individual("/actuators/1/value", Data(1.1), nullptr),
              IsUnexpected("couldn't find mapping with path"));
}

TEST_F(AstarteTestInterfaceValidation, CheckDatatType) {
  auto check = [](std::string astarte_type_str, auto value) -> bool {
    auto type_res = astarte_type_from_str(astarte_type_str);
    if (!type_res) {
      return false;
    }
    Mapping m("/test", type_res.value(), std::nullopt, std::nullopt, std::nullopt, std::nullopt,
              std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
    return m.check_data_type(Data(value)).has_value();
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

TEST_F(AstarteTestInterfaceValidation, FiniteDoubleCheck) {
  // Test that checking data type explicitly rejects NaN and Inf for doubles
  auto type_res = astarte_type_from_str("double");
  ASSERT_TRUE(type_res.has_value());

  Mapping m("/test", type_res.value(), std::nullopt, std::nullopt, std::nullopt, std::nullopt,
            std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt);

  EXPECT_THAT(m.check_data_type(Data(std::numeric_limits<double>::quiet_NaN())),
              IsUnexpected("Astarte data double is not a number"));
  EXPECT_THAT(m.check_data_type(Data(std::numeric_limits<double>::infinity())),
              IsUnexpected("Astarte data double is not a number"));
  EXPECT_THAT(m.check_data_type(Data(1.0)), IsExpected());

  // Check double arrays
  auto array_type_res = astarte_type_from_str("doublearray");
  ASSERT_TRUE(array_type_res.has_value());
  Mapping m_arr("/test_arr", array_type_res.value(), std::nullopt, std::nullopt, std::nullopt,
                std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt);

  EXPECT_THAT(m_arr.check_data_type(
                  Data(std::vector<double>{1.0, std::numeric_limits<double>::quiet_NaN()})),
              IsUnexpected("Astarte data double is not a number"));
  EXPECT_THAT(m_arr.check_data_type(Data(std::vector<double>{1.0, 2.0})), IsExpected());
}

TEST(AstarteTestInterfaceGetQoS, GetQosLevels) {
  // 1. Test Individual Aggregation (default) with mixed QoS
  json ind_json = {
      {"interface_name", "test.Individual"},
      {"version_major", 1},
      {"version_minor", 0},
      {"type", "datastream"},
      {"ownership", "device"},
      {"mappings",
       json::array({
           {{"endpoint", "/unreliable"}, {"type", "integer"}, {"reliability", "unreliable"}},
           {{"endpoint", "/guaranteed"}, {"type", "integer"}, {"reliability", "guaranteed"}},
           {{"endpoint", "/unique"}, {"type", "integer"}, {"reliability", "unique"}},
           {{"endpoint", "/default"}, {"type", "integer"}}  // default should be unreliable
       })}};

  auto ind_iface_res = Interface::try_from_json(ind_json);
  ASSERT_THAT(ind_iface_res, IsExpected());
  const auto& ind_iface = ind_iface_res.value();

  // 0 = unreliable, 1 = guaranteed, 2 = unique (matches MQTT QoS levels)
  EXPECT_THAT(ind_iface.get_qos("/unreliable"), IsExpected(0));
  EXPECT_THAT(ind_iface.get_qos("/guaranteed"), IsExpected(1));
  EXPECT_THAT(ind_iface.get_qos("/unique"), IsExpected(2));
  EXPECT_THAT(ind_iface.get_qos("/default"), IsExpected(0));
  EXPECT_THAT(ind_iface.get_qos("/invalid/path"), IsUnexpected("couldn't find mapping"));

  // 2. Test Object Aggregation
  // For objects, get_qos should return the reliability of the first mapping
  json obj_json = {
      {"interface_name", "test.Object"},
      {"version_major", 1},
      {"version_minor", 0},
      {"type", "datastream"},
      {"ownership", "device"},
      {"aggregation", "object"},
      {"mappings",
       json::array({{{"endpoint", "/lat"}, {"type", "double"}, {"reliability", "guaranteed"}},
                    {{"endpoint", "/long"}, {"type", "double"}, {"reliability", "guaranteed"}}})}};

  auto obj_iface_res = Interface::try_from_json(obj_json);
  ASSERT_THAT(obj_iface_res, IsExpected());
  const auto& obj_iface = obj_iface_res.value();

  EXPECT_THAT(obj_iface.get_qos("/lat"), IsExpected(1));
  EXPECT_THAT(obj_iface.get_qos("/random"), IsExpected(1));
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
    ASSERT_THAT(res, IsUnexpected("the new interface has a different ownership"));
  }

  // type mismatch (expected: datastream, got: properties)
  {
    auto iface = update({{"type", "properties"}});
    auto res = introspection_.checked_insert(std::move(iface));
    ASSERT_THAT(res, IsUnexpected("the new interface has a different type"));
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
    ASSERT_THAT(res, IsUnexpected("the new major version is lower"));
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
    ASSERT_THAT(res, IsUnexpected("the new minor version is lower"));
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
    ASSERT_THAT(res, IsUnexpected("couldn't find interface"));
  }
}

TEST_F(AstarteTestIntrospection, ValidateObject) {
  // Test the validate_object method on the interface loaded in SetUp (test.Test, aggregated object)
  auto iface = introspection_.get("test.Test").value();

  DatastreamObject obj_data = {{"double_endpoint", Data(1.5)}, {"integer_endpoint", Data(42)}};

  EXPECT_THAT(iface->validate_object("/1", obj_data, nullptr), IsExpected());

  // Invalid data type in object
  DatastreamObject bad_data = {{"double_endpoint", Data(std::string("string instead of double"))}};

  EXPECT_THAT(iface->validate_object("/1", bad_data, nullptr),
              IsUnexpected("Astarte data type and mapping type do not match"));

  // Path that doesn't match object structure (extra fields)
  DatastreamObject unknown_field = {{"unknown_endpoint", Data(1.0)}};

  EXPECT_THAT(iface->validate_object("/1", unknown_field, nullptr),
              IsUnexpected("couldn't find mapping with path"));
}

#endif
