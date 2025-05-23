# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.19)

project(qt LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

if(QT_VERSION_MAJOR EQUAL 6)
    include(${Qt6_DIR}/Qt6CoreMacros.cmake OPTIONAL)
    include(${Qt6_DIR}/Qt6Macros.cmake OPTIONAL)
endif()

# Add the root SDK directory as subdirectory
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../.. ${CMAKE_CURRENT_BINARY_DIR}/lib_build)

# Build the executable
if(QT_VERSION_MAJOR EQUAL 6)
    qt_add_executable(qt
        main.cpp
    )
else()
    add_executable(qt
        main.cpp
)
endif()

# Link against Qt and Astarte SDK
target_link_libraries(qt
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        astarte_device_sdk
)

# Install rules
include(GNUInstallDirs)

install(TARGETS qt
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_generate_deploy_app_script(
        TARGET qt
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})
endif()

