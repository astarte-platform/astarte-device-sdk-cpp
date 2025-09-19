# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0
#
# This file contains all functions needed to add MQTT transport support.

# Defines MQTT-specific CMake options and displays them.
function(astarte_sdk_add_mqtt_options)
    # Nothing for now
endfunction()

# Finds and/or downloads the dependencies required for MQTT transport.
function(astarte_sdk_configure_mqtt_dependencies)
    # TODO: add option to use system MQTT

    # Fetch and configure the Paho C++ library
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

    # Library to manage ulr
    set(URL_GIT_REPOSITORY https://github.com/ada-url/ada.git)
    set(URL_GIT_TAG v3.2.7)
    FetchContent_Declare(url GIT_REPOSITORY ${URL_GIT_REPOSITORY} GIT_TAG ${URL_GIT_TAG})
    FetchContent_MakeAvailable(url)
endfunction()

# Adds MQTT source files and links required libraries to the main target.
function(astarte_sdk_add_mqtt_transport)
    # Add MQTT transport sources
    file(GLOB astarte_sdk_src_transport "${CMAKE_CURRENT_LIST_DIR}/src/mqtt/*.cpp")
    target_sources(astarte_device_sdk PRIVATE ${astarte_sdk_src_transport})

    # Link with MQTT
    target_link_libraries(astarte_device_sdk PRIVATE PahoMqttCpp::paho-mqttpp3)

    # Link with cpr HTTP library
    target_link_libraries(
        astarte_device_sdk
        PRIVATE cpr::cpr
        PRIVATE nlohmann_json::nlohmann_json
        PRIVATE tomlplusplus::tomlplusplus
        PRIVATE ada::ada
    )
endfunction()

# Creates and installs the pkg-config file for the mqtt-enabled SDK.
function(astarte_sdk_install_mqtt_pkgconfig)
    # TODO: implement the install steps
endfunction()
