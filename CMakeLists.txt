# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.15)

project(
  AstarteDeviceSDKcpp
  VERSION 1.0
  LANGUAGES CXX)

# Protobuf
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")

# Protobuf-compiler
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

# gRPC
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")
set(_GRPC_GRPCPP gRPC::grpc++)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

# Download and build the astarte-message-hub-proto code
include(FetchContent)

# Logging library
set(SPDLOG_GITHUB_URL https://github.com/gabime/spdlog.git)
set(SPDLOG_REPO_BRANCH v1.15.1)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY ${SPDLOG_GITHUB_URL}
  GIT_TAG        ${SPDLOG_REPO_BRANCH}
)

# Fetch and make spdlog available
FetchContent_MakeAvailable(spdlog)

set(MSGHUB_PROTO_REPO_URL https://github.com/astarte-platform/astarte-message-hub-proto.git)
set(MSGHUB_PROTO_REPO_BRANCH master)

FetchContent_Declare(
    astarte_msghub_proto
    GIT_REPOSITORY ${MSGHUB_PROTO_REPO_URL}
    GIT_TAG        ${MSGHUB_PROTO_REPO_BRANCH}
)
# make available
FetchContent_MakeAvailable(astarte_msghub_proto)

# Create a library from the source code
add_library(astarte_device_sdk_lib)

# Add all the library sources and headers
target_include_directories(astarte_device_sdk_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(astarte_device_sdk_lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/private)
file(GLOB astarte_sdk_src "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
target_sources(astarte_device_sdk_lib PRIVATE ${astarte_sdk_src})

# Add the proto sources and headers
file(GLOB astarte_msghub_proto_src "${astarte_msghub_proto_SOURCE_DIR}/cpp/astarteplatform/msghub/*.cc")
target_include_directories(astarte_device_sdk_lib PUBLIC "${astarte_msghub_proto_SOURCE_DIR}/cpp")
target_sources(astarte_device_sdk_lib PRIVATE ${astarte_msghub_proto_src})
target_link_libraries(astarte_device_sdk_lib
  spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>
  absl::check
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})
