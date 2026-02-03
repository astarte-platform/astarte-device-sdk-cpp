# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)
project(unit)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.16.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Library to manage json
set(JSON_GIT_URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_Declare(json URL ${JSON_GIT_URL})
FetchContent_MakeAvailable(json)

enable_testing()

add_executable(unit_test data_test.cpp msg_test.cpp errors_test.cpp exponential_backoff_test.cpp)

if(ASTARTE_TRANSPORT_GRPC)
    target_sources(unit_test PRIVATE conversion_test.cpp)
else()
    target_sources(unit_test PRIVATE crypto_test.cpp device_id_test.cpp introspection_test.cpp)
endif()

# Add the Astarte sdk root directory
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/lib_build)
target_include_directories(unit_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../private)

target_link_libraries(
    unit_test
    astarte_device_sdk
    GTest::gtest_main
    gmock
    nlohmann_json::nlohmann_json
)

include(GoogleTest)
gtest_discover_tests(unit_test)
