// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cpr/cpr.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <format>
#include <memory>
#include <nlohmann/json.hpp>
#include <stop_token>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/device.hpp"
#include "astarte_device_sdk/errors.hpp"
#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "exceptions.hpp"
#include "shared_queue.hpp"
#include "utils.hpp"

using json = nlohmann::json;

using AstarteDeviceSdk::AstarteData;
using AstarteDeviceSdk::AstarteDatastreamIndividual;
using AstarteDeviceSdk::AstarteDatastreamObject;
using AstarteDeviceSdk::AstarteDevice;
using AstarteDeviceSdk::AstarteError;
using AstarteDeviceSdk::AstarteMessage;
using AstarteDeviceSdk::AstarteOwnership;
using AstarteDeviceSdk::AstartePropertyIndividual;
using AstarteDeviceSdk::AstarteStoredProperty;

/// @brief Convert a time_point to a UTC string
/// @param timestamp
/// @return string A string representing the UTC time in the format YYYY-MM-DDTHH:MM:SS.sssZ rturned
/// by Astarte
auto time_point_to_utc(const std::chrono::system_clock::time_point* timestamp) -> std::string {
  auto ms_since_epoch =
      std::chrono::duration_cast<std::chrono::milliseconds>(timestamp->time_since_epoch());
  auto s_since_epoch = std::chrono::duration_cast<std::chrono::seconds>(ms_since_epoch);
  long long milliseconds = (ms_since_epoch - s_since_epoch).count();

  std::time_t c_time = std::chrono::system_clock::to_time_t(*timestamp);

  // get the UTC time structure
  std::tm* tm_gmt = std::gmtime(&c_time);
  if (!tm_gmt) {
    return "Error: Failed to convert to UTC time.";
  }

  std::ostringstream oss;
  oss << std::put_time(tm_gmt, "%Y-%m-%dT%H:%M:%S")                // format YYYY-MM-DDTHH:MM:SS
      << "." << std::setw(3) << std::setfill('0') << milliseconds  // format .sss
      << "Z";                                                      // append the 'Z' for UTC

  return oss.str();
}

class TestAction {
 public:
  explicit TestAction() : should_fail_(false) {}
  explicit TestAction(bool should_fail) : should_fail_(should_fail) {}
  virtual auto execute_unchecked(const std::string& case_name) const
      -> AstarteDeviceSdk::astarte_tl::expected<void, AstarteError> = 0;
  virtual void execute(const std::string& case_name) {
    try {
      auto res = execute_unchecked(case_name);
      if (!res) {
        throw EndToEndAstarteDeviceException(astarte_fmt::format("{}", res.error()));
      }
    } catch (const std::exception& e) {
      if (should_fail_) {
        spdlog::debug("[{}] Catched exception: {}", case_name, e.what());
        spdlog::info("[{}] Test action failed as expected...", case_name);
      } else {
        throw e;
      }
    }
  }
  void attach_device(const std::shared_ptr<AstarteDevice>& device,
                     const std::shared_ptr<SharedQueue<AstarteMessage>>& rx_queue) {
    device_ = device;
    rx_queue_ = rx_queue;
  }

  void configure_curl(const std::string& astarte_base_url, const std::string& appengine_token,
                      const std::string& realm, const std::string& device_id) {
    astarte_base_url_ = astarte_base_url;
    appengine_token_ = appengine_token;
    realm_ = realm;
    device_id_ = device_id;
  }

 protected:
  std::shared_ptr<AstarteDevice> device_;
  std::shared_ptr<SharedQueue<AstarteMessage>> rx_queue_;
  std::string astarte_base_url_;
  std::string appengine_token_;
  std::string realm_;
  std::string device_id_;
  bool should_fail_;
};

class TestActionSleep : public TestAction {
 public:
  static std::shared_ptr<TestActionSleep> Create(std::chrono::seconds seconds) {
    return std::shared_ptr<TestActionSleep>(new TestActionSleep(seconds));
  }

