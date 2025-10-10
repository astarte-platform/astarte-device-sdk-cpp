# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0
#
# This file contains all functions needed to add MQTT transport support.

include_guard(GLOBAL)

# Defines MQTT-specific CMake options and displays them.
function(astarte_sdk_add_mqtt_options)
    option(ASTARTE_USE_SYSTEM_MQTT "Use the system installed MQTT library" OFF)

    message(STATUS "  MQTT Options:")
    message(STATUS "    ASTARTE_USE_SYSTEM_MQTT: ${ASTARTE_USE_SYSTEM_MQTT}")
endfunction()

# Finds and/or downloads the dependencies required for MQTT transport.
function(astarte_sdk_configure_mqtt_dependencies)
    # Fetch and configure the Paho C++ library
    if(ASTARTE_USE_SYSTEM_MQTT)
        find_package(PahoMqttCpp REQUIRED)
    else()
        FetchContent_Declare(
            paho-mqtt-cpp
            GIT_REPOSITORY https://github.com/eclipse/paho.mqtt.cpp.git
            GIT_TAG v1.5.3
        )

        # Set build options
        set(PAHO_BUILD_EXAMPLES OFF CACHE BOOL "")
        set(PAHO_BUILD_TESTS OFF CACHE BOOL "")
        set(PAHO_WITH_MQTT_C ON CACHE BOOL "")

        FetchContent_MakeAvailable(paho-mqtt-cpp)
    endif()
endfunction()

# Adds MQTT sources and private headers to the provided lists.
#
# @param ASTARTE_MQTT_PUBLIC_HEADERS The name of the list variable for public headers.
# @param ASTARTE_MQTT_SOURCES The name of the list variable for source files.
# @param ASTARTE_MQTT_PRIVATE_HEADERS The name of the list variable for private headers.
function(
    astarte_sdk_add_mqtt_sources
    ASTARTE_MQTT_PUBLIC_HEADERS
    ASTARTE_MQTT_SOURCES
    ASTARTE_MQTT_PRIVATE_HEADERS
)
    list(APPEND ${ASTARTE_MQTT_PUBLIC_HEADERS} "include/astarte_device_sdk/device_mqtt.hpp")
    list(APPEND ${ASTARTE_MQTT_SOURCES} "src/mqtt/device_mqtt.cpp")
    set(${ASTARTE_MQTT_PUBLIC_HEADERS} ${${ASTARTE_MQTT_PUBLIC_HEADERS}} PARENT_SCOPE)
    set(${ASTARTE_MQTT_SOURCES} ${${ASTARTE_MQTT_SOURCES}} PARENT_SCOPE)
endfunction()

# Adds MQTT source files and links required libraries to the main target.
function(astarte_sdk_add_mqtt_transport)
    # Link with MQTT
    if(ASTARTE_USE_SYSTEM_MQTT)
        target_link_libraries(astarte_device_sdk PRIVATE PahoMqttCpp::paho-mqttpp3-static)
    else()
        target_link_libraries(astarte_device_sdk PRIVATE PahoMqttCpp::paho-mqttpp3)
    endif()
endfunction()

# Creates and installs the pkg-config file for the mqtt-enabled SDK.
function(astarte_sdk_install_mqtt_pkgconfig)
    # TODO: implement the install steps
endfunction()
