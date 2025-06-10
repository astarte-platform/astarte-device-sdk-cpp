// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cpr/cpr.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <nlohmann/json.hpp>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/device.hpp"
#include "astarte_device_sdk/msg.hpp"
#include "astarte_device_sdk/object.hpp"
#include "exceptions.hpp"
#include "shared_queue.hpp"

using json = nlohmann::json;

using AstarteDeviceSdk::AstarteData;
using AstarteDeviceSdk::AstarteDevice;
using AstarteDeviceSdk::AstarteDatastreamIndividual;
using AstarteDeviceSdk::AstartePropertyIndividual;
using AstarteDeviceSdk::AstarteMessage;
using AstarteDeviceSdk::AstarteDatastreamObject;

class TestAction {
 public:
  virtual void execute(const std::string& case_name) const = 0;
  void attach_device(const std::shared_ptr<AstarteDevice>& device,
                     const std::shared_ptr<SharedQueue<AstarteMessage>>& rx_queue,
                     const std::shared_ptr<std::atomic_bool>& kill_reception) {
    device_ = device;
    rx_queue_ = rx_queue;
    kill_reception_ = kill_reception;
  }
  void configure_curl(const std::string& appengine_url, const std::string& appengine_token,
                      const std::string& realm, const std::string& device_id) {
    appengine_url_ = appengine_url;
    appengine_token_ = appengine_token;
    realm_ = realm;
    device_id_ = device_id;
  }

 protected:
  std::shared_ptr<AstarteDevice> device_;
  std::shared_ptr<SharedQueue<AstarteMessage>> rx_queue_;
  std::shared_ptr<std::atomic_bool> kill_reception_;
  std::string appengine_url_;
  std::string appengine_token_;
  std::string realm_;
  std::string device_id_;
};

class TestActionSleep : public TestAction {
 public:
  TestActionSleep(std::chrono::milliseconds milliseconds) : duration_(milliseconds) {}
  TestActionSleep(std::chrono::seconds seconds)
      : duration_(std::chrono::duration_cast<std::chrono::milliseconds>(seconds)) {}
  void execute(const std::string& case_name) const override {
    spdlog::info("[{}] Sleeping...", case_name);
    std::this_thread::sleep_for(duration_);
  }

 private:
  std::chrono::milliseconds duration_;
};

class TestActionConnect : public TestAction {
 public:
  void execute(const std::string& case_name) const override {
    spdlog::info("[{}] Connecting...", case_name);
    device_->connect();
  }
};

class TestActionDisconnect : public TestAction {
 public:
  void execute(const std::string& case_name) const override {
    spdlog::info("[{}] Disconnecting...", case_name);
    device_->disconnect();
    *kill_reception_ = true;
  }
};

class TestActionCheckDeviceStatus : public TestAction {
 public:
  TestActionCheckDeviceStatus(bool connected, const std::vector<std::string>& introspection)
      : connected_(connected), introspection_(introspection) {}
  void execute(const std::string& case_name) const override {
    spdlog::info("[{}] Checking device status...", case_name);
    std::string request_url = appengine_url_ + "/v1/" + realm_ + "/devices/" + device_id_;
    spdlog::debug("HTTP GET: {}", request_url);
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
    json introspection = response_json["introspection"];
    for (const std::string& interface : introspection_) {
      spdlog::debug("Searching for interface {} in introspection.", interface);
      if (!introspection.contains(interface)) {
        spdlog::error("Device introspection is missing interface: {}", interface);
        throw EndToEndMismatchException("Device introspection is missing one interface.");
      }
    }
  }

 private:
  bool connected_;
  std::vector<std::string> introspection_;
};

class TestActionTransmitMQTTData : public TestAction {
 public:
  TestActionTransmitMQTTData(const AstarteMessage& message) : message_(message) {}
  void execute(const std::string& case_name) const override {
    spdlog::info("[{}] Transmitting MQTT data...", case_name);
    const std::variant<AstarteDatastreamIndividual, AstarteDatastreamObject,
                       AstartePropertyIndividual>& var_data(message_.into());
    if (std::holds_alternative<AstarteDatastreamIndividual>(var_data)) {
      const AstarteDatastreamIndividual& data = std::get<AstarteDatastreamIndividual>(var_data);
      device_->send_individual(message_.get_interface(), message_.get_path(), data.get_value(),
                               nullptr);
    }
    if (std::holds_alternative<AstarteDatastreamObject>(var_data)) {
      const AstarteDatastreamObject& data = std::get<AstarteDatastreamObject>(var_data);
      device_->send_object(message_.get_interface(), message_.get_path(), data, nullptr);
    }
    // TODO: Handle properties
  }

