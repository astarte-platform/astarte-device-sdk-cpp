// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "astarte_device_sdk/data.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <vector>

#include "astarte_device_sdk/formatter.hpp"

using AstarteDeviceSdk::AstarteData;
using AstarteDeviceSdk::AstarteType;

using testing::ContainerEq;

bool CompareAstarteData(AstarteData data, const char* value) {
  return astarte_fmt::format("{}", data) == value;
}

TEST(AstarteTestData, InstantiationInteger) {
  int32_t value = 52;
  auto data = AstarteData(value);
  auto original = data.into<int32_t>();
  EXPECT_EQ(value, original);
}
TEST(AstarteTestData, FormatInteger) {
  int32_t value = 52;
  auto data = AstarteData(value);
  EXPECT_PRED2(CompareAstarteData, data, "52");
}

TEST(AstarteTestData, InstantiationLongInteger) {
  int64_t value = 52;
  auto data = AstarteData(value);
  auto original = data.into<int64_t>();
  EXPECT_EQ(value, original);
}
TEST(AstarteTestData, FormatLongInteger) {
  int64_t value = 52;
  auto data = AstarteData(value);
  EXPECT_PRED2(CompareAstarteData, data, "52");
}

TEST(AstarteTestData, InstantiationDouble) {
  double value = 43.5;
  auto data = AstarteData(value);
  auto original = data.into<double>();
  EXPECT_FLOAT_EQ(value, original);
}
TEST(AstarteTestData, FormatDouble) {
  double value = 43.5;
  auto data = AstarteData(value);
  EXPECT_PRED2(CompareAstarteData, data, "43.5");
}

TEST(AstarteTestData, InstantiationBoolean) {
  bool value = true;
  auto data = AstarteData(value);
  auto original = data.into<bool>();
  EXPECT_EQ(value, original);
}
TEST(AstarteTestData, FormatBoolean) {
  bool value = true;
  auto data = AstarteData(value);
  EXPECT_PRED2(CompareAstarteData, data, "true");
}

TEST(AstarteTestData, InstantiationString) {
  std::string value = "Test string";
  auto data = AstarteData(value);
  auto original = data.into<std::string>();
  EXPECT_STREQ(value.c_str(), original.c_str());
}
TEST(AstarteTestData, FormatString) {
  std::string value = "Test string";
  auto data = AstarteData(value);
  EXPECT_PRED2(CompareAstarteData, data, "\"Test string\"");
}

TEST(AstarteTestData, InstantiationStringView) {
  std::string_view value = "Test string";
  auto data = AstarteData(value);
  auto original_string = data.into<std::string>();
  auto original_string_view = data.into<std::string_view>();
  EXPECT_STREQ(value.data(), original_string.c_str());
  ASSERT_NE(value.data(), original_string_view.data());     // Memory locations differ
  EXPECT_STREQ(value.data(), original_string_view.data());  // Content is the same
}
TEST(AstarteTestData, FormatStringView) {
  std::string_view value = "Test string";
  auto data = AstarteData(value);
  EXPECT_PRED2(CompareAstarteData, data, "\"Test string\"");
}

TEST(AstarteTestData, InstantiationBinaryBlob) {
  std::vector<uint8_t> value = {0x12U, 0x22U, 0x42};
  auto data = AstarteData(value);
  auto original = data.into<std::vector<uint8_t>>();
  EXPECT_THAT(value, ContainerEq(original));
}
TEST(AstarteTestData, FormatBinaryBlob) {
  std::vector<uint8_t> value = {0x12U, 0x22U, 0x42};
  auto data = AstarteData(value);
  EXPECT_PRED2(CompareAstarteData, data, "\"EiJC\"");
}

