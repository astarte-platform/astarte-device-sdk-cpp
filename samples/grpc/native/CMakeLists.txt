# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.15)

# Ensure the main project is found
project(SampleExecutable)

# Option
option(SAMPLE_USE_SYSTEM_ASTARTE_LIB "Use system installed Astarte device lib" ON)

message(STATUS "--------------------------------------------------")
message(STATUS "Simple sample configuration:")
message(STATUS "  SAMPLE_USE_SYSTEM_ASTARTE_LIB: ${SAMPLE_USE_SYSTEM_ASTARTE_LIB}")
message(STATUS "--------------------------------------------------")

# Astarte message hub protos
if(SAMPLE_USE_SYSTEM_ASTARTE_LIB)
    find_package(astarte_device_sdk REQUIRED)
else()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../.. ${CMAKE_CURRENT_BINARY_DIR}/lib_build)
endif()

# Add the executable
add_executable(app main.cpp)

# Link the library created in the root directory
if(SAMPLE_USE_SYSTEM_ASTARTE_LIB)
    target_link_libraries(app PRIVATE astarte_device_sdk::astarte_device_sdk)
else()
    target_link_libraries(app PRIVATE astarte_device_sdk)
endif()
