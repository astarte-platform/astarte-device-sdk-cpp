// (C) Copyright 2025 - 2026, SECO Mind Srl
//
// SPDX-License-Identifier: Apache-2.0

#ifndef ASTARTE_INTROSPECTION_H
#define ASTARTE_INTROSPECTION_H

#include <string>
#include <string_view>

#include "astarte_device_sdk/errors.hpp"
#include "mqtt/interface.hpp"

namespace AstarteDeviceSdk {

/**
 * @brief Represents a collection of Astarte interface.
 *
 * The introspection represents the set of device supported interfaces.
 * See the Astarte documentation for more details.
 * https://docs.astarte-platform.org/astarte/latest/080-mqtt-v1-protocol.html#introspection
 */
class Introspection {
 public:
  /**
   * @brief Construct an empty Introspection.
   */
  Introspection() = default;

  /**
   * @brief Try to insert and Interface into the Introspection.
   *
   * @param interface The interface to add.
   * @return an error if the operation fails
   */
  auto checked_insert(Interface interface) -> astarte_tl::expected<void, AstarteError>;

  /**
   * @brief Return a view over the introspection values.
   *
   * @return a view over the introspection interfaces.
   */
  [[nodiscard]] auto values() const { return std::views::values(interfaces_); }

  /**
   * @brief get an interface reference if the interface is contained in the device introspection.
   *
   * @param interface_name the interface name.
   * @return the interface reference if found inside the introspection, an error otherwise.
   */
  [[nodiscard]] auto get(const std::string& interface_name)
      -> astarte_tl::expected<Interface*, AstarteError>;

 private:
  /**
   * @brief A map containing the interfaces in the Device Introspection synced with Astarte.
   */
  std::map<std::string, Interface> interfaces_;
};

}  // namespace AstarteDeviceSdk

#endif  // ASTARTE_INTROSPECTION_H
