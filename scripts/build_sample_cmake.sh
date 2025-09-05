#!/bin/bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

error_exit() {
    echo "Error: $1" >&2
    exit 1
}

# Configures and builds a CMake project for a specified sample application.
# Handles fresh builds, CMake configuration, and the final build process.
#
# Arguments:
#   $1: lib_src_dir - The path to the library source directory.
#   $2: sample_to_build - The name of the sample being built.
#   $3: system_grpc - 'true' to use system gRPC, 'false' otherwise.
#   $4: jobs - The number of parallel jobs for 'make'.
#   $5: qt_path (optional) - The path to the Qt installation.
#   $6: qt_version (optional) - The major version of Qt to use.
#
# Example Usage:
#   build_sample_with_cmake "$(pwd)" "qt" "false" $(nproc --all) "$HOME/Qt/6.8.1/gcc_64/lib/cmake/Qt6" "6"
#
build_sample_with_cmake() {
    local lib_src_dir="$1"
    local sample_to_build="$2"
    local system_grpc="$3"
    local jobs="$4"
    local qt_path="$5"
    local qt_version="$6"

    # --- Argument Validation ---
    if [ -z "$lib_src_dir" ]; then
        error_exit "Library source directory not provided."
    fi
    if [ -z "$sample_to_build" ]; then
        error_exit "Sample to build not provided."
    fi
    if [ -z "$system_grpc" ]; then
        error_exit "System grpc option not provided."
    fi
    if [[ "$sample_to_build" == "qt" && -z "$qt_path" ]]; then
        error_exit "Qt path must be provided for the 'qt' sample."
    fi
    if [[ "$sample_to_build" == "qt" && -z "$qt_version" ]]; then
        error_exit "Qt version must be provided for the 'qt' sample."
    fi

    local sample_src_dir="${lib_src_dir}/samples/${sample_to_build}"
    local build_dir="${sample_src_dir}/build"

    # Create build directory if it doesn't exist
    mkdir -p "$build_dir"

    # Navigate to build directory
    cd "$build_dir" || error_exit "Failed to navigate to $build_dir"

    # --- Configure CMake options ---
    echo "Running CMake for $sample_to_build sample..."
    local cmake_options_array=()
    cmake_options_array+=("-DCMAKE_CXX_STANDARD=20")
    cmake_options_array+=("-DCMAKE_CXX_STANDARD_REQUIRED=ON")
    cmake_options_array+=("-DCMAKE_POLICY_VERSION_MINIMUM=3.15")
    cmake_options_array+=("-DASTARTE_PUBLIC_SPDLOG_DEP=ON")
    cmake_options_array+=("-DSAMPLE_USE_SYSTEM_ASTARTE_LIB=OFF")

    if [ "$system_grpc" = true ]; then
        cmake_options_array+=("-DASTARTE_USE_SYSTEM_GRPC=ON")
    fi

    if [[ "$sample_to_build" == "qt" ]]; then
        if [[ -z "$qt_path" ]]; then
            error_exit "Error: qt_path not specified for Qt sample."
        fi

        # Check consistency between qt_path and qt_version
        if [[ "$qt_version" == "6" && "$qt_path" != *"Qt6"* ]]; then
            echo " Mismatch: --qt_version is 6 but qt_path does not look like Qt6."
            error_exit "Please check your --qt_path or --qt_version."
        elif [[ "$qt_version" == "5" && "$qt_path" != *"Qt5"* ]]; then
            echo "Mismatch: --qt_version is 5 but qt_path does not look like Qt5."
            error_exit "Please check your --qt_path or --qt_version."
        fi

        cmake_options_array+=("-DCMAKE_PREFIX_PATH=$qt_path")
        cmake_options_array+=("-DUSE_QT6=$([ "$qt_version" == "6" ] && echo "ON" || echo "OFF")")
    fi

    # Run CMake
    if ! cmake "${cmake_options_array[@]}" "$sample_src_dir"; then
        error_exit "CMake configuration failed for $sample_to_build sample."
    fi

    # Build the project
    echo "Building $sample_to_build sample with make -j $jobs ..."
    if ! make -j "$jobs"; then
        error_exit "Make build failed for $sample_to_build sample."
    fi

    # Return to the original directory to avoid side effects
    cd - || error_exit "Failed to return to the original directory."
    echo "CMake build process complete."
}
