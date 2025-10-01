# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.23)

project(AstarteDeviceSdk VERSION 0.7.0 LANGUAGES CXX)

# Setup library options
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
message(STATUS "Astarte SDK configuration:")
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
    set(MSGHUB_PROTO_GITHUB_URL https://github.com/sorru94/astarte-message-hub-proto.git)
    set(MSGHUB_PROTO_GIT_TAG improve_cpp_cmake)
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
target_sources(
    astarte_device_sdk
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS "include"
            FILES
                "include/astarte_device_sdk/data.hpp"
                "include/astarte_device_sdk/device_grpc.hpp"
                "include/astarte_device_sdk/device.hpp"
                "include/astarte_device_sdk/exceptions.hpp"
                "include/astarte_device_sdk/formatter.hpp"
                "include/astarte_device_sdk/individual.hpp"
                "include/astarte_device_sdk/msg.hpp"
                "include/astarte_device_sdk/object.hpp"
                "include/astarte_device_sdk/ownership.hpp"
                "include/astarte_device_sdk/property.hpp"
                "include/astarte_device_sdk/stored_property.hpp"
                "include/astarte_device_sdk/type.hpp"
    PRIVATE
        "src/data.cpp"
        "src/device_grpc_impl.cpp"
        "src/device_grpc.cpp"
        "src/exceptions.cpp"
        "src/grpc_converter.cpp"
        "src/grpc_interceptors.cpp"
        "src/individual.cpp"
        "src/msg.cpp"
        "src/object.cpp"
        "src/property.cpp"
        "src/stored_property.cpp"
    PRIVATE
        FILE_SET private_headers
            TYPE HEADERS
            BASE_DIRS "private"
            FILES
                "private/device_grpc_impl.hpp"
                "private/exponential_backoff.hpp"
                "private/grpc_converter.hpp"
                "private/grpc_formatter.hpp"
                "private/grpc_interceptors.hpp"
                "private/shared_queue.hpp"
)

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

# Set target properties
set_target_properties(astarte_device_sdk PROPERTIES POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS})
set_target_properties(
    astarte_device_sdk
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${AstarteDeviceSdk_BINARY_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${AstarteDeviceSdk_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${AstarteDeviceSdk_BINARY_DIR}"
)

# Installation of the library is only avaliable when the system installed dependencies are used
if(ASTARTE_USE_SYSTEM_GRPC AND ASTARTE_USE_SYSTEM_SPDLOG)
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

    install(
        TARGETS astarte_device_sdk astarte_msghub_proto
        EXPORT astarte_device_sdk-target
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(
        EXPORT astarte_device_sdk-target
        FILE astarte_device_sdk-target.cmake
        NAMESPACE astarte_device_sdk::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/astarte_device_sdk
    )

    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/astarte_device_sdkConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/astarte_device_sdkConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/astarte_device_sdk"
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

    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/astarte_device_sdk.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )
endif()
