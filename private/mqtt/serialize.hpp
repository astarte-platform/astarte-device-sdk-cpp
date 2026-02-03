// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_DATA_SERIALIZATION_H
#define ASTARTE_DATA_SERIALIZATION_H

#include <chrono>
#include <nlohmann/json.hpp>
#include <string>

#include "astarte_device_sdk/data.hpp"
#include "astarte_device_sdk/object.hpp"

namespace astarte::device::bson {

using json = nlohmann::json;

/**
 * @brief Serialize Data to BSON bytes.
 *
 * @param bson a reference to the BSON object to populate.
 * @param key the BSON key related to the Astarte data value.
 * @param data Astarte individual data to serialize to BSON.
 * @param timestamp Timestamp value to serialize to BSON if not null.
 */
void serialize_astarte_individual(json& bson, const std::string& key, const Data& data,
                                  const std::chrono::system_clock::time_point* timestamp);

/**
 * @brief Serialize DatastreamObject to BSON bytes.
 *
 * @param bson a reference to the BSON object to populate.
 * @param object Astarte object data to serialize to BSON.
 * @param timestamp Timestamp value to serialize to BSON if not null.
 */
void serialize_astarte_object(json& bson, const DatastreamObject& object,
                              const std::chrono::system_clock::time_point* timestamp);

}  // namespace astarte::device::bson

#endif  // ASTARTE_DATA_SERIALIZATION_H
