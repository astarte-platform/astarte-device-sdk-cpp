// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <spdlog/spdlog.h>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

#include "astarte_device_sdk/data.hpp"
#if defined(ASTARTE_TRANSPORT_GRPC)
#include "astarte_device_sdk/device_grpc.hpp"
#else
#include "astarte_device_sdk/device_mqtt.hpp"
#endif
#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/msg.hpp"

using AstarteDeviceSdk::AstarteData;
using AstarteDeviceSdk::AstarteDatastreamIndividual;
using AstarteDeviceSdk::AstarteDatastreamObject;
using AstarteDeviceSdk::AstarteDevice;
#if defined(ASTARTE_TRANSPORT_GRPC)
using AstarteDeviceSdk::AstarteDeviceGRPC;
#else
using AstarteDeviceSdk::AstarteDeviceMQTT;
#endif
using AstarteDeviceSdk::AstarteMessage;
using AstarteDeviceSdk::AstartePropertyIndividual;

using namespace std::chrono_literals;

void reception_handler(std::stop_token token, std::shared_ptr<AstarteDevice> device) {
  while (!token.stop_requested()) {
    auto incoming = device->poll_incoming(100ms);
    if (incoming.has_value()) {
      AstarteMessage msg(incoming.value());
      spdlog::info("Received message.");
      spdlog::info("Interface name: {}", msg.get_interface());
      spdlog::info("Path: {}", msg.get_path());
      if (msg.is_datastream()) {
        if (msg.is_individual()) {
          spdlog::info("Type: individual datastream");
          const auto& data(msg.into<AstarteDatastreamIndividual>());
          spdlog::info("Value: {}", data);
        } else {
          spdlog::info("Type: object datastream");
          const auto& data(msg.into<AstarteDatastreamObject>());
          spdlog::info("Value: {}", data);
        }
      } else {
        spdlog::info("Type: individual property");
        const auto& data(msg.into<AstartePropertyIndividual>());
        spdlog::info("Value: {}", data);
      }
    }
  }
}

