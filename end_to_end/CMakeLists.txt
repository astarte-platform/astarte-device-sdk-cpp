# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.15)
project(EndToEndTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(cpr REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(tomlplusplus REQUIRED)
find_package(astarte_device_sdk REQUIRED)

# Add the executable
add_executable(app ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link the libraries created in the root directory
target_link_libraries(
    app
    PRIVATE astarte_device_sdk::astarte_device_sdk
    PRIVATE cpr::cpr
    PRIVATE nlohmann_json::nlohmann_json
    PRIVATE tomlplusplus::tomlplusplus
)

if(ASTARTE_TRANSPORT_GRPC)
    message(STATUS "Propagating ASTARTE_TRANSPORT_GRPC definition to app target.")
    target_compile_definitions(app PRIVATE ASTARTE_TRANSPORT_GRPC)
endif()

if(NOT ASTARTE_TRANSPORT_GRPC)
    # Link the library used only for MQTT
    target_link_libraries(app PRIVATE ada::ada)
endif()
