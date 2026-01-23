#!/bin/bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

# --- Configuration ---
fresh_mode=false
transport=grpc
jobs=$(nproc --all)
project_root=$(pwd) # Assuming this script is always run from the root of this project
venv_dir=".venv"
external_tools=false
conan_package_name="conan"
conan_package_version="2.20.1"
clang_tidy_package_name="clang-tidy"
clang_tidy_package_version="19.1.0"
jsonschema_package_name="jsonschema"
jsonschema_package_version="4.25.1"

# --- Helper Functions ---
display_help() {
    cat << EOF
Usage: $0 [OPTIONS]

Description:
  This script configures and builds the project, then runs clang-tidy.

Options:
  --fresh               Clear out the build directory ($build_dir) before processing.
  --transport <TR>      Specify the transport to use (mqtt or grpc). Default: $transport.
  -j, --jobs <N>        Specify the number of parallel jobs for make. Default: $jobs.
  -h, --help            Show this help message and exit.

Conan options:
  --ext-tools    Do not setup the venv and python tooling for the build within the script.
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

# --- Environment and dependency setup ---
build_dir="${project_root}/build"
cmake_user_presets="${project_root}/CMakeUserPresets.json"

# shellcheck source=/dev/null
source ./scripts/setup_python.sh
if [ "$external_tools" = false ]; then
    setup_python_venv $venv_dir
    install_conan $conan_package_name $conan_package_version
fi
install_tidy $clang_tidy_package_name $clang_tidy_package_version
install_json $jsonschema_package_name $jsonschema_package_version

# --- Clean previous builds ---
if [ "$fresh_mode" = true ]; then
    chmod +x ./scripts/clean_sample.py
    ./scripts/clean_sample.py "--build_dir=$build_dir" "--deps_mgmt=conan" "--presets_file=$cmake_user_presets"
fi

--- Perform a build of the sample ---
chmod +x ./scripts/build_for_tidy_conan.py
./scripts/build_for_tidy_conan.py "$(pwd)" "$transport" || error_exit "Conan build failed"

# --- Run clang-tidy ---
echo "Running clang-tidy..."

# Prepare clang-tidy options
tidy_options_array=()
tidy_options_array+=("-warnings-as-errors=*")
tidy_options_array+=("-extra-arg=-std=c++20")
tidy_options_array+=("-header-filter=.*astarte-device-sdk-cpp.*")
tidy_options_array+=("-exclude-header-filter=.*\\.pb\\.h")
tidy_options_array+=("-p=${build_dir}")

# Define source files for clang-tidy
source_files=(
    "src/"*.cpp
)
if [ "$transport" == "grpc" ]; then
    source_files+=("src/grpc/"*.cpp)
else
    source_files+=("src/mqtt/"*.cpp)
fi

echo "Analyzing ${#source_files[@]} file(s) with clang-tidy:"
printf ' - %s\n' "${source_files[@]}"
if ! printf "%s\0" "${source_files[@]}" | xargs -0 -P "$jobs" -n 1 clang-tidy "${tidy_options_array[@]}"; then
    error_exit "clang-tidy analysis failed."
fi
