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
        find_package(cpr REQUIRED)
        find_package(nlohmann_json REQUIRED)
        find_package(MbedTLS REQUIRED)
        find_package(Boost REQUIRED)

        if(NOT TARGET ada::ada)
            find_package(ada REQUIRED)
        endif()
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
        set(PAHO_BUILD_SHARED ON CACHE BOOL "")
        set(PAHO_WITH_SSL ON CACHE BOOL "")
        FetchContent_MakeAvailable(paho-mqtt-cpp)

        # Library to handle HTTP requests
        set(CPR_GIT_REPOSITORY https://github.com/libcpr/cpr.git)
        set(CPR_GIT_TAG 1.11.2)
        FetchContent_Declare(cpr GIT_REPOSITORY ${CPR_GIT_REPOSITORY} GIT_TAG ${CPR_GIT_TAG})
        FetchContent_MakeAvailable(cpr)

        # Library to manage json
        set(JSON_GIT_URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
        FetchContent_Declare(json URL ${JSON_GIT_URL})
        FetchContent_MakeAvailable(json)

        # Library to manage url
        set(URL_GIT_REPOSITORY https://github.com/ada-url/ada.git)
        set(URL_GIT_TAG v3.2.4)
        FetchContent_Declare(ada GIT_REPOSITORY ${URL_GIT_REPOSITORY} GIT_TAG ${URL_GIT_TAG})
        FetchContent_MakeAvailable(ada)

        # Cryptographic library
        set(MBEDTLS_GIT_REPOSITORY https://github.com/Mbed-TLS/mbedtls.git)
        set(MBEDTLS_GIT_TAG v4.0.0)
        FetchContent_Declare(
            MbedTLS
            GIT_REPOSITORY ${MBEDTLS_GIT_REPOSITORY}
            GIT_TAG ${MBEDTLS_GIT_TAG}
        )
        # Disable programs and tests to keep the build fast and minimal.
        set(ENABLE_TESTING OFF CACHE BOOL "Disable Mbed TLS tests")
        set(ENABLE_PROGRAMS OFF CACHE BOOL "Disable Mbed TLS example programs")
        set(CONFIG_MBEDTLS_PSA_CRYPTO_C ON CACHE BOOL "Enable PSA in Mbed TLS")
        FetchContent_MakeAvailable(MbedTLS)

        set(BOOST_INCLUDE_LIBRARIES uuid headers)
        set(BOOST_ENABLE_CMAKE ON)
        set(BUILD_TESTING OFF)
        set(BOOST_GIT_REPOSITORY https://github.com/boostorg/boost.git)
        set(BOOST_GIT_TAG boost-1.89.0)
        FetchContent_Declare(Boost GIT_REPOSITORY ${BOOST_GIT_REPOSITORY} GIT_TAG ${BOOST_GIT_TAG})
        FetchContent_MakeAvailable(Boost)
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
    list(
        APPEND
        ${ASTARTE_MQTT_PUBLIC_HEADERS}
        "include/astarte_device_sdk/mqtt/config.hpp"
        "include/astarte_device_sdk/mqtt/device_mqtt.hpp"
        "include/astarte_device_sdk/mqtt/errors.hpp"
        "include/astarte_device_sdk/mqtt/pairing.hpp"
        "include/astarte_device_sdk/mqtt/connection.hpp"
        "include/astarte_device_sdk/mqtt/formatter.hpp"
    )
    list(
        APPEND
        ${ASTARTE_MQTT_SOURCES}
        "src/mqtt/config.cpp"
        "src/mqtt/crypto.cpp"
        "src/mqtt/device_mqtt_impl.cpp"
        "src/mqtt/device_mqtt.cpp"
        "src/mqtt/errors.cpp"
        "src/mqtt/pairing.cpp"
    )
    list(
        APPEND
        ${ASTARTE_MQTT_PRIVATE_HEADERS}
        "private/mqtt/crypto.hpp"
        "private/mqtt/device_mqtt_impl.hpp"
    )
    set(${ASTARTE_MQTT_PUBLIC_HEADERS} ${${ASTARTE_MQTT_PUBLIC_HEADERS}} PARENT_SCOPE)
    set(${ASTARTE_MQTT_SOURCES} ${${ASTARTE_MQTT_SOURCES}} PARENT_SCOPE)
    set(${ASTARTE_MQTT_PRIVATE_HEADERS} ${${ASTARTE_MQTT_PRIVATE_HEADERS}} PARENT_SCOPE)
endfunction()

# Adds MQTT source files and links required libraries to the main target.
function(astarte_sdk_add_mqtt_transport)
    # Link with MQTT
    if(ASTARTE_USE_SYSTEM_MQTT)
        target_link_libraries(astarte_device_sdk PRIVATE PahoMqttCpp::paho-mqttpp3-static)
        target_link_libraries(astarte_device_sdk PRIVATE Boost::headers)
    else()
        target_link_libraries(astarte_device_sdk PRIVATE PahoMqttCpp::paho-mqttpp3)
        target_link_libraries(astarte_device_sdk PRIVATE Boost::uuid PRIVATE Boost::headers)
    endif()

    # Link with cpr HTTP library
    target_link_libraries(
        astarte_device_sdk
        PRIVATE cpr::cpr
        PRIVATE nlohmann_json::nlohmann_json
        PUBLIC MbedTLS::mbedtls
        PUBLIC MbedTLS::mbedx509
        PUBLIC ada::ada
    )
endfunction()

# Adds mqtt-specific targets to the installation list.
function(astarte_sdk_add_mqtt_install_targets TARGET_LIST_VAR)
    if(NOT ASTARTE_USE_SYSTEM_MQTT)
        list(APPEND ${TARGET_LIST_VAR} ada nlohmann_json cpr paho-mqtt3as)
    endif()

    set(${TARGET_LIST_VAR} ${${TARGET_LIST_VAR}} PARENT_SCOPE)
endfunction()

# Creates and installs the pkg-config file for the mqtt-enabled SDK.
function(astarte_sdk_install_mqtt_pkgconfig)
    # TODO: implement the install steps
endfunction()
