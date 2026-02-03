// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <spdlog/spdlog.h>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/grpc/device_grpc.hpp"
#include "astarte_device_sdk/msg.hpp"

using astarte::device::Data;
using astarte::device::DatastreamIndividual;
using astarte::device::DatastreamObject;
using astarte::device::Device;
using astarte::device::DeviceGrpc;
using astarte::device::FileOpenError;
using astarte::device::InvalidInputError;
using astarte::device::Message;
using astarte::device::PropertyIndividual;

using namespace std::chrono_literals;

void reception_handler(std::stop_token token, std::shared_ptr<Device> device) {
  while (!token.stop_requested()) {
    auto incoming = device->poll_incoming(100ms);
    if (incoming.has_value()) {
      Message msg(incoming.value());
      spdlog::info("Received message.");
      spdlog::info("Interface name: {}", msg.get_interface());
      spdlog::info("Path: {}", msg.get_path());
      if (msg.is_datastream()) {
        if (msg.is_individual()) {
          spdlog::info("Type: individual datastream");
          const auto& data(msg.into<DatastreamIndividual>());
          spdlog::info("Value: {}", data);
        } else {
          spdlog::info("Type: object datastream");
          const auto& data(msg.into<DatastreamObject>());
          spdlog::info("Value: {}", data);
        }
      } else {
        spdlog::info("Type: individual property");
        const auto& data(msg.into<PropertyIndividual>());
        spdlog::info("Value: {}", data);
      }
    }
  }
}