TEST(AstarteTestData, InstantiationDatetime) {
  std::chrono::system_clock::time_point value =
      std::chrono::sys_days(std::chrono::year_month_day(
          std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
      std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0);

  auto data = AstarteData(value);
  auto original = data.into<std::chrono::system_clock::time_point>();
  EXPECT_EQ(value, original);
}
TEST(AstarteTestData, FormatDatetime) {
  std::chrono::system_clock::time_point value =
      std::chrono::sys_days(std::chrono::year_month_day(
          std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
      std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0);

  auto data = AstarteData(value);
  EXPECT_PRED2(CompareAstarteData, data, "\"1994-04-12T10:15:00.000Z\"");
}

TEST(AstarteTestData, InstantiationIntegerArray) {
  std::vector<int32_t> value{12, 43, 11, 0};
  auto data = AstarteData(value);
  auto original = data.into<std::vector<int32_t>>();
  EXPECT_THAT(original, ContainerEq(value));
}
TEST(AstarteTestData, FormatIntegerArray) {
  std::vector<int32_t> value{12, 43, 11, 0};
  auto data = AstarteData(value);
  EXPECT_PRED2(CompareAstarteData, data, "[12, 43, 11, 0]");
}

TEST(AstarteTestData, InstantiationLongIntegerArray) {
  std::vector<int64_t> value{0, 8589934592, 11};
  auto data = AstarteData(value);
  auto original = data.into<std::vector<int64_t>>();
  EXPECT_THAT(original, ContainerEq(value));
}
TEST(AstarteTestData, FormatLongIntegerArray) {
  std::vector<int64_t> value{0, 8589934592, 11};
  auto data = AstarteData(value);
  EXPECT_PRED2(CompareAstarteData, data, "[0, 8589934592, 11]");
}

TEST(AstarteTestData, InstantiationDoubleArray) {
  std::vector<double> value{0.0, 43.2};
  auto data = AstarteData(value);
  auto original = data.into<std::vector<double>>();
  EXPECT_THAT(original, ContainerEq(value));
}
TEST(AstarteTestData, FormatDoubleArray) {
  std::vector<double> value{0.0, 43.2};
  auto data = AstarteData(value);
  EXPECT_PRED2(CompareAstarteData, data, "[0, 43.2]");
}

TEST(AstarteTestData, InstantiationBooleanArray) {
  std::vector<bool> value{true, false, false};
  auto data = AstarteData(value);
  auto original = data.into<std::vector<bool>>();
  EXPECT_THAT(original, ContainerEq(value));
}
TEST(AstarteTestData, FormatBooleanArray) {
  std::vector<bool> value{true, false, false};
  auto data = AstarteData(value);
  EXPECT_PRED2(CompareAstarteData, data, "[true, false, false]");
}

TEST(AstarteTestData, InstantiationStringArray) {
  std::vector<std::string> value{"Hello", "C++"};
  auto data = AstarteData(value);
  auto original = data.into<std::vector<std::string>>();
  EXPECT_THAT(original, ContainerEq(value));
}
TEST(AstarteTestData, FormatStringArray) {
  std::vector<std::string> value{"Hello", "C++"};
  auto data = AstarteData(value);
  EXPECT_PRED2(CompareAstarteData, data, "[\"Hello\", \"C++\"]");
}

TEST(AstarteTestData, InstantiationBinaryBlobArray) {
  std::vector<std::vector<uint8_t>> value{{0x12U, 0x22U, 0x42}, {0x10U, 0x8FU}};
  auto data = AstarteData(value);
  auto original = data.into<std::vector<std::vector<uint8_t>>>();
  EXPECT_THAT(original, ContainerEq(value));
}
TEST(AstarteTestData, FormatBinaryBlobArray) {
  std::vector<std::vector<uint8_t>> value{{0x12U, 0x22U, 0x42}, {0x10U, 0x8FU}};
  auto data = AstarteData(value);
  EXPECT_PRED2(CompareAstarteData, data, "[\"EiJC\", \"EI8=\"]");
}

TEST(AstarteTestData, InstantiationDatetimeArray) {
  std::chrono::system_clock::time_point datetime0 =
      std::chrono::sys_days(std::chrono::year_month_day(
          std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
      std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0);
  std::chrono::system_clock::time_point datetime1 =
      std::chrono::sys_days(std::chrono::year_month_day(
          std::chrono::year(1984), std::chrono::month(5), std::chrono::day(2))) +
      std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0);
  std::vector<std::chrono::system_clock::time_point> value{datetime0, datetime1};

  auto data = AstarteData(value);
  auto original = data.into<std::vector<std::chrono::system_clock::time_point>>();
  EXPECT_THAT(original, ContainerEq(value));
}
TEST(AstarteTestData, FormatDatetimeArray) {
  std::chrono::system_clock::time_point datetime0 =
      std::chrono::sys_days(std::chrono::year_month_day(
          std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
      std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0);
  std::chrono::system_clock::time_point datetime1 =
      std::chrono::sys_days(std::chrono::year_month_day(
          std::chrono::year(1984), std::chrono::month(5), std::chrono::day(2))) +
      std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0);
  std::vector<std::chrono::system_clock::time_point> value{datetime0, datetime1};

  auto data = AstarteData(value);
  EXPECT_PRED2(CompareAstarteData, data,
               "[\"1994-04-12T10:15:00.000Z\", \"1984-05-02T10:15:00.000Z\"]");
}

TEST(AstarteTestData, TryIntoInteger) {
  int32_t value = 52;
  auto data = AstarteData(value);
  auto original = data.try_into<int32_t>();
  EXPECT_EQ(value, original.value());
}
TEST(AstarteTestData, TryIntoLongInteger) {
  int64_t value = 52;
  auto data = AstarteData(value);
  auto original = data.try_into<int64_t>();
  EXPECT_EQ(value, original.value());
}
TEST(AstarteTestData, TryIntoDouble) {
  double value = 43.5;
  auto data = AstarteData(value);
  auto original = data.try_into<double>();
  EXPECT_FLOAT_EQ(value, original.value());
}
TEST(AstarteTestData, TryIntoBoolean) {
  bool value = true;
  auto data = AstarteData(value);
  auto original = data.try_into<bool>();
  EXPECT_EQ(value, original.value());
}
TEST(AstarteTestData, TryIntoString) {
  std::string value = "Test string";
  auto data = AstarteData(value);
  std::optional<std::string> original = data.try_into<std::string>();
  EXPECT_STREQ(value.c_str(), original->c_str());
}
TEST(AstarteTestData, TryIntoStringView) {
  std::string_view value = "Test string";
  auto data = AstarteData(value);
  std::optional<std::string> original_string = data.try_into<std::string>();
  EXPECT_STREQ(value.data(), original_string->c_str());
  std::optional<std::string_view> original_string_view = data.try_into<std::string_view>();
  EXPECT_STREQ(value.data(), original_string_view->data());
}
TEST(AstarteTestData, TryIntoBinaryBlob) {
  std::vector<uint8_t> value = {0x12U, 0x22U, 0x42};
  auto data = AstarteData(value);
  auto original = data.try_into<std::vector<uint8_t>>();
  EXPECT_THAT(value, ContainerEq(original.value()));
}
TEST(AstarteTestData, TryIntoDatetime) {
  std::chrono::system_clock::time_point value =
      std::chrono::sys_days(std::chrono::year_month_day(
          std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
      std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0);

  auto data = AstarteData(value);
  auto original = data.try_into<std::chrono::system_clock::time_point>();
  EXPECT_EQ(value, original.value());
}

TEST(AstarteTestData, TryIntoIntegerArray) {
  std::vector<int32_t> value{12, 43, 11, 0};
  auto data = AstarteData(value);
  auto original = data.try_into<std::vector<int32_t>>();
  EXPECT_THAT(original.value(), ContainerEq(value));
}
TEST(AstarteTestData, TryIntoLongIntegerArray) {
  std::vector<int64_t> value{0, 8589934592, 11};
  auto data = AstarteData(value);
  auto original = data.try_into<std::vector<int64_t>>();
  EXPECT_THAT(original.value(), ContainerEq(value));
}
TEST(AstarteTestData, TryIntoDoubleArray) {
  std::vector<double> value{0.0, 43.2};
  auto data = AstarteData(value);
  auto original = data.try_into<std::vector<double>>();
  EXPECT_THAT(original.value(), ContainerEq(value));
}
TEST(AstarteTestData, TryIntoBooleanArray) {
  std::vector<bool> value{true, false, false};
  auto data = AstarteData(value);
  auto original = data.try_into<std::vector<bool>>();
  EXPECT_THAT(original.value(), ContainerEq(value));
}
TEST(AstarteTestData, TryIntoStringArray) {
  std::vector<std::string> value{"Hello", "C++"};
  auto data = AstarteData(value);
  auto original = data.try_into<std::vector<std::string>>();
  EXPECT_THAT(original.value(), ContainerEq(value));
}
TEST(AstarteTestData, TryIntoBinaryBlobArray) {
  std::vector<std::vector<uint8_t>> value{{0x12U, 0x22U, 0x42}, {0x10U, 0x8FU}};
  auto data = AstarteData(value);
  auto original = data.try_into<std::vector<std::vector<uint8_t>>>();
  EXPECT_THAT(original.value(), ContainerEq(value));
}
TEST(AstarteTestData, TryIntoDatetimeArray) {
  std::chrono::system_clock::time_point datetime0 =
      std::chrono::sys_days(std::chrono::year_month_day(
          std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
      std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0);
  std::chrono::system_clock::time_point datetime1 =
      std::chrono::sys_days(std::chrono::year_month_day(
          std::chrono::year(1984), std::chrono::month(5), std::chrono::day(2))) +
      std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0);
  std::vector<std::chrono::system_clock::time_point> value{datetime0, datetime1};

  auto data = AstarteData(value);
  auto original = data.try_into<std::vector<std::chrono::system_clock::time_point>>();
  EXPECT_THAT(original.value(), ContainerEq(value));
}
