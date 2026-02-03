#!/bin/bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

# --- Configuration ---
fresh_mode=false
external_tools=false
transport=grpc
end_to_end_src_dir="end_to_end"
build_dir="${end_to_end_src_dir}/build"
cmake_user_presets="${end_to_end_src_dir}/CMakeUserPresets.json"
venv_dir=".venv"
conan_package_name="conan"
conan_package_version="2.20.1"

# --- Helper Functions ---
display_help() {
    cat << EOF
Usage: $0 [OPTIONS]

Builds the end-to-end samples.

Options:
  --fresh             Build from scratch (removes $build_dir).
  --transport <TR>    Specify the transport to use. One of: grpc (default) or mqtt.
  --ext-tools         Do not setup the venv and python tooling for the build within the script.
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
        --transport)
            transport="$2"
            if [[ ! "$transport" =~ ^('mqtt'|'grpc')$ ]]; then
                error_exit "Invalid transport '$transport'. Use mqtt or grpc."
            fi
            shift 2
            ;;
        --ext-tools) external_tools=true; shift ;;
        -h|--help) display_help; exit 0 ;;
        *) display_help; error_exit "Unknown option: $1" ;;
    esac
done

# --- Build Logic ---
echo "Configuration:"
echo "  Fresh mode: $fresh_mode"
echo "  Transport: $transport"
echo "  External tools: $external_tools"
echo ""

# --- Environment and dependency setup ---
if [ "$external_tools" = false ]; then
    # shellcheck source=/dev/null
    source ./scripts/setup_python.sh
    setup_python_venv $venv_dir
    install_conan $conan_package_name $conan_package_version
fi

# Clean build directory if --fresh is set
if [ "$fresh_mode" = true ]; then
    echo "Fresh build requested. Removing $build_dir, $cmake_user_presets and conan library..."
    rm -rf "$build_dir"
    rm -f "$cmake_user_presets"
    conan remove astarte-device-sdk -c
    echo "Cleaning Conan build and source cache to free space..."
    conan cache clean "*" --source --download
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
conan_options_array+=("--options=transport=$transport")
if ! conan create . "${conan_options_array[@]}"; then
    error_exit "Conan package creation failed for the library."
fi

echo "Cleaning intermediate build artifacts..."
if ! conan cache clean "*" --source --download; then
    error_exit "Conan cache cleaning failed for the end to end tests."
fi

# --- Run conan conan on the sample ---
echo "Running Conan for the end to end tests..."

# Enter the sample folder
cd "${end_to_end_src_dir}" || error_exit "Failed to navigate to $end_to_end_src_dir"

# Build the sample
if ! conan build . --output-folder=build "${conan_options_array[@]}"; then
    error_exit "Conan build failed for the end to end tests."
fi

# Clean the Conan cache
if ! conan cache clean "*" --source --download; then
    error_exit "Conan cache cleaning failed for the end to end tests."
fi

echo "Build complete the end to end tests. Executable should be in: $build_dir/"
