// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>
#include <vector>

#include "action.hpp"
#include "case.hpp"
#include "constants/astarte_interfaces.hpp"
#include "constants/astarte_time.hpp"

namespace testcases {
using namespace std::chrono_literals;

using AstarteDeviceSdk::AstarteOwnership;

std::vector<Action> set_all_properties() {
  return {
      Actions::TransmitDeviceData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                                 "/integer_endpoint",
                                                 AstartePropertyIndividual(AstarteData(12)))),
      Actions::TransmitDeviceData(
          AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/longinteger_endpoint",
                         AstartePropertyIndividual(AstarteData(17179869184)))),
      Actions::TransmitDeviceData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                                 "/double_endpoint",
                                                 AstartePropertyIndividual(AstarteData(54.4)))),
      Actions::TransmitDeviceData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                                 "/boolean_endpoint",
                                                 AstartePropertyIndividual(AstarteData(true)))),
      Actions::TransmitDeviceData(
          AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/string_endpoint",
                         AstartePropertyIndividual(AstarteData(std::string("Hello C++!"))))),
      Actions::TransmitDeviceData(AstarteMessage(
          astarte_interfaces::DeviceProperty::INTERFACE, "/datetime_endpoint",
          AstartePropertyIndividual(AstarteData(std::chrono::system_clock::time_point(
              std::chrono::sys_days(std::chrono::year_month_day(
                  std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
              std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0)))))),
      Actions::TransmitDeviceData(AstarteMessage(
          astarte_interfaces::DeviceProperty::INTERFACE, "/binaryblob_endpoint",
          AstartePropertyIndividual(AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5})))),
      Actions::TransmitDeviceData(
          AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/integerarray_endpoint",
                         AstartePropertyIndividual(AstarteData(std::vector<int32_t>{13, 2})))),
      Actions::TransmitDeviceData(AstarteMessage(
          astarte_interfaces::DeviceProperty::INTERFACE, "/longintegerarray_endpoint",
          AstartePropertyIndividual(AstarteData(std::vector<int64_t>{17179869184, 5})))),
      Actions::TransmitDeviceData(
          AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/doublearray_endpoint",
                         AstartePropertyIndividual(AstarteData(std::vector<double>{0.5})))),
      Actions::TransmitDeviceData(
          AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/booleanarray_endpoint",
                         AstartePropertyIndividual(AstarteData(std::vector<bool>{false, true})))),
      Actions::TransmitDeviceData(AstarteMessage(
          astarte_interfaces::DeviceProperty::INTERFACE, "/stringarray_endpoint",
          AstartePropertyIndividual(
              AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})))),
      Actions::TransmitDeviceData(AstarteMessage(
          astarte_interfaces::DeviceProperty::INTERFACE, "/datetimearray_endpoint",
          AstartePropertyIndividual(AstarteData(std::vector<std::chrono::system_clock::time_point>{
              std::chrono::sys_days(std::chrono::year_month_day(
                  std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                  std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0),
              std::chrono::sys_days(std::chrono::year_month_day(
                  std::chrono::year(1985), std::chrono::month(5), std::chrono::day(22))) +
                  std::chrono::hours(0) + std::chrono::minutes(0) + std::chrono::seconds(12),
          })))),
      Actions::TransmitDeviceData(
          AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "/binaryblobarray_endpoint",
                         AstartePropertyIndividual(AstarteData(std::vector<std::vector<uint8_t>>{
                             {0x23, 0x43, 0xF5}, {0x43, 0xF3, 0x00}})))),
  };
}

