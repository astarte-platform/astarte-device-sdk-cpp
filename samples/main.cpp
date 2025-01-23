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
  std::filesystem::path device_interface_file_path =
      "samples/interfaces/org.astarte-platform.cpp.examples.DeviceDatastream.json";
  msghub_client.add_interface_from_json(device_interface_file_path);
  std::filesystem::path server_interface_file_path =
      "samples/interfaces/org.astarte-platform.cpp.examples.ServerDatastream.json";
  msghub_client.add_interface_from_json(server_interface_file_path);

  msghub_client.connect();

  std::this_thread::sleep_for(std::chrono::seconds(3));

  std::string interface_name("org.astarte-platform.cpp.examples.DeviceDatastream");

  std::string integer_path("/integer_endpoint");
  AstarteIndividual integer_value = AstarteDeviceSdk::to_astarte_individual(43);
  msghub_client.stream_data(interface_name, integer_path, integer_value);

  std::string longinteger_path("/longinteger_endpoint");
  AstarteIndividual longinteger_value = AstarteDeviceSdk::to_astarte_individual(8589934592);
  msghub_client.stream_data(interface_name, longinteger_path, longinteger_value);

  std::string double_path("/double_endpoint");
  AstarteIndividual double_value = AstarteDeviceSdk::to_astarte_individual(43.5);
  msghub_client.stream_data(interface_name, double_path, double_value);

  std::string boolean_path("/boolean_endpoint");
  AstarteIndividual boolean_value = AstarteDeviceSdk::to_astarte_individual(true);
  msghub_client.stream_data(interface_name, boolean_path, boolean_value);

  std::string string_path("/string_endpoint");
  std::string hello_string("Hello from cpp!");
  AstarteIndividual string_value = AstarteDeviceSdk::to_astarte_individual(hello_string);
  msghub_client.stream_data(interface_name, string_path, string_value);

  std::string binaryblob_path("/binaryblob_endpoint");
  std::vector<uint8_t> binaryblob = {10, 20, 30, 40, 50};
  AstarteIndividual binaryblob_value = AstarteDeviceSdk::to_astarte_individual(binaryblob);
  msghub_client.stream_data(interface_name, binaryblob_path, binaryblob_value);

  std::string datetime_path("/datetime_endpoint");
  AstarteIndividual datetime_value =
      AstarteDeviceSdk::to_astarte_individual(std::chrono::system_clock::now());
  msghub_client.stream_data(interface_name, datetime_path, datetime_value);

  std::string integerarray_path("/integerarray_endpoint");
  std::vector<int32_t> integerarray = {10, 20, 30, 40, 50};
  AstarteIndividual integerarray_value = AstarteDeviceSdk::to_astarte_individual(integerarray);
  msghub_client.stream_data(interface_name, integerarray_path, integerarray_value);

  std::string longintegerarray_path("/longintegerarray_endpoint");
  std::vector<int64_t> longintegerarray = {8589934592, 8589934593, 8589939592};
  AstarteIndividual longintegerarray_value =
      AstarteDeviceSdk::to_astarte_individual(longintegerarray);
  msghub_client.stream_data(interface_name, longintegerarray_path, longintegerarray_value);

  std::string doubleararray_path("/doublearray_endpoint");
  std::vector<double> doublearray = {0.0};
  AstarteIndividual doublearray_value = AstarteDeviceSdk::to_astarte_individual(doublearray);
  msghub_client.stream_data(interface_name, doubleararray_path, doublearray_value);

  std::string booleanarray_path("/booleanarray_endpoint");
  std::vector<bool> booleanarray = {true, false, true};
  AstarteIndividual booleanarray_value = AstarteDeviceSdk::to_astarte_individual(booleanarray);
  msghub_client.stream_data(interface_name, booleanarray_path, booleanarray_value);

  std::string stringarray_path("/stringarray_endpoint");
  std::vector<std::string> stringarray = {"Hello ", "world ", "from ", "C++"};
  AstarteIndividual stringarray_value = AstarteDeviceSdk::to_astarte_individual(stringarray);
  msghub_client.stream_data(interface_name, stringarray_path, stringarray_value);

  std::string binaryblobarray_path("/binaryblobarray_endpoint");
  std::vector<std::vector<uint8_t>> binaryblobarray = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
  AstarteIndividual binaryblobarray_value =
      AstarteDeviceSdk::to_astarte_individual(binaryblobarray);
  msghub_client.stream_data(interface_name, binaryblobarray_path, binaryblobarray_value);

  std::string datetimearray_path("/datetimearray_endpoint");
  std::vector<std::chrono::system_clock::time_point> datetimearray = {
      std::chrono::system_clock::now(), std::chrono::system_clock::now()};
  AstarteIndividual datetimearray_value = AstarteDeviceSdk::to_astarte_individual(datetimearray);
  msghub_client.stream_data(interface_name, datetimearray_path, datetimearray_value);

  std::this_thread::sleep_for(std::chrono::seconds(30));

  msghub_client.disconnect();

  std::this_thread::sleep_for(std::chrono::seconds(3));
  return 0;
}
