#!/bin/bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

# --- Configuration ---
fresh_mode=false
system_grpc=false
jobs=$(nproc --all)
build_dir="end_to_end/build"

# --- Helper Functions ---
display_help() {
    cat << EOF
Usage: $0 [OPTIONS]

Builds the end-to-end samples.

Options:
  --fresh         Build from scratch (removes $build_dir).
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
echo "  Fresh Mode: $fresh_mode"
echo "  Build Directory: $build_dir"
echo "  Parallel Jobs (make): $jobs"
echo "  Use System gRPC: $system_grpc"
echo ""

# Clean build directory if --fresh is set
if [ "$fresh_mode" = true ]; then
    if [ -d "$build_dir" ]; then
        echo "Fresh build requested. Removing $build_dir..."
        if ! rm -rf "$build_dir"; then
            error_exit "Failed to remove build directory '$build_dir'."
        fi
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
    error_exit "Failed to navigate to '$build_dir'."
fi

# Configure CMake
echo "Running CMake..."
cmake_options_array=()
if [ "$system_grpc" = true ]; then
    cmake_options_array+=("-DASTARTE_USE_SYSTEM_GRPC=ON")
fi
cmake_options_array+=("-DCMAKE_POSITION_INDEPENDENT_CODE=ON")
cmake_options_array+=("-DCMAKE_POLICY_VERSION_MINIMUM=3.15")
cmake_options_array+=("-DASTARTE_PUBLIC_SPDLOG_DEP=ON")

echo "CMake options: ${cmake_options_array[*]}"
if ! cmake "${cmake_options_array[@]}" ..; then
    error_exit "CMake configuration failed."
fi

# Build the project
echo "Building with make -j $jobs ..."
if ! make -j "$jobs"; then
    error_exit "Make build failed."
fi
