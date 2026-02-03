// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cpr/cpr.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <filesystem>
#include <format>
#include <functional>
#include <memory>
#include <nlohmann/json.hpp>
#include <stop_token>
#include <string>
#include <thread>
#include <variant>

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

using astarte::device::AstarteDatastreamObject;
using astarte::device::AstarteMessage;
using astarte::device::AstarteOwnership;
using astarte::device::AstartePropertyIndividual;
using astarte::device::AstarteStoredProperty;
using astarte::device::Data;
using astarte::device::DatastreamIndividual;
using astarte::device::Device;
using astarte::device::Error;

// -----------------------------------------------------------------------------
// Context & Types
// -----------------------------------------------------------------------------

struct TestHttpConfig {
  std::string astarte_base_url;
  std::string appengine_token;
  std::string realm;
};

struct TestCaseContext {
  std::string device_id;
  std::shared_ptr<Device> device;
  std::shared_ptr<SharedQueue<AstarteMessage>> rx_queue;
  TestHttpConfig http;
};

using Action = std::function<void(const TestCaseContext&)>;

// -----------------------------------------------------------------------------
// Implementation details
// -----------------------------------------------------------------------------

namespace actions_helpers {

// Helper to construct the full URL for REST actions
inline std::string build_url(const TestCaseContext& ctx, const std::string& path_suffix = "") {
  return ctx.http.astarte_base_url + "/appengine/v1/" + ctx.http.realm + "/devices/" +
         ctx.device_id + path_suffix;
}

// Logic for validating Datastream Individual responses
inline void check_datastream_individual(const json& response_json, const AstarteMessage& msg) {
  if (!response_json.contains(msg.get_path())) {
    spdlog::error("Missing entry '{}' in REST data.", msg.get_path());
    spdlog::info("Fetched data: {}", response_json.dump());
    throw EndToEndHTTPException("Fetching of data through REST API failed.");
  }

  const auto& expected_data(msg.into<DatastreamIndividual>());
  json expected_data_json = json::parse(astarte_fmt::format("{}", expected_data));
  json fetched_data = response_json[msg.get_path()]["value"];

  if (expected_data_json != fetched_data) {
    spdlog::error("Expected data: {}", expected_data_json.dump());
    spdlog::error("Fetched data: {}", fetched_data.dump());
    throw EndToEndMismatchException("Fetched REST API data differs from expected data.");
  }

  // TODO(sorru94): check timestamp correctness
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

// Logic for validating Datastream Object responses
inline void check_datastream_aggregate(const json& response_json, const AstarteMessage& msg) {
  if (!response_json.contains(msg.get_path())) {
    spdlog::error("Missing entry '{}' in REST data.", msg.get_path());
    spdlog::info("Fetched data: {}", response_json.dump());
    throw EndToEndHTTPException("Fetching of data through REST API failed.");
  }

  const auto& expected_data(msg.into<AstarteDatastreamObject>());
  json expected_data_json = json::parse(astarte_fmt::format("{}", expected_data));

  // Retrieve the last object (most recent)
  size_t last = response_json[msg.get_path()].size() - 1;
  json fetched_data = response_json[msg.get_path()][last];

  // TODO(sorru94): check timestamp correctness
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

// Logic for validating Property responses
inline void check_individual_property(const json& response_json, const AstarteMessage& msg,
                                      const AstartePropertyIndividual& expected_data) {
  if (!response_json.contains(msg.get_path())) {
    spdlog::error("Missing entry '{}' in REST data.", msg.get_path());
    spdlog::info("Fetched data: {}", response_json.dump());
    throw EndToEndHTTPException("Fetching of data through REST API failed.");
  }

  json expected_data_json = json::parse(astarte_fmt::format("{}", expected_data));
  json fetched_data = response_json[msg.get_path()];

  if (expected_data_json != fetched_data) {
    spdlog::error("Expected data: {}", expected_data_json.dump());
    spdlog::error("Fetched data: {}", fetched_data.dump());
    throw EndToEndMismatchException("Fetched REST API data differs from expected data.");
  }
}

inline void check_property_unset(const json& response_json, const AstarteMessage& msg) {
  if (response_json.contains(msg.get_path())) {
    spdlog::error("Found entry '{}' in REST data.", msg.get_path());
    throw EndToEndMismatchException("Fetched REST API data differs from expected data.");
  }
}

}  // namespace actions_helpers

// -----------------------------------------------------------------------------
// actions
// -----------------------------------------------------------------------------

namespace actions {

// -----------------------------------------------------------------------------
// Meta actions
// -----------------------------------------------------------------------------

// Wrapper to inverse success criteria (Expect Failure)
inline Action ExpectFailure(Action action) {
  return [action](const TestCaseContext& ctx) {
    try {
      action(ctx);
    } catch (const std::exception& e) {
      spdlog::debug("Caught expected exception: {}", e.what());
      spdlog::info("Action failed as expected.");
      return;
    }
    throw EndToEndException("Action succeeded but was expected to fail.");
  };
}

// -----------------------------------------------------------------------------
// Utility actions
// -----------------------------------------------------------------------------

inline Action Sleep(std::chrono::milliseconds duration) {
  return [duration](const TestCaseContext&) {
    spdlog::info("Sleeping for {}ms...", duration.count());
    std::this_thread::sleep_for(duration);
  };
}

inline Action Sleep(std::chrono::seconds duration) {
  return Sleep(std::chrono::duration_cast<std::chrono::milliseconds>(duration));
}

// -----------------------------------------------------------------------------
// Connection actions
// -----------------------------------------------------------------------------

inline Action Connect() {
  return [](const TestCaseContext& ctx) {
    spdlog::info("Connecting...");
    auto res = ctx.device->connect();
    if (!res) {
      throw EndToEndAstarteDeviceException(astarte_fmt::format("{}", res.error()));
    }
    while (!ctx.device->is_connected()) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  };
}

inline Action Disconnect() {
  return [](const TestCaseContext& ctx) {
    spdlog::info("Disconnecting...");
    auto res = ctx.device->disconnect();
    if (!res) {
      throw EndToEndAstarteDeviceException(astarte_fmt::format("{}", res.error()));
    }
  };
}

// -----------------------------------------------------------------------------
// Interface Management
// -----------------------------------------------------------------------------

inline Action AddInterfaceString(std::string interface_json) {
  return [json_str = std::move(interface_json)](const TestCaseContext& ctx) {
    spdlog::info("Adding interface from string...");
    auto res = ctx.device->add_interface_from_str(json_str);
    if (!res) {
      throw EndToEndAstarteDeviceException(astarte_fmt::format("{}", res.error()));
    }
  };
}

inline Action AddInterfaceFile(std::filesystem::path interface_file) {
  return [file = std::move(interface_file)](const TestCaseContext& ctx) {
    spdlog::info("Adding interface from file...");
    auto res = ctx.device->add_interface_from_file(file);
    if (!res) {
      throw EndToEndAstarteDeviceException(astarte_fmt::format("{}", res.error()));
    }
  };
}

inline Action RemoveInterface(std::string interface_name) {
  return [name = std::move(interface_name)](const TestCaseContext& ctx) {
    spdlog::info("Removing interface...");
    auto res = ctx.device->remove_interface(name);
    if (!res) {
      throw EndToEndAstarteDeviceException(astarte_fmt::format("{}", res.error()));
    }
  };
}

// -----------------------------------------------------------------------------
// Device SDK Data Operations (MQTT/GRPC)
// -----------------------------------------------------------------------------

inline Action TransmitDeviceData(
    AstarteMessage message,
    std::optional<std::chrono::system_clock::time_point> timestamp = std::nullopt) {
  return [msg = std::move(message), ts = timestamp](const TestCaseContext& ctx) {
    spdlog::info("Transmitting MQTT data...");
    astarte::device::astarte_tl::expected<void, Error> res;

    // Use current time if timestamp not provided, or specific time if provided
    auto ts_ptr = ts.has_value() ? &ts.value() : nullptr;

    if (msg.is_datastream()) {
      if (msg.is_individual()) {
        const auto& data(msg.into<DatastreamIndividual>());
        res = ctx.device->send_individual(msg.get_interface(), msg.get_path(), data.get_value(),
                                          ts_ptr);
      } else {
        const auto& data(msg.into<AstarteDatastreamObject>());
        res = ctx.device->send_object(msg.get_interface(), msg.get_path(), data, ts_ptr);
      }
    } else {
      const auto& data(msg.into<AstartePropertyIndividual>());
      if (data.get_value().has_value()) {
        res =
            ctx.device->set_property(msg.get_interface(), msg.get_path(), data.get_value().value());
      } else {
        res = ctx.device->unset_property(msg.get_interface(), msg.get_path());
      }
    }

    if (!res) throw EndToEndAstarteDeviceException(astarte_fmt::format("{}", res.error()));
  };
}

inline Action ReadReceivedDeviceData(AstarteMessage expected_message) {
  return [expected = std::move(expected_message)](const TestCaseContext& ctx) {
    spdlog::info("Reading received MQTT data...");
    auto start = std::chrono::high_resolution_clock::now();

    while (ctx.rx_queue->empty()) {
      std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
      if (elapsed.count() >= 1.0) {
        spdlog::error("Device could not receive the expected data from MQTT in 1 second");
        throw EndToEndTimeoutException("Device didn't receive the expected data from Astarte.");
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    AstarteMessage received = ctx.rx_queue->pop().value();
    if (received != expected) {
      spdlog::error("Received message differs from expected.");
      spdlog::error("Received: {}", received);
      spdlog::error("Expected: {}", expected);
      throw EndToEndMismatchException("Expected and received data differ.");
    }
  };
}

inline Action GetDeviceProperty(std::string interface_name, std::string path,
                                AstartePropertyIndividual expected) {
  return [iface = std::move(interface_name), pth = std::move(path),
          expected_prop = std::move(expected)](const TestCaseContext& ctx) {
    spdlog::info("Getting property from device...");
    auto res = ctx.device->get_property(iface, pth);
    if (!res) {
      throw EndToEndAstarteDeviceException(astarte_fmt::format("{}", res.error()));
    }
    if (res.value() != expected_prop) {
      spdlog::error("Fetched property differs from expected.");
      spdlog::error("Fetched: {}", res.value());
      spdlog::error("Expected: {}", expected_prop);
      throw EndToEndMismatchException("Fetched and expected properties differ.");
    }
  };
}

inline Action GetDeviceProperties(std::string interface_name,
                                  std::list<AstarteStoredProperty> expected_list) {
  return [iface = std::move(interface_name),
          expected = std::move(expected_list)](const TestCaseContext& ctx) {
    spdlog::info("Getting properties from device...");
    auto res = ctx.device->get_properties(iface);
    if (!res) {
      throw EndToEndAstarteDeviceException(astarte_fmt::format("{}", res.error()));
    }
    if (!compare_lists(res.value(), expected)) {
      spdlog::error("Fetched properties differs from expected.");
      spdlog::error("Fetched: {}", format_list(res.value()));
      spdlog::error("Expected: {}", format_list(expected));
      throw EndToEndMismatchException("Fetched and expected properties differ.");
    }
  };
}

inline Action GetAllFilteredProperties(std::optional<AstarteOwnership> ownership,
                                       std::list<AstarteStoredProperty> expected_list) {
  return [own = ownership, expected = std::move(expected_list)](const TestCaseContext& ctx) {
    spdlog::info("Getting all properties from device...");
    auto res = ctx.device->get_all_properties(own);
    if (!res) {
      throw EndToEndAstarteDeviceException(astarte_fmt::format("{}", res.error()));
    }
    if (!compare_lists(res.value(), expected)) {
      spdlog::error("Fetched properties differs from expected.");
      spdlog::error("Fetched: {}", format_list(res.value()));
      spdlog::error("Expected: {}", format_list(expected));
      throw EndToEndMismatchException("Fetched and expected properties differ.");
    }
  };
}

// -----------------------------------------------------------------------------
// REST API actions (HTTP)
// -----------------------------------------------------------------------------

inline Action CheckDeviceStatus(
    bool expected_connection_status,
    std::optional<std::vector<std::string>> expected_introspection = std::nullopt) {
  return [expected_conn = expected_connection_status,
          expected_intro = std::move(expected_introspection)](const TestCaseContext& ctx) {
    spdlog::info("Checking device status...");
    std::string url = actions_helpers::build_url(ctx);
    spdlog::trace("HTTP GET: {}", url);

    auto response = cpr::Get(cpr::Url{url}, cpr::Header{{"Content-Type", "application/json"}},
                             cpr::Header{{"Authorization", "Bearer " + ctx.http.appengine_token}});

    if (response.status_code != 200) {
      spdlog::error("HTTP GET failed, status code: {}", response.status_code);
      throw EndToEndHTTPException("Fetching device status through REST API failed.");
    }

    json response_json = json::parse(response.text)["data"];
    bool actual_conn = response_json["connected"];

    if (actual_conn != expected_conn) {
      spdlog::error("Expected: {}", expected_conn ? "connected" : "disconnected");
      spdlog::error("Actual: {}", actual_conn ? "connected" : "disconnected");
      throw EndToEndMismatchException("Mismatch in connection status.");
    }

    if (expected_intro) {
      json introspection = response_json["introspection"];
      for (const std::string& interface : expected_intro.value()) {
        if (!introspection.contains(interface)) {
          spdlog::error("Device introspection is missing interface: {}", interface);
          throw EndToEndMismatchException("Device introspection is missing one interface.");
        }
      }
    }
  };
}

inline Action TransmitRESTData(AstarteMessage message) {
  return [msg = std::move(message)](const TestCaseContext& ctx) {
    spdlog::info("Transmitting REST data...");
    std::string url =
        actions_helpers::build_url(ctx, "/interfaces/" + msg.get_interface() + msg.get_path());
    spdlog::info("REQUEST: {}", url);

    auto make_payload = [](const auto& data) {
      return astarte_fmt::format(R"({{"data":{}}})", data);
    };

    if (msg.is_datastream()) {
      std::string payload;
      if (msg.is_individual()) {
        payload = make_payload(msg.into<DatastreamIndividual>());
      } else {
        payload = make_payload(msg.into<AstarteDatastreamObject>());
      }
      spdlog::trace("HTTP POST: {} {}", url, payload);
      auto res = cpr::Post(cpr::Url{url}, cpr::Body{payload},
                           cpr::Header{{"Content-Type", "application/json"}},
                           cpr::Header{{"Authorization", "Bearer " + ctx.http.appengine_token}});
      if (res.status_code != 200) {
        throw EndToEndHTTPException("Transmission of data through REST API failed.");
      }
    } else {
      const auto data(msg.into<AstartePropertyIndividual>());
      if (data.get_value().has_value()) {
        std::string payload = make_payload(data);
        auto res = cpr::Post(cpr::Url{url}, cpr::Body{payload},
                             cpr::Header{{"Content-Type", "application/json"}},
                             cpr::Header{{"Authorization", "Bearer " + ctx.http.appengine_token}});
        if (res.status_code != 200) {
          throw EndToEndHTTPException("Transmission of data through REST API failed.");
        }
      } else {
        auto res = cpr::Delete(
            cpr::Url{url}, cpr::Body{}, cpr::Header{{"Content-Type", "application/json"}},
            cpr::Header{{"Authorization", "Bearer " + ctx.http.appengine_token}});
        if (res.status_code != 204) {
          throw EndToEndHTTPException("Transmission of data through REST API failed.");
        }
      }
    }
  };
}

inline Action FetchRESTData(
    AstarteMessage message,
    std::optional<std::chrono::system_clock::time_point> timestamp = std::nullopt) {
  return [msg = std::move(message), ts = timestamp](const TestCaseContext& ctx) {
    spdlog::info("Fetching REST data...");
    std::string url = actions_helpers::build_url(ctx, "/interfaces/" + msg.get_interface());

    auto response = cpr::Get(cpr::Url{url}, cpr::Header{{"Content-Type", "application/json"}},
                             cpr::Header{{"Authorization", "Bearer " + ctx.http.appengine_token}});

    if (response.status_code != 200) {
      spdlog::error("HTTP GET failed, status code: {}", response.status_code);
      throw EndToEndHTTPException("Fetching data through REST API failed.");
    }

    json response_json = json::parse(response.text)["data"];

    if (msg.is_datastream()) {
      if (msg.is_individual()) {
        actions_helpers::check_datastream_individual(response_json, msg);
      } else {
        actions_helpers::check_datastream_aggregate(response_json, msg);
      }
    } else {
      const auto expected_data(msg.into<AstartePropertyIndividual>());
      if (expected_data.get_value().has_value()) {
        actions_helpers::check_individual_property(response_json, msg, expected_data);
      } else {
        actions_helpers::check_property_unset(response_json, msg);
      }
    }
  };
}

}  // namespace actions
