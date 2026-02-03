// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>

#include "action.hpp"
#include "case.hpp"
#include "constants/data_sets.hpp"
#include "constants/interfaces.hpp"
#include "constants/time.hpp"

namespace testcases {
using namespace std::chrono_literals;

TestCase device_datastream(std::string device_id) {
    return TestCase(
        "Device to Astarte",
        {actions::Connect(),
         actions::Sleep(1s),

         // Transmit actions
         actions::TransmitDeviceData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::Integer::ENDPOINT_FULL,
                 AstarteDatastreamIndividual(constants::data_sets::Integer::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::TransmitDeviceData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::LongInteger::ENDPOINT_FULL,
                 AstarteDatastreamIndividual(constants::data_sets::LongInteger::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::TransmitDeviceData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::Double::ENDPOINT_FULL,
                 AstarteDatastreamIndividual(constants::data_sets::Double::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::TransmitDeviceData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::Boolean::ENDPOINT_FULL,
                 AstarteDatastreamIndividual(constants::data_sets::Boolean::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::TransmitDeviceData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::String::ENDPOINT_FULL,
                 AstarteDatastreamIndividual(constants::data_sets::String::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::TransmitDeviceData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::Datetime::ENDPOINT_FULL,
                 AstarteDatastreamIndividual(constants::data_sets::Datetime::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::TransmitDeviceData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::BinaryBlob::ENDPOINT_FULL,
                 AstarteDatastreamIndividual(constants::data_sets::BinaryBlob::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::TransmitDeviceData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::IntegerArray::ENDPOINT_FULL,
                 AstarteDatastreamIndividual(constants::data_sets::IntegerArray::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::TransmitDeviceData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::LongIntegerArray::ENDPOINT_FULL,
                 AstarteDatastreamIndividual(constants::data_sets::LongIntegerArray::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::TransmitDeviceData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::DoubleArray::ENDPOINT_FULL,
                 AstarteDatastreamIndividual(constants::data_sets::DoubleArray::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::TransmitDeviceData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::BooleanArray::ENDPOINT_FULL,
                 AstarteDatastreamIndividual(constants::data_sets::BooleanArray::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::TransmitDeviceData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::StringArray::ENDPOINT_FULL,
                 AstarteDatastreamIndividual(constants::data_sets::StringArray::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::TransmitDeviceData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::DatetimeArray::ENDPOINT_FULL,
                 AstarteDatastreamIndividual(constants::data_sets::DatetimeArray::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::TransmitDeviceData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::BinaryBlobArray::ENDPOINT_FULL,
                 AstarteDatastreamIndividual(constants::data_sets::BinaryBlobArray::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::Sleep(1s),

         // Fetch actions
         actions::FetchRESTData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::Integer::ENDPOINT_PARTIAL,
                 AstarteDatastreamIndividual(constants::data_sets::Integer::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::FetchRESTData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::LongInteger::ENDPOINT_PARTIAL,
                 AstarteDatastreamIndividual(constants::data_sets::LongInteger::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::FetchRESTData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::Double::ENDPOINT_PARTIAL,
                 AstarteDatastreamIndividual(constants::data_sets::Double::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::FetchRESTData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::Boolean::ENDPOINT_PARTIAL,
                 AstarteDatastreamIndividual(constants::data_sets::Boolean::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::FetchRESTData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::String::ENDPOINT_PARTIAL,
                 AstarteDatastreamIndividual(constants::data_sets::String::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::FetchRESTData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::Datetime::ENDPOINT_PARTIAL,
                 AstarteDatastreamIndividual(constants::data_sets::Datetime::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::FetchRESTData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::BinaryBlob::ENDPOINT_PARTIAL,
                 AstarteDatastreamIndividual(constants::data_sets::BinaryBlob::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::FetchRESTData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::IntegerArray::ENDPOINT_PARTIAL,
                 AstarteDatastreamIndividual(constants::data_sets::IntegerArray::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::FetchRESTData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::LongIntegerArray::ENDPOINT_PARTIAL,
                 AstarteDatastreamIndividual(constants::data_sets::LongIntegerArray::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::FetchRESTData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::DoubleArray::ENDPOINT_PARTIAL,
                 AstarteDatastreamIndividual(constants::data_sets::DoubleArray::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::FetchRESTData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::BooleanArray::ENDPOINT_PARTIAL,
                 AstarteDatastreamIndividual(constants::data_sets::BooleanArray::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::FetchRESTData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::StringArray::ENDPOINT_PARTIAL,
                 AstarteDatastreamIndividual(constants::data_sets::StringArray::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::FetchRESTData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::DatetimeArray::ENDPOINT_PARTIAL,
                 AstarteDatastreamIndividual(constants::data_sets::DatetimeArray::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::FetchRESTData(
             AstarteMessage(
                 constants::interfaces::DeviceDatastream::INTERFACE,
                 constants::data_sets::BinaryBlobArray::ENDPOINT_PARTIAL,
                 AstarteDatastreamIndividual(constants::data_sets::BinaryBlobArray::DATA)
             ),
             constants::time::TIMESTAMP
         ),

         actions::Sleep(1s),
         actions::Disconnect(),
         actions::Sleep(1s)},
        device_id
    );
}
}  // namespace testcases
