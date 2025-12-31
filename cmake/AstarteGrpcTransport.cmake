# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0
#
# This file contains all functions needed to add gRPC transport support.

include_guard(GLOBAL)

# Defines gRPC-specific CMake options and displays them.
function(astarte_sdk_add_grpc_options)
    option(ASTARTE_PUBLIC_PROTO_DEP "Make message hub proto dependency public" OFF)
    set(ASTARTE_MESSAGE_HUB_PROTO_DIR CACHE PATH "Directory of astarte-message-hub-proto repo")

    message(STATUS "  gRPC Options:")
    message(STATUS "    ASTARTE_PUBLIC_PROTO_DEP:      ${ASTARTE_PUBLIC_PROTO_DEP}")
    message(STATUS "    ASTARTE_MESSAGE_HUB_PROTO_DIR: ${ASTARTE_MESSAGE_HUB_PROTO_DIR}")
endfunction()

# Finds and/or downloads the dependencies required for gRPC transport.
function(astarte_sdk_configure_grpc_dependencies)
    # Astarte message hub protos
    if(ASTARTE_MESSAGE_HUB_PROTO_DIR)
        add_subdirectory(${ASTARTE_MESSAGE_HUB_PROTO_DIR} astarte_msghub_proto)
    else()
        set(MSGHUB_PROTO_GITHUB_URL
            https://github.com/astarte-platform/astarte-message-hub-proto.git
        )
        set(MSGHUB_PROTO_GIT_TAG release-0.9)
        FetchContent_Declare(
            astarte_msghub_proto
            GIT_REPOSITORY ${MSGHUB_PROTO_GITHUB_URL}
            GIT_TAG ${MSGHUB_PROTO_GIT_TAG}
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
        )
        FetchContent_MakeAvailable(astarte_msghub_proto)
    endif()
endfunction()

# Adds gRPC sources and private headers to the provided lists.
#
# @param ASTARTE_GRPC_PUBLIC_HEADERS The name of the list variable for public headers.
# @param ASTARTE_GRPC_SOURCES The name of the list variable for source files.
# @param ASTARTE_GRPC_PRIVATE_HEADERS The name of the list variable for private headers.
function(
    astarte_sdk_add_grpc_sources
    ASTARTE_GRPC_PUBLIC_HEADERS
    ASTARTE_GRPC_SOURCES
    ASTARTE_GRPC_PRIVATE_HEADERS
)
    list(APPEND ${ASTARTE_GRPC_PUBLIC_HEADERS} "include/astarte_device_sdk/grpc/device_grpc.hpp")
    list(
        APPEND
        ${ASTARTE_GRPC_SOURCES}
        "src/grpc/device_grpc_impl.cpp"
        "src/grpc/device_grpc.cpp"
        "src/grpc/grpc_converter.cpp"
        "src/grpc/grpc_interceptors.cpp"
    )
    list(
        APPEND
        ${ASTARTE_GRPC_PRIVATE_HEADERS}
        "private/grpc/device_grpc_impl.hpp"
        "private/grpc/grpc_converter.hpp"
        "private/grpc/grpc_formatter.hpp"
        "private/grpc/grpc_interceptors.hpp"
    )
    set(${ASTARTE_GRPC_PUBLIC_HEADERS} ${${ASTARTE_GRPC_PUBLIC_HEADERS}} PARENT_SCOPE)
    set(${ASTARTE_GRPC_SOURCES} ${${ASTARTE_GRPC_SOURCES}} PARENT_SCOPE)
    set(${ASTARTE_GRPC_PRIVATE_HEADERS} ${${ASTARTE_GRPC_PRIVATE_HEADERS}} PARENT_SCOPE)
endfunction()

# Adds gRPC source files and links required libraries to the main target.
function(astarte_sdk_add_grpc_transport)
    # Link with gRPC, Protobuf, and the message hub proto library
    target_link_libraries(
        astarte_device_sdk
        PRIVATE ${_GRPC_CPP} ${_REFLECTION} ${_PROTOBUF_LIBPROTOBUF}
    )

    if(ASTARTE_PUBLIC_PROTO_DEP)
        target_link_libraries(astarte_device_sdk PUBLIC astarte_msghub_proto)
    else()
        target_link_libraries(astarte_device_sdk PRIVATE astarte_msghub_proto)
    endif()

    target_compile_definitions(astarte_device_sdk PUBLIC ASTARTE_TRANSPORT_GRPC)
endfunction()

# Adds gRPC-specific targets to the installation list.
function(astarte_sdk_add_grpc_install_targets TARGET_LIST_VAR)
    list(APPEND ${TARGET_LIST_VAR} astarte_msghub_proto)
    set(${TARGET_LIST_VAR} ${${TARGET_LIST_VAR}} PARENT_SCOPE)
endfunction()

# Creates and installs the pkg-config file for the gRPC-enabled SDK.
function(astarte_sdk_install_grpc_pkgconfig)
    set(PC_NAME "astarte_device_sdk")
    set(PC_DESCRIPTION "Astarte Device SDK Cpp (gRPC transport)")
    set(PC_VERSION ${PROJECT_VERSION})
    set(PC_REQUIRES "spdlog astarte_msghub_proto grpc++ protobuf")
    set(PC_REQUIRES_PRIVATE "")
    set(PC_LIB "-lastarte_device_sdk -lgrpc++_reflection -laddress_sorting -lre2 -lupb --as-needed")
    set(PC_LIBS_PRIVATE "")
    configure_file(
        cmake/pkg-config-template.pc.in
        ${CMAKE_CURRENT_BINARY_DIR}/astarte_device_sdk.pc
        @ONLY
    )

    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/astarte_device_sdk.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )
endfunction()