 private:
  AstarteMessage message_;
};

class TestActionReadReceivedMQTTData : public TestAction {
 public:
  TestActionReadReceivedMQTTData(const AstarteMessage& message) : message_(message) {}
  void execute(const std::string& case_name) const override {
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
      spdlog::error("Received: {}", received.format());
      spdlog::error("Expected: {}", message_.format());
      throw EndToEndMismatchException("Expected and received data differ.");
    }
  }

 private:
  AstarteMessage message_;
};

class TestActionTransmitRESTData : public TestAction {
 public:
  TestActionTransmitRESTData(const AstarteMessage& message) : message_(message) {}
  void execute(const std::string& case_name) const override {
    spdlog::info("[{}] Transmitting REST data...", case_name);
    std::string request_url = appengine_url_ + "/v1/" + realm_ + "/devices/" + device_id_ +
                              "/interfaces/" + message_.get_interface() + message_.get_path();
    const std::variant<AstarteDatastreamIndividual, AstarteDatastreamObject,
                       AstartePropertyIndividual>& msg_content(message_.into());
    if (std::holds_alternative<AstarteDatastreamIndividual>(msg_content)) {
      const AstarteDatastreamIndividual& indiv = std::get<AstarteDatastreamIndividual>(msg_content);
      std::string payload = "{\"data\":" + indiv.format() + "}";
      spdlog::debug("HTTP POST: {} {}", request_url, payload);
      cpr::Response post_response =
          cpr::Post(cpr::Url{request_url}, cpr::Body{payload},
                    cpr::Header{{"Content-Type", "application/json"}},
                    cpr::Header{{"Authorization", "Bearer " + appengine_token_}});
      if (post_response.status_code != 200) {
        spdlog::error("HTTP POST failed, status code: {}", post_response.status_code);
        throw EndToEndHTTPException("Transmission of data through REST API failed.");
      }
    }
    if (std::holds_alternative<AstarteDatastreamObject>(msg_content)) {
      const AstarteDatastreamObject& obj = std::get<AstarteDatastreamObject>(msg_content);
      // TODO: Encode an object
    }
    // TODO: Encode properties
  }

 private:
  AstarteMessage message_;
};

class TestActionFetchRESTData : public TestAction {
 public:
  TestActionFetchRESTData(const AstarteMessage& message) : message_(message) {}
  void execute(const std::string& case_name) const override {
    spdlog::info("[{}] Fetching REST data...", case_name);
    std::string request_url = appengine_url_ + "/v1/" + realm_ + "/devices/" + device_id_ +
                              "/interfaces/" + message_.get_interface();
    spdlog::debug("HTTP GET: {}", request_url);
    cpr::Response get_response =
        cpr::Get(cpr::Url{request_url}, cpr::Header{{"Content-Type", "application/json"}},
                 cpr::Header{{"Authorization", "Bearer " + appengine_token_}});
    if (get_response.status_code != 200) {
      spdlog::error("HTTP GET failed, status code: {}", get_response.status_code);
      throw EndToEndHTTPException("Fetching data through REST API failed.");
    }
    json response_json = json::parse(get_response.text)["data"];
    if (!response_json.contains(message_.get_path())) {
      spdlog::error("Missing entry '{}' in REST data.", message_.get_path());
      spdlog::info("Fetched data: {}", response_json.dump());
      throw EndToEndHTTPException("Fetching of data through REST API failed.");
    }
    json fetched_data = response_json[message_.get_path()]["value"];
    const std::variant<AstarteDatastreamIndividual, AstarteDatastreamObject,
                       AstartePropertyIndividual>& msg_content(message_.into());
    // NOTE: This if/else might be simplified by just formatting the message content directly
    if (std::holds_alternative<AstarteDatastreamIndividual>(msg_content)) {
      const AstarteDatastreamIndividual& expected_data =
          std::get<AstarteDatastreamIndividual>(msg_content);
      json expected_data_json = json::parse(expected_data.format());
      if (expected_data_json != fetched_data) {
        spdlog::error("Fetched data: {}", fetched_data.dump());
        spdlog::error("Expected data: {}", expected_data_json.dump());
        throw EndToEndMismatchException("Fetched REST API data differs from expected data.");
      }
    }
    if (std::holds_alternative<AstarteDatastreamObject>(msg_content)) {
      const AstarteDatastreamObject& obj = std::get<AstarteDatastreamObject>(msg_content);
      // TODO: Parse an expected Object
    }
    // TODO: Parse properties
  }

 private:
  AstarteMessage message_;
};