std::vector<Action> check_all_properties_have_value() {
  return {
      Actions::FetchRESTData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                            "integer_endpoint",
                                            AstartePropertyIndividual(AstarteData(12)))),
      Actions::FetchRESTData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                            "longinteger_endpoint",
                                            AstartePropertyIndividual(AstarteData(17179869184)))),
      Actions::FetchRESTData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                            "double_endpoint",
                                            AstartePropertyIndividual(AstarteData(54.4)))),
      Actions::FetchRESTData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                            "boolean_endpoint",
                                            AstartePropertyIndividual(AstarteData(true)))),
      Actions::FetchRESTData(
          AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "string_endpoint",
                         AstartePropertyIndividual(AstarteData(std::string("Hello C++!"))))),
      Actions::FetchRESTData(AstarteMessage(
          astarte_interfaces::DeviceProperty::INTERFACE, "datetime_endpoint",
          AstartePropertyIndividual(AstarteData(std::chrono::system_clock::time_point(
              std::chrono::sys_days{1994y / 4 / 12} + 10h + 15min))))),
      Actions::FetchRESTData(AstarteMessage(
          astarte_interfaces::DeviceProperty::INTERFACE, "binaryblob_endpoint",
          AstartePropertyIndividual(AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5})))),
      Actions::FetchRESTData(
          AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "integerarray_endpoint",
                         AstartePropertyIndividual(AstarteData(std::vector<int32_t>{13, 2})))),
      Actions::FetchRESTData(AstarteMessage(
          astarte_interfaces::DeviceProperty::INTERFACE, "longintegerarray_endpoint",
          AstartePropertyIndividual(AstarteData(std::vector<int64_t>{17179869184, 5})))),
      Actions::FetchRESTData(
          AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "doublearray_endpoint",
                         AstartePropertyIndividual(AstarteData(std::vector<double>{0.5})))),
      Actions::FetchRESTData(
          AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "booleanarray_endpoint",
                         AstartePropertyIndividual(AstarteData(std::vector<bool>{false, true})))),
      Actions::FetchRESTData(AstarteMessage(
          astarte_interfaces::DeviceProperty::INTERFACE, "stringarray_endpoint",
          AstartePropertyIndividual(
              AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})))),
      Actions::FetchRESTData(AstarteMessage(
          astarte_interfaces::DeviceProperty::INTERFACE, "datetimearray_endpoint",
          AstartePropertyIndividual(AstarteData(std::vector<std::chrono::system_clock::time_point>{
              std::chrono::sys_days{1994y / 4 / 12} + 10h + 15min,
              std::chrono::sys_days{1985y / 5 / 22} + 12s,
          })))),
      Actions::FetchRESTData(
          AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE, "binaryblobarray_endpoint",
                         AstartePropertyIndividual(AstarteData(std::vector<std::vector<uint8_t>>{
                             {0x23, 0x43, 0xF5}, {0x43, 0xF3, 0x00}})))),
  };
}

std::vector<Action> unset_all_properties() {
  return {
      Actions::TransmitDeviceData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                                 "/integer_endpoint",
                                                 AstartePropertyIndividual(std::nullopt))),
      Actions::TransmitDeviceData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                                 "/longinteger_endpoint",
                                                 AstartePropertyIndividual(std::nullopt))),
      Actions::TransmitDeviceData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                                 "/double_endpoint",
                                                 AstartePropertyIndividual(std::nullopt))),
      Actions::TransmitDeviceData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                                 "/boolean_endpoint",
                                                 AstartePropertyIndividual(std::nullopt))),
      Actions::TransmitDeviceData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                                 "/string_endpoint",
                                                 AstartePropertyIndividual(std::nullopt))),
      Actions::TransmitDeviceData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                                 "/datetime_endpoint",
                                                 AstartePropertyIndividual(std::nullopt))),
      Actions::TransmitDeviceData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                                 "/binaryblob_endpoint",
                                                 AstartePropertyIndividual(std::nullopt))),
      Actions::TransmitDeviceData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                                 "/integerarray_endpoint",
                                                 AstartePropertyIndividual(std::nullopt))),
      Actions::TransmitDeviceData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                                 "/longintegerarray_endpoint",
                                                 AstartePropertyIndividual(std::nullopt))),
      Actions::TransmitDeviceData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                                 "/doublearray_endpoint",
                                                 AstartePropertyIndividual(std::nullopt))),
      Actions::TransmitDeviceData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                                 "/booleanarray_endpoint",
                                                 AstartePropertyIndividual(std::nullopt))),
      Actions::TransmitDeviceData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                                 "/stringarray_endpoint",
                                                 AstartePropertyIndividual(std::nullopt))),
      Actions::TransmitDeviceData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                                 "/datetimearray_endpoint",
                                                 AstartePropertyIndividual(std::nullopt))),
      Actions::TransmitDeviceData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                                 "/binaryblobarray_endpoint",
                                                 AstartePropertyIndividual(std::nullopt))),
  };
}

