# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.19)
project(grpc_qt LANGUAGES CXX)

# Option
option(USE_QT6 "Use Qt 6 instead of Qt 5" ON)
option(SAMPLE_USE_SYSTEM_ASTARTE_LIB "Use system installed Astarte device lib" ON)

message(STATUS "--------------------------------------------------")
message(STATUS "Qt sample configuration:")
message(STATUS "  USE_QT6:                       ${USE_QT6}")
message(STATUS "  SAMPLE_USE_SYSTEM_ASTARTE_LIB: ${SAMPLE_USE_SYSTEM_ASTARTE_LIB}")
message(STATUS "--------------------------------------------------")

# Overwrite some of the Astarte features
set(ASTARTE_PUBLIC_SPDLOG_DEP ON CACHE BOOL "Overwrite the Astarte spdlog visibility")

# Process the configuration options
if(USE_QT6)
    set(QT_VERSION_MAJOR 6)
else()
    set(QT_VERSION_MAJOR 5)
endif()

# Qt features
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Overwrite some openssl configuration
if(WIN32)
    option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
    set(OPENSSL_NO_ASM ON CACHE BOOL "Overwrite the Openssl configuration")
endif()

# Find required Qt modules
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

# Include extra Qt6 macros if using Qt 6
if(QT_VERSION_MAJOR EQUAL 6)
    include(${Qt6_DIR}/Qt6CoreMacros.cmake OPTIONAL)
    include(${Qt6_DIR}/Qt6Macros.cmake OPTIONAL)
endif()

# Add the root SDK directory as a subdirectory
if(SAMPLE_USE_SYSTEM_ASTARTE_LIB)
    find_package(astarte_device_sdk REQUIRED)
else()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../.. ${CMAKE_CURRENT_BINARY_DIR}/lib_build)
endif()

# Build the executable
if(QT_VERSION_MAJOR EQUAL 6)
    qt_add_executable(app main.cpp)
else()
    add_executable(app main.cpp)
endif()

# Link against Qt and Astarte SDK
if(SAMPLE_USE_SYSTEM_ASTARTE_LIB)
    target_link_libraries(
        app
        PRIVATE Qt${QT_VERSION_MAJOR}::Core astarte_device_sdk::astarte_device_sdk
    )
else()
    target_link_libraries(app PRIVATE Qt${QT_VERSION_MAJOR}::Core astarte_device_sdk)
endif()

# Install rules
include(GNUInstallDirs)

install(
    TARGETS app
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_generate_deploy_app_script(
        TARGET app
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})
endif()
