# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.15)

set(ASTARTE_PROTOBUF_VERSION 30.0.0)

# Ensure the main project is found
project(MqttExample)

# Add the root directory to make the library definition available
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../.. ${CMAKE_CURRENT_BINARY_DIR}/lib_build)

# This block will only be processed if ASTARTE_TRANSPORT_GRPC is OFF
if(NOT ASTARTE_TRANSPORT_GRPC)
    message(STATUS "ASTARTE_TRANSPORT_GRPC is OFF. Proceed with building.")

    # Add the executable
    add_executable(app main.cpp)

    # Link the library created in the root directory
    target_link_libraries(app PRIVATE astarte_device_sdk PRIVATE nlohmann_json::nlohmann_json)
else()
    message(STATUS "ASTARTE_TRANSPORT_GRPC is ON. Skip example build.")
endif()
