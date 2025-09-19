# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.15)

# Ensure the main project is found
project(MqttExample)

# Add the root directory to make the library definition available
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../.. ${CMAKE_CURRENT_BINARY_DIR}/lib_build)

if(ASTARTE_TRANSPORT_GRPC)
    message(FATAL_ERROR "ASTARTE_TRANSPORT_GRPC is ON. Cannot build MQTT example.")
endif()

# Add the executable
add_executable(app main.cpp)

# Link the library created in the root directory
target_link_libraries(
    app
    PRIVATE astarte_device_sdk
    PRIVATE tomlplusplus::tomlplusplus
    PRIVATE ada::ada
)
