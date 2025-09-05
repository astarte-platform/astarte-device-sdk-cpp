#!/bin/bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

# --- Configuration ---
fresh_mode=false
sample_to_build=""
venv_dir=".venv"
conan_package_name="conan"
conan_package_version="2.20.1"

# --- Helper Functions ---
display_help() {
    cat << EOF
Usage: $0 <sample_name> [OPTIONS]

<sample_name> can be 'simple'.

Common Options:
  --fresh         Build the sample from scratch (removes its build directory).
  -h, --help      Display this help message.
EOF
}

error_exit() {
    echo "Error: $1" >&2
    exit 1
}

# --- Argument Parsing ---
if [[ -z "$1" ]]; then
    display_help
    error_exit "No sample specified. Please choose 'simple'."
fi

sample_to_build="$1"
shift

if [[ "$sample_to_build" != "simple" ]]; then
    display_help
    error_exit "Invalid sample name: '$sample_to_build'. Must be 'simple'."
fi

# Now parse the rest of the arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --fresh) fresh_mode=true; shift ;;
        -h|--help) display_help; exit 0 ;;
        *) display_help; error_exit "Unknown option: $1" ;;
    esac
done

# --- Environment and dependency setup ---
echo "Setting up Python environment and dependencies for conan..."

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

# Install or verify conan version
echo "Checking/installing $conan_package_name version $conan_package_version..."
installed_version=$(pip show "$conan_package_name" | grep Version | awk '{print $2}' || true)
if [ "$installed_version" != "$conan_package_version" ]; then
    echo "Installing $conan_package_name==$conan_package_version..."
    if ! pip install "$conan_package_name==$conan_package_version"; then
        error_exit "Failed to install $conan_package_name version $conan_package_version."
    fi
else
    echo "$conan_package_name version $conan_package_version is already installed."
fi

# --- Build Logic ---
case "$sample_to_build" in
    simple)
        sample_src_dir="samples/simple"
        build_dir="${sample_src_dir}/build"
        cmake_user_presets="${sample_src_dir}/CMakeUserPresets.json"
        ;;
esac

# Clean build if --fresh is set
if [ "$fresh_mode" = true ]; then
    echo "Fresh build requested. Removing $build_dir, $cmake_user_presets and conan library..."
    rm -rf "$build_dir"
    rm -f "$cmake_user_presets"
    conan remove astarte-device-sdk -c
fi

# Detect the default conan profile
if ! conan profile detect --exist-ok; then
    error_exit "Conan profile detection failed."
fi

# --- Run conan conan on the library ---
echo "Creating the library using Conan..."

conan_options_array=()
conan_options_array+=("--build=missing")
conan_options_array+=("--settings=build_type=Debug")
conan_options_array+=("--settings=compiler.cppstd=20")
if ! conan create . "${conan_options_array[@]}"; then
    error_exit "Conan package creation failed for the library."
fi

# --- Run conan conan on the sample ---
echo "Running Conan for $sample_to_build sample..."

# Enter the sample folder
cd "${sample_src_dir}" || error_exit "Failed to navigate to $sample_src_dir"

# Install the sample
if ! conan install . --output-folder=build "${conan_options_array[@]}"; then
    error_exit "Conan install failed for $sample_to_build sample."
fi

# Build the sample
if ! conan build . --output-folder=build "${conan_options_array[@]}"; then
    error_exit "Conan install failed for $sample_to_build sample."
fi

echo "Build complete for $sample_to_build sample. Executable should be in: $build_dir/"
