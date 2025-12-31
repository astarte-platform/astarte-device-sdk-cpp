# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

project(AstarteDeviceSdk VERSION 0.8.1 LANGUAGES CXX)

include(FetchContent)
include(cmake/AstarteGrpcTransport.cmake)
include(cmake/AstarteMqttTransport.cmake)

# Setup library options
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
check_cxx_source_compiles(
    "
  #include <expected>
  #include <string>
  int main() {
    std::expected<int, std::string> success(42);
    return 0;
  }
  "
    HAS_STD_EXPECTED
)

if(NOT HAS_STD_EXPECTED)
    option(ASTARTE_USE_SYSTEM_TL_EXPECTED "Use system installed tl-expected" OFF)
endif()

if(HAS_STD_FORMAT)
    # C++20 with <format> available to compiler requires to enable the following spdlog feature
    set(SPDLOG_USE_STD_FORMAT true)
endif()

message(STATUS "--------------------------------------------------")
message(STATUS "Astarte SDK configuration:")
message(STATUS "  ASTARTE_TRANSPORT_GRPC:          ${ASTARTE_TRANSPORT_GRPC}")
message(STATUS "  ASTARTE_USE_SYSTEM_SPDLOG:       ${ASTARTE_USE_SYSTEM_SPDLOG}")
if(NOT HAS_STD_EXPECTED)
    message(STATUS "  ASTARTE_USE_SYSTEM_TL_EXPECTED:  ${ASTARTE_USE_SYSTEM_TL_EXPECTED}")
endif()
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

if(NOT HAS_STD_EXPECTED)
    message(STATUS "Standard expected not found. Using a custom library...")
    if(NOT ASTARTE_USE_SYSTEM_TL_EXPECTED)
        set(TL_EXPECTED_GITHUB_URL https://github.com/TartanLlama/expected.git)
        set(TL_EXPECTED_GIT_TAG v1.2.0)
        FetchContent_Declare(
            tl-expected
            GIT_REPOSITORY ${TL_EXPECTED_GITHUB_URL}
            GIT_TAG ${TL_EXPECTED_GIT_TAG}
        )
        FetchContent_MakeAvailable(tl-expected)
    else()
        find_package(tl-expected REQUIRED)
    endif()
endif()

if(ASTARTE_TRANSPORT_GRPC)
    astarte_sdk_configure_grpc_dependencies()
else()
    astarte_sdk_configure_mqtt_dependencies()
endif()

# --- Core Library Target Definition ---
add_library(astarte_device_sdk)
target_compile_features(astarte_device_sdk PUBLIC cxx_std_20)
set(_ASTARTE_PUBLIC_HEADERS
    "include/astarte_device_sdk/data.hpp"
    "include/astarte_device_sdk/device.hpp"
    "include/astarte_device_sdk/errors.hpp"
    "include/astarte_device_sdk/formatter.hpp"
    "include/astarte_device_sdk/individual.hpp"
    "include/astarte_device_sdk/msg.hpp"
    "include/astarte_device_sdk/object.hpp"
    "include/astarte_device_sdk/ownership.hpp"
    "include/astarte_device_sdk/property.hpp"
    "include/astarte_device_sdk/stored_property.hpp"
    "include/astarte_device_sdk/type.hpp"
)
set(_ASTARTE_SOURCES
    "src/data.cpp"
    "src/errors.cpp"
    "src/individual.cpp"
    "src/msg.cpp"
    "src/object.cpp"
    "src/property.cpp"
    "src/stored_property.cpp"
)
set(_ASTARTE_PRIVATE_HEADERS "private/exponential_backoff.hpp" "private/shared_queue.hpp")
if(ASTARTE_TRANSPORT_GRPC)
    astarte_sdk_add_grpc_sources(_ASTARTE_PUBLIC_HEADERS _ASTARTE_SOURCES _ASTARTE_PRIVATE_HEADERS)
else()
    astarte_sdk_add_mqtt_sources(_ASTARTE_PUBLIC_HEADERS _ASTARTE_SOURCES _ASTARTE_PRIVATE_HEADERS)
endif()

target_sources(
    astarte_device_sdk
    PUBLIC FILE_SET HEADERS BASE_DIRS "include" FILES ${_ASTARTE_PUBLIC_HEADERS}
    PRIVATE ${_ASTARTE_SOURCES}
    PRIVATE
        FILE_SET private_headers TYPE HEADERS BASE_DIRS "private" FILES ${_ASTARTE_PRIVATE_HEADERS}
)

# Link generic dependencies
if(ASTARTE_PUBLIC_SPDLOG_DEP)
    target_link_libraries(astarte_device_sdk PUBLIC spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
else()
    target_link_libraries(astarte_device_sdk PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
endif()

if(ASTARTE_TRANSPORT_GRPC)
    astarte_sdk_add_grpc_transport()
else()
    astarte_sdk_add_mqtt_transport()
endif()
if(NOT HAS_STD_EXPECTED)
    target_link_libraries(astarte_device_sdk PUBLIC tl::expected)
    target_compile_definitions(astarte_device_sdk PUBLIC ASTARTE_USE_TL_EXPECTED)
endif()

# Set target properties
set_target_properties(
    astarte_device_sdk
    PROPERTIES
        POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS}
        ARCHIVE_OUTPUT_DIRECTORY "${AstarteDeviceSdk_BINARY_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${AstarteDeviceSdk_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${AstarteDeviceSdk_BINARY_DIR}"
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

configure_package_config_file(
    "cmake/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/astarte_device_sdkConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/astarte_device_sdk"
)

# Generate the version file for package version checking
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/astarte_device_sdkConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

set(INSTALL_TARGETS_LIST astarte_device_sdk)
if(ASTARTE_TRANSPORT_GRPC)
    astarte_sdk_add_grpc_install_targets(INSTALL_TARGETS_LIST)
else()
    astarte_sdk_add_mqtt_install_targets(INSTALL_TARGETS_LIST)
endif()

install(
    TARGETS ${INSTALL_TARGETS_LIST}
    EXPORT astarte_device_sdk-target
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# TODO: re-enable this once the installation of the package using cmake has been fully evaluated
# install(
#     EXPORT astarte_device_sdk-target
#     FILE astarte_device_sdk-target.cmake
#     NAMESPACE astarte_device_sdk::
#     DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/astarte_device_sdk
# )

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/astarte_device_sdkConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/astarte_device_sdkConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/astarte_device_sdk"
)

if(ASTARTE_TRANSPORT_GRPC)
    astarte_sdk_install_grpc_pkgconfig()
else()
    astarte_sdk_install_mqtt_pkgconfig()
endif()
