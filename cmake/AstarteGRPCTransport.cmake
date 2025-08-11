# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0
#
# This file contains all functions needed to add gRPC transport support.

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
        set(MSGHUB_PROTO_GIT_TAG v0.8.4)
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

# Adds gRPC source files and links required libraries to the main target.
function(astarte_sdk_add_grpc_transport)
    # Add gRPC transport sources
    file(GLOB astarte_sdk_src_transport "${CMAKE_CURRENT_LIST_DIR}/src/grpc/*.cpp")
    target_sources(astarte_device_sdk PRIVATE ${astarte_sdk_src_transport})

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
