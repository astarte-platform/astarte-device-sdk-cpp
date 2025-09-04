#!/bin/bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

# --- Configuration ---
fresh_mode=false
system_grpc=false
jobs=$(nproc --all)
sample_to_build=""
qt_path="$HOME/Qt/6.8.1/gcc_64/lib/cmake/Qt6"
qt_version=6
project_root=$(pwd) # Assuming this script is always run from the root of this project

# --- Helper Functions ---
display_help() {
    cat << EOF
Usage: $0 <sample_name> [OPTIONS]

<sample_name> can be 'simple' or 'qt'.

Common Options:
  --fresh         Build the sample from scratch (removes its build directory).
  --system_grpc   Use the system gRPC instead of building it from scratch.
  -j, --jobs <N>  Specify the number of parallel jobs for make. Default: $jobs.
  -h, --help      Display this help message.

Qt Sample Specific Options:
  --qt_path <PATH>   Path to QtXConfig.cmake (e.g., ~/Qt/6.8.1/gcc_64/lib/cmake/Qt6). Default: $qt_path.
  --qt_version <VER> Qt version to use (5 or 6). Default: $qt_version.
EOF
}

error_exit() {
    echo "Error: $1" >&2
    exit 1
}

# --- Argument Parsing ---
if [[ -z "$1" ]]; then
    display_help
    error_exit "No sample specified. Please choose 'simple' or 'qt'."
fi

sample_to_build="$1"
shift

if [[ "$sample_to_build" != "simple" && "$sample_to_build" != "qt" ]]; then
    display_help
    error_exit "Invalid sample name: '$sample_to_build'. Must be 'simple' or 'qt'."
fi

# Now parse the rest of the arguments
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
        --qt_path)
            qt_path="$2"
            shift 2
            ;;
        --qt_version)
            if [[ "$2" == "6" ]]; then
                qt_version=6
            elif [[ "$2" == "5" ]]; then
                qt_version=5
            else
                error_exit "Invalid Qt version: $2 (expected 5 or 6)."
            fi
            shift 2
            ;;
        -h|--help) display_help; exit 0 ;;
        *) display_help; error_exit "Unknown option: $1" ;;
    esac
done

# --- Build Logic ---

case "$sample_to_build" in
    simple)
        sample_src_dir="samples/simple"
        build_dir="${sample_src_dir}/build"
        ;;
    qt)
        sample_src_dir="samples/qt"
        build_dir="${sample_src_dir}/build"
        ;;
esac

# Clean build if --fresh is set
if [ "$fresh_mode" = true ]; then
    echo "Fresh build requested. Removing $build_dir..."
    rm -rf "$build_dir"
fi

# Create build directory if it doesn't exist
mkdir -p "$build_dir"

# Navigate to build directory
cd "$build_dir" || error_exit "Failed to navigate to $build_dir"

# --- Configure CMake Options ---
echo "Running CMake for $sample_to_build sample..."
cmake_options_array=()
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
        error_exit "Error: --qt_path not specified for Qt sample."
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
if ! cmake "${cmake_options_array[@]}" "$project_root/$sample_src_dir"; then
    error_exit "CMake configuration failed for $sample_to_build sample."
fi

# Build the project
echo "Building $sample_to_build sample with make -j $jobs ..."
if ! make -j "$jobs"; then
    error_exit "Make build failed for $sample_to_build sample."
fi

echo "Build complete for $sample_to_build sample. Executable should be in: $build_dir/"
