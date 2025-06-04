# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(
  AstarteDeviceSDKcpp
  VERSION 0.5.99
  LANGUAGES CXX)

include(FetchContent)

# Configuration options for this library
option(ASTARTE_ENABLE_FORMAT "Enable formatting of Astarte device data" OFF)
option(ASTARTE_PUBLIC_PROTO_DEP "Make message hub proto dependency public" OFF)

# Logging library
set(SPDLOG_GITHUB_URL https://github.com/gabime/spdlog.git)
set(SPDLOG_GIT_TAG v1.15.1)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY ${SPDLOG_GITHUB_URL}
  GIT_TAG        ${SPDLOG_GIT_TAG}
  SYSTEM
)
FetchContent_MakeAvailable(spdlog)

if(ASTARTE_ENABLE_FORMAT)
  # Base 64 library
  set(BASE64_GITHUB_URL https://github.com/aklomp/base64.git)
  set(BASE64_GIT_TAG v0.5.2)
  FetchContent_Declare(
    base64
    GIT_REPOSITORY ${BASE64_GITHUB_URL}
    GIT_TAG        ${BASE64_GIT_TAG}
    SYSTEM
  )
  FetchContent_MakeAvailable(base64)
endif()

# Astarte message hub protos
set(MSGHUB_PROTO_GITHUB_URL https://github.com/astarte-platform/astarte-message-hub-proto.git)
set(MSGHUB_PROTO_GIT_TAG 9cd8ca061841ab539b63252427e149a676258281)
FetchContent_Declare(
  astarte_msghub_proto
  GIT_REPOSITORY ${MSGHUB_PROTO_GITHUB_URL}
  GIT_TAG        ${MSGHUB_PROTO_GIT_TAG}
  GIT_SHALLOW    TRUE
  GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(astarte_msghub_proto)

# Create a library from the source code
add_library(astarte_device_sdk)
target_include_directories(astarte_device_sdk PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(astarte_device_sdk PRIVATE ${CMAKE_CURRENT_LIST_DIR}/private)
file(GLOB astarte_sdk_src "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
target_sources(astarte_device_sdk PRIVATE ${astarte_sdk_src})

# Link the dependencies within the library
if(ASTARTE_ENABLE_FORMAT)
    target_compile_definitions(astarte_device_sdk PUBLIC ASTARTE_FORMAT_ENABLED)
    target_link_libraries(astarte_device_sdk PRIVATE base64)
endif()
target_link_libraries(astarte_device_sdk PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
if(ASTARTE_PUBLIC_PROTO_DEP)
target_link_libraries(astarte_device_sdk PUBLIC astarte_msghub_proto)
else()
target_link_libraries(astarte_device_sdk PRIVATE astarte_msghub_proto)
endif()
