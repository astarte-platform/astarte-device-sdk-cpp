#!/bin/bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

# --- Configuration ---
fresh_mode=false
system_grpc=false
cpp_standard=20
jobs=$(nproc --all)
build_dir="unit/build"

# --- Helper Functions ---
display_help() {
    cat << EOF
Usage: $0 [OPTIONS]

Build and run unit tests.

Options:
  --fresh             Build from scratch (removes $build_dir).
  --stdcpp <VER>      Specify the C++ standard to use (17 or 20). Default: $cpp_standard.
  --system_grpc       Use system gRPC. If not set, gRPC will be built from source (if configured in CMake).
  -j, --jobs <N>      Specify the number of parallel jobs for make. Default: $jobs.
  -h, --help          Display this help message.
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
        --stdcpp)
            cpp_standard="$2"
            if [[ ! "$cpp_standard" =~ ^(17|20)$ ]]; then
                error_exit "Invalid C++ standard '$cpp_standard'. Use 17 or 20."
            fi
            shift 2
            ;;
        --system_grpc) system_grpc=true; shift ;;
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
echo "  C++ Standard: $cpp_standard"
echo "  Jobs: $jobs"
echo "  Build Directory: $build_dir"
echo "  Fresh Mode: $fresh_mode"
echo "  Use System gRPC: $system_grpc"
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
cmake_options_array+=("-DCMAKE_CXX_STANDARD=$cpp_standard")
cmake_options_array+=("-DCMAKE_CXX_STANDARD_REQUIRED=ON")
cmake_options_array+=("-DCMAKE_POLICY_VERSION_MINIMUM=3.15")
cmake_options_array+=("-DASTARTE_PUBLIC_SPDLOG_DEP=ON")
cmake_options_array+=("-DASTARTE_PUBLIC_PROTO_DEP=ON")
if [ "$system_grpc" = true ]; then
    cmake_options_array+=("-DASTARTE_USE_SYSTEM_GRPC=ON")
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
