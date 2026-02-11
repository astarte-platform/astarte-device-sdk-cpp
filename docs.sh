#!/bin/bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0
#!/bin/bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

# --- Configuration ---
doxygen_version="1.13.2"
fresh_mode=false
docs_dir="doc"
docs_build_dir_name="_build"
astarte_sdk_version="0.9.0"
doxyfile_name="astarte-device-sdk.doxyfile.in"
script_initial_pwd="$PWD"

# --- Helper Functions ---
display_help() {
    cat << EOF
Usage: $0 [OPTIONS]

Generates Doxygen documentation.

Options:
  --fresh          Clean the documentation build directory ($docs_dir/$docs_build_dir_name) before generation.
  --help           Display this help message and exit.
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
        --help) display_help; exit 0 ;;
        *) display_help; error_exit "Unknown option: $1" ;;
    esac
done

# --- Pre-flight Checks ---
if ! command -v wget &> /dev/null; then
    error_exit "wget command could not be found. Please install wget."
fi
if ! command -v tar &> /dev/null; then
    error_exit "tar command could not be found. Please install tar."
fi

# --- Documentation Generation Logic ---

# Determine Doxygen C++ standard macro value based on CLI argument
doxygen_cpp_standard_macro_value=""
doxygen_cpp_standard_macro_value="202002L"

echo "Configuration:"
echo "  Doxygen Version: $doxygen_version"
echo "  Fresh Mode: $fresh_mode"
echo "  Documentation Directory: $docs_dir"
echo "  Build Subdirectory: $docs_build_dir_name"
echo "  Doxyfile: $doxyfile_name"
echo ""

# Define full build path
full_docs_build_dir="$docs_dir/$docs_build_dir_name"

# Clean build directory if --fresh is set
if [ "$fresh_mode" = true ]; then
    if [ -d "$full_docs_build_dir" ]; then
        echo "Fresh mode enabled. Removing $full_docs_build_dir..."
        rm -rf "$full_docs_build_dir"
    else
        echo "Fresh mode enabled, but $full_docs_build_dir does not exist. Skipping removal."
    fi
fi

# Create documentation and build directories if they don't exist
echo "Ensuring build directory '$full_docs_build_dir' exist..."
if ! mkdir -p "$full_docs_build_dir"; then
    error_exit "Failed to create documentation build directory '$full_docs_build_dir'."
fi

# Define Doxygen paths
doxygen_install_dir="$full_docs_build_dir/doxygen_installation"
doxygen_extracted_dir_name="doxygen-${doxygen_version}"
doxygen_bin="$doxygen_install_dir/$doxygen_extracted_dir_name/bin/doxygen"
doxygen_archive_filename="doxygen-${doxygen_version}.linux.bin.tar.gz"
doxygen_download_url_version=${doxygen_version//./_} # e.g., 1.10.0 -> 1_10_0
doxygen_download_url="https://github.com/doxygen/doxygen/releases/download/Release_${doxygen_download_url_version}/${doxygen_archive_filename}"

# Download and extract Doxygen if not found
if [[ ! -x "$doxygen_bin" ]]; then
    echo "Doxygen executable not found at '$doxygen_bin'."
    echo "Attempting to download and install Doxygen version $doxygen_version..."

    if ! mkdir -p "$doxygen_install_dir"; then
        error_exit "Failed to create Doxygen installation directory '$doxygen_install_dir'."
    fi

    echo "Changing directory to '$doxygen_install_dir' for download..."
    if ! cd "$doxygen_install_dir"; then
        error_exit "Failed to navigate to '$doxygen_install_dir'."
    fi

    if [[ -f "$doxygen_archive_filename" ]]; then
        echo "Archive $doxygen_archive_filename already exists. Skipping download."
    else
        echo "Downloading Doxygen from $doxygen_download_url..."
        if ! wget --no-verbose -O "$doxygen_archive_filename" "$doxygen_download_url"; then
            rm -f "$doxygen_archive_filename" # Clean up partial download
            error_exit "Failed to download Doxygen. Please check the version ($doxygen_version) and URL."
        fi
    fi

    echo "Extracting Doxygen archive $doxygen_archive_filename..."
    # Remove existing extracted directory to ensure clean extraction
    if [ -d "$doxygen_extracted_dir_name" ]; then
        echo "Removing existing Doxygen extracted directory '$doxygen_extracted_dir_name'..."
        rm -rf "$doxygen_extracted_dir_name"
    fi
    if ! tar xf "$doxygen_archive_filename"; then
        error_exit "Failed to extract Doxygen archive."
    fi

    echo "Returning to initial script directory..."
    if ! cd "$script_initial_pwd"; then
        error_exit "Failed to return to script initial directory '$script_initial_pwd'."
    fi

    if [[ ! -x "$doxygen_bin" ]]; then
        error_exit "Doxygen executable still not found at '$doxygen_bin' after extraction."
    fi
    echo "Doxygen installed successfully to '$doxygen_install_dir/$doxygen_extracted_dir_name'."
else
    echo "Doxygen executable found at '$doxygen_bin'."
fi

# Navigate to the documentation directory to run Doxygen
echo "Changing directory to '$docs_dir' to run Doxygen..."
if ! cd "$docs_dir"; then
    error_exit "Failed to navigate to documentation directory '$docs_dir'."
fi

# Set environment variables for Doxygen
export DOXYGEN_OUT="./$docs_build_dir_name"
export DOXYGEN_ASTARTE_DEVICE_SDK_BASE=".."
export DOXYGEN_ASTARTE_DEVICE_SDK_VERSION="$astarte_sdk_version"
export DOXYGEN_ASTARTE_DEVICE_SDK_MACROS="__cplusplus=$doxygen_cpp_standard_macro_value"

echo "Running Doxygen with configuration:"
echo "  DOXYGEN_OUT=$DOXYGEN_OUT"
echo "  DOXYGEN_ASTARTE_DEVICE_SDK_BASE=$DOXYGEN_ASTARTE_DEVICE_SDK_BASE"
echo "  DOXYGEN_ASTARTE_DEVICE_SDK_VERSION=$DOXYGEN_ASTARTE_DEVICE_SDK_VERSION"
echo "  DOXYGEN_ASTARTE_DEVICE_SDK_MACROS=$DOXYGEN_ASTARTE_DEVICE_SDK_MACROS"
echo "  Doxygen binary: $doxygen_bin"
echo "  Doxyfile: $doxyfile_name"

# Run Doxygen (use the absolute path to the binary determined earlier)
absolute_doxygen_bin="$script_initial_pwd/$full_docs_build_dir/doxygen_installation/$doxygen_extracted_dir_name/bin/doxygen"

if [[ ! -x "$absolute_doxygen_bin" ]]; then
    error_exit "Cannot find Doxygen binary at '$absolute_doxygen_bin' when trying to execute."
fi

if ! "$absolute_doxygen_bin" "$doxyfile_name"; then
    error_exit "Doxygen generation failed."
fi

echo "Doxygen documentation generated successfully in '$PWD/$docs_build_dir_name'."
