// (C) Copyright 2025, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <string_view>

namespace constants::interfaces {

struct DeviceDatastream {
  static constexpr std::string_view INTERFACE =
      "org.astarte-platform.cpp.end-to-end.DeviceDatastream";
  static constexpr std::string_view FILE =
      "end_to_end/interfaces/org.astarte-platform.cpp.end-to-end.DeviceDatastream.json";
};
struct ServerDatastream {
  static constexpr std::string_view INTERFACE =
      "org.astarte-platform.cpp.end-to-end.ServerDatastream";
  static constexpr std::string_view FILE =
      "end_to_end/interfaces/org.astarte-platform.cpp.end-to-end.ServerDatastream.json";
};

struct DeviceProperty {
  static constexpr std::string_view INTERFACE =
      "org.astarte-platform.cpp.end-to-end.DeviceProperty";
  static constexpr std::string_view FILE =
      "end_to_end/interfaces/org.astarte-platform.cpp.end-to-end.DeviceProperty.json";
};
struct ServerProperty {
  static constexpr std::string_view INTERFACE =
      "org.astarte-platform.cpp.end-to-end.ServerProperty";
  static constexpr std::string_view FILE =
      "end_to_end/interfaces/org.astarte-platform.cpp.end-to-end.ServerProperty.json";
};

struct DeviceAggregate {
  static constexpr std::string_view INTERFACE =
      "org.astarte-platform.cpp.end-to-end.DeviceAggregate";
  static constexpr std::string_view FILE =
      "end_to_end/interfaces/org.astarte-platform.cpp.end-to-end.DeviceAggregate.json";
};
struct ServerAggregate {
  static constexpr std::string_view INTERFACE =
      "org.astarte-platform.cpp.end-to-end.ServerAggregate";
  static constexpr std::string_view FILE =
      "end_to_end/interfaces/org.astarte-platform.cpp.end-to-end.ServerAggregate.json";
};
}  // namespace constants::interfaces
