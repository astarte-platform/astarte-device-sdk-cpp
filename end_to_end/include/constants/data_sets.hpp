// (C) Copyright 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>
#include <string>
#include <string_view>
#include <vector>

#include "astarte_device_sdk/data.hpp"

namespace constants::data_sets {

using AstarteDeviceSdk::AstarteData;
using namespace std::chrono_literals;

struct Integer {
  static constexpr std::string_view ENDPOINT_PARTIAL = "integer_endpoint";
  static constexpr std::string_view ENDPOINT_FULL = "/integer_endpoint";
  static constexpr std::string_view ENDPOINT_SENSOR = "/sensor1/integer_endpoint";
  static inline const AstarteData DATA = AstarteData(12);
};

struct LongInteger {
  static constexpr std::string_view ENDPOINT_PARTIAL = "longinteger_endpoint";
  static constexpr std::string_view ENDPOINT_FULL = "/longinteger_endpoint";
  static constexpr std::string_view ENDPOINT_SENSOR = "/sensor1/longinteger_endpoint";
  static inline const AstarteData DATA = AstarteData(17179869184);
};

struct Double {
  static constexpr std::string_view ENDPOINT_PARTIAL = "double_endpoint";
  static constexpr std::string_view ENDPOINT_FULL = "/double_endpoint";
  static constexpr std::string_view ENDPOINT_SENSOR = "/sensor1/double_endpoint";
  static inline const AstarteData DATA = AstarteData(54.4);
};

struct Boolean {
  static constexpr std::string_view ENDPOINT_PARTIAL = "boolean_endpoint";
  static constexpr std::string_view ENDPOINT_FULL = "/boolean_endpoint";
  static constexpr std::string_view ENDPOINT_SENSOR = "/sensor1/boolean_endpoint";
  static inline const AstarteData DATA = AstarteData(true);
};

struct String {
  static constexpr std::string_view ENDPOINT_PARTIAL = "string_endpoint";
  static constexpr std::string_view ENDPOINT_FULL = "/string_endpoint";
  static constexpr std::string_view ENDPOINT_SENSOR = "/sensor1/string_endpoint";
  static inline const AstarteData DATA = AstarteData(std::string("Hello C++!"));
};

struct Datetime {
  static constexpr std::string_view ENDPOINT_PARTIAL = "datetime_endpoint";
  static constexpr std::string_view ENDPOINT_FULL = "/datetime_endpoint";
  static constexpr std::string_view ENDPOINT_SENSOR = "/sensor1/datetime_endpoint";
  static inline const AstarteData DATA = AstarteData(std::chrono::system_clock::time_point(
      std::chrono::sys_days(std::chrono::year_month_day(
          std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
      10h + 15min));
};

struct BinaryBlob {
  static constexpr std::string_view ENDPOINT_PARTIAL = "binaryblob_endpoint";
  static constexpr std::string_view ENDPOINT_FULL = "/binaryblob_endpoint";
  static constexpr std::string_view ENDPOINT_SENSOR = "/sensor1/binaryblob_endpoint";
  static inline const AstarteData DATA = AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5});
};

struct IntegerArray {
  static constexpr std::string_view ENDPOINT_PARTIAL = "integerarray_endpoint";
  static constexpr std::string_view ENDPOINT_FULL = "/integerarray_endpoint";
  static constexpr std::string_view ENDPOINT_SENSOR = "/sensor1/integerarray_endpoint";
  static inline const AstarteData DATA = AstarteData(std::vector<int32_t>{13, 2});
};

struct LongIntegerArray {
  static constexpr std::string_view ENDPOINT_PARTIAL = "longintegerarray_endpoint";
  static constexpr std::string_view ENDPOINT_FULL = "/longintegerarray_endpoint";
  static constexpr std::string_view ENDPOINT_SENSOR = "/sensor1/longintegerarray_endpoint";
  static inline const AstarteData DATA = AstarteData(std::vector<int64_t>{17179869184, 5});
};

struct DoubleArray {
  static constexpr std::string_view ENDPOINT_PARTIAL = "doublearray_endpoint";
  static constexpr std::string_view ENDPOINT_FULL = "/doublearray_endpoint";
  static constexpr std::string_view ENDPOINT_SENSOR = "/sensor1/doublearray_endpoint";
  static inline const AstarteData DATA = AstarteData(std::vector<double>{0.5});
};

struct BooleanArray {
  static constexpr std::string_view ENDPOINT_PARTIAL = "booleanarray_endpoint";
  static constexpr std::string_view ENDPOINT_FULL = "/booleanarray_endpoint";
  static constexpr std::string_view ENDPOINT_SENSOR = "/sensor1/booleanarray_endpoint";
  static inline const AstarteData DATA = AstarteData(std::vector<bool>{false, true});
};

struct StringArray {
  static constexpr std::string_view ENDPOINT_PARTIAL = "stringarray_endpoint";
  static constexpr std::string_view ENDPOINT_FULL = "/stringarray_endpoint";
  static constexpr std::string_view ENDPOINT_SENSOR = "/sensor1/stringarray_endpoint";
  static inline const AstarteData DATA =
      AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"});
};

struct DatetimeArray {
  static constexpr std::string_view ENDPOINT_PARTIAL = "datetimearray_endpoint";
  static constexpr std::string_view ENDPOINT_FULL = "/datetimearray_endpoint";
  static constexpr std::string_view ENDPOINT_SENSOR = "/sensor1/datetimearray_endpoint";
  static inline const AstarteData DATA =
      AstarteData(std::vector<std::chrono::system_clock::time_point>{
          std::chrono::sys_days(std::chrono::year_month_day(
              std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
              10h + 15min,
          std::chrono::sys_days(std::chrono::year_month_day(
              std::chrono::year(1985), std::chrono::month(5), std::chrono::day(22))) +
              12s,
      });
};

struct BinaryBlobArray {
  static constexpr std::string_view ENDPOINT_PARTIAL = "binaryblobarray_endpoint";
  static constexpr std::string_view ENDPOINT_FULL = "/binaryblobarray_endpoint";
  static constexpr std::string_view ENDPOINT_SENSOR = "/sensor1/binaryblobarray_endpoint";
  static inline const AstarteData DATA =
      AstarteData(std::vector<std::vector<uint8_t>>{{0x23, 0x43, 0xF5}, {0x43, 0xF3, 0x00}});
};

}  // namespace constants::data_sets
