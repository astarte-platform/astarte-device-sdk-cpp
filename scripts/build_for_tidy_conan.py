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

import sys
import os
import subprocess
import argparse


def error_exit(message):
    """
    Prints an error message to stderr and exits with status code 1.

    Args:
        message (str): The error message to display before exiting.
    """
    print(f"Error: {message}", file=sys.stderr)
    sys.exit(1)


def run_command(command, cwd=None, error_message=None):
    """
    Runs a shell command using subprocess and handles errors.

    Args:
        command (list): The shell command to execute, provided as a list of strings.
        cwd (str, optional): The working directory in which to run the command. Defaults to None.
        error_message (str, optional): A custom error message to display if the command fails.
                                       If None, a default message showing the command is used.
    """
    try:
        subprocess.run(command, check=True, cwd=cwd)
    except subprocess.CalledProcessError:
        if error_message:
            error_exit(error_message)
        else:
            error_exit(f"Command failed: {' '.join(command)}")


def build_for_tidy_with_conan(lib_src_dir, transport):
    """
    Builds the library with Conan, and generates the library's compilation database.

    Args:
        lib_src_dir (str): The absolute or relative path to the library source directory.
        transport (str): The transport layer to configure for the build (must be 'grpc' or 'mqtt').
    """

    # --- Argument Validation ---
    if not lib_src_dir:
        error_exit("Library source directory not provided.")

    if not os.path.isdir(lib_src_dir):
        error_exit(f"Library source directory does not exist: {lib_src_dir}")

    if transport not in ["grpc", "mqtt"]:
        error_exit("Transport should one of: 'grpc' or 'mqtt'.")

    # Detect the default conan profile
    print("Detecting Conan profile...")
    run_command(
        ["conan", "profile", "detect", "--exist-ok"],
        error_message="Conan profile detection failed.",
    )

    # --- Build the library to generate compile_commands.json ---
    print("Building library to generate compilation database...")

    conan_lib_build_cmd = [
        "conan",
        "build",
        lib_src_dir,
        "--output-folder=build",
        "--build=missing",
        f"--options=&:transport={transport}",
        "--settings=build_type=Debug",
        "--settings=compiler.cppstd=20",
        "--settings:build=compiler.cppstd=20",
        "--conf=tools.cmake.cmaketoolchain:extra_variables={'CMAKE_EXPORT_COMPILE_COMMANDS': 'ON'}",
    ]

    run_command(conan_lib_build_cmd, error_message="Failed to build library.")

    print("Conan build process complete.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Build library as Conan project for clang-tidy.")

    parser.add_argument("lib_src_dir", help="The path to the library source directory.")
    parser.add_argument("transport", choices=["grpc", "mqtt"], help="The transport system to use.")

    args = parser.parse_args()

    build_for_tidy_with_conan(args.lib_src_dir, args.transport)
