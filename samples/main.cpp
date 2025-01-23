// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include "astarte_device_sdk/device.h"
#include "astarte_device_sdk/individual.h"

using AstarteDeviceSdk::AstarteDevice;
using AstarteDeviceSdk::AstarteIndividual;

int main(int argc, char **argv) {
  std::string server_addr = "localhost:50051";
  std::string node_id("aa04dade-9401-4c37-8c6a-d8da15b083ae");
  AstarteDevice msghub_client(server_addr, node_id);

  // Those paths assume the user is calling the Astarte executable from the root of this project.
  std::filesystem::path device_individual_interface_file_path =
      "samples/interfaces/org.astarte-platform.cpp.examples.DeviceDatastream.json";
  msghub_client.add_interface_from_json(device_individual_interface_file_path);
  std::filesystem::path server_individual_interface_file_path =
      "samples/interfaces/org.astarte-platform.cpp.examples.ServerDatastream.json";
  msghub_client.add_interface_from_json(server_individual_interface_file_path);
  std::filesystem::path device_property_interface_file_path =
      "samples/interfaces/org.astarte-platform.cpp.examples.DeviceProperty.json";
  msghub_client.add_interface_from_json(device_property_interface_file_path);
  std::filesystem::path device_aggregated_interface_file_path =
      "samples/interfaces/org.astarte-platform.cpp.examples.DeviceAggregate.json";
  msghub_client.add_interface_from_json(device_aggregated_interface_file_path);
  std::filesystem::path server_aggregated_interface_file_path =
      "samples/interfaces/org.astarte-platform.cpp.examples.ServerAggregate.json";
  msghub_client.add_interface_from_json(server_aggregated_interface_file_path);
  std::filesystem::path server_property_interface_file_path =
      "samples/interfaces/org.astarte-platform.cpp.examples.ServerProperty.json";
  msghub_client.add_interface_from_json(server_property_interface_file_path);

  msghub_client.connect();

  std::this_thread::sleep_for(std::chrono::seconds(3));

  {
    std::string interface_name("org.astarte-platform.cpp.examples.DeviceDatastream");
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

    std::string integer_path("/integer_endpoint");
    AstarteIndividual integer_value = AstarteDeviceSdk::to_astarte_individual(43);
    msghub_client.stream_individual(interface_name, integer_path, integer_value, &now);

    std::string longinteger_path("/longinteger_endpoint");
    AstarteIndividual longinteger_value = AstarteDeviceSdk::to_astarte_individual(8589934592);
    msghub_client.stream_individual(interface_name, longinteger_path, longinteger_value, &now);

    std::string double_path("/double_endpoint");
    AstarteIndividual double_value = AstarteDeviceSdk::to_astarte_individual(43.5);
    msghub_client.stream_individual(interface_name, double_path, double_value, &now);

    std::string boolean_path("/boolean_endpoint");
    AstarteIndividual boolean_value = AstarteDeviceSdk::to_astarte_individual(true);
    msghub_client.stream_individual(interface_name, boolean_path, boolean_value, &now);

    std::string string_path("/string_endpoint");
    std::string hello_string("Hello from cpp!");
    AstarteIndividual string_value = AstarteDeviceSdk::to_astarte_individual(hello_string);
    msghub_client.stream_individual(interface_name, string_path, string_value, &now);

    std::string binaryblob_path("/binaryblob_endpoint");
    std::vector<uint8_t> binaryblob = {10, 20, 30, 40, 50};
    AstarteIndividual binaryblob_value = AstarteDeviceSdk::to_astarte_individual(binaryblob);
    msghub_client.stream_individual(interface_name, binaryblob_path, binaryblob_value, &now);

    std::string datetime_path("/datetime_endpoint");
    AstarteIndividual datetime_value =
        AstarteDeviceSdk::to_astarte_individual(std::chrono::system_clock::now());
    msghub_client.stream_individual(interface_name, datetime_path, datetime_value, &now);

    std::string integerarray_path("/integerarray_endpoint");
    std::vector<int32_t> integerarray = {10, 20, 30, 40, 50};
    AstarteIndividual integerarray_value = AstarteDeviceSdk::to_astarte_individual(integerarray);
    msghub_client.stream_individual(interface_name, integerarray_path, integerarray_value, &now);

    std::string longintegerarray_path("/longintegerarray_endpoint");
    std::vector<int64_t> longintegerarray = {8589934592, 8589934593, 8589939592};
    AstarteIndividual longintegerarray_value =
        AstarteDeviceSdk::to_astarte_individual(longintegerarray);
    msghub_client.stream_individual(interface_name, longintegerarray_path, longintegerarray_value,
                                    &now);

    std::string doubleararray_path("/doublearray_endpoint");
    std::vector<double> doublearray = {0.0};
    AstarteIndividual doublearray_value = AstarteDeviceSdk::to_astarte_individual(doublearray);
    msghub_client.stream_individual(interface_name, doubleararray_path, doublearray_value, &now);

    std::string booleanarray_path("/booleanarray_endpoint");
    std::vector<bool> booleanarray = {true, false, true};
    AstarteIndividual booleanarray_value = AstarteDeviceSdk::to_astarte_individual(booleanarray);
    msghub_client.stream_individual(interface_name, booleanarray_path, booleanarray_value, &now);

    std::string stringarray_path("/stringarray_endpoint");
    std::vector<std::string> stringarray = {"Hello ", "world ", "from ", "C++"};
    AstarteIndividual stringarray_value = AstarteDeviceSdk::to_astarte_individual(stringarray);
    msghub_client.stream_individual(interface_name, stringarray_path, stringarray_value, &now);

    std::string binaryblobarray_path("/binaryblobarray_endpoint");
    std::vector<std::vector<uint8_t>> binaryblobarray = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    AstarteIndividual binaryblobarray_value =
        AstarteDeviceSdk::to_astarte_individual(binaryblobarray);
    msghub_client.stream_individual(interface_name, binaryblobarray_path, binaryblobarray_value,
                                    &now);

    std::string datetimearray_path("/datetimearray_endpoint");
    std::vector<std::chrono::system_clock::time_point> datetimearray = {
        std::chrono::system_clock::now(), std::chrono::system_clock::now()};
    AstarteIndividual datetimearray_value = AstarteDeviceSdk::to_astarte_individual(datetimearray);
    msghub_client.stream_individual(interface_name, datetimearray_path, datetimearray_value, &now);

    std::this_thread::sleep_for(std::chrono::seconds(5));
  }

  {
    std::string interface_name("org.astarte-platform.cpp.examples.DeviceAggregate");
    std::string common_path("/sensor15");

    std::unordered_map<std::string, AstarteIndividual> data = {
        {"integer_endpoint", AstarteDeviceSdk::to_astarte_individual(43)},
        {"longinteger_endpoint", AstarteDeviceSdk::to_astarte_individual(8589934592)},
        {"double_endpoint", AstarteDeviceSdk::to_astarte_individual(43.5)},
        {"boolean_endpoint", AstarteDeviceSdk::to_astarte_individual(true)},
        {"string_endpoint",
         AstarteDeviceSdk::to_astarte_individual(std::string("Hello from cpp!"))},
        {"binaryblob_endpoint",
         AstarteDeviceSdk::to_astarte_individual(std::vector<uint8_t>{10, 20, 30, 40, 50})},
        {"datetime_endpoint",
         AstarteDeviceSdk::to_astarte_individual(std::chrono::system_clock::now())},
        {"integerarray_endpoint",
         AstarteDeviceSdk::to_astarte_individual(std::vector<int32_t>{10, 20, 30, 40, 50})},
        {"longintegerarray_endpoint", AstarteDeviceSdk::to_astarte_individual(std::vector<int64_t>{
                                          8589934592, 8589934593, 8589939592})},
        {"doublearray_endpoint", AstarteDeviceSdk::to_astarte_individual(std::vector<double>{0.0})},
        {"booleanarray_endpoint",
         AstarteDeviceSdk::to_astarte_individual(std::vector<bool>{true, false, true})},
        {"stringarray_endpoint", AstarteDeviceSdk::to_astarte_individual(
                                     std::vector<std::string>{"Hello ", "world ", "from ", "C++"})},
        {"binaryblobarray_endpoint",
         AstarteDeviceSdk::to_astarte_individual(
             std::vector<std::vector<uint8_t>>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})},
        {"datetimearray_endpoint",
         AstarteDeviceSdk::to_astarte_individual(std::vector<std::chrono::system_clock::time_point>{
             std::chrono::system_clock::now(), std::chrono::system_clock::now()})}};

    msghub_client.stream_aggregated(interface_name, common_path, data, NULL);

    std::this_thread::sleep_for(std::chrono::seconds(5));
  }

  {
    std::string interface_name("org.astarte-platform.cpp.examples.DeviceProperty");

    std::string integer_path("/integer_endpoint");
    AstarteIndividual integer_value = AstarteDeviceSdk::to_astarte_individual(43);
    msghub_client.set_property(interface_name, integer_path, integer_value);

    std::string longinteger_path("/longinteger_endpoint");
    AstarteIndividual longinteger_value = AstarteDeviceSdk::to_astarte_individual(8589934592);
    msghub_client.set_property(interface_name, longinteger_path, longinteger_value);

    std::string double_path("/double_endpoint");
    AstarteIndividual double_value = AstarteDeviceSdk::to_astarte_individual(43.5);
    msghub_client.set_property(interface_name, double_path, double_value);

    std::string boolean_path("/boolean_endpoint");
    AstarteIndividual boolean_value = AstarteDeviceSdk::to_astarte_individual(true);
    msghub_client.set_property(interface_name, boolean_path, boolean_value);

    std::string string_path("/string_endpoint");
    std::string hello_string("Hello from cpp!");
    AstarteIndividual string_value = AstarteDeviceSdk::to_astarte_individual(hello_string);
    msghub_client.set_property(interface_name, string_path, string_value);

    std::string binaryblob_path("/binaryblob_endpoint");
    std::vector<uint8_t> binaryblob = {10, 20, 30, 40, 50};
    AstarteIndividual binaryblob_value = AstarteDeviceSdk::to_astarte_individual(binaryblob);
    msghub_client.set_property(interface_name, binaryblob_path, binaryblob_value);

    std::string datetime_path("/datetime_endpoint");
    AstarteIndividual datetime_value =
        AstarteDeviceSdk::to_astarte_individual(std::chrono::system_clock::now());
    msghub_client.set_property(interface_name, datetime_path, datetime_value);

    std::string integerarray_path("/integerarray_endpoint");
    std::vector<int32_t> integerarray = {10, 20, 30, 40, 50};
    AstarteIndividual integerarray_value = AstarteDeviceSdk::to_astarte_individual(integerarray);
    msghub_client.set_property(interface_name, integerarray_path, integerarray_value);

    std::string longintegerarray_path("/longintegerarray_endpoint");
    std::vector<int64_t> longintegerarray = {8589934592, 8589934593, 8589939592};
    AstarteIndividual longintegerarray_value =
        AstarteDeviceSdk::to_astarte_individual(longintegerarray);
    msghub_client.set_property(interface_name, longintegerarray_path, longintegerarray_value);

    std::string doubleararray_path("/doublearray_endpoint");
    std::vector<double> doublearray = {0.0};
    AstarteIndividual doublearray_value = AstarteDeviceSdk::to_astarte_individual(doublearray);
    msghub_client.set_property(interface_name, doubleararray_path, doublearray_value);

    std::string booleanarray_path("/booleanarray_endpoint");
    std::vector<bool> booleanarray = {true, false, true};
    AstarteIndividual booleanarray_value = AstarteDeviceSdk::to_astarte_individual(booleanarray);
    msghub_client.set_property(interface_name, booleanarray_path, booleanarray_value);

    std::string stringarray_path("/stringarray_endpoint");
    std::vector<std::string> stringarray = {"Hello ", "world ", "from ", "C++"};
    AstarteIndividual stringarray_value = AstarteDeviceSdk::to_astarte_individual(stringarray);
    msghub_client.set_property(interface_name, stringarray_path, stringarray_value);

    std::string binaryblobarray_path("/binaryblobarray_endpoint");
    std::vector<std::vector<uint8_t>> binaryblobarray = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    AstarteIndividual binaryblobarray_value =
        AstarteDeviceSdk::to_astarte_individual(binaryblobarray);
    msghub_client.set_property(interface_name, binaryblobarray_path, binaryblobarray_value);

    std::string datetimearray_path("/datetimearray_endpoint");
    std::vector<std::chrono::system_clock::time_point> datetimearray = {
        std::chrono::system_clock::now(), std::chrono::system_clock::now()};
    AstarteIndividual datetimearray_value = AstarteDeviceSdk::to_astarte_individual(datetimearray);
    msghub_client.set_property(interface_name, datetimearray_path, datetimearray_value);

    std::this_thread::sleep_for(std::chrono::seconds(10));
  }

  {
    std::string interface_name("org.astarte-platform.cpp.examples.DeviceProperty");

    std::string integer_path("/integer_endpoint");
    msghub_client.unset_property(interface_name, integer_path);

    std::string longinteger_path("/longinteger_endpoint");
    msghub_client.unset_property(interface_name, longinteger_path);

    std::string double_path("/double_endpoint");
    msghub_client.unset_property(interface_name, double_path);

    std::string boolean_path("/boolean_endpoint");
    msghub_client.unset_property(interface_name, boolean_path);

    std::string string_path("/string_endpoint");
    msghub_client.unset_property(interface_name, string_path);

    std::string binaryblob_path("/binaryblob_endpoint");
    msghub_client.unset_property(interface_name, binaryblob_path);

    std::string datetime_path("/datetime_endpoint");
    msghub_client.unset_property(interface_name, datetime_path);

    std::string integerarray_path("/integerarray_endpoint");
    msghub_client.unset_property(interface_name, integerarray_path);

    std::string longintegerarray_path("/longintegerarray_endpoint");
    msghub_client.unset_property(interface_name, longintegerarray_path);

    std::string doubleararray_path("/doublearray_endpoint");
    msghub_client.unset_property(interface_name, doubleararray_path);

    std::string booleanarray_path("/booleanarray_endpoint");
    msghub_client.unset_property(interface_name, booleanarray_path);

    std::string stringarray_path("/stringarray_endpoint");
    msghub_client.unset_property(interface_name, stringarray_path);

    std::string binaryblobarray_path("/binaryblobarray_endpoint");
    msghub_client.unset_property(interface_name, binaryblobarray_path);

    std::string datetimearray_path("/datetimearray_endpoint");
    msghub_client.unset_property(interface_name, datetimearray_path);

    std::this_thread::sleep_for(std::chrono::seconds(10));
  }

  msghub_client.disconnect();

  std::this_thread::sleep_for(std::chrono::seconds(3));
  return 0;
}
