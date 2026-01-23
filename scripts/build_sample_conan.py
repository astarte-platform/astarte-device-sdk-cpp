#!/usr/bin/env python3

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

import sys
import os
import subprocess
import argparse


def error_exit(message):
    """
    Prints an error message to stderr and exits with status code 1.

    Args:
        message (str): The error message to display.
    """
    print(f"Error: {message}", file=sys.stderr)
    sys.exit(1)


def run_command(command, cwd=None, error_message=None):
    """
    Runs a shell command using subprocess and handles errors.

    Args:
        command (list): A list of strings representing the command and its arguments.
        cwd (str, optional): The directory to change to before running the command. Defaults to None.
        error_message (str, optional): A custom error message to print if the command fails.
                                       If None, prints the command itself.
    """
    try:
        subprocess.run(command, check=True, cwd=cwd)
    except subprocess.CalledProcessError:
        if error_message:
            error_exit(error_message)
        else:
            error_exit(f"Command failed: {' '.join(command)}")


def build_sample_with_conan(lib_src_dir, sample_to_build, transport, qt_version=None):
    """
    Builds the Conan package from the provided directory and then builds a specified sample.

    Args:
        lib_src_dir (str): The path to the library source directory.
        sample_to_build (str): The name of the sample to build.
        transport (str): The transport system to use ('grpc' or 'mqtt').
        qt_version (str, optional): The major version of Qt to use if the sample is "grpc/qt".
    """

    # --- Argument Validation ---
    if not lib_src_dir:
        error_exit("Library source directory not provided.")

    if not os.path.isdir(lib_src_dir):
        error_exit(f"Library source directory does not exist: {lib_src_dir}")

    if not sample_to_build:
        error_exit("Sample to build not provided.")

    if transport not in ["grpc", "mqtt"]:
        error_exit("Transport should one of: 'grpc' or 'mqtt'.")

    if sample_to_build == "grpc/qt" and not qt_version:
        error_exit("Qt version must be provided for the 'qt' sample.")

    # Detect the default conan profile
    print("Detecting Conan profile...")
    run_command(
        ["conan", "profile", "detect", "--exist-ok"],
        error_message="Conan profile detection failed.",
    )

    # --- Run conan create on the library ---
    print("Creating the library using Conan...")

    # Define base settings
    conan_create_cmd = [
        "conan",
        "create",
        lib_src_dir,
        "--build=missing",
        f"--options=&:transport={transport}",
        "--settings=build_type=Debug",
        "--settings=compiler.cppstd=20",
        "--settings:build=compiler.cppstd=20",
    ]

    run_command(conan_create_cmd, error_message="Conan package creation failed for the library.")

    # --- Run conan build on the sample ---
    print(f"Running Conan for {sample_to_build} sample...")

    # Construct the sample directory path
    sample_src_dir = os.path.join(lib_src_dir, "samples", sample_to_build)

    if not os.path.exists(sample_src_dir):
        error_exit(f"Failed to find sample directory at: {sample_src_dir}")

    # Build options for the sample
    conan_build_cmd = [
        "conan",
        "build",
        ".",
        "--output-folder=build",
        f"--options=astarte-device-sdk/*:transport={transport}",
        "--build=missing",
        "--settings=build_type=Debug",
        "--settings=compiler.cppstd=20",
    ]

    # Handle Qt specific options
    if sample_to_build == "grpc/qt":
        qt_bool = "True" if str(qt_version) == "6" else "False"
        conan_build_cmd.append(f"--options=qt6={qt_bool}")

    # Run the build command inside the sample directory
    run_command(
        conan_build_cmd,
        cwd=sample_src_dir,
        error_message=f"Conan build failed for {sample_to_build} sample.",
    )

    print("Conan build process complete.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Build Conan project and sample.")

    parser.add_argument("lib_src_dir", help="The path to the library source directory.")
    parser.add_argument(
        "sample_to_build", help="The name of the sample to build (e.g., 'grpc/qt')."
    )
    parser.add_argument("transport", choices=["grpc", "mqtt"], help="The transport system to use.")
    parser.add_argument(
        "qt_version",
        nargs="?",
        default=None,
        help="The major version of Qt (required if sample is 'grpc/qt').",
    )

    args = parser.parse_args()

    build_sample_with_conan(args.lib_src_dir, args.sample_to_build, args.transport, args.qt_version)