int main(int argc, char** argv) {
  spdlog::set_level(spdlog::level::debug);
  std::string server_addr = "localhost:50051";
  std::string node_id("aa04dade-9401-4c37-8c6a-d8da15b083ae");
#if defined(ASTARTE_TRANSPORT_GRPC)
  std::shared_ptr<AstarteDeviceGRPC> device =
      std::make_shared<AstarteDeviceGRPC>(server_addr, node_id);
#else
  std::shared_ptr<AstarteDeviceMQTT> device =
      std::make_shared<AstarteDeviceMQTT>(server_addr, node_id);
#endif

  // Those paths assume the user is calling the Astarte executable from the root of this project.
  std::filesystem::path device_individual_interface_file_path =
      "samples/grpc_native/interfaces/org.astarte-platform.cpp.examples.DeviceDatastream.json";
  device->add_interface_from_file(device_individual_interface_file_path);
  std::filesystem::path server_individual_interface_file_path =
      "samples/grpc_native/interfaces/org.astarte-platform.cpp.examples.ServerDatastream.json";
  device->add_interface_from_file(server_individual_interface_file_path);
  std::filesystem::path device_property_interface_file_path =
      "samples/grpc_native/interfaces/org.astarte-platform.cpp.examples.DeviceProperty.json";
  device->add_interface_from_file(device_property_interface_file_path);
  std::filesystem::path device_aggregated_interface_file_path =
      "samples/grpc_native/interfaces/org.astarte-platform.cpp.examples.DeviceAggregate.json";
  device->add_interface_from_file(device_aggregated_interface_file_path);
  std::filesystem::path server_aggregated_interface_file_path =
      "samples/grpc_native/interfaces/org.astarte-platform.cpp.examples.ServerAggregate.json";
  device->add_interface_from_file(server_aggregated_interface_file_path);
  std::filesystem::path server_property_interface_file_path =
      "samples/grpc_native/interfaces/org.astarte-platform.cpp.examples.ServerProperty.json";
  device->add_interface_from_file(server_property_interface_file_path);

  device->connect();

  do {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  } while (!device->is_connected());

  // Start a reception thread for the Astarte device
  auto reception_thread = std::jthread(reception_handler, device);

  {
    std::string interface_name("org.astarte-platform.cpp.examples.DeviceDatastream");
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

    std::string integer_path("/integer_endpoint");
    AstarteData integer_value = AstarteData(43);
    device->send_individual(interface_name, integer_path, integer_value, &now);

    std::string longinteger_path("/longinteger_endpoint");
    AstarteData longinteger_value = AstarteData(8589934592);
    device->send_individual(interface_name, longinteger_path, longinteger_value, &now);

    std::string double_path("/double_endpoint");
    AstarteData double_value = AstarteData(43.5);
    device->send_individual(interface_name, double_path, double_value, &now);

    std::string boolean_path("/boolean_endpoint");
    AstarteData boolean_value = AstarteData(true);
    device->send_individual(interface_name, boolean_path, boolean_value, &now);

    std::string string_path("/string_endpoint");
    std::string hello_string("Hello from cpp!");
    AstarteData string_value = AstarteData(hello_string);
    device->send_individual(interface_name, string_path, string_value, &now);

    std::string binaryblob_path("/binaryblob_endpoint");
    std::vector<uint8_t> binaryblob = {10, 20, 30, 40, 50};
    AstarteData binaryblob_value = AstarteData(binaryblob);
    device->send_individual(interface_name, binaryblob_path, binaryblob_value, &now);

    std::string datetime_path("/datetime_endpoint");
    AstarteData datetime_value = AstarteData(std::chrono::system_clock::now());
    device->send_individual(interface_name, datetime_path, datetime_value, &now);

    std::string integerarray_path("/integerarray_endpoint");
    std::vector<int32_t> integerarray = {10, 20, 30, 40, 50};
    AstarteData integerarray_value = AstarteData(integerarray);
    device->send_individual(interface_name, integerarray_path, integerarray_value, &now);

    std::string longintegerarray_path("/longintegerarray_endpoint");
    std::vector<int64_t> longintegerarray = {8589934592, 8589934593, 8589939592};
    AstarteData longintegerarray_value = AstarteData(longintegerarray);
    device->send_individual(interface_name, longintegerarray_path, longintegerarray_value, &now);

    std::string doubleararray_path("/doublearray_endpoint");
    std::vector<double> doublearray = {0.0};
    AstarteData doublearray_value = AstarteData(doublearray);
    device->send_individual(interface_name, doubleararray_path, doublearray_value, &now);

    std::string booleanarray_path("/booleanarray_endpoint");
    std::vector<bool> booleanarray = {true, false, true};
    AstarteData booleanarray_value = AstarteData(booleanarray);
    device->send_individual(interface_name, booleanarray_path, booleanarray_value, &now);

    std::string stringarray_path("/stringarray_endpoint");
    std::vector<std::string> stringarray = {"Hello ", "world ", "from ", "C++"};
    AstarteData stringarray_value = AstarteData(stringarray);
    device->send_individual(interface_name, stringarray_path, stringarray_value, &now);

    std::string binaryblobarray_path("/binaryblobarray_endpoint");
    std::vector<std::vector<uint8_t>> binaryblobarray = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    AstarteData binaryblobarray_value = AstarteData(binaryblobarray);
    device->send_individual(interface_name, binaryblobarray_path, binaryblobarray_value, &now);

    std::string datetimearray_path("/datetimearray_endpoint");
    std::vector<std::chrono::system_clock::time_point> datetimearray = {
        std::chrono::system_clock::now(), std::chrono::system_clock::now()};
    AstarteData datetimearray_value = AstarteData(datetimearray);
    device->send_individual(interface_name, datetimearray_path, datetimearray_value, &now);

    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  {
    std::string interface_name("org.astarte-platform.cpp.examples.DeviceAggregate");
    std::string common_path("/sensor15");

    AstarteDatastreamObject data = {
        {"integer_endpoint", AstarteData(43)},
        {"longinteger_endpoint", AstarteData(8589934592)},
        {"double_endpoint", AstarteData(43.5)},
        {"boolean_endpoint", AstarteData(true)},
        {"string_endpoint", AstarteData(std::string("Hello from cpp!"))},
        {"binaryblob_endpoint", AstarteData(std::vector<uint8_t>{10, 20, 30, 40, 50})},
        {"datetime_endpoint", AstarteData(std::chrono::system_clock::now())},
        {"integerarray_endpoint", AstarteData(std::vector<int32_t>{10, 20, 30, 40, 50})},
        {"longintegerarray_endpoint",
         AstarteData(std::vector<int64_t>{8589934592, 8589934593, 8589939592})},
        {"doublearray_endpoint", AstarteData(std::vector<double>{0.0})},
        {"booleanarray_endpoint", AstarteData(std::vector<bool>{true, false, true})},
        {"stringarray_endpoint",
         AstarteData(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})},
        {"binaryblobarray_endpoint",
         AstarteData(std::vector<std::vector<uint8_t>>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})},
        {"datetimearray_endpoint",
         AstarteData(std::vector<std::chrono::system_clock::time_point>{
             std::chrono::system_clock::now(), std::chrono::system_clock::now()})}};
    device->send_object(interface_name, common_path, data, NULL);

    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  {
    std::string interface_name("org.astarte-platform.cpp.examples.DeviceProperty");

    std::string integer_path("/integer_endpoint");
    AstarteData integer_value = AstarteData(43);
    device->set_property(interface_name, integer_path, integer_value);

    std::string longinteger_path("/longinteger_endpoint");
    AstarteData longinteger_value = AstarteData(8589934592);
    device->set_property(interface_name, longinteger_path, longinteger_value);

    std::string double_path("/double_endpoint");
    AstarteData double_value = AstarteData(43.5);
    device->set_property(interface_name, double_path, double_value);

    std::string boolean_path("/boolean_endpoint");
    AstarteData boolean_value = AstarteData(true);
    device->set_property(interface_name, boolean_path, boolean_value);

    std::string string_path("/string_endpoint");
    AstarteData string_value = AstarteData(std::string("Hello from cpp!"));
    device->set_property(interface_name, string_path, string_value);

    std::string binaryblob_path("/binaryblob_endpoint");
    std::vector<uint8_t> binaryblob = {10, 20, 30, 40, 50};
    AstarteData binaryblob_value = AstarteData(binaryblob);
    device->set_property(interface_name, binaryblob_path, binaryblob_value);

    std::string datetime_path("/datetime_endpoint");
    AstarteData datetime_value = AstarteData(std::chrono::system_clock::now());
    device->set_property(interface_name, datetime_path, datetime_value);

    std::string integerarray_path("/integerarray_endpoint");
    std::vector<int32_t> integerarray = {10, 20, 30, 40, 50};
    AstarteData integerarray_value = AstarteData(integerarray);
    device->set_property(interface_name, integerarray_path, integerarray_value);

    std::string longintegerarray_path("/longintegerarray_endpoint");
    std::vector<int64_t> longintegerarray = {8589934592, 8589934593, 8589939592};
    AstarteData longintegerarray_value = AstarteData(longintegerarray);
    device->set_property(interface_name, longintegerarray_path, longintegerarray_value);

    std::string doubleararray_path("/doublearray_endpoint");
    std::vector<double> doublearray = {0.0};
    AstarteData doublearray_value = AstarteData(doublearray);
    device->set_property(interface_name, doubleararray_path, doublearray_value);

    std::string booleanarray_path("/booleanarray_endpoint");
    std::vector<bool> booleanarray = {true, false, true};
    AstarteData booleanarray_value = AstarteData(booleanarray);
    device->set_property(interface_name, booleanarray_path, booleanarray_value);

    std::string stringarray_path("/stringarray_endpoint");
    std::vector<std::string> stringarray = {"Hello ", "world ", "from ", "C++"};
    AstarteData stringarray_value = AstarteData(stringarray);
    device->set_property(interface_name, stringarray_path, stringarray_value);

    std::string binaryblobarray_path("/binaryblobarray_endpoint");
    std::vector<std::vector<uint8_t>> binaryblobarray = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    AstarteData binaryblobarray_value = AstarteData(binaryblobarray);
    device->set_property(interface_name, binaryblobarray_path, binaryblobarray_value);

    std::string datetimearray_path("/datetimearray_endpoint");
    std::vector<std::chrono::system_clock::time_point> datetimearray = {
        std::chrono::system_clock::now(), std::chrono::system_clock::now()};
    AstarteData datetimearray_value = AstarteData(datetimearray);
    device->set_property(interface_name, datetimearray_path, datetimearray_value);

    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  {
    std::string interface_name("org.astarte-platform.cpp.examples.DeviceProperty");

    std::string integer_path("/integer_endpoint");
    device->unset_property(interface_name, integer_path);

    std::string longinteger_path("/longinteger_endpoint");
    device->unset_property(interface_name, longinteger_path);

    std::string double_path("/double_endpoint");
    device->unset_property(interface_name, double_path);

    std::string boolean_path("/boolean_endpoint");
    device->unset_property(interface_name, boolean_path);

    std::string string_path("/string_endpoint");
    device->unset_property(interface_name, string_path);

    std::string binaryblob_path("/binaryblob_endpoint");
    device->unset_property(interface_name, binaryblob_path);

    std::string datetime_path("/datetime_endpoint");
    device->unset_property(interface_name, datetime_path);

    std::string integerarray_path("/integerarray_endpoint");
    device->unset_property(interface_name, integerarray_path);

    std::string longintegerarray_path("/longintegerarray_endpoint");
    device->unset_property(interface_name, longintegerarray_path);

    std::string doubleararray_path("/doublearray_endpoint");
    device->unset_property(interface_name, doubleararray_path);

    std::string booleanarray_path("/booleanarray_endpoint");
    device->unset_property(interface_name, booleanarray_path);

    std::string stringarray_path("/stringarray_endpoint");
    device->unset_property(interface_name, stringarray_path);

    std::string binaryblobarray_path("/binaryblobarray_endpoint");
    device->unset_property(interface_name, binaryblobarray_path);

    std::string datetimearray_path("/datetimearray_endpoint");
    device->unset_property(interface_name, datetimearray_path);

    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  // Wait to receive some messages
  std::this_thread::sleep_for(std::chrono::seconds(20));

  device->disconnect();

  std::this_thread::sleep_for(std::chrono::seconds(3));
  std::exit(EXIT_SUCCESS);
}
