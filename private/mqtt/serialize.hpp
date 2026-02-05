// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DATA_SERIALIZATION_H
#define ASTARTE_DATA_SERIALIZATION_H

/**
 * @file private/mqtt/serialize.hpp
 * @brief BSON serialization utilities for Astarte data.
 *
 * @details This file provides functions to serialize Astarte data structures (individuals
 * and objects) into BSON format for transmission over MQTT.
 */

#include <chrono>
#include <nlohmann/json.hpp>
#include <string>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/object.hpp"

namespace astarte::device::mqtt::bson {

using json = nlohmann::json;

/**
 * @brief Serializes an Astarte Data individual to BSON.
 *
 * @param[in,out] bson A reference to the JSON/BSON object to populate.
 * @param[in] key The BSON key associated with the Astarte data value.
 * @param[in] data The Astarte individual data to serialize.
 * @param[in] timestamp Optional timestamp to include in the serialization.
 */
void serialize_astarte_individual(json& bson, const std::string& key, const Data& data,
                                  const std::chrono::system_clock::time_point* timestamp);

/**
 * @brief Serializes a DatastreamObject to BSON.
 *
 * @param[in,out] bson A reference to the JSON/BSON object to populate.
 * @param[in] object The Astarte object data to serialize.
 * @param[in] timestamp Optional timestamp to include in the serialization.
 */
void serialize_astarte_object(json& bson, const DatastreamObject& object,
                              const std::chrono::system_clock::time_point* timestamp);

}  // namespace astarte::device::mqtt::bson

#endif  // ASTARTE_DATA_SERIALIZATION_H