  static std::shared_ptr<TestActionSleep> Create(std::chrono::milliseconds milliseconds) {
    return std::shared_ptr<TestActionSleep>(new TestActionSleep(milliseconds));
  }

  auto execute_unchecked(const std::string& case_name) const
      -> AstarteDeviceSdk::astarte_tl::expected<void, AstarteError> override {
    spdlog::info("[{}] Sleeping for {}ms...", case_name, duration_.count());
    std::this_thread::sleep_for(duration_);
    return {};
  }

 private:
  TestActionSleep(std::chrono::milliseconds milliseconds) : duration_(milliseconds) {}

  TestActionSleep(std::chrono::seconds seconds)
      : duration_(std::chrono::duration_cast<std::chrono::milliseconds>(seconds)) {}

  std::chrono::milliseconds duration_;
};

class TestActionAddInterfaceString : public TestAction {
 public:
  static std::shared_ptr<TestActionAddInterfaceString> Create(std::string_view interface_json,
                                                              bool should_fail = false) {
    return std::shared_ptr<TestActionAddInterfaceString>(
        new TestActionAddInterfaceString(interface_json, should_fail));
  }

  auto execute_unchecked(const std::string& case_name) const
      -> AstarteDeviceSdk::astarte_tl::expected<void, AstarteError> override {
    spdlog::info("[{}] Adding interface from string...", case_name);
    return device_->add_interface_from_str(interface_json_);
  }

 private:
  TestActionAddInterfaceString(std::string_view interface_json, bool should_fail)
      : TestAction(should_fail), interface_json_(interface_json) {}

  std::string interface_json_;
};

class TestActionAddInterfaceFile : public TestAction {
 public:
  static std::shared_ptr<TestActionAddInterfaceFile> Create(
      const std::filesystem::path& interface_file, bool should_fail = false) {
    return std::shared_ptr<TestActionAddInterfaceFile>(
        new TestActionAddInterfaceFile(interface_file, should_fail));
  }

  auto execute_unchecked(const std::string& case_name) const
      -> AstarteDeviceSdk::astarte_tl::expected<void, AstarteError> override {
    spdlog::info("[{}] Adding interface from file...", case_name);
    return device_->add_interface_from_file(interface_file_);
  }

 private:
  TestActionAddInterfaceFile(const std::filesystem::path& interface_file, bool should_fail)
      : TestAction(should_fail), interface_file_(interface_file) {}

  std::filesystem::path interface_file_;
};

class TestActionRemoveInterface : public TestAction {
 public:
  static std::shared_ptr<TestActionRemoveInterface> Create(std::string_view interface_name,
                                                           bool should_fail = false) {
    return std::shared_ptr<TestActionRemoveInterface>(
        new TestActionRemoveInterface(interface_name, should_fail));
  }

  auto execute_unchecked(const std::string& case_name) const
      -> AstarteDeviceSdk::astarte_tl::expected<void, AstarteError> override {
    spdlog::info("[{}] Removing interface...", case_name);
    return device_->remove_interface(interface_name_);
  }

 private:
  TestActionRemoveInterface(std::string_view interface_name, bool should_fail)
      : TestAction(should_fail), interface_name_(interface_name) {}

  std::string interface_name_;
};

class TestActionConnect : public TestAction {
 public:
  static std::shared_ptr<TestActionConnect> Create() {
    return std::shared_ptr<TestActionConnect>(new TestActionConnect());
  }

  auto execute_unchecked(const std::string& case_name) const
      -> AstarteDeviceSdk::astarte_tl::expected<void, AstarteError> override {
    spdlog::info("[{}] Connecting...", case_name);
    return device_->connect().transform([&]() {
      do {
        std::this_thread::sleep_for(std::chrono::seconds(1));
      } while (!device_->is_connected());
    });
  }

 private:
  TestActionConnect() = default;
};

class TestActionDisconnect : public TestAction {
 public:
  static std::shared_ptr<TestActionDisconnect> Create() {
    return std::shared_ptr<TestActionDisconnect>(new TestActionDisconnect());
  }

