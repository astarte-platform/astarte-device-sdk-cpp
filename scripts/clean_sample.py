#!/usr/bin/env python3

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

"""
Checked using pylint with the following command (pip install pylint):
python -m pylint --rcfile=./scripts/.pylintrc ./scripts/*.py
Formatted using black with the following command (pip install black):
python -m black --line-length 100 ./scripts/*.py
"""

import os
import sys
import shutil
import subprocess
import argparse
import stat


def on_rm_error(func, path, exc_info):
    """
    Error handler for shutil.rmtree.
    If the error is due to an access error (read only file),
    it attempts to add write permission and then retries.
    """
    if not os.access(path, os.W_OK):
        os.chmod(path, stat.S_IWUSR)
        func(path)
    else:
        raise


def error_exit(message):
    """
    Prints an error message to stderr and exits with status code 1.

    Args:
        message (str): The error message to display.
    """
    print(f"Error: {message}", file=sys.stderr)
    sys.exit(1)


def clean_sample(build_dir, deps_management, cmake_user_presets):
    """
    Cleans previous build artifacts.

    Args:
        build_dir (str): The path to the build directory to remove.
        deps_management (str): The dependency management system used.
        cmake_user_presets (str): The path to the CMakeUserPresets.json file (only used if deps_management is "conan").
    """

    print("Fresh build requested. Removing old build files...")

    if os.path.exists(build_dir):
        try:
            shutil.rmtree(build_dir, onerror=on_rm_error)
            print(f"Removed directory: {build_dir}")
        except OSError as e:
            if os.name == "nt":
                # If on Windows use 'rmdir' which is more robust.
                print(f"Standard delete failed. Attempting Windows system force delete...")
                subprocess.run(f'rmdir /s /q "{build_dir}"', shell=True, check=False)
                if not os.path.exists(build_dir):
                    print(f"Removed directory (via system command): {build_dir}")
                else:
                    error_exit(f"Could not remove {build_dir} even with force delete.")
            else:
                # If not on Windows, re-raise the original error
                raise

    if deps_management == "conan":
        if cmake_user_presets and os.path.exists(cmake_user_presets):
            try:
                os.remove(cmake_user_presets)
                print(f"Removed file: {cmake_user_presets}")
            except OSError as e:
                print(f"Error removing {cmake_user_presets}: {e}", file=sys.stderr)

        print("Removing Conan package (astarte-device-sdk)...")
        try:
            subprocess.run(["conan", "remove", "astarte-device-sdk", "-c"], check=False)
        except FileNotFoundError:
            print("Warning: Could not run 'conan remove'. Skipping.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Clean build artifacts for Astarte Device SDK samples."
    )

    # Paths and configurations
    parser.add_argument("--build_dir", required=True, help="Path to the build directory.")
    parser.add_argument(
        "--deps_mgmt",
        default="conan",
        choices=["conan", "fetch", "system"],
        help="Dependency management strategy.",
    )
    parser.add_argument(
        "--presets_file",
        default=None,
        help="Path to CMakeUserPresets.json (required for conan cleanup).",
    )

    args = parser.parse_args()

    clean_sample(
        build_dir=args.build_dir,
        deps_management=args.deps_mgmt,
        cmake_user_presets=args.presets_file,
    )
