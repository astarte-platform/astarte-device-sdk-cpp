# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.15)

project(AstarteDeviceSDKcpp VERSION 0.7.0 LANGUAGES CXX)

# Setup libraries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

include(FetchContent)

# Configuration options for this library
option(ASTARTE_USE_SYSTEM_SPDLOG "Use system installed spdlog" OFF)
option(ASTARTE_PUBLIC_SPDLOG_DEP "Make spdlog dependency public" OFF)
option(ASTARTE_PUBLIC_PROTO_DEP "Make message hub proto dependency public" OFF)
set(ASTARTE_MESSAGE_HUB_PROTO_DIR
    CACHE PATH
    "Directory to an already downloaded astarte-message-hub-proto repository"
)

# check if std::format is actually supported.
include(CheckCXXSourceCompiles)
check_cxx_source_compiles(
    "
  #include <format>
  int main() {
    auto s = std::format(\"Hello {}\", 42);
    return 0;
  }
  "
    HAS_STD_FORMAT
)

if(HAS_STD_FORMAT)
    # C++20 with <format> available to compiler requires to enable the following spdlog feature
    set(SPDLOG_USE_STD_FORMAT true)
endif()

message(STATUS "--------------------------------------------------")
message(STATUS "Astarte SDK Configuration:")
message(STATUS "  ASTARTE_USE_SYSTEM_SPDLOG:       ${ASTARTE_USE_SYSTEM_SPDLOG}")
message(STATUS "  ASTARTE_PUBLIC_SPDLOG_DEP:       ${ASTARTE_PUBLIC_SPDLOG_DEP}")
message(STATUS "  ASTARTE_PUBLIC_PROTO_DEP:        ${ASTARTE_PUBLIC_PROTO_DEP}")
message(STATUS "  ASTARTE_MESSAGE_HUB_PROTO_DIR:   ${ASTARTE_MESSAGE_HUB_PROTO_DIR}")
message(STATUS "--------------------------------------------------")

# Logging library

if(NOT ASTARTE_USE_SYSTEM_SPDLOG)
    set(SPDLOG_GITHUB_URL https://github.com/gabime/spdlog.git)
    set(SPDLOG_GIT_TAG v1.15.3)
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY ${SPDLOG_GITHUB_URL}
        GIT_TAG ${SPDLOG_GIT_TAG}
        SYSTEM
    )
    FetchContent_MakeAvailable(spdlog)
else()
    find_package(spdlog REQUIRED)
endif()

# Astarte message hub protos
if(ASTARTE_MESSAGE_HUB_PROTO_DIR)
    add_subdirectory(${ASTARTE_MESSAGE_HUB_PROTO_DIR} astarte_msghub_proto)
else()
    set(MSGHUB_PROTO_GITHUB_URL https://github.com/astarte-platform/astarte-message-hub-proto.git)
    set(MSGHUB_PROTO_GIT_TAG release-0.8)
    FetchContent_Declare(
        astarte_msghub_proto
        GIT_REPOSITORY ${MSGHUB_PROTO_GITHUB_URL}
        GIT_TAG ${MSGHUB_PROTO_GIT_TAG}
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(astarte_msghub_proto)
endif()

# Create a library from the source code
add_library(astarte_device_sdk)
target_compile_features(astarte_device_sdk PUBLIC cxx_std_20)
target_include_directories(
    astarte_device_sdk
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include> $<INSTALL_INTERFACE:include>
)
target_include_directories(astarte_device_sdk PRIVATE ${CMAKE_CURRENT_LIST_DIR}/private)
file(GLOB astarte_sdk_src "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
target_sources(astarte_device_sdk PRIVATE ${astarte_sdk_src})

# Link with the msghub grpc
target_link_libraries(
    astarte_device_sdk
    PRIVATE ${_GRPC_CPP} ${_REFLECTION} ${_PROTOBUF_LIBPROTOBUF}
)

if(ASTARTE_PUBLIC_SPDLOG_DEP)
    target_link_libraries(astarte_device_sdk PUBLIC spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
else()
    target_link_libraries(astarte_device_sdk PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
endif()
if(ASTARTE_PUBLIC_PROTO_DEP)
    target_link_libraries(astarte_device_sdk PUBLIC astarte_msghub_proto)
else()
    target_link_libraries(astarte_device_sdk PRIVATE astarte_msghub_proto)
endif()

# Set public headers to install
file(GLOB_RECURSE SDK_PUBLIC_HEADERS "${CMAKE_CURRENT_LIST_DIR}/include/*.hpp")

set_target_properties(
    astarte_device_sdk
    PROPERTIES PUBLIC_HEADER "${SDK_PUBLIC_HEADERS}" POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS}
)

include(GNUInstallDirs)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    "cmake/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/astarte_device_sdkConfig.cmake"
    INSTALL_DESTINATION "lib/cmake/astarte_device_sdk"
)

set(INSTALL_TARGETS_LIST astarte_device_sdk astarte_msghub_proto)
if(NOT ASTARTE_USE_SYSTEM_SPDLOG)
    list(APPEND INSTALL_TARGETS_LIST spdlog)
endif()
if(NOT ASTARTE_USE_SYSTEM_GRPC)
    list(
        APPEND
        INSTALL_TARGETS_LIST
        # gRPC C++ libraries
        grpc++
        grpc++_reflection
        # gRPC C-core libraries
        grpc
        gpr
        address_sorting
        # Protobuf library
        libprotobuf
        # Dependencies of gRPC revealed by CMake export errors
        upb_json_lib
        upb_textformat_lib
        zlibstatic
        re2
        # OpenSSL libs (gRPC builds them as 'ssl' and 'crypto')
        ssl
        crypto
        # Deeper dependencies of the UPB library
        upb_mini_descriptor_lib
        upb_wire_lib
        # Base-level UPB and UTF8 dependencies
        upb_base_lib
        upb_mem_lib
        upb_message_lib
        utf8_range_lib
    )
endif()

install(
    TARGETS ${INSTALL_TARGETS_LIST}
    EXPORT astarte_device_sdk-target
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/astarte_device_sdk
)

install(
    EXPORT astarte_device_sdk-target
    FILE astarte_device_sdk-target.cmake
    NAMESPACE astarte_device_sdk::
    DESTINATION lib/cmake/astarte_device_sdk
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/astarte_device_sdkConfig.cmake"
    DESTINATION "lib/cmake/astarte_device_sdk"
)

# create pkg-conf .pc for astarte_device_sdk
set(PC_NAME "astarte_device_sdk")
set(PC_DESCRIPTION "Astarte Device SDK Cpp")
set(PC_VERSION ${PROJECT_VERSION})
set(PC_REQUIRES "spdlog astarte_msghub_proto grpc++ protobuf")
set(PC_REQUIRES_PRIVATE "")
set(PC_LIB "-lastarte_device_sdk -lgrpc++_reflection -laddress_sorting -lre2 -lupb --as-needed")
set(PC_LIBS_PRIVATE "")
configure_file(cmake/pkg-config-template.pc.in astarte_device_sdk.pc @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/astarte_device_sdk.pc DESTINATION lib/pkgconfig)
