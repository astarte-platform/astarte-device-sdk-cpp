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

TestCase server_property(std::string device_id) {
    return TestCase(
        "Server property to Device",
        {actions::Connect(),
         actions::Sleep(1s),

         // set server properties
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::Integer::ENDPOINT_SENSOR,
             AstartePropertyIndividual(AstarteData(43))
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::LongInteger::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::LongInteger::DATA)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::Double::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::Double::DATA)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::Boolean::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::Boolean::DATA)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::String::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::String::DATA)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::Datetime::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::Datetime::DATA)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::BinaryBlob::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::BinaryBlob::DATA)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::IntegerArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::IntegerArray::DATA)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::LongIntegerArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::LongIntegerArray::DATA)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::DoubleArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::DoubleArray::DATA)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::BooleanArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::BooleanArray::DATA)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::StringArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::StringArray::DATA)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::DatetimeArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::DatetimeArray::DATA)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::BinaryBlobArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::BinaryBlobArray::DATA)
         )),

         actions::Sleep(1s),

         // check if server property have been set
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::Integer::ENDPOINT_SENSOR,
             AstartePropertyIndividual(AstarteData(43))
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::LongInteger::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::LongInteger::DATA)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::Double::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::Double::DATA)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::Boolean::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::Boolean::DATA)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::String::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::String::DATA)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::Datetime::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::Datetime::DATA)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::BinaryBlob::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::BinaryBlob::DATA)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::IntegerArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::IntegerArray::DATA)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::LongIntegerArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::LongIntegerArray::DATA)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::DoubleArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::DoubleArray::DATA)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::BooleanArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::BooleanArray::DATA)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::StringArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::StringArray::DATA)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::DatetimeArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::DatetimeArray::DATA)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::BinaryBlobArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::BinaryBlobArray::DATA)
         )),

         actions::Sleep(1s),

         // unset server properties
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::Integer::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::LongInteger::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::Double::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::Boolean::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::String::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::Datetime::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::BinaryBlob::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::IntegerArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::LongIntegerArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::DoubleArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::BooleanArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::StringArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::DatetimeArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::BinaryBlobArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),

         actions::Sleep(1s),

         // check unset server property
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::Integer::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::LongInteger::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::Double::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::Boolean::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::String::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::Datetime::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::BinaryBlob::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::IntegerArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::LongIntegerArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::DoubleArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::BooleanArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::StringArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::DatetimeArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::BinaryBlobArray::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),

         actions::Sleep(1s),
         actions::Disconnect(),
         actions::Sleep(1s)},
        device_id
    );
}

TestCase server_property_on_new_device(std::string device_id) {
    return TestCase(
        "Server property to a new Device",
        {actions::Sleep(1s),
         actions::Connect(),
         actions::Sleep(1s),

         // set server properties
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::Integer::ENDPOINT_SENSOR,
             AstartePropertyIndividual(AstarteData(43))
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::LongInteger::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::LongInteger::DATA)
         )),

         // disconnect and reconnect
         actions::Sleep(1s),
         actions::Disconnect(),
         actions::Sleep(1s),
         actions::Connect(),
         actions::Sleep(1s),

         // check if server property have been received
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::Integer::ENDPOINT_SENSOR,
             AstartePropertyIndividual(AstarteData(43))
         )),
         actions::ReadReceivedDeviceData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::LongInteger::ENDPOINT_SENSOR,
             AstartePropertyIndividual(constants::data_sets::LongInteger::DATA)
         )),

         actions::Sleep(1s),

         // unset the properties
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::Integer::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),
         actions::TransmitRESTData(AstarteMessage(
             constants::interfaces::ServerProperty::INTERFACE,
             constants::data_sets::LongInteger::ENDPOINT_SENSOR,
             AstartePropertyIndividual(std::nullopt)
         )),

         actions::Sleep(1s),
         actions::Disconnect(),
         actions::Sleep(1s)},
        device_id
    );
}
}  // namespace testcases
