<#
.SYNOPSIS
    Build sample script (Windows/PowerShell)
    (C) Copyright 2025, SECO Mind Srl
    SPDX-License-Identifier: Apache-2.0

.DESCRIPTION
    PowerShell port of build_sample.sh.
    Arguments are mapped to match the Bash script variables.

.EXAMPLE
    .\build_sample.ps1 grpc/native
.EXAMPLE
    .\build_sample.ps1 grpc/qt -transport mqtt -fresh
.EXAMPLE
    .\build_sample.ps1 grpc/qt -deps-mgmt fetch -qt_version 6
#>

[CmdletBinding()]
param(
    # Positional argument: <sample_name>
    [Parameter(Mandatory=$true, Position=0)]
    [ValidateSet("grpc/native", "grpc/qt", "mqtt/native")]
    [string]$sample_to_build,

    # --transport
    [Parameter(Mandatory=$false)]
    [ValidateSet("grpc", "mqtt")]
    [string]$transport = "grpc",

    # --fresh
    [switch]$fresh,

    # --deps-mgmt
    [Alias("deps-mgmt")]
    [ValidateSet("conan", "fetch", "system")]
    [string]$deps_management = "conan",

    # --ext-tools
    [Alias("ext-tools")]
    [switch]$external_tools,

    # --qt_version
    [ValidateSet(5, 6)]
    [int]$qt_version = 6,

    # --qt_path
    [string]$qt_path = "$env:USERPROFILE\Qt\6.5.3\msvc2019_64\lib\cmake\Qt6",

    # Internal configuration
    [string]$venv_dir = ".venv"
)

$ErrorActionPreference = "Stop"

# --- Configuration Constants ---
$conan_package_name = "conan"
$conan_package_version = "2.20.1"
$cmake_package_version = "4.2.0"

# --- Helper functions ---
function Exit-WithError {
    param([string]$Message)
    Write-Error "Error: $Message"
    exit 1
}

# --- Environment and dependency setup ---
$sample_src_dir = ""
switch ($sample_to_build) {
    "grpc/native" { $sample_src_dir = "samples\grpc\native" }
    "grpc/qt"     { $sample_src_dir = "samples\grpc\qt" }
    "mqtt/native" { $sample_src_dir = "samples\mqtt\native" }
    Default       { Exit-WithError "Unknown sample directory mapping." }
}

$build_dir = Join-Path $sample_src_dir "build"
$cmake_user_presets = Join-Path $sample_src_dir "CMakeUserPresets.json"

# --- Python/Conan Setup ---
if ($deps_management -eq "conan" -and -not $external_tools) {
    $setup_script = ".\scripts\setup_python.ps1"
    if (Test-Path $setup_script) {
        . $setup_script
        try {
            Setup-PythonVenv $venv_dir
            Install-Conan $conan_package_name $conan_package_version
            Install-Cmake $cmake_package_version
        } catch {
            Exit-WithError "Failed to run python setup functions. Ensure setup_python.ps1 exists and works."
        }
    } else {
        Write-Warning "Could not find '$setup_script'. Skipping internal Python/Conan setup."
    }
}

# --- Clean previous builds ---
if ($fresh) {
    Write-Host "Cleaning previous builds..." -ForegroundColor Cyan
    python .\scripts\clean_sample.py "--build_dir=$build_dir" "--deps_mgmt=$deps_management" "--presets_file=$cmake_user_presets"
    if ($LASTEXITCODE -ne 0) { Exit-WithError "Clean script failed" }
}

# --- Perform a build ---
$current_dir = Get-Location
$nproc = $env:NUMBER_OF_PROCESSORS

if ($deps_management -eq "conan") {
    Write-Host "Building with Conan..." -ForegroundColor Cyan
    python .\scripts\build_sample_conan.py "$current_dir" "$sample_to_build" "$transport" "$qt_version"

} elseif ($deps_management -eq "fetch") {
    Write-Host "Building with CMake Fetch..." -ForegroundColor Cyan
    python .\scripts\build_sample_cmake.py "$current_dir" "$sample_to_build" "$transport" "$nproc" "$qt_path" "$qt_version"

} else { # "system"
    Write-Host "Building with System Deps..." -ForegroundColor Cyan
    python .\scripts\build_sample_cmake.py "$current_dir" "$sample_to_build" "$transport" "$nproc" "$qt_path" "$qt_version" --system_grpc
}

if ($LASTEXITCODE -ne 0) { Exit-WithError "Build script failed." }

Write-Host "Build complete for $sample_to_build sample." -ForegroundColor Green
Write-Host "Executable should be in: $build_dir\" -ForegroundColor Green