int main(int argc, char** argv) {
  spdlog::set_level(spdlog::level::debug);
  std::string server_addr = "localhost:50051";
  std::string node_id("aa04dade-9401-4c37-8c6a-d8da15b083ae");
  std::shared_ptr<DeviceGrpc> device = std::make_shared<DeviceGrpc>(server_addr, node_id);

  // Those paths assume the user is calling the Astarte executable from the root of this project.
  const std::filesystem::path base_path = "samples/grpc/native/interfaces";
  const std::vector<std::string> interface_files = {
      "org.astarte-platform.cpp.examples.DeviceDatastream.json",
      "org.astarte-platform.cpp.examples.ServerDatastream.json",
      "org.astarte-platform.cpp.examples.DeviceProperty.json",
      "org.astarte-platform.cpp.examples.DeviceAggregate.json",
      "org.astarte-platform.cpp.examples.ServerAggregate.json",
      "org.astarte-platform.cpp.examples.ServerProperty.json"};

  spdlog::info("Loading interfaces from {}...", base_path.string());
  for (const auto& file_name : interface_files) {
    std::filesystem::path full_path = base_path / file_name;
    auto res = device->add_interface_from_file(full_path);
    if (!res) {
      spdlog::critical(std::get<FileOpenError>(res.error()).message());
      return EXIT_FAILURE;
    }
    spdlog::debug("Successfully added interface: {}", full_path.string());
  }
  spdlog::info("All interfaces loaded successfully.");

  auto res = device->connect();
  if (!res) {
    spdlog::critical("Device connection failed");
    spdlog::critical(res.error());
    return EXIT_FAILURE;
  }

  do {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  } while (!device->is_connected());

  // Start a reception thread for the Astarte device
  auto reception_thread = std::jthread(reception_handler, device);

  {
    std::string interface_name("org.astarte-platform.cpp.examples.DeviceDatastream");
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

    std::string integer_path("/integer_endpoint");
    Data integer_value = Data(43);
    auto res = device->send_individual(interface_name, integer_path, integer_value, &now);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string longinteger_path("/longinteger_endpoint");
    Data longinteger_value = Data(8589934592);
    res = device->send_individual(interface_name, longinteger_path, longinteger_value, &now);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string double_path("/double_endpoint");
    Data double_value = Data(43.5);
    res = device->send_individual(interface_name, double_path, double_value, &now);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string boolean_path("/boolean_endpoint");
    Data boolean_value = Data(true);
    res = device->send_individual(interface_name, boolean_path, boolean_value, &now);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string string_path("/string_endpoint");
    std::string hello_string("Hello from cpp!");
    Data string_value = Data(hello_string);
    res = device->send_individual(interface_name, string_path, string_value, &now);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string binaryblob_path("/binaryblob_endpoint");
    std::vector<uint8_t> binaryblob = {10, 20, 30, 40, 50};
    Data binaryblob_value = Data(binaryblob);
    res = device->send_individual(interface_name, binaryblob_path, binaryblob_value, &now);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string datetime_path("/datetime_endpoint");
    Data datetime_value = Data(std::chrono::system_clock::now());
    res = device->send_individual(interface_name, datetime_path, datetime_value, &now);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string integerarray_path("/integerarray_endpoint");
    std::vector<int32_t> integerarray = {10, 20, 30, 40, 50};
    Data integerarray_value = Data(integerarray);
    res = device->send_individual(interface_name, integerarray_path, integerarray_value, &now);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string longintegerarray_path("/longintegerarray_endpoint");
    std::vector<int64_t> longintegerarray = {8589934592, 8589934593, 8589939592};
    Data longintegerarray_value = Data(longintegerarray);
    res = device->send_individual(interface_name, longintegerarray_path, longintegerarray_value,
                                  &now);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string doubleararray_path("/doublearray_endpoint");
    std::vector<double> doublearray = {0.0};
    Data doublearray_value = Data(doublearray);
    res = device->send_individual(interface_name, doubleararray_path, doublearray_value, &now);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string booleanarray_path("/booleanarray_endpoint");
    std::vector<bool> booleanarray = {true, false, true};
    Data booleanarray_value = Data(booleanarray);
    res = device->send_individual(interface_name, booleanarray_path, booleanarray_value, &now);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string stringarray_path("/stringarray_endpoint");
    std::vector<std::string> stringarray = {"Hello ", "world ", "from ", "C++"};
    Data stringarray_value = Data(stringarray);
    res = device->send_individual(interface_name, stringarray_path, stringarray_value, &now);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string binaryblobarray_path("/binaryblobarray_endpoint");
    std::vector<std::vector<uint8_t>> binaryblobarray = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    Data binaryblobarray_value = Data(binaryblobarray);
    res =
        device->send_individual(interface_name, binaryblobarray_path, binaryblobarray_value, &now);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string datetimearray_path("/datetimearray_endpoint");
    std::vector<std::chrono::system_clock::time_point> datetimearray = {
        std::chrono::system_clock::now(), std::chrono::system_clock::now()};
    Data datetimearray_value = Data(datetimearray);
    res = device->send_individual(interface_name, datetimearray_path, datetimearray_value, &now);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  {
    std::string interface_name("org.astarte-platform.cpp.examples.DeviceAggregate");
    std::string common_path("/sensor15");

    DatastreamObject data = {
        {"integer_endpoint", Data(43)},
        {"longinteger_endpoint", Data(8589934592)},
        {"double_endpoint", Data(43.5)},
        {"boolean_endpoint", Data(true)},
        {"string_endpoint", Data(std::string("Hello from cpp!"))},
        {"binaryblob_endpoint", Data(std::vector<uint8_t>{10, 20, 30, 40, 50})},
        {"datetime_endpoint", Data(std::chrono::system_clock::now())},
        {"integerarray_endpoint", Data(std::vector<int32_t>{10, 20, 30, 40, 50})},
        {"longintegerarray_endpoint",
         Data(std::vector<int64_t>{8589934592, 8589934593, 8589939592})},
        {"doublearray_endpoint", Data(std::vector<double>{0.0})},
        {"booleanarray_endpoint", Data(std::vector<bool>{true, false, true})},
        {"stringarray_endpoint",
         Data(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})},
        {"binaryblobarray_endpoint",
         Data(std::vector<std::vector<uint8_t>>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})},
        {"datetimearray_endpoint",
         Data(std::vector<std::chrono::system_clock::time_point>{
             std::chrono::system_clock::now(), std::chrono::system_clock::now()})}};
    auto res = device->send_object(interface_name, common_path, data, NULL);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  {
    std::string interface_name("org.astarte-platform.cpp.examples.DeviceProperty");

    std::string integer_path("/integer_endpoint");
    Data integer_value = Data(43);
    auto res = device->set_property(interface_name, integer_path, integer_value);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string longinteger_path("/longinteger_endpoint");
    Data longinteger_value = Data(8589934592);
    res = device->set_property(interface_name, longinteger_path, longinteger_value);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string double_path("/double_endpoint");
    Data double_value = Data(43.5);
    res = device->set_property(interface_name, double_path, double_value);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string boolean_path("/boolean_endpoint");
    Data boolean_value = Data(true);
    res = device->set_property(interface_name, boolean_path, boolean_value);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string string_path("/string_endpoint");
    Data string_value = Data(std::string("Hello from cpp!"));
    res = device->set_property(interface_name, string_path, string_value);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string binaryblob_path("/binaryblob_endpoint");
    std::vector<uint8_t> binaryblob = {10, 20, 30, 40, 50};
    Data binaryblob_value = Data(binaryblob);
    res = device->set_property(interface_name, binaryblob_path, binaryblob_value);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string datetime_path("/datetime_endpoint");
    Data datetime_value = Data(std::chrono::system_clock::now());
    res = device->set_property(interface_name, datetime_path, datetime_value);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string integerarray_path("/integerarray_endpoint");
    std::vector<int32_t> integerarray = {10, 20, 30, 40, 50};
    Data integerarray_value = Data(integerarray);
    res = device->set_property(interface_name, integerarray_path, integerarray_value);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string longintegerarray_path("/longintegerarray_endpoint");
    std::vector<int64_t> longintegerarray = {8589934592, 8589934593, 8589939592};
    Data longintegerarray_value = Data(longintegerarray);
    res = device->set_property(interface_name, longintegerarray_path, longintegerarray_value);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string doubleararray_path("/doublearray_endpoint");
    std::vector<double> doublearray = {0.0};
    Data doublearray_value = Data(doublearray);
    res = device->set_property(interface_name, doubleararray_path, doublearray_value);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string booleanarray_path("/booleanarray_endpoint");
    std::vector<bool> booleanarray = {true, false, true};
    Data booleanarray_value = Data(booleanarray);
    res = device->set_property(interface_name, booleanarray_path, booleanarray_value);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string stringarray_path("/stringarray_endpoint");
    std::vector<std::string> stringarray = {"Hello ", "world ", "from ", "C++"};
    Data stringarray_value = Data(stringarray);
    res = device->set_property(interface_name, stringarray_path, stringarray_value);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string binaryblobarray_path("/binaryblobarray_endpoint");
    std::vector<std::vector<uint8_t>> binaryblobarray = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    Data binaryblobarray_value = Data(binaryblobarray);
    res = device->set_property(interface_name, binaryblobarray_path, binaryblobarray_value);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string datetimearray_path("/datetimearray_endpoint");
    std::vector<std::chrono::system_clock::time_point> datetimearray = {
        std::chrono::system_clock::now(), std::chrono::system_clock::now()};
    Data datetimearray_value = Data(datetimearray);
    res = device->set_property(interface_name, datetimearray_path, datetimearray_value);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  {
    std::string interface_name("org.astarte-platform.cpp.examples.DeviceProperty");

    std::string integer_path("/integer_endpoint");
    auto res = device->unset_property(interface_name, integer_path);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string longinteger_path("/longinteger_endpoint");
    res = device->unset_property(interface_name, longinteger_path);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string double_path("/double_endpoint");
    res = device->unset_property(interface_name, double_path);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string boolean_path("/boolean_endpoint");
    res = res = device->unset_property(interface_name, boolean_path);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string string_path("/string_endpoint");
    res = device->unset_property(interface_name, string_path);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string binaryblob_path("/binaryblob_endpoint");
    res = device->unset_property(interface_name, binaryblob_path);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string datetime_path("/datetime_endpoint");
    res = device->unset_property(interface_name, datetime_path);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string integerarray_path("/integerarray_endpoint");
    res = device->unset_property(interface_name, integerarray_path);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string longintegerarray_path("/longintegerarray_endpoint");
    res = device->unset_property(interface_name, longintegerarray_path);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string doubleararray_path("/doublearray_endpoint");
    res = device->unset_property(interface_name, doubleararray_path);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string booleanarray_path("/booleanarray_endpoint");
    res = device->unset_property(interface_name, booleanarray_path);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string stringarray_path("/stringarray_endpoint");
    res = device->unset_property(interface_name, stringarray_path);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string binaryblobarray_path("/binaryblobarray_endpoint");
    res = device->unset_property(interface_name, binaryblobarray_path);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::string datetimearray_path("/datetimearray_endpoint");
    res = device->unset_property(interface_name, datetimearray_path);
    if (!res) {
      spdlog::critical(std::get<InvalidInputError>(res.error()).message());
      return EXIT_FAILURE;
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  // Wait to receive some messages
  std::this_thread::sleep_for(std::chrono::seconds(20));

  res = device->disconnect();
  if (!res) {
    spdlog::critical("Device disconnection failed");
    spdlog::critical(res.error());
    return EXIT_FAILURE;
  }

  std::this_thread::sleep_for(std::chrono::seconds(3));
  std::exit(EXIT_SUCCESS);
}