  auto execute_unchecked(const std::string& case_name) const
      -> AstarteDeviceSdk::astarte_tl::expected<void, AstarteError> override {
    spdlog::info("[{}] Disconnecting...", case_name);
    return device_->disconnect();
  }

 private:
  TestActionDisconnect() = default;
};

class TestActionCheckDeviceStatus : public TestAction {
 public:
  static std::shared_ptr<TestActionCheckDeviceStatus> Create(
      bool connected, const std::optional<std::vector<std::string>>& introspection = std::nullopt) {
    return std::shared_ptr<TestActionCheckDeviceStatus>(
        new TestActionCheckDeviceStatus(connected, introspection));
  }

  auto execute_unchecked(const std::string& case_name) const
      -> AstarteDeviceSdk::astarte_tl::expected<void, AstarteError> override {
    spdlog::info("[{}] Checking device status...", case_name);
    std::string request_url =
        astarte_base_url_ + "/appengine/v1/" + realm_ + "/devices/" + device_id_;
    spdlog::trace("HTTP GET: {}", request_url);

    cpr::Response get_response =
        cpr::Get(cpr::Url{request_url}, cpr::Header{{"Content-Type", "application/json"}},
                 cpr::Header{{"Authorization", "Bearer " + appengine_token_}});

    if (get_response.status_code != 200) {
      spdlog::error("HTTP GET failed, status code: {}", get_response.status_code);
      throw EndToEndHTTPException("Fetching device status through REST API failed.");
    }

    json response_json = json::parse(get_response.text)["data"];
    bool connected = response_json["connected"];
    if (connected != connected_) {
      spdlog::error("Expected: {}", connected ? "connected" : "disconnected");
      spdlog::error("Actual: {}", connected_ ? "connected" : "disconnected");
      throw EndToEndMismatchException("Mismatch in connection status.");
    }

    if (introspection_) {
      json introspection = response_json["introspection"];
      for (const std::string& interface : introspection_.value()) {
        spdlog::debug("Searching for interface {} in introspection.", interface);
        if (!introspection.contains(interface)) {
          spdlog::error("Device introspection is missing interface: {}", interface);
          throw EndToEndMismatchException("Device introspection is missing one interface.");
        }
      }
    }
    return {};
  }

 private:
  TestActionCheckDeviceStatus(bool connected,
                              const std::optional<std::vector<std::string>>& introspection)
      : connected_(connected), introspection_(introspection) {}

  bool connected_;
  std::optional<std::vector<std::string>> introspection_;
};

class TestActionTransmitMqttData : public TestAction {
 public:
  static std::shared_ptr<TestActionTransmitMqttData> Create(const AstarteMessage& message,
                                                            bool should_fail = false) {
    return std::shared_ptr<TestActionTransmitMqttData>(
        new TestActionTransmitMqttData(message, should_fail));
  }

  static std::shared_ptr<TestActionTransmitMqttData> Create(
      const AstarteMessage& message, const std::chrono::system_clock::time_point timestamp,
      bool should_fail = false) {
    return std::shared_ptr<TestActionTransmitMqttData>(
        new TestActionTransmitMqttData(message, timestamp, should_fail));
  }

  auto execute_unchecked(const std::string& case_name) const
      -> AstarteDeviceSdk::astarte_tl::expected<void, AstarteError> override {
    spdlog::info("[{}] Transmitting MQTT data...", case_name);
    if (message_.is_datastream()) {
      if (message_.is_individual()) {
        const auto& data(message_.into<AstarteDatastreamIndividual>());
        return device_->send_individual(message_.get_interface(), message_.get_path(),
                                        data.get_value(), timestamp_.get());
      }
      const auto& data(message_.into<AstarteDatastreamObject>());
      return device_->send_object(message_.get_interface(), message_.get_path(), data,
                                  timestamp_.get());
    }
    const auto& data(message_.into<AstartePropertyIndividual>());
    if (data.get_value().has_value()) {
      return device_->set_property(message_.get_interface(), message_.get_path(),
                                   data.get_value().value());
    }
    return device_->unset_property(message_.get_interface(), message_.get_path());
  }

