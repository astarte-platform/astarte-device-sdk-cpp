#!/bin/bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

# --- Configuration ---
fresh_mode=false
system_grpc=false
cpp_standard=20
jobs=$(nproc --all)
build_dir="samples/grpc/build"

# --- Helper Functions ---
display_help() {
    cat << EOF
Usage: $0 [OPTIONS]

Options:
  --fresh         Build the samples from scratch (removes $build_dir).
  --stdcpp <VER>  Specify the C++ standard to use (17 or 20). Default: $cpp_standard.
  --system_grpc   Use the system gRPC instead of building it from scratch.
  -j, --jobs <N>  Specify the number of parallel jobs for make. Default: $jobs.
  -h, --help      Display this help message.
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

# Clean build if --fresh is set
if [ "$fresh_mode" = true ]; then
    echo "Fresh build requested. Removing $build_dir..."
    rm -rf "$build_dir"
fi

# Create build directory if it doesn't exist
mkdir -p "$build_dir"

# Navigate to build directory
cd "$build_dir" || error_exit "Failed to navigate to $build_dir"

# Run CMake
echo "Running CMake..."
cmake_options_array=()
if [ "$system_grpc" = true ]; then
    cmake_options_array+=("-DUSE_SYSTEM_GRPC=ON")
fi
cmake_options_array+=("-DCMAKE_CXX_STANDARD=$cpp_standard")
cmake_options_array+=("-DCMAKE_CXX_STANDARD_REQUIRED=ON")
cmake_options_array+=("-DCMAKE_POLICY_VERSION_MINIMUM=3.15")

if ! cmake "${cmake_options_array[@]}" ..; then
    error_exit "CMake configuration failed."
fi

# Build the project
echo "Building with make -j $jobs ..."
if ! make -j "$jobs"; then
    error_exit "Make build failed."
fi
