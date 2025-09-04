#!/bin/bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

# --- Configuration ---
fresh_mode=false
system_grpc=false
jobs=$(nproc --all)
project_root=$(pwd) # Assuming this script is always run from the root of this project
sample_src_dir="samples/simple"
build_dir="${sample_src_dir}/build"
venv_dir=".venv"
clang_tidy_package_name="clang-tidy"
clang_tidy_package_version="19.1.0"

# --- Helper Functions ---
display_help() {
    cat << EOF
Usage: $0 [OPTIONS]

Description:
  This script configures and builds the project, then runs clang-tidy.

Options:
  --fresh            Clear out the build directory ($build_dir) before processing.
  --system_grpc      Use the system gRPC instead of building it from scratch.
  -j, --jobs <N>     Specify the number of parallel jobs for make.
                     Default: $jobs (uses N-1 cores, or 1 if only 1 core).
  -h, --help         Show this help message and exit.
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

# --- Environment and Dependency Setup ---
echo "Setting up Python environment and dependencies..."

# Check for python3
if ! command -v python3 &> /dev/null; then
    error_exit "python3 could not be found. Please install Python 3."
fi

# Create virtual environment if it doesn't exist
if [ ! -d "$venv_dir" ]; then
    echo "Creating Python virtual environment in $venv_dir..."
    if ! python3 -m venv "$venv_dir"; then
        error_exit "Failed to create Python virtual environment."
    fi
fi

# Activate virtual environment
# shellcheck source=/dev/null
if ! source "$venv_dir/bin/activate"; then
    error_exit "Failed to activate Python virtual environment."
fi

# Upgrade pip
echo "Upgrading pip..."
if ! pip install --upgrade pip; then
    error_exit "Failed to upgrade pip."
fi

# Install or verify clang-tidy version
echo "Checking/installing $clang_tidy_package_name version $clang_tidy_package_version..."
installed_version=$(pip show "$clang_tidy_package_name" | grep Version | awk '{print $2}' || true)
if [ "$installed_version" != "$clang_tidy_package_version" ]; then
    echo "Installing $clang_tidy_package_name==$clang_tidy_package_version..."
    if ! pip install "$clang_tidy_package_name==$clang_tidy_package_version"; then
        error_exit "Failed to install $clang_tidy_package_name version $clang_tidy_package_version."
    fi
else
    echo "$clang_tidy_package_name version $clang_tidy_package_version is already installed."
fi

# --- Build Logic ---

# Clean build if --fresh is set
if [ "$fresh_mode" = true ]; then
    echo "Fresh mode enabled. Removing $build_dir..."
    rm -rf "$build_dir"
fi

# Create build directory if it doesn't exist
mkdir -p "$build_dir"

# Navigate to build directory
cd "$build_dir" || error_exit "Failed to navigate to $build_dir"

# Run CMake
echo "Running CMake..."
cmake_options_array=()
cmake_options_array+=("-DCMAKE_CXX_STANDARD=20")
cmake_options_array+=("-DCMAKE_CXX_STANDARD_REQUIRED=ON")
cmake_options_array+=("-DCMAKE_POLICY_VERSION_MINIMUM=3.15")
cmake_options_array+=("-DASTARTE_PUBLIC_SPDLOG_DEP=ON")
cmake_options_array+=("-DSAMPLE_USE_SYSTEM_ASTARTE_LIB=OFF")
cmake_options_array+=("-DCMAKE_EXPORT_COMPILE_COMMANDS=ON")
if [ "$system_grpc" = true ]; then
    cmake_options_array+=("-DASTARTE_USE_SYSTEM_GRPC=ON")
fi

if ! cmake "${cmake_options_array[@]}" "$project_root/$sample_src_dir"; then
    error_exit "CMake configuration failed."
fi

# Build the project
echo "Building with make -j $jobs ..."
if ! make -j "$jobs"; then
    error_exit "Make build failed."
fi

# Return to the original directory from where the script was called,
# before running clang-tidy, as clang-tidy paths might be relative to project root.
echo "Returning to $project_root..."
if ! cd "$project_root"; then
    error_exit "Failed to return to original directory: $project_root"
fi

# --- Run clang-tidy ---
echo "Running clang-tidy..."

# Prepare clang-tidy options
tidy_options_array=()
tidy_options_array+=("-warnings-as-errors=*")
tidy_options_array+=("-extra-arg=-std=c++20")
tidy_options_array+=("-header-filter='.*(?<!pb)\.hpp$'")
tidy_options_array+=("-p=${build_dir}")

# Define source files for clang-tidy
source_files=(
    "src/"*.cpp
)

echo "Analyzing ${#source_files[@]} file(s) with clang-tidy..."
if ! clang-tidy "${tidy_options_array[@]}" "${source_files[@]}"; then
    error_exit "clang-tidy analysis failed."
fi
