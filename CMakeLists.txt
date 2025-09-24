# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.15)

project(AstarteDeviceSDKcpp VERSION 0.7.0 LANGUAGES CXX)

include(FetchContent)
include(cmake/AstarteGRPCTransport.cmake)
include(cmake/AstarteMQTTTransport.cmake)

# Setup libraries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# Configuration options for this library
option(ASTARTE_TRANSPORT_GRPC "Enable gRPC transport" ON)
option(ASTARTE_USE_SYSTEM_SPDLOG "Use system installed spdlog" OFF)
option(ASTARTE_PUBLIC_SPDLOG_DEP "Make spdlog dependency public" OFF)

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
message(STATUS "  ASTARTE_TRANSPORT_GRPC:          ${ASTARTE_TRANSPORT_GRPC}")
message(STATUS "  ASTARTE_USE_SYSTEM_SPDLOG:       ${ASTARTE_USE_SYSTEM_SPDLOG}")
message(STATUS "  ASTARTE_PUBLIC_SPDLOG_DEP:       ${ASTARTE_PUBLIC_SPDLOG_DEP}")

if(ASTARTE_TRANSPORT_GRPC)
    astarte_sdk_add_grpc_options()
else()
    astarte_sdk_add_mqtt_options()
endif()

message(STATUS "--------------------------------------------------")

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

if(ASTARTE_TRANSPORT_GRPC)
    astarte_sdk_configure_grpc_dependencies()
else()
    astarte_sdk_configure_mqtt_dependencies()
endif()

# --- Core Library Target Definition ---
add_library(astarte_device_sdk)
target_compile_features(astarte_device_sdk PUBLIC cxx_std_20)
target_include_directories(
    astarte_device_sdk
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include> $<INSTALL_INTERFACE:include>
)
target_include_directories(astarte_device_sdk PRIVATE ${CMAKE_CURRENT_LIST_DIR}/private)
file(GLOB astarte_sdk_src "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
target_sources(astarte_device_sdk PRIVATE ${astarte_sdk_src})

# Link generic dependencies
if(ASTARTE_PUBLIC_SPDLOG_DEP)
    target_link_libraries(astarte_device_sdk PUBLIC spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
else()
    target_link_libraries(astarte_device_sdk PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
endif()

if(ASTARTE_TRANSPORT_GRPC)
    target_compile_definitions(astarte_device_sdk PUBLIC ASTARTE_TRANSPORT_GRPC)
    astarte_sdk_add_grpc_transport()
else()
    astarte_sdk_add_mqtt_transport()
endif()

# --- Generic Installation Rules ---
include(CMakePackageConfigHelpers)

configure_package_config_file(
    "cmake/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/astarte_device_sdkConfig.cmake"
    INSTALL_DESTINATION "lib/cmake/astarte_device_sdk"
)

set(INSTALL_TARGETS_LIST astarte_device_sdk)
if(NOT ASTARTE_USE_SYSTEM_SPDLOG)
    list(APPEND INSTALL_TARGETS_LIST spdlog)
endif()
if(ASTARTE_TRANSPORT_GRPC)
    astarte_sdk_add_grpc_install_targets(INSTALL_TARGETS_LIST)
else()
    astarte_sdk_add_mqtt_install_targets(INSTALL_TARGETS_LIST)
endif()

set_target_properties(astarte_device_sdk PROPERTIES POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS})

install(
    TARGETS ${INSTALL_TARGETS_LIST}
    EXPORT astarte_device_sdk-target
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# install the entire public include directory, preserving its structure.
# This way we don't miss out any header file, such as those in subdirecotries
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

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

if(ASTARTE_TRANSPORT_GRPC)
    astarte_sdk_install_grpc_pkgconfig()
else()
    astarte_sdk_install_mqtt_pkgconfig()
endif()
