// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>
#include <vector>

#include "action.hpp"
#include "case.hpp"
#include "constants/data_sets.hpp"
#include "constants/interfaces.hpp"
#include "constants/time.hpp"

namespace testcases {
using namespace std::chrono_literals;

using astarte::device::AstarteOwnership;

std::vector<Action> set_all_properties() {
    return {
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::Integer::ENDPOINT_FULL,
            AstartePropertyIndividual(constants::data_sets::Integer::DATA)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::LongInteger::ENDPOINT_FULL,
            AstartePropertyIndividual(constants::data_sets::LongInteger::DATA)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::Double::ENDPOINT_FULL,
            AstartePropertyIndividual(constants::data_sets::Double::DATA)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::Boolean::ENDPOINT_FULL,
            AstartePropertyIndividual(constants::data_sets::Boolean::DATA)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::String::ENDPOINT_FULL,
            AstartePropertyIndividual(constants::data_sets::String::DATA)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::Datetime::ENDPOINT_FULL,
            AstartePropertyIndividual(constants::data_sets::Datetime::DATA)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::BinaryBlob::ENDPOINT_FULL,
            AstartePropertyIndividual(constants::data_sets::BinaryBlob::DATA)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::IntegerArray::ENDPOINT_FULL,
            AstartePropertyIndividual(constants::data_sets::IntegerArray::DATA)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::LongIntegerArray::ENDPOINT_FULL,
            AstartePropertyIndividual(constants::data_sets::LongIntegerArray::DATA)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::DoubleArray::ENDPOINT_FULL,
            AstartePropertyIndividual(constants::data_sets::DoubleArray::DATA)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::BooleanArray::ENDPOINT_FULL,
            AstartePropertyIndividual(constants::data_sets::BooleanArray::DATA)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::StringArray::ENDPOINT_FULL,
            AstartePropertyIndividual(constants::data_sets::StringArray::DATA)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::DatetimeArray::ENDPOINT_FULL,
            AstartePropertyIndividual(constants::data_sets::DatetimeArray::DATA)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::BinaryBlobArray::ENDPOINT_FULL,
            AstartePropertyIndividual(constants::data_sets::BinaryBlobArray::DATA)
        )),
    };
}

std::vector<Action> check_all_properties_have_value() {
    return {
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::Integer::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(constants::data_sets::Integer::DATA)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::LongInteger::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(constants::data_sets::LongInteger::DATA)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::Double::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(constants::data_sets::Double::DATA)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::Boolean::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(constants::data_sets::Boolean::DATA)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::String::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(constants::data_sets::String::DATA)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::Datetime::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(constants::data_sets::Datetime::DATA)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::BinaryBlob::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(constants::data_sets::BinaryBlob::DATA)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::IntegerArray::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(constants::data_sets::IntegerArray::DATA)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::LongIntegerArray::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(constants::data_sets::LongIntegerArray::DATA)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::DoubleArray::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(constants::data_sets::DoubleArray::DATA)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::BooleanArray::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(constants::data_sets::BooleanArray::DATA)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::StringArray::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(constants::data_sets::StringArray::DATA)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::DatetimeArray::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(constants::data_sets::DatetimeArray::DATA)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::BinaryBlobArray::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(constants::data_sets::BinaryBlobArray::DATA)
        )),
    };
}

std::vector<Action> unset_all_properties() {
    return {
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::Integer::ENDPOINT_FULL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::LongInteger::ENDPOINT_FULL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::Double::ENDPOINT_FULL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::Boolean::ENDPOINT_FULL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::String::ENDPOINT_FULL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::Datetime::ENDPOINT_FULL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::BinaryBlob::ENDPOINT_FULL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::IntegerArray::ENDPOINT_FULL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::LongIntegerArray::ENDPOINT_FULL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::DoubleArray::ENDPOINT_FULL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::BooleanArray::ENDPOINT_FULL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::StringArray::ENDPOINT_FULL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::DatetimeArray::ENDPOINT_FULL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::TransmitDeviceData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::BinaryBlobArray::ENDPOINT_FULL,
            AstartePropertyIndividual(std::nullopt)
        )),
    };
}

std::vector<Action> check_all_properties_no_value() {
    return {
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::Integer::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::LongInteger::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::Double::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::Boolean::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::String::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::Datetime::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::BinaryBlob::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::IntegerArray::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::LongIntegerArray::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::DoubleArray::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::BooleanArray::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::StringArray::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::DatetimeArray::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(std::nullopt)
        )),
        actions::FetchRESTData(Message(
            constants::interfaces::DeviceProperty::INTERFACE,
            constants::data_sets::BinaryBlobArray::ENDPOINT_PARTIAL,
            AstartePropertyIndividual(std::nullopt)
        )),
    };
}

