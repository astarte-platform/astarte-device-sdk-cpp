// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#include "mqtt/serialize.hpp"

#include <algorithm>
#include <chrono>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/formatter.hpp"
#include "astarte_device_sdk/object.hpp"

namespace AstarteDeviceSdk::bson {

using json = nlohmann::json;

constexpr uint8_t BSON_TYPE_EOO = 0x00;

void serialize_astarte_individual(json& bson, const std::string& key, const AstarteData& data,
                                  const std::chrono::system_clock::time_point* timestamp) {
  std::visit(
      [&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;

        // handle Binary Blob
        if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
          bson.emplace(key, json::binary(arg, BSON_TYPE_EOO));
        }
        // handle DateTime (std::chrono::system_clock::time_point)
        else if constexpr (std::is_same_v<T, std::chrono::system_clock::time_point>) {
          // Convert to milliseconds
          auto millis =
              std::chrono::duration_cast<std::chrono::milliseconds>(arg.time_since_epoch()).count();

          bson.emplace(key, millis);
        }
        // handle Array of Binaries
        else if constexpr (std::is_same_v<T, std::vector<std::vector<uint8_t>>>) {
          std::vector<json> binary_array;
          binary_array.reserve(arg.size());
          std::transform(arg.begin(), arg.end(), std::back_inserter(binary_array),
                         [](const auto& bin) { return json::binary(bin, BSON_TYPE_EOO); });

          bson.emplace(key, binary_array);
        }
        // handle Array of DateTimes
        else if constexpr (std::is_same_v<T, std::vector<std::chrono::system_clock::time_point>>) {
          std::vector<int64_t> dates;
          dates.reserve(arg.size());
          std::transform(arg.begin(), arg.end(), std::back_inserter(dates), [](const auto& time) {
            return std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch())
                .count();
          });

          bson.emplace(key, dates);
        }
        // default handler (int, double, string, bool, and the respective vectors)
        else {
          bson.emplace(key, arg);
        }
      },
      data.get_raw_data());

  if (timestamp != nullptr) {
    bson.emplace(
        "t", std::chrono::duration_cast<std::chrono::milliseconds>(timestamp->time_since_epoch())
                 .count());
  }
}

void serialize_astarte_object(json& bson, const AstarteDatastreamObject& object,
                              const std::chrono::system_clock::time_point* timestamp) {
  json inner_bson;
  for (const auto& [endpoint_path, data] : object) {
    // we insert the timestamp only in the outer bson
    serialize_astarte_individual(inner_bson, endpoint_path, data, nullptr);
  }
  bson.emplace("v", inner_bson);

  if (timestamp != nullptr) {
    bson.emplace(
        "t", std::chrono::duration_cast<std::chrono::milliseconds>(timestamp->time_since_epoch())
                 .count());
  }
}

}  // namespace AstarteDeviceSdk::bson
