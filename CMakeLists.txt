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
set(ASTARTE_MESSAGE_HUB_PROTO_DIR CACHE PATH "Directory to an already downloaded astarte-message-hub-proto repository")

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
if(ASTARTE_MESSAGE_HUB_PROTO_DIR)
    add_subdirectory(${ASTARTE_MESSAGE_HUB_PROTO_DIR} astarte_msghub_proto)
else()
    set(MSGHUB_PROTO_GITHUB_URL https://github.com/astarte-platform/astarte-message-hub-proto.git)
    set(MSGHUB_PROTO_GIT_TAG v0.8.1)
    FetchContent_Declare(
        astarte_msghub_proto
        GIT_REPOSITORY ${MSGHUB_PROTO_GITHUB_URL}
        GIT_TAG        ${MSGHUB_PROTO_GIT_TAG}
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
    )
    FetchContent_MakeAvailable(astarte_msghub_proto)
endif()

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

# Set public headers to install
file(GLOB_RECURSE SDK_PUBLIC_HEADERS "${CMAKE_CURRENT_LIST_DIR}/include/*.hpp")

set_target_properties(astarte_device_sdk
    PROPERTIES
    PUBLIC_HEADER "${SDK_PUBLIC_HEADERS}"
)


install(TARGETS astarte_device_sdk
    RUNTIME
        DESTINATION bin
    LIBRARY
        DESTINATION lib
    ARCHIVE
        DESTINATION lib
    PUBLIC_HEADER
        DESTINATION include/astarte_device_sdk
)

# create pkg-conf .pc for astarte_device_sdk
set(PC_NAME "astarte_device_sdk")
set(PC_DESCRIPTION "Astarte Device SDK Cpp")
set(PC_VERSION ${PROJECT_VERSION})
set(PC_REQUIRES "spdlog astarte_message_hub_proto grpc++ grpc protobuf re2 openssl")
set(PC_REQUIRES_PRIVATE "")
set(PC_LIB "-lastarte_device_sdk -laddress_sorting -lupb")
set(PC_LIBS_PRIVATE "")
configure_file(
    cmake/pkg-config-template.pc.in
    astarte_device_sdk.pc
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/astarte_device_sdk.pc  DESTINATION lib/pkgconfig)