std::vector<Action> check_all_properties_no_value() {
  return {
      Actions::FetchRESTData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                            "integer_endpoint",
                                            AstartePropertyIndividual(std::nullopt))),
      Actions::FetchRESTData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                            "longinteger_endpoint",
                                            AstartePropertyIndividual(std::nullopt))),
      Actions::FetchRESTData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                            "double_endpoint",
                                            AstartePropertyIndividual(std::nullopt))),
      Actions::FetchRESTData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                            "boolean_endpoint",
                                            AstartePropertyIndividual(std::nullopt))),
      Actions::FetchRESTData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                            "string_endpoint",
                                            AstartePropertyIndividual(std::nullopt))),
      Actions::FetchRESTData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                            "datetime_endpoint",
                                            AstartePropertyIndividual(std::nullopt))),
      Actions::FetchRESTData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                            "binaryblob_endpoint",
                                            AstartePropertyIndividual(std::nullopt))),
      Actions::FetchRESTData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                            "integerarray_endpoint",
                                            AstartePropertyIndividual(std::nullopt))),
      Actions::FetchRESTData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                            "longintegerarray_endpoint",
                                            AstartePropertyIndividual(std::nullopt))),
      Actions::FetchRESTData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                            "doublearray_endpoint",
                                            AstartePropertyIndividual(std::nullopt))),
      Actions::FetchRESTData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                            "booleanarray_endpoint",
                                            AstartePropertyIndividual(std::nullopt))),
      Actions::FetchRESTData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                            "stringarray_endpoint",
                                            AstartePropertyIndividual(std::nullopt))),
      Actions::FetchRESTData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                            "datetimearray_endpoint",
                                            AstartePropertyIndividual(std::nullopt))),
      Actions::FetchRESTData(AstarteMessage(astarte_interfaces::DeviceProperty::INTERFACE,
                                            "binaryblobarray_endpoint",
                                            AstartePropertyIndividual(std::nullopt))),
  };
}