 private:
  TestActionTransmitMqttData(const AstarteMessage& message, bool should_fail)
      : TestAction(should_fail), message_(message), timestamp_(nullptr) {}

  TestActionTransmitMqttData(const AstarteMessage& message,
                             const std::chrono::system_clock::time_point timestamp,
                             bool should_fail)
      : TestAction(should_fail),
        message_(message),
        timestamp_(std::make_unique<std::chrono::system_clock::time_point>(timestamp)) {}

  AstarteMessage message_;
  std::unique_ptr<std::chrono::system_clock::time_point> timestamp_;
};

class TestActionReadReceivedMqttData : public TestAction {
 public:
  static std::shared_ptr<TestActionReadReceivedMqttData> Create(const AstarteMessage& message) {
    return std::shared_ptr<TestActionReadReceivedMqttData>(
        new TestActionReadReceivedMqttData(message));
  }

  auto execute_unchecked(const std::string& case_name) const
      -> AstarteDeviceSdk::astarte_tl::expected<void, AstarteError> override {
    spdlog::info("[{}] Reading received MQTT data...", case_name);
    auto start = std::chrono::high_resolution_clock::now();
    while (rx_queue_->empty()) {
      std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
      if (elapsed.count() >= 1.0) {
        spdlog::error("Device could not receive the expected data from MQTT in 1 second");
        throw EndToEndTimeoutException("Device didn't receive the expected data from Astarte.");
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    AstarteMessage received = rx_queue_->pop().value();
    if (received != message_) {
      spdlog::error("Received message differs from expected.");
      spdlog::error("Received: {}", received);
      spdlog::error("Expected: {}", message_);
      throw EndToEndMismatchException("Expected and received data differ.");
    }
    return {};
  }

 private:
  TestActionReadReceivedMqttData(const AstarteMessage& message) : message_(message) {}

  AstarteMessage message_;
};

class TestActionTransmitRESTData : public TestAction {
 public:
  static std::shared_ptr<TestActionTransmitRESTData> Create(const AstarteMessage& message) {
    return std::shared_ptr<TestActionTransmitRESTData>(new TestActionTransmitRESTData(message));
  }

  auto execute_unchecked(const std::string& case_name) const
      -> AstarteDeviceSdk::astarte_tl::expected<void, AstarteError> override {
    spdlog::info("[{}] Transmitting REST data...", case_name);
    std::string request_url = astarte_base_url_ + "/appengine/v1/" + realm_ + "/devices/" +
                              device_id_ + "/interfaces/" + message_.get_interface() +
                              message_.get_path();

    spdlog::info("REQUEST: {}", request_url);

    if (message_.is_datastream()) {
      if (message_.is_individual()) {
        const auto& data(message_.into<AstarteDatastreamIndividual>());
        std::string payload = astarte_fmt::format(R"({{"data":{}}})", data);
        spdlog::trace("HTTP POST: {} {}", request_url, payload);
        cpr::Response post_response =
            cpr::Post(cpr::Url{request_url}, cpr::Body{payload},
                      cpr::Header{{"Content-Type", "application/json"}},
                      cpr::Header{{"Authorization", "Bearer " + appengine_token_}});
        if (post_response.status_code != 200) {
          spdlog::error("HTTP POST failed, status code: {}", post_response.status_code);
          throw EndToEndHTTPException("Transmission of data through REST API failed.");
        }
      } else {
        const auto& data(message_.into<AstarteDatastreamObject>());
        std::string payload = astarte_fmt::format(R"({{"data":{}}})", data);
        spdlog::trace("HTTP POST: {} {}", request_url, payload);
        cpr::Response post_response =
            cpr::Post(cpr::Url{request_url}, cpr::Body{payload},
                      cpr::Header{{"Content-Type", "application/json"}},
                      cpr::Header{{"Authorization", "Bearer " + appengine_token_}});
        if (post_response.status_code != 200) {
          spdlog::error("HTTP POST failed, status code: {}", post_response.status_code);
          throw EndToEndHTTPException("Transmission of data through REST API failed.");
        }
      }
    } else {  // Encode properties
      const auto data(message_.into<AstartePropertyIndividual>());

      if (data.get_value().has_value()) {
        spdlog::debug("sending server property");
        std::string payload = astarte_fmt::format(R"({{"data":{}}})", data);
        spdlog::trace("HTTP POST: {} {}", request_url, payload);
        cpr::Response post_response =
            cpr::Post(cpr::Url{request_url}, cpr::Body{payload},
                      cpr::Header{{"Content-Type", "application/json"}},
                      cpr::Header{{"Authorization", "Bearer " + appengine_token_}});

        if (post_response.status_code != 200) {
          spdlog::error("HTTP POST failed, status code: {}", post_response.status_code);
          throw EndToEndHTTPException("Transmission of data through REST API failed.");
        }
      } else {
        spdlog::debug("unset server property");
        cpr::Response delete_response = cpr::Delete(
            cpr::Url{request_url}, cpr::Body{}, cpr::Header{{"Content-Type", "application/json"}},
            cpr::Header{{"Authorization", "Bearer " + appengine_token_}});

        if (delete_response.status_code != 204) {
          spdlog::error("HTTP DELETE failed, status code: {}", delete_response.status_code);
          throw EndToEndHTTPException("Transmission of data through REST API failed.");
        }
      }
    }
    return {};
  }

 private:
  TestActionTransmitRESTData(const AstarteMessage& message) : message_(message) {}

  AstarteMessage message_;
};

class TestActionFetchRESTData : public TestAction {
 public:
  static std::shared_ptr<TestActionFetchRESTData> Create(const AstarteMessage& message) {
    return std::shared_ptr<TestActionFetchRESTData>(new TestActionFetchRESTData(message));
  }
  static std::shared_ptr<TestActionFetchRESTData> Create(
      const AstarteMessage& message, const std::chrono::system_clock::time_point timestamp) {
    return std::shared_ptr<TestActionFetchRESTData>(
        new TestActionFetchRESTData(message, timestamp));
  }

  auto execute_unchecked(const std::string& case_name) const
      -> AstarteDeviceSdk::astarte_tl::expected<void, AstarteError> override {
    spdlog::info("[{}] Fetching REST data...", case_name);

    std::string request_url = astarte_base_url_ + "/appengine/v1/" + realm_ + "/devices/" +
                              device_id_ + "/interfaces/" + message_.get_interface();
    cpr::Response get_response =
        cpr::Get(cpr::Url{request_url}, cpr::Header{{"Content-Type", "application/json"}},
                 cpr::Header{{"Authorization", "Bearer " + appengine_token_}});

    if (get_response.status_code != 200) {
      spdlog::error("HTTP GET failed, status code: {}", get_response.status_code);
      throw EndToEndHTTPException("Fetching data through REST API failed.");
    }

    json response_json = json::parse(get_response.text)["data"];

    if (message_.is_datastream()) {
      if (message_.is_individual()) {
        spdlog::debug("fetching datastream individual");
        check_datastream_individual(response_json);
      } else {
        spdlog::debug("fetching datastream aggregate");
        check_datastream_aggregate(response_json);
      }
    } else {
      const auto expected_data(message_.into<AstartePropertyIndividual>());

      if (expected_data.get_value().has_value()) {
        spdlog::debug("fetching property");
        check_individual_property(response_json, expected_data);
      } else {
        spdlog::debug("checking unset");
        check_property_unset(response_json);
      }
    }
    return {};
  }

 private:
  TestActionFetchRESTData(const AstarteMessage& message) : message_(message), timestamp_(nullptr) {}

  TestActionFetchRESTData(const AstarteMessage& message,
                          const std::chrono::system_clock::time_point timestamp)
      : message_(message),
        timestamp_(std::make_unique<std::chrono::system_clock::time_point>(timestamp)) {}

  void check_datastream_individual(json response_json) const {
    if (!response_json.contains(message_.get_path())) {
      spdlog::error("Missing entry '{}' in REST data.", message_.get_path());
      spdlog::info("Fetched data: {}", response_json.dump());
      throw EndToEndHTTPException("Fetching of data through REST API failed.");
    }

    const auto& expected_data(message_.into<AstarteDatastreamIndividual>());
    json expected_data_json = json::parse(astarte_fmt::format("{}", expected_data));
    json fetched_data = response_json[message_.get_path()]["value"];

    if (expected_data_json != fetched_data) {
      spdlog::error("Expected data: {}", expected_data_json.dump());
      spdlog::error("Fetched data: {}", fetched_data.dump());
      throw EndToEndMismatchException("Fetched REST API data differs from expected data.");
    }

    // FIXME: check timestamp correctness
    // Once issue [#938](https://github.com/astarte-platform/astarte/issues/938) of astarte is
    // solved, it should be possible to check the timestamp value (thus, decommenting the lines
    // below). In the meantime, we skip this check.
    // std::string expected_timestamp = time_point_to_utc(timestamp_.get());
    // std::string fetched_timestamp = response_json[message_.get_path()]["timestamp"];
    // if (expected_timestamp != fetched_timestamp) {
    //   spdlog::error("{}", response_json[message_.get_path()].dump());
    //   spdlog::error("Expected timestamp: {}", expected_timestamp);
    //   spdlog::error("Fetched timestamp: {}", fetched_timestamp);
    //   throw EndToEndMismatchException("Fetched REST API timestamp differs from expected
    //   data.");
    // }
  }

  void check_datastream_aggregate(json response_json) const {
    if (!response_json.contains(message_.get_path())) {
      spdlog::error("Missing entry '{}' in REST data.", message_.get_path());
      spdlog::info("Fetched data: {}", response_json.dump());
      throw EndToEndHTTPException("Fetching of data through REST API failed.");
    }

    const auto& expected_data(message_.into<AstarteDatastreamObject>());

    json expected_data_json = json::parse(astarte_fmt::format("{}", expected_data));

    // Every time the test is repeated, the object size increases by one, because
    // it retrieves every object data tha has been sent to that interface up to that point.
    // We only take the last object (i.e., the most recent)
    size_t last = response_json[message_.get_path()].size() - 1;
    json fetched_data = response_json[message_.get_path()][last];

    // FIXME: check timestamp correctness
    // Once issue [#938](https://github.com/astarte-platform/astarte/issues/938) of astarte is
    // solved, it should be possible to check the timestamp value (thus, decommenting the line
    // below). In the meantime, we skip this check.
    // expected_data_json.push_back({"timestamp", time_point_to_utc(timestamp_.get())});
    fetched_data.erase("timestamp");

    if (expected_data_json != fetched_data) {
      spdlog::error("Fetched data: {}", fetched_data.dump());
      spdlog::error("Expected data: {}", expected_data_json.dump());
      throw EndToEndMismatchException("Fetched REST API data differs from expected data.");
    }
  }

  void check_individual_property(json response_json,
                                 AstarteDeviceSdk::AstartePropertyIndividual expected_data) const {
    if (!response_json.contains(message_.get_path())) {
      spdlog::error("Missing entry '{}' in REST data.", message_.get_path());
      spdlog::info("Fetched data: {}", response_json.dump());
      throw EndToEndHTTPException("Fetching of data through REST API failed.");
    }

    json expected_data_json = json::parse(astarte_fmt::format("{}", expected_data));
    // unlike the device datastream, the fetched property does not contain the `value` field
    json fetched_data = response_json[message_.get_path()];

    if (expected_data_json != fetched_data) {
      spdlog::error("Expected data: {}", expected_data_json.dump());
      spdlog::error("Fetched data: {}", fetched_data.dump());
      throw EndToEndMismatchException("Fetched REST API data differs from expected data.");
    }
  }

  void check_property_unset(json response_json) const {
    // unlike the device datastream, the fetched property does not contain the `value` field
    json fetched_data = response_json[message_.get_path()];

    if (!fetched_data.is_null()) {
      spdlog::error("Fetched data: {}", fetched_data.dump());
      throw EndToEndMismatchException("Fetched REST API data differs from expected data.");
    }
  }

  AstarteMessage message_;
  std::unique_ptr<std::chrono::system_clock::time_point> timestamp_;
};

class TestActionGetDeviceProperty : public TestAction {
 public:
  static std::shared_ptr<TestActionGetDeviceProperty> Create(
      std::string_view interface_name, const std::string& path,
      const AstartePropertyIndividual& property) {
    return std::shared_ptr<TestActionGetDeviceProperty>(
        new TestActionGetDeviceProperty(interface_name, path, property));
  }

  auto execute_unchecked(const std::string& case_name) const
      -> AstarteDeviceSdk::astarte_tl::expected<void, AstarteError> override {
    spdlog::info("[{}] Getting property from device...", case_name);
    return device_->get_property(interface_name_, path_)
        .transform([&](const AstartePropertyIndividual& property) {
          if (property != property_) {
            spdlog::error("Fetched property differs from expected.");
            spdlog::error("Fetched: {}", property);
            spdlog::error("Expected: {}", property_);
            throw EndToEndMismatchException("Fetched and expected properties differ.");
          }
        });
  }

 private:
  TestActionGetDeviceProperty(std::string_view interface_name, const std::string& path,
                              const AstartePropertyIndividual& property)
      : interface_name_(interface_name), path_(path), property_(property) {}

  std::string interface_name_;
  std::string path_;
  AstartePropertyIndividual property_;
};

class TestActionGetDeviceProperties : public TestAction {
 public:
  static std::shared_ptr<TestActionGetDeviceProperties> Create(
      std::string_view interface_name, const std::list<AstarteStoredProperty>& properties) {
    return std::shared_ptr<TestActionGetDeviceProperties>(
        new TestActionGetDeviceProperties(interface_name, properties));
  }

  auto execute_unchecked(const std::string& case_name) const
      -> AstarteDeviceSdk::astarte_tl::expected<void, AstarteError> override {
    spdlog::info("[{}] Getting properties from device...", case_name);
    return device_->get_properties(interface_name_)
        .transform([&](const std::list<AstarteStoredProperty>& properties) {
          if (!compare_lists(properties, properties_)) {
            spdlog::error("Fetched properties differs from expected.");
            spdlog::error("Fetched: {}", format_list(properties));
            spdlog::error("Expected: {}", format_list(properties_));
            throw EndToEndMismatchException("Fetched and expected properties differ.");
          }
        });
  }

 private:
  TestActionGetDeviceProperties(std::string_view interface_name,
                                const std::list<AstarteStoredProperty>& properties)
      : interface_name_(interface_name), properties_(properties) {}

  std::string interface_name_;
  std::list<AstarteStoredProperty> properties_;
};

class TestActionGetAllFilteredProperties : public TestAction {
 public:
  static std::shared_ptr<TestActionGetAllFilteredProperties> Create(
      const std::optional<AstarteOwnership>& ownership,
      const std::list<AstarteStoredProperty>& properties) {
    return std::shared_ptr<TestActionGetAllFilteredProperties>(
        new TestActionGetAllFilteredProperties(ownership, properties));
  }

  auto execute_unchecked(const std::string& case_name) const
      -> AstarteDeviceSdk::astarte_tl::expected<void, AstarteError> override {
    spdlog::info("[{}] Getting all properties from device...", case_name);
    return device_->get_all_properties(ownership_)
        .transform([&](const std::list<AstarteStoredProperty>& properties) {
          if (!compare_lists(properties, properties_)) {
            spdlog::error("Fetched properties differs from expected.");
            spdlog::error("Fetched: {}", format_list(properties));
            spdlog::error("Expected: {}", format_list(properties_));
            throw EndToEndMismatchException("Fetched and expected properties differ.");
          }
        });
  }

 private:
  TestActionGetAllFilteredProperties(const std::optional<AstarteOwnership>& ownership,
                                     const std::list<AstarteStoredProperty>& properties)
      : ownership_(ownership), properties_(properties) {}

  std::optional<AstarteOwnership> ownership_;
  std::list<AstarteStoredProperty> properties_;
};
