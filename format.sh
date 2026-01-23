#!/bin/bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

# --- Configuration ---
check_only=false
venv_dir=".venv"
clang_format_package_name="clang-format"
clang_format_package_version="21.1.8"
gersemi_package_name="gersemi"
gersemi_package_version="0.21.0"
end_to_end_testcases="end_to_end/include/testcases"
end_to_end_testcases_format="end_to_end/include/testcases/.clang-format"

# --- Helper Functions ---
display_help() {
    cat << EOF
Usage: $0 [OPTIONS]

Description:
  This script checks or applies clang-format to specified C++ source and header files.

Options:
  --check-only    Run clang-format in check mode (outputs diff and errors but does not modify files).
  -h, --help      Show this help message and exit.
EOF
}

error_exit() {
    echo "Error: $1" >&2
    exit 1
}

run_clang_format() {
    local style="$1"
    local context_name="$2"
    shift 2
    local files=("${@}")

    if [ ${#files[@]} -eq 0 ]; then
        error_exit "No files found for ${context_name}."
    fi

    echo "Executing: clang-format --style='${style}' ${command_args[*]} ${files[*]}"
    if ! clang-format --style="${style}" "${command_args[@]}" "${files[@]}"; then
        if [ "$check_only" = true ]; then
            error_exit "clang-format check for ${context_name} failed. Some files need formatting."
        else
            error_exit "clang-format application for ${context_name} failed."
        fi
    fi
}

# --- Argument Parsing ---
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --check-only)
            check_only=true
            shift
            ;;
        -h|--help) display_help; exit 0;;
        *) display_help; error_exit "Unknown option: $1";;
    esac
done

# --- Environment and Dependency Setup ---
echo "Setting up Python environment and dependencies for clang-format and gersemi..."

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

# Install or verify clang-format version
echo "Checking/installing $clang_format_package_name version $clang_format_package_version..."
installed_version=$(pip show "$clang_format_package_name" | grep Version | awk '{print $2}' || true)
if [ "$installed_version" != "$clang_format_package_version" ]; then
    echo "Installing $clang_format_package_name==$clang_format_package_version..."
    if ! pip install "$clang_format_package_name==$clang_format_package_version"; then
        error_exit "Failed to install $clang_format_package_name version $clang_format_package_version."
    fi
else
    echo "$clang_format_package_name version $clang_format_package_version is already installed."
fi

# Install or verify gersemi version
echo "Checking/installing $gersemi_package_name version $gersemi_package_version..."
installed_version=$(pip show "$gersemi_package_name" | grep Version | awk '{print $2}' || true)
if [ "$installed_version" != "$gersemi_package_version" ]; then
    echo "Installing $gersemi_package_name==$gersemi_package_version..."
    if ! pip install "$gersemi_package_name==$gersemi_package_version"; then
        error_exit "Failed to install $gersemi_package_name version $gersemi_package_version."
    fi
else
    echo "$clang_format_package_name version $clang_format_package_version is already installed."
fi

# --- File Formatting Logic ---

# Define file patterns to format/check
file_patterns=(
    "src/"*.cpp
	"src/grpc/"*.cpp
    "src/mqtt/"*.cpp
    "src/mqtt/connection/"*.cpp
    "include/astarte_device_sdk/"*.hpp
	"include/astarte_device_sdk/grpc/"*.hpp
    "include/astarte_device_sdk/mqtt/"*.hpp
    "include/astarte_device_sdk/mqtt/connection/"*.hpp
    "private/"*.hpp
	"private/grpc/"*.hpp
	"private/mqtt/"*.hpp
    "samples/"*/*/*.cpp
    "samples/"*/*/*.hpp
    "unit/"*.cpp
    "end_to_end/src/"*.cpp
    "end_to_end/include/"*.hpp
    "end_to_end/include/constants/"*.hpp
)
file_patterns_end_to_end_testcases=(
    "end_to_end/include/testcases/"*.hpp
	"end_to_end/include/testcases/mqtt/"*.hpp
)

command_args=()
if [ "$check_only" = true ]; then
    echo "Running clang-format in check mode (dry run)..."
    command_args+=("--dry-run" "-Werror")
else
    echo "Applying clang-format to files..."
    command_args+=("-i")
fi

# Run on standard files
run_clang_format "file" "standard files" "${file_patterns[@]}"

# Run on end-to-end test cases
run_clang_format "file:${end_to_end_testcases_format}" "end-to-end testcases" "${file_patterns_end_to_end_testcases[@]}"

if [ "$check_only" = true ]; then
    echo "Clang-format check passed. All specified files are correctly formatted."
else
    echo "Clang-format applied successfully."
fi

# Define cmake files to format/check
cmake_files=(
    "CMakeLists.txt"
    "cmake/AstarteGrpcTransport.cmake"
    "cmake/AstarteMqttTransport.cmake"
    "unit/CMakeLists.txt"
    "end_to_end/CMakeLists.txt"
    "samples/grpc/qt/CMakeLists.txt"
    "samples/grpc/native/CMakeLists.txt"
    "samples/mqtt/native/CMakeLists.txt"
)

gersemi_args=("--config" ".gersemirc")
if [ "$check_only" = true ]; then
    echo "Running gersemi in check mode..."
    gersemi_args+=("-c")
else
    echo "Applying gersemi to files..."
    gersemi_args+=("-i")
fi

# Run gersemi on all files at once
echo "Executing: gersemi ${gersemi_args[*]} ${cmake_files[*]}"
if ! gersemi "${gersemi_args[@]}" "${cmake_files[@]}"; then
    if [ "$check_only" = true ]; then
        error_exit "gersemi check failed. Some files need formatting."
    else
        error_exit "gersemi application failed."
    fi
fi

if [ "$check_only" = true ]; then
    echo "gersemi check passed. All specified files are correctly formatted."
else
    echo "gersemi applied successfully."
fi
