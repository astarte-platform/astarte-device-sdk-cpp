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

TestCase server_datastream(std::string device_id) {
    return TestCase(
        "Astarte to device",
        {actions::Connect(),
         actions::Sleep(1s),

         actions::TransmitRESTData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::Integer::ENDPOINT_FULL,
             DatastreamIndividual(Data((int32_t)43))
         )),
         actions::TransmitRESTData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::LongInteger::ENDPOINT_FULL,
             DatastreamIndividual(constants::data_sets::LongInteger::DATA)
         )),
         actions::TransmitRESTData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::Double::ENDPOINT_FULL,
             DatastreamIndividual(Data(4245.23))
         )),
         actions::TransmitRESTData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::Boolean::ENDPOINT_FULL,
             DatastreamIndividual(Data(false))
         )),
         actions::TransmitRESTData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::String::ENDPOINT_FULL,
             DatastreamIndividual(Data(std::string("Hello world")))
         )),
         actions::TransmitRESTData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::Datetime::ENDPOINT_FULL,
             DatastreamIndividual(constants::data_sets::Datetime::DATA)
         )),
         actions::TransmitRESTData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::BinaryBlob::ENDPOINT_FULL,
             DatastreamIndividual(constants::data_sets::BinaryBlob::DATA)
         )),
         actions::TransmitRESTData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::IntegerArray::ENDPOINT_FULL,
             DatastreamIndividual(Data(std::vector<int32_t>{32, 43, 0, 3332}))
         )),
         actions::TransmitRESTData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::LongIntegerArray::ENDPOINT_FULL,
             DatastreamIndividual(Data(std::vector<int64_t>{17179869184, 0, 6486}))
         )),
         actions::TransmitRESTData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::DoubleArray::ENDPOINT_FULL,
             DatastreamIndividual(Data(std::vector<double>{0.0, 23.2}))
         )),
         actions::TransmitRESTData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::BooleanArray::ENDPOINT_FULL,
             DatastreamIndividual(Data(std::vector<bool>{false, true, true}))
         )),
         actions::TransmitRESTData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::StringArray::ENDPOINT_FULL,
             DatastreamIndividual(
                 Data(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})
             )
         )),
         actions::TransmitRESTData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::DatetimeArray::ENDPOINT_FULL,
             DatastreamIndividual(constants::data_sets::DatetimeArray::DATA)
         )),
         actions::TransmitRESTData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::BinaryBlobArray::ENDPOINT_FULL,
             DatastreamIndividual(constants::data_sets::BinaryBlobArray::DATA)
         )),

         actions::Sleep(1s),

         actions::ReadReceivedDeviceData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::Integer::ENDPOINT_FULL,
             DatastreamIndividual(Data((int32_t)43))
         )),
         actions::ReadReceivedDeviceData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::LongInteger::ENDPOINT_FULL,
             DatastreamIndividual(constants::data_sets::LongInteger::DATA)
         )),
         actions::ReadReceivedDeviceData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::Double::ENDPOINT_FULL,
             DatastreamIndividual(Data(4245.23))
         )),
         actions::ReadReceivedDeviceData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::Boolean::ENDPOINT_FULL,
             DatastreamIndividual(Data(false))
         )),
         actions::ReadReceivedDeviceData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::String::ENDPOINT_FULL,
             DatastreamIndividual(Data(std::string("Hello world")))
         )),
         actions::ReadReceivedDeviceData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::Datetime::ENDPOINT_FULL,
             DatastreamIndividual(constants::data_sets::Datetime::DATA)
         )),
         actions::ReadReceivedDeviceData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::BinaryBlob::ENDPOINT_FULL,
             DatastreamIndividual(constants::data_sets::BinaryBlob::DATA)
         )),
         actions::ReadReceivedDeviceData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::IntegerArray::ENDPOINT_FULL,
             DatastreamIndividual(Data(std::vector<int32_t>{32, 43, 0, 3332}))
         )),
         actions::ReadReceivedDeviceData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::LongIntegerArray::ENDPOINT_FULL,
             DatastreamIndividual(Data(std::vector<int64_t>{17179869184, 0, 6486}))
         )),
         actions::ReadReceivedDeviceData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::DoubleArray::ENDPOINT_FULL,
             DatastreamIndividual(Data(std::vector<double>{0.0, 23.2}))
         )),
         actions::ReadReceivedDeviceData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::BooleanArray::ENDPOINT_FULL,
             DatastreamIndividual(Data(std::vector<bool>{false, true, true}))
         )),
         actions::ReadReceivedDeviceData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::StringArray::ENDPOINT_FULL,
             DatastreamIndividual(
                 Data(std::vector<std::string>{"Hello ", "world ", "from ", "C++"})
             )
         )),
         actions::ReadReceivedDeviceData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::DatetimeArray::ENDPOINT_FULL,
             DatastreamIndividual(constants::data_sets::DatetimeArray::DATA)
         )),
         actions::ReadReceivedDeviceData(Message(
             constants::interfaces::ServerDatastream::INTERFACE,
             constants::data_sets::BinaryBlobArray::ENDPOINT_FULL,
             DatastreamIndividual(constants::data_sets::BinaryBlobArray::DATA)
         )),

         actions::Sleep(1s),
         actions::Disconnect(),
         actions::Sleep(1s)},
        device_id
    );
}
}  // namespace testcases
