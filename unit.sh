#!/bin/bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

# --- Configuration ---
fresh_mode=false
transport=grpc
system_transport=false
jobs=$(nproc --all)
build_dir="unit/build"

# --- Helper Functions ---
display_help() {
    cat << EOF
Usage: $0 [OPTIONS]

Build and run unit tests.

Options:
  --fresh               Build from scratch (removes $build_dir).
  --transport <TR>      Specify the transport to use (mqtt or grpc). Default: $transport.
  --system_transport    Use the system trasnport (gRPC or MQTT) instead of building it from scratch.
  -j, --jobs <N>        Specify the number of parallel jobs for make. Default: $jobs.
  -h, --help            Display this help message.
EOF
}
error_exit() {
    echo "Error: $1" >&2
    exit 1
}

# --- Argument Parsing ---
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --fresh) fresh_mode=true; shift ;;
        --transport)
            transport="$2"
            if [[ ! "$transport" =~ ^('mqtt'|'grpc')$ ]]; then
                error_exit "Invalid transport '$transport'. Use mqtt or grpc."
            fi
            shift 2
            ;;
        --system_transport) system_transport=true; shift ;;
        -j|--jobs)
            jobs="$2"
            if ! [[ "$jobs" =~ ^[0-9]+$ && "$jobs" -gt 0 ]]; then
                error_exit "Invalid argument for --jobs. Please provide a positive number."
            fi
            shift 2
            ;;
        -h|--help) display_help; exit 0 ;;
        *) display_help; error_exit "Unknown option: $1" ;;
    esac
done

# --- Build Logic ---

echo "Configuration:"
echo "  Jobs: $jobs"
echo "  Build Directory: $build_dir"
echo "  Fresh Mode: $fresh_mode"
echo "  Transport: $transport"
echo "  Use System transport: $system_transport"
echo ""

# Clean build if --fresh is set
if [ "$fresh_mode" = true ]; then
    if [ -d "$build_dir" ]; then
        echo "Fresh build requested. Removing $build_dir..."
        rm -rf "$build_dir"
    else
        echo "Fresh build requested, but $build_dir does not exist. Skipping removal."
    fi
fi

# Create build directory if it doesn't exist
echo "Ensuring build directory '$build_dir' exists..."
if ! mkdir -p "$build_dir"; then
    error_exit "Failed to create build directory '$build_dir'."
fi

# Navigate to build directory
echo "Changing directory to '$build_dir'..."
if ! cd "$build_dir"; then
    error_exit "Failed to navigate to '$build_dir'. Make sure you are running this script from the project root (parent of the 'unit' directory)."
fi

# Configure CMake
echo "Running CMake..."
cmake_options_array=()
cmake_options_array+=("-DCMAKE_CXX_STANDARD=20")
cmake_options_array+=("-DCMAKE_CXX_STANDARD_REQUIRED=ON")
cmake_options_array+=("-DCMAKE_POLICY_VERSION_MINIMUM=3.15")
cmake_options_array+=("-DASTARTE_PUBLIC_SPDLOG_DEP=ON")
cmake_options_array+=("-DASTARTE_PUBLIC_PROTO_DEP=ON")
cmake_options_array+=("-DCMAKE_POSITION_INDEPENDENT_CODE=ON")

if [[ "$transport" == "grpc" ]]; then
    cmake_options_array+=("-DASTARTE_TRANSPORT_GRPC=ON")
else
    cmake_options_array+=("-DASTARTE_TRANSPORT_GRPC=OFF")
fi

if [ "$system_transport" = true ] && [[ "$transport" == "grpc" ]]; then
    cmake_options_array+=("-DASTARTE_USE_SYSTEM_GRPC=ON")
elif [ "$system_transport" = true ] && [[ "$transport" == "mqtt" ]]; then
    cmake_options_array+=("-DASTARTE_USE_SYSTEM_MQTT=ON")
fi

echo "CMake options: ${cmake_options_array[*]}"
if ! cmake "${cmake_options_array[@]}" ..; then
    error_exit "CMake configuration failed."
fi

# Build the project
echo "Building with make -j $jobs ..."
if ! make -j "$jobs"; then
    error_exit "Make build failed."
fi

# Run tests using CTest
echo "Running tests with ctest..."
if ! ctest --output-on-failure --tests-regex "^AstarteTest.*"; then
    error_exit "CTest execution failed or some tests failed."
fi
