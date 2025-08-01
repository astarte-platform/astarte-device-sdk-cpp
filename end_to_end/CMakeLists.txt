# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.15)
project(EndToEndTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git GIT_TAG 1.11.2)
FetchContent_MakeAvailable(cpr)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)

# Add the root directory to make the library definition available
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/lib_build)

# Add the executable
add_executable(end_to_end_runner ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_include_directories(end_to_end_runner PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link the library created in the root directory
target_link_libraries(
    end_to_end_runner
    PRIVATE astarte_device_sdk
    PRIVATE cpr::cpr
    PRIVATE nlohmann_json::nlohmann_json
    PRIVATE tomlplusplus::tomlplusplus
)