std::vector<Action> get_properties() {
    return {
        actions::GetDeviceProperty(
            std::string(constants::interfaces::DeviceProperty::INTERFACE),
            std::string(constants::data_sets::Integer::ENDPOINT_FULL),
            AstartePropertyIndividual(constants::data_sets::Integer::DATA)
        ),
        actions::GetDeviceProperty(
            std::string(constants::interfaces::DeviceProperty::INTERFACE),
            std::string(constants::data_sets::Double::ENDPOINT_FULL),
            AstartePropertyIndividual(constants::data_sets::Double::DATA)
        ),
        actions::GetDeviceProperty(
            std::string(constants::interfaces::DeviceProperty::INTERFACE),
            std::string(constants::data_sets::Boolean::ENDPOINT_FULL),
            AstartePropertyIndividual(constants::data_sets::Boolean::DATA)
        ),
        actions::GetDeviceProperty(
            std::string(constants::interfaces::DeviceProperty::INTERFACE),
            std::string(constants::data_sets::IntegerArray::ENDPOINT_FULL),
            AstartePropertyIndividual(constants::data_sets::IntegerArray::DATA)
        ),
        actions::GetDeviceProperty(
            std::string(constants::interfaces::DeviceProperty::INTERFACE),
            std::string(constants::data_sets::DoubleArray::ENDPOINT_FULL),
            AstartePropertyIndividual(constants::data_sets::DoubleArray::DATA)
        ),
        actions::GetDeviceProperties(
            std::string(constants::interfaces::DeviceProperty::INTERFACE),
            {AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::Integer::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::Integer::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::LongInteger::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::LongInteger::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::Double::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::Double::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::Boolean::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::Boolean::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::String::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::String::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::Datetime::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::Datetime::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::BinaryBlob::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::BinaryBlob::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::IntegerArray::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::IntegerArray::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::LongIntegerArray::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::LongIntegerArray::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::DoubleArray::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::DoubleArray::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::BooleanArray::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::BooleanArray::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::StringArray::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::StringArray::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::DatetimeArray::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::DatetimeArray::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::BinaryBlobArray::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::BinaryBlobArray::DATA
             )}
        ),
        actions::GetAllFilteredProperties(
            std::nullopt,
            {AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::Integer::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::Integer::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::LongInteger::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::LongInteger::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::Double::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::Double::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::Boolean::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::Boolean::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::String::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::String::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::Datetime::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::Datetime::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::BinaryBlob::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::BinaryBlob::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::IntegerArray::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::IntegerArray::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::LongIntegerArray::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::LongIntegerArray::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::DoubleArray::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::DoubleArray::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::BooleanArray::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::BooleanArray::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::StringArray::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::StringArray::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::DatetimeArray::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::DatetimeArray::DATA
             ),
             AstarteStoredProperty(
                 constants::interfaces::DeviceProperty::INTERFACE,
                 std::string(constants::data_sets::BinaryBlobArray::ENDPOINT_FULL),
                 0,
                 AstarteOwnership::kDevice,
                 constants::data_sets::BinaryBlobArray::DATA
             )}
        ),
    };
}

TestCase device_property(std::string device_id) {
    auto set_all_props = set_all_properties();
    auto unset_all_props = unset_all_properties();
    auto check_all_props_with_value = check_all_properties_have_value();
    auto check_all_props_no_value = check_all_properties_no_value();

    std::vector<Action> actions = {actions::Connect(), actions::Sleep(1s)};
    actions.insert(actions.end(), set_all_props.begin(), set_all_props.end());
    actions.push_back(actions::Sleep(1s));
    actions.insert(
        actions.end(), check_all_props_with_value.begin(), check_all_props_with_value.end()
    );
    actions.push_back(actions::Sleep(1s));
    actions.insert(actions.end(), unset_all_props.begin(), unset_all_props.end());
    actions.push_back(actions::Sleep(1s));
    actions.insert(actions.end(), check_all_props_no_value.begin(), check_all_props_no_value.end());
    actions.push_back(actions::Sleep(1s));
    actions.push_back(actions::Disconnect());
    actions.push_back(actions::Sleep(1s));

    return TestCase("Device property to Astarte", actions, device_id);
}

TestCase device_property_getter(std::string device_id) {
    auto set_all_props = set_all_properties();
    auto get_props = get_properties();
    auto unset_all_props = unset_all_properties();

    std::vector<Action> actions = {actions::Connect(), actions::Sleep(1s)};
    actions.insert(actions.end(), set_all_props.begin(), set_all_props.end());
    actions.push_back(actions::Sleep(1s));
    actions.insert(actions.end(), get_props.begin(), get_props.end());
    actions.push_back(actions::Sleep(1s));
    actions.insert(actions.end(), unset_all_props.begin(), unset_all_props.end());
    actions.push_back(actions::Sleep(1s));
    actions.push_back(actions::Disconnect());
    actions.push_back(actions::Sleep(1s));

    return TestCase("Device property getter", actions, device_id);
}

}  // namespace testcases
