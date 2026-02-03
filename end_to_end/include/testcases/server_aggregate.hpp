// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "action.hpp"
#include "case.hpp"
#include "constants/data_sets.hpp"
#include "constants/interfaces.hpp"

namespace testcases {
using namespace std::chrono_literals;

TestCase server_aggregate(std::string device_id) {
    DatastreamObject astarte_obj = {
        {std::string(constants::data_sets::Integer::ENDPOINT_PARTIAL),
         constants::data_sets::Integer::DATA},
        {std::string(constants::data_sets::Double::ENDPOINT_PARTIAL),
         constants::data_sets::Double::DATA},
        {std::string(constants::data_sets::Boolean::ENDPOINT_PARTIAL),
         constants::data_sets::Boolean::DATA},
        {std::string(constants::data_sets::LongInteger::ENDPOINT_PARTIAL),
         constants::data_sets::LongInteger::DATA},
        {std::string(constants::data_sets::String::ENDPOINT_PARTIAL),
         constants::data_sets::String::DATA},
        {std::string(constants::data_sets::Datetime::ENDPOINT_PARTIAL),
         constants::data_sets::Datetime::DATA},
        {std::string(constants::data_sets::BinaryBlob::ENDPOINT_PARTIAL),
         constants::data_sets::BinaryBlob::DATA},
        {std::string(constants::data_sets::IntegerArray::ENDPOINT_PARTIAL),
         constants::data_sets::IntegerArray::DATA},
        {std::string(constants::data_sets::LongIntegerArray::ENDPOINT_PARTIAL),
         constants::data_sets::LongIntegerArray::DATA},
        {std::string(constants::data_sets::DoubleArray::ENDPOINT_PARTIAL),
         constants::data_sets::DoubleArray::DATA},
        {std::string(constants::data_sets::BooleanArray::ENDPOINT_PARTIAL),
         constants::data_sets::BooleanArray::DATA},
        {std::string(constants::data_sets::StringArray::ENDPOINT_PARTIAL),
         constants::data_sets::StringArray::DATA},
        {std::string(constants::data_sets::BinaryBlobArray::ENDPOINT_PARTIAL),
         constants::data_sets::BinaryBlobArray::DATA},
        {std::string(constants::data_sets::DatetimeArray::ENDPOINT_PARTIAL),
         constants::data_sets::DatetimeArray::DATA}
    };

    return TestCase(
        "Send Astarte Aggregate",
        {actions::Connect(),
         actions::Sleep(1s),

         actions::TransmitRESTData(
             Message(constants::interfaces::ServerAggregate::INTERFACE, "/sensor1", astarte_obj)
         ),

         actions::Sleep(1s),

         actions::ReadReceivedDeviceData(
             Message(constants::interfaces::ServerAggregate::INTERFACE, "/sensor1", astarte_obj)
         ),

         actions::Sleep(1s),
         actions::Disconnect(),
         actions::Sleep(1s)},
        device_id
    );
}
}  // namespace testcases
