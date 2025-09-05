#!/bin/bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

# --- Configuration ---
sample_to_build=""
fresh_mode=false
deps_management="conan"
external_tools=false
qt_version=6
qt_path="$HOME/Qt/6.5.3/gcc_64/lib/cmake/Qt6"
venv_dir=".venv"
conan_package_name="conan"
conan_package_version="2.20.1"

# --- Helper functions ---
display_help() {
    cat << EOF
Usage: $0 <sample_name> [OPTIONS]

<sample_name> can be 'simple' or 'qt'.

Common options:
  --fresh        Build the sample from scratch (removes its build directory).
  --deps-mgmt    Select a possible dependency management strategy. One of: conan (default), fetch, system.
  -h, --help     Display this help message.

Conan options:
  --ext-tools    Do not setup the venv and python tooling for the build within the script.

Qt sample options:
  --qt_version        Qt version to use (5 or 6). Default: $qt_version.
  --qt_path <PATH>    Path to QtXConfig.cmake (e.g., ~/Qt/6.8.1/gcc_64/lib/cmake/Qt6). Default: $qt_path.

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

while [[ "$#" -gt 0 ]]; do
    case $1 in
        --fresh) fresh_mode=true; shift ;;
        --deps-mgmt)
            if [[ "$2" == "conan" ]]; then
                deps_management="conan"
            elif [[ "$2" == "fetch" ]]; then
                deps_management="fetch"
            elif [[ "$2" == "system" ]]; then
                deps_management="system"
            else
                error_exit "Invalid dependency management: $2 (expected conan, fetch or system)."
            fi
            shift 2
            ;;
        --ext-tools) external_tools=true; shift ;;
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
        --qt_path)
            qt_path="$2"
            shift 2
            ;;
        -h|--help) display_help; exit 0 ;;
        *) display_help; error_exit "Unknown option: $1" ;;
    esac
done

# --- Environment and dependency setup ---
case "$sample_to_build" in
    simple) sample_src_dir="samples/simple" ;;
    qt) sample_src_dir="samples/qt" ;;
esac
build_dir="${sample_src_dir}/build"
cmake_user_presets="${sample_src_dir}/CMakeUserPresets.json"

if [[ "$deps_management" = "conan" && "$external_tools" = false ]]; then
    # shellcheck source=/dev/null
    source ./scripts/setup_conan_env.sh
    setup_python_conan_env $venv_dir $conan_package_name $conan_package_version
fi

# --- Clean previous builds ---
if [ "$fresh_mode" = true ]; then
    echo "Fresh build requested. Removing old build files..."
    rm -rf "$build_dir"
    if [[ "$deps_management" = "conan" ]]; then
        rm -f "$cmake_user_presets"
        conan remove astarte-device-sdk -c
    fi
fi

# --- Perform a build with the desired dependency manager ---
if [[ "$deps_management" == "conan" ]]; then
    # shellcheck source=/dev/null
    source ./scripts/build_sample_conan.sh
    build_sample_with_conan "$(pwd)" "$sample_to_build" "$qt_version"
elif [[ "$deps_management" == "fetch" ]]; then
    # shellcheck source=/dev/null
    source ./scripts/build_sample_cmake.sh
    build_sample_with_cmake "$(pwd)" "$sample_to_build" "false" "$(nproc --all)" "$qt_path" "$qt_version"
else # "system"
    # shellcheck source=/dev/null
    source ./scripts/build_sample_cmake.sh
    build_sample_with_cmake "$(pwd)" "$sample_to_build" "true" "$(nproc --all)" "$qt_path" "$qt_version"
fi

echo "Build complete for $sample_to_build sample. Executable should be in: $build_dir/"