std::vector<Action> get_properties() {
  return {
      Actions::GetDeviceProperty(std::string(astarte_interfaces::DeviceProperty::INTERFACE),
                                 "/integer_endpoint", AstartePropertyIndividual(AstarteData(12))),
      Actions::GetDeviceProperty(std::string(astarte_interfaces::DeviceProperty::INTERFACE),
                                 "/double_endpoint", AstartePropertyIndividual(AstarteData(54.4))),
      Actions::GetDeviceProperty(std::string(astarte_interfaces::DeviceProperty::INTERFACE),
                                 "/boolean_endpoint", AstartePropertyIndividual(AstarteData(true))),
      Actions::GetDeviceProperty(
          std::string(astarte_interfaces::DeviceProperty::INTERFACE), "/integerarray_endpoint",
          AstartePropertyIndividual(AstarteData(std::vector<int32_t>{13, 2}))),
      Actions::GetDeviceProperty(std::string(astarte_interfaces::DeviceProperty::INTERFACE),
                                 "/doublearray_endpoint",
                                 AstartePropertyIndividual(AstarteData(std::vector<double>{0.5}))),
      Actions::GetDeviceProperties(
          std::string(astarte_interfaces::DeviceProperty::INTERFACE),
          {AstarteStoredProperty(astarte_interfaces::DeviceProperty::INTERFACE, "/integer_endpoint",
                                 0, AstarteOwnership::kDevice, AstarteData(12)),
           AstarteStoredProperty(astarte_interfaces::DeviceProperty::INTERFACE,
                                 "/longinteger_endpoint", 0, AstarteOwnership::kDevice,
                                 AstarteData(17179869184)),
           AstarteStoredProperty(astarte_interfaces::DeviceProperty::INTERFACE, "/double_endpoint",
                                 0, AstarteOwnership::kDevice, AstarteData(54.4)),
           AstarteStoredProperty(astarte_interfaces::DeviceProperty::INTERFACE, "/boolean_endpoint",
                                 0, AstarteOwnership::kDevice, AstarteData(true)),
           AstarteStoredProperty(astarte_interfaces::DeviceProperty::INTERFACE, "/string_endpoint",
                                 0, AstarteOwnership::kDevice,
                                 AstarteData(std::string("Hello C++!"))),
           AstarteStoredProperty(
               astarte_interfaces::DeviceProperty::INTERFACE, "/datetime_endpoint", 0,
               AstarteOwnership::kDevice,
               AstarteData(std::chrono::system_clock::time_point(
                   std::chrono::sys_days(std::chrono::year_month_day(
                       std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                   std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0)))),
           AstarteStoredProperty(astarte_interfaces::DeviceProperty::INTERFACE,
                                 "/binaryblob_endpoint", 0, AstarteOwnership::kDevice,
                                 AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5})),
           AstarteStoredProperty(astarte_interfaces::DeviceProperty::INTERFACE,
                                 "/integerarray_endpoint", 0, AstarteOwnership::kDevice,
                                 AstarteData(std::vector<int32_t>{13, 2})),
           AstarteStoredProperty(astarte_interfaces::DeviceProperty::INTERFACE,
                                 "/longintegerarray_endpoint", 0, AstarteOwnership::kDevice,
                                 AstarteData(std::vector<int64_t>{17179869184, 5})),
           AstarteStoredProperty(astarte_interfaces::DeviceProperty::INTERFACE,
                                 "/doublearray_endpoint", 0, AstarteOwnership::kDevice,
                                 AstarteData(std::vector<double>{0.5})),
           AstarteStoredProperty(astarte_interfaces::DeviceProperty::INTERFACE,
                                 "/booleanarray_endpoint", 0, AstarteOwnership::kDevice,
                                 AstarteData(std::vector<bool>{false, true})),
           AstarteStoredProperty(
               astarte_interfaces::DeviceProperty::INTERFACE, "/stringarray_endpoint", 0,
               AstarteOwnership::kDevice,
               AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})),
           AstarteStoredProperty(
               astarte_interfaces::DeviceProperty::INTERFACE, "/datetimearray_endpoint", 0,
               AstarteOwnership::kDevice,
               AstarteData(std::vector<std::chrono::system_clock::time_point>{
                   std::chrono::sys_days(std::chrono::year_month_day(
                       std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                       std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0),
                   std::chrono::sys_days(std::chrono::year_month_day(
                       std::chrono::year(1985), std::chrono::month(5), std::chrono::day(22))) +
                       std::chrono::hours(0) + std::chrono::minutes(0) + std::chrono::seconds(12),
               })),
           AstarteStoredProperty(astarte_interfaces::DeviceProperty::INTERFACE,
                                 "/binaryblobarray_endpoint", 0, AstarteOwnership::kDevice,
                                 AstarteData(std::vector<std::vector<uint8_t>>{
                                     {0x23, 0x43, 0xF5}, {0x43, 0xF3, 0x00}}))}),
      Actions::GetAllFilteredProperties(
          std::nullopt,
          {AstarteStoredProperty(astarte_interfaces::DeviceProperty::INTERFACE, "/integer_endpoint",
                                 0, AstarteOwnership::kDevice, AstarteData(12)),
           AstarteStoredProperty(astarte_interfaces::DeviceProperty::INTERFACE,
                                 "/longinteger_endpoint", 0, AstarteOwnership::kDevice,
                                 AstarteData(17179869184)),
           AstarteStoredProperty(astarte_interfaces::DeviceProperty::INTERFACE, "/double_endpoint",
                                 0, AstarteOwnership::kDevice, AstarteData(54.4)),
           AstarteStoredProperty(astarte_interfaces::DeviceProperty::INTERFACE, "/boolean_endpoint",
                                 0, AstarteOwnership::kDevice, AstarteData(true)),
           AstarteStoredProperty(astarte_interfaces::DeviceProperty::INTERFACE, "/string_endpoint",
                                 0, AstarteOwnership::kDevice,
                                 AstarteData(std::string("Hello C++!"))),
           AstarteStoredProperty(
               astarte_interfaces::DeviceProperty::INTERFACE, "/datetime_endpoint", 0,
               AstarteOwnership::kDevice,
               AstarteData(std::chrono::system_clock::time_point(
                   std::chrono::sys_days(std::chrono::year_month_day(
                       std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                   std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0)))),
           AstarteStoredProperty(astarte_interfaces::DeviceProperty::INTERFACE,
                                 "/binaryblob_endpoint", 0, AstarteOwnership::kDevice,
                                 AstarteData(std::vector<uint8_t>{0x23, 0x43, 0xF5})),
           AstarteStoredProperty(astarte_interfaces::DeviceProperty::INTERFACE,
                                 "/integerarray_endpoint", 0, AstarteOwnership::kDevice,
                                 AstarteData(std::vector<int32_t>{13, 2})),
           AstarteStoredProperty(astarte_interfaces::DeviceProperty::INTERFACE,
                                 "/longintegerarray_endpoint", 0, AstarteOwnership::kDevice,
                                 AstarteData(std::vector<int64_t>{17179869184, 5})),
           AstarteStoredProperty(astarte_interfaces::DeviceProperty::INTERFACE,
                                 "/doublearray_endpoint", 0, AstarteOwnership::kDevice,
                                 AstarteData(std::vector<double>{0.5})),
           AstarteStoredProperty(astarte_interfaces::DeviceProperty::INTERFACE,
                                 "/booleanarray_endpoint", 0, AstarteOwnership::kDevice,
                                 AstarteData(std::vector<bool>{false, true})),
           AstarteStoredProperty(
               astarte_interfaces::DeviceProperty::INTERFACE, "/stringarray_endpoint", 0,
               AstarteOwnership::kDevice,
               AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})),
           AstarteStoredProperty(
               astarte_interfaces::DeviceProperty::INTERFACE, "/datetimearray_endpoint", 0,
               AstarteOwnership::kDevice,
               AstarteData(std::vector<std::chrono::system_clock::time_point>{
                   std::chrono::sys_days(std::chrono::year_month_day(
                       std::chrono::year(1994), std::chrono::month(4), std::chrono::day(12))) +
                       std::chrono::hours(10) + std::chrono::minutes(15) + std::chrono::seconds(0),
                   std::chrono::sys_days(std::chrono::year_month_day(
                       std::chrono::year(1985), std::chrono::month(5), std::chrono::day(22))) +
                       std::chrono::hours(0) + std::chrono::minutes(0) + std::chrono::seconds(12),
               })),
           AstarteStoredProperty(astarte_interfaces::DeviceProperty::INTERFACE,
                                 "/binaryblobarray_endpoint", 0, AstarteOwnership::kDevice,
                                 AstarteData(std::vector<std::vector<uint8_t>>{
                                     {0x23, 0x43, 0xF5}, {0x43, 0xF3, 0x00}}))}),
  };
}

