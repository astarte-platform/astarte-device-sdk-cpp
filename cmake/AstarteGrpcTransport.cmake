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
        set(MSGHUB_PROTO_GIT_TAG release-0.10)
        FetchContent_Declare(
            astarte_msghub_proto
            GIT_REPOSITORY ${MSGHUB_PROTO_GITHUB_URL}
            GIT_TAG ${MSGHUB_PROTO_GIT_TAG}
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
        )
        FetchContent_MakeAvailable(astarte_msghub_proto)
    endif()

    # Setups grpc
    astarte_setup_grpc(
            _ASTARTE_INTERNAL_PROTOC      # Output: Path to the protoc executable
            _ASTARTE_INTERNAL_GRPC_PLUGIN # Output: Path to the grpc_cpp_plugin executable
            _ASTARTE_INTERNAL_PROTO_INC   # Output: Path to standard include directory
    )

    # Persist them so other functions can see them
    set(_ASTARTE_INTERNAL_PROTOC "${_ASTARTE_INTERNAL_PROTOC}" CACHE INTERNAL "Path to protoc")
    set(_ASTARTE_INTERNAL_GRPC_PLUGIN
        "${_ASTARTE_INTERNAL_GRPC_PLUGIN}"
        CACHE INTERNAL
        "Path to grpc_cpp_plugin"
    )
    set(_ASTARTE_INTERNAL_PROTO_INC
        "${_ASTARTE_INTERNAL_PROTO_INC}"
        CACHE INTERNAL
        "Path to standard proto include dir"
    )
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
    # Set a temporary variable for the visibility flag
    set(_ASTARTE_PROTO_VISIBILITY_FLAG "")
    if(NOT ASTARTE_PUBLIC_PROTO_DEP)
        set(_ASTARTE_PROTO_VISIBILITY_FLAG PRIVATE_VISIBILITY)
    endif()

    # Generate the proto files and attach them to the target
    astarte_proto_attach_to_target(astarte_device_sdk
        ${_ASTARTE_PROTO_VISIBILITY_FLAG}
        PROTOBUF_PROTOC                "${_ASTARTE_INTERNAL_PROTOC}"
        GRPC_CPP_PLUGIN_EXECUTABLE     "${_ASTARTE_INTERNAL_GRPC_PLUGIN}"
        PROTOBUF_STANDARD_INCLUDE_DIR  "${_ASTARTE_INTERNAL_PROTO_INC}"
    )

    target_compile_definitions(astarte_device_sdk PUBLIC ASTARTE_TRANSPORT_GRPC)
endfunction()

# Adds gRPC-specific targets to the installation list.
function(astarte_sdk_add_grpc_install_targets TARGET_LIST_VAR)
    # No specific targets to add for gRPC transport
endfunction()

# Creates and installs the pkg-config file for the gRPC-enabled SDK.
function(astarte_sdk_install_grpc_pkgconfig)
    set(PC_NAME "astarte_device_sdk")
    set(PC_DESCRIPTION "Astarte Device SDK Cpp (gRPC transport)")
    set(PC_VERSION ${PROJECT_VERSION})
    # Configure PkgConfig dependencies based on visibility
    if(ASTARTE_PUBLIC_PROTO_DEP)
        set(PC_REQUIRES "spdlog grpc++ protobuf")
        set(PC_REQUIRES_PRIVATE "")
    else()
        set(PC_REQUIRES "spdlog")
        set(PC_REQUIRES_PRIVATE "grpc++ protobuf")
    endif()
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
