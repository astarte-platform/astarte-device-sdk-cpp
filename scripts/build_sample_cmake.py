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


def build_sample_with_cmake(
    lib_src_dir, sample_to_build, jobs, *, system_grpc=False, qt_path=None, qt_version=None
):
    """
    Configures and builds a CMake project for a specified sample application.

    Args:
        lib_src_dir (str): The path to the library source directory.
        sample_to_build (str): The name of the sample being built.
        jobs (str): The number of parallel jobs for 'make'.
        system_grpc (bool): Whether to use system gRPC (Default: False).
        qt_path (str, optional): The path to the Qt installation (Required if sample is 'qt').
        qt_version (str, optional): The major version of Qt (Required if sample is 'qt').
    """

    # --- Argument Validation ---
    if not lib_src_dir:
        error_exit("Library source directory not provided.")
    if not sample_to_build:
        error_exit("Sample to build not provided.")
    if system_grpc is None:
        error_exit("System grpc option not provided.")

    if sample_to_build == "qt":
        if not qt_path:
            error_exit("Qt path must be provided for the 'qt' sample.")
        if not qt_version:
            error_exit("Qt version must be provided for the 'qt' sample.")

    # Define paths
    sample_src_dir = os.path.join(lib_src_dir, "samples", sample_to_build)
    build_dir = os.path.join(sample_src_dir, "build")

    # Create build directory if it doesn't exist
    os.makedirs(build_dir, exist_ok=True)

    # --- Configure CMake options ---
    print(f"Running CMake for {sample_to_build} sample...")

    cmake_options = [
        "-DCMAKE_CXX_STANDARD=20",
        "-DCMAKE_CXX_STANDARD_REQUIRED=ON",
        "-DCMAKE_POLICY_VERSION_MINIMUM=3.15",
        "-DASTARTE_PUBLIC_SPDLOG_DEP=ON",
        "-DSAMPLE_USE_SYSTEM_ASTARTE_LIB=OFF",
    ]

    # Handle system gRPC flag
    if system_grpc:
        cmake_options.append("-DASTARTE_USE_SYSTEM_GRPC=ON")

    if sample_to_build == "qt":
        # Check consistency between qt_path and qt_version
        if str(qt_version) == "6" and "Qt6" not in qt_path:
            print(" Mismatch: --qt_version is 6 but qt_path does not look like Qt6.")
            error_exit("Please check your --qt_path or --qt_version.")
        elif str(qt_version) == "5" and "Qt5" not in qt_path:
            print("Mismatch: --qt_version is 5 but qt_path does not look like Qt5.")
            error_exit("Please check your --qt_path or --qt_version.")

        cmake_options.append(f"-DCMAKE_PREFIX_PATH={qt_path}")

        use_qt6_val = "ON" if str(qt_version) == "6" else "OFF"
        cmake_options.append(f"-DUSE_QT6={use_qt6_val}")

    # Run CMake Configuration
    # Command: cmake [options] -S <source> -B <build>
    config_cmd = ["cmake"] + cmake_options + ["-S", sample_src_dir, "-B", build_dir]

    run_command(
        config_cmd, error_message=f"CMake configuration failed for {sample_to_build} sample."
    )

    # --- Build the project ---
    print(f"Building {sample_to_build} sample with make -j {jobs} ...")

    # Command: cmake --build <dir> --parallel <jobs>
    build_cmd = ["cmake", "--build", build_dir, "--parallel", str(jobs)]

    run_command(build_cmd, error_message=f"Make build failed for {sample_to_build} sample.")

    print("CMake build process complete.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Build sample with CMake (Non-Conan).")

    parser.add_argument("lib_src_dir", help="The path to the library source directory.")
    parser.add_argument("sample_to_build", help="The name of the sample being built.")
    parser.add_argument("jobs", help="The number of parallel jobs for 'make'.")

    # Optional arguments (Flags)
    parser.add_argument(
        "--system_grpc", action="store_true", help="Use system gRPC (default: False)."
    )

    # Optional arguments (nargs='?')
    parser.add_argument("qt_path", nargs="?", default=None, help="The path to the Qt installation.")
    parser.add_argument(
        "qt_version", nargs="?", default=None, help="The major version of Qt to use."
    )

    args = parser.parse_args()

    build_sample_with_cmake(
        args.lib_src_dir,
        args.sample_to_build,
        args.jobs,
        system_grpc=args.system_grpc,
        qt_path=args.qt_path,
        qt_version=args.qt_version,
    )
