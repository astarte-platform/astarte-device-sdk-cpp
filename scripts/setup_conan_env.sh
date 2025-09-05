#!/bin/bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

error_exit() {
    echo "Error: $1" >&2
    exit 1
}

# Sets up a Python virtual environment, activates it, and installs a specific version of Conan.
# This function is designed to be sourced and used in other bash scripts.
#
# Arguments:
#   $1: venv_dir - The directory path to create the virtual environment in.
#   $2: conan_package_name - The name of the conan package (e.g., "conan").
#   $3: conan_package_version - The specific version of the conan package to install.
#
# Example Usage:
#   source setup_conan_env.sh
#   setup_python_conan_env "./.venv" "conan" "2.0.17"
#
setup_python_conan_env() {
    local venv_dir="$1"
    local conan_package_name="$2"
    local conan_package_version="$3"

    # --- Argument Validation ---
    if [ -z "$venv_dir" ]; then
        error_exit "Virtual environment directory not provided."
    fi
    if [ -z "$conan_package_name" ]; then
        error_exit "Conan package name not provided."
    fi
    if [ -z "$conan_package_version" ]; then
        error_exit "Conan package version not provided."
    fi

    echo "Setting up Python environment and dependencies for $conan_package_name..."

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
    if ! python3 -m pip install --upgrade pip; then
        error_exit "Failed to upgrade pip."
    fi

    # Install or verify conan version
    echo "Checking/installing $conan_package_name version $conan_package_version..."
    local installed_version
    installed_version=$(pip show "$conan_package_name" | grep Version | awk '{print $2}' || true)
    if [ "$installed_version" != "$conan_package_version" ]; then
        echo "Installing $conan_package_name==$conan_package_version..."
        if ! python3 -m pip install "$conan_package_name==$conan_package_version"; then
            error_exit "Failed to install $conan_package_name version $conan_package_version."
        fi
    else
        echo "$conan_package_name version $conan_package_version is already installed."
    fi

    echo "Conan environment setup complete."
}
