// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include <spdlog/spdlog.h>

#include <chrono>
#include <format>
#include <optional>
#include <string>
#include <string_view>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/errors.hpp"
#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/mqtt/config.hpp"
#include "astarte_device_sdk/mqtt/device_mqtt.hpp"
#include "astarte_device_sdk/mqtt/pairing.hpp"
#include "config.hpp"
#include "store.hpp"

int main() {
  spdlog::set_level(spdlog::level::debug);

  auto cfg = Config("samples/mqtt/native/config.toml");

  auto api_res =
      astarte::device::mqtt::PairingApi::create(cfg.realm, cfg.device_id, cfg.astarte_base_url);
  if (!api_res) {
    spdlog::error("Pairing API creation failure. Err: \n{}", api_res.error());
    return 0;
  }
  auto api = api_res.value();

  auto db = init_db("samples/mqtt/native/example.db");

  // flag used to state if the credential secret has been stored in db
  bool in_db = false;
  // check if the device is already registered
  auto cred_opt = credential_secret_from_db(db, cfg.device_id);
  if (cred_opt) {
    spdlog::debug("device credential secret found in db");
    in_db = true;
  }

  if (cfg.features.registration_enabled() && !in_db) {
    assert(cfg.pairing_token);

    auto secret_res = api.register_device(cfg.pairing_token.value());
    if (!secret_res) {
      spdlog::error("failed to register the device: {}", secret_res.error());
      return EXIT_FAILURE;
    }
    auto secret = secret_res.value();
    spdlog::trace("credential secret: {}", secret);

    store_cred_secret(db, cfg.device_id, secret);
    in_db = true;
    cred_opt = std::optional(secret);
  }

  if (cfg.features.connection_enabled()) {
    if (!in_db && !cfg.pairing_token && !cfg.credential_secret) {
      spdlog::error("neither pairing token nor credential secret has been set");
      return EXIT_FAILURE;
    }

    auto mqtt_cfg = [&] {
      if (in_db) {
        return astarte::device::mqtt::MqttConfig::with_credential_secret(
            cfg.realm, cfg.device_id, *cred_opt,
            astarte_fmt::format("{}/pairing", cfg.astarte_base_url), cfg.store_dir);
      } else {
        assert(cfg.credential_secret);

        // first, store the cred secret in the db for future usage
        store_cred_secret(db, cfg.device_id, cfg.credential_secret.value());

        return astarte::device::mqtt::MqttConfig::with_credential_secret(
            cfg.realm, cfg.device_id, cfg.credential_secret.value(),
            astarte_fmt::format("{}/pairing", cfg.astarte_base_url), cfg.store_dir);
      }
    }();

    // here you can modify the mqtt_cfg options, such as the keepalive interval, the connection
    // timeout period, etc.

    auto device_res = astarte::device::mqtt::AstarteDeviceMqtt::create(std::move(mqtt_cfg));
    if (!device_res) {
      spdlog::error("device creation error: {}", device_res.error());
      return EXIT_FAILURE;
    }
    auto device = *std::move(device_res);

    const std::vector<std::string_view> interfaces = {
        "samples/mqtt/native/interfaces/org.astarte-platform.cpp.examples.DeviceAggregate.json",
        "samples/mqtt/native/interfaces/org.astarte-platform.cpp.examples.DeviceDatastream.json",
        "samples/mqtt/native/interfaces/org.astarte-platform.cpp.examples.DeviceProperty.json",
        "samples/mqtt/native/interfaces/org.astarte-platform.cpp.examples.ServerAggregate.json",
        "samples/mqtt/native/interfaces/org.astarte-platform.cpp.examples.ServerDatastream.json",
        "samples/mqtt/native/interfaces/org.astarte-platform.cpp.examples.ServerProperty.json"};

    for (const auto interface : interfaces) {
      auto add_interface_res = device.add_interface_from_file(interface);
      if (!add_interface_res) {
        spdlog::error("Failed to add interface {}. Error: {}", interface,
                      add_interface_res.error());
        return EXIT_FAILURE;
      }
    }

    auto conn_res = device.connect();
    if (!conn_res) {
      spdlog::error("connection error: {}", conn_res.error());
      return EXIT_FAILURE;
    }

    sleep(5);

    // sending device datastream individual
    {
      auto interface_name("org.astarte-platform.cpp.examples.DeviceDatastream");
      auto now = std::chrono::system_clock::time_point(std::chrono::system_clock::now());

      auto integer_path("/integer_endpoint");
      auto integer_value = astarte::device::Data(43);
      auto send_res = device.send_individual(interface_name, integer_path, integer_value, &now);
      if (!send_res) {
        spdlog::error("send datastream individual error: {}", send_res.error());
        return EXIT_FAILURE;
      }

      auto longinteger_path("/longinteger_endpoint");
      auto longinteger_value = astarte::device::Data(8589934592);
      send_res = device.send_individual(interface_name, longinteger_path, longinteger_value, &now);
      if (!send_res) {
        spdlog::error("send datastream individual error: {}", send_res.error());
        return EXIT_FAILURE;
      }

      auto double_path("/double_endpoint");
      auto double_value = astarte::device::Data(43.5);
      send_res = device.send_individual(interface_name, double_path, double_value, &now);
      if (!send_res) {
        spdlog::error("send datastream individual error: {}", send_res.error());
        return EXIT_FAILURE;
      }

      auto boolean_path("/boolean_endpoint");
      auto boolean_value = astarte::device::Data(true);
      send_res = device.send_individual(interface_name, boolean_path, boolean_value, &now);
      if (!send_res) {
        spdlog::error("send datastream individual error: {}", send_res.error());
        return EXIT_FAILURE;
      }

      auto string_path("/string_endpoint");
      auto hello_string = std::string("Hello from cpp!");
      auto string_value = astarte::device::Data(hello_string);
      send_res = device.send_individual(interface_name, string_path, string_value, &now);
      if (!send_res) {
        spdlog::error("send datastream individual error: {}", send_res.error());
        return EXIT_FAILURE;
      }

      auto binaryblob_path("/binaryblob_endpoint");
      std::vector<uint8_t> binaryblob = {10, 20, 30, 40, 50};
      auto binaryblob_value = astarte::device::Data(binaryblob);
      send_res = device.send_individual(interface_name, binaryblob_path, binaryblob_value, &now);
      if (!send_res) {
        spdlog::error("send datastream individual error: {}", send_res.error());
        return EXIT_FAILURE;
      }

      auto datetime_path("/datetime_endpoint");
      auto datetime_value = astarte::device::Data(std::chrono::system_clock::now());
      send_res = device.send_individual(interface_name, datetime_path, datetime_value, &now);
      if (!send_res) {
        spdlog::error("send datastream individual error: {}", send_res.error());
        return EXIT_FAILURE;
      }

      auto integerarray_path("/integerarray_endpoint");
      std::vector<int32_t> integerarray = {10, 20, 30, 40, 50};
      auto integerarray_value = astarte::device::Data(integerarray);
      send_res =
          device.send_individual(interface_name, integerarray_path, integerarray_value, &now);
      if (!send_res) {
        spdlog::error("send datastream individual error: {}", send_res.error());
        return EXIT_FAILURE;
      }

      auto longintegerarray_path("/longintegerarray_endpoint");
      std::vector<int64_t> longintegerarray = {8589934592, 8589934593, 8589939592};
      auto longintegerarray_value = astarte::device::Data(longintegerarray);
      send_res = device.send_individual(interface_name, longintegerarray_path,
                                        longintegerarray_value, &now);
      if (!send_res) {
        spdlog::error("send datastream individual error: {}", send_res.error());
        return EXIT_FAILURE;
      }

      auto doubleararray_path("/doublearray_endpoint");
      std::vector<double> doublearray = {1.2, 3.4};
      auto doublearray_value = astarte::device::Data(doublearray);
      send_res =
          device.send_individual(interface_name, doubleararray_path, doublearray_value, &now);
      if (!send_res) {
        spdlog::error("send datastream individual error: {}", send_res.error());
        return EXIT_FAILURE;
      }

      auto booleanarray_path("/booleanarray_endpoint");
      std::vector<bool> booleanarray = {true, false, true};
      auto booleanarray_value = astarte::device::Data(booleanarray);
      send_res =
          device.send_individual(interface_name, booleanarray_path, booleanarray_value, &now);
      if (!send_res) {
        spdlog::error("send datastream individual error: {}", send_res.error());
        return EXIT_FAILURE;
      }

      auto stringarray_path("/stringarray_endpoint");
      std::vector<std::string> stringarray = {"Hello ", "world ", "from ", "C++"};
      auto stringarray_value = astarte::device::Data(stringarray);
      send_res = device.send_individual(interface_name, stringarray_path, stringarray_value, &now);
      if (!send_res) {
        spdlog::error("send datastream individual error: {}", send_res.error());
        return EXIT_FAILURE;
      }

      auto binaryblobarray_path("/binaryblobarray_endpoint");
      std::vector<std::vector<uint8_t>> binaryblobarray = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
      auto binaryblobarray_value = astarte::device::Data(binaryblobarray);
      send_res =
          device.send_individual(interface_name, binaryblobarray_path, binaryblobarray_value, &now);
      if (!send_res) {
        spdlog::error("send datastream individual error: {}", send_res.error());
        return EXIT_FAILURE;
      }

      auto datetimearray_path("/datetimearray_endpoint");
      std::vector<std::chrono::system_clock::time_point> datetimearray = {
          std::chrono::system_clock::now(), std::chrono::system_clock::now()};
      auto datetimearray_value = astarte::device::Data(datetimearray);
      send_res =
          device.send_individual(interface_name, datetimearray_path, datetimearray_value, &now);
      if (!send_res) {
        spdlog::error("send datastream individual error: {}", send_res.error());
        return EXIT_FAILURE;
      }

      sleep(3);
    }

    {
      auto interface_name = std::string("org.astarte-platform.cpp.examples.DeviceAggregate");
      auto common_path = std::string("/sensor15");

      astarte::device::DatastreamObject data = {
          {"integer_endpoint", astarte::device::Data(43)},
          {"longinteger_endpoint", astarte::device::Data(8589934592)},
          {"double_endpoint", astarte::device::Data(43.5)},
          {"boolean_endpoint", astarte::device::Data(true)},
          {"string_endpoint", astarte::device::Data(std::string("Hello from cpp!"))},
          {"binaryblob_endpoint", astarte::device::Data(std::vector<uint8_t>{10, 20, 30, 40, 50})},
          {"datetime_endpoint", astarte::device::Data(std::chrono::system_clock::now())},
          {"integerarray_endpoint",
           astarte::device::Data(std::vector<int32_t>{10, 20, 30, 40, 50})},
          {"longintegerarray_endpoint",
           astarte::device::Data(std::vector<int64_t>{8589934592, 8589934593, 8589939592})},
          {"doublearray_endpoint", astarte::device::Data(std::vector<double>{1.2, 3.4})},
          {"booleanarray_endpoint", astarte::device::Data(std::vector<bool>{true, false, true})},
          {"stringarray_endpoint",
           astarte::device::Data(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})},
          {"binaryblobarray_endpoint", astarte::device::Data(std::vector<std::vector<uint8_t>>{
                                           {1, 2, 3}, {4, 5, 6}, {7, 8, 9}})},
          {"datetimearray_endpoint",
           astarte::device::Data(std::vector<std::chrono::system_clock::time_point>{
               std::chrono::system_clock::now(), std::chrono::system_clock::now()})}};

      auto send_res = device.send_object(interface_name, common_path, data, nullptr);
      if (!send_res) {
        spdlog::error("send datastream object error: {}", send_res.error());
        return EXIT_FAILURE;
      }

      sleep(3);
    }

    auto disconn_res = device.disconnect();
    if (!disconn_res) {
      spdlog::error("disconnection error: {}", disconn_res.error());
      return EXIT_FAILURE;
    }

    sleep(5);

    conn_res = device.connect();
    if (!conn_res) {
      spdlog::error("connection error: {}", conn_res.error());
      return EXIT_FAILURE;
    }

    sleep(5);

    disconn_res = device.disconnect();
    if (!disconn_res) {
      spdlog::error("disconnection error: {}", disconn_res.error());
      return EXIT_FAILURE;
    }
  }

  return 0;
}