TestCase device_property(std::string device_id) {
  auto set_all_props = set_all_properties();
  auto unset_all_props = unset_all_properties();
  auto check_all_props_with_value = check_all_properties_have_value();
  auto check_all_props_no_value = check_all_properties_no_value();

  std::vector<Action> actions = {Actions::Connect(), Actions::Sleep(1s)};
  actions.insert(actions.end(), set_all_props.begin(), set_all_props.end());
  actions.push_back(Actions::Sleep(1s));
  actions.insert(actions.end(), check_all_props_with_value.begin(),
                 check_all_props_with_value.end());
  actions.push_back(Actions::Sleep(1s));
  actions.insert(actions.end(), unset_all_props.begin(), unset_all_props.end());
  actions.push_back(Actions::Sleep(1s));
  actions.insert(actions.end(), check_all_props_no_value.begin(), check_all_props_no_value.end());
  actions.push_back(Actions::Sleep(1s));
  actions.push_back(Actions::Disconnect());
  actions.push_back(Actions::Sleep(1s));

  return TestCase("Device property to Astarte", actions, device_id);
}

TestCase device_property_getter(std::string device_id) {
  auto set_all_props = set_all_properties();
  auto get_props = get_properties();
  auto unset_all_props = unset_all_properties();

  std::vector<Action> actions = {Actions::Connect(), Actions::Sleep(1s)};
  actions.insert(actions.end(), set_all_props.begin(), set_all_props.end());
  actions.push_back(Actions::Sleep(1s));
  actions.insert(actions.end(), get_props.begin(), get_props.end());
  actions.push_back(Actions::Sleep(1s));
  actions.insert(actions.end(), unset_all_props.begin(), unset_all_props.end());
  actions.push_back(Actions::Sleep(1s));
  actions.push_back(Actions::Disconnect());
  actions.push_back(Actions::Sleep(1s));

  return TestCase("Device property getter", actions, device_id);
}

}  // namespace testcases
