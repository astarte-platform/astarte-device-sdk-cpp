# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

function Show-Error {
    param (
        [string]$Message
    )
    Write-Host "Error: $Message" -ForegroundColor Yellow
    Write-Error "Error: $Message" -ErrorAction Stop
    exit 1
}

<#
.SYNOPSIS
Sets up a Python virtual environment, activates it, and upgrades pip.

.PARAMETER VenvDir
The directory path to create the virtual environment in.

.EXAMPLE
Setup-PythonVenv "./.venv"
#>
function Setup-PythonVenv {
    param (
        [string]$VenvDir
    )

    # --- Argument Validation ---
    if ([string]::IsNullOrWhiteSpace($VenvDir)) {
        Show-Error "Virtual environment directory not provided."
    }

    Write-Host "Setting up Python environment in $VenvDir..."

    # Check for python
    if (-not (Get-Command "python" -ErrorAction SilentlyContinue)) {
        Show-Error "python could not be found. Please install Python."
    }

    # Specific check for the venv module
    try {
        python -c "import venv" 2>$null
        if ($LASTEXITCODE -ne 0) { throw }
    }
    catch {
        Show-Error "The python 'venv' module is missing or failed to load."
    }

    # Create virtual environment if it doesn't exist
    if (-not (Test-Path "$VenvDir")) {
        Write-Host "Creating Python virtual environment in $VenvDir..."
        python -m venv "$VenvDir"
        if ($LASTEXITCODE -ne 0) {
            Show-Error "Failed to create Python virtual environment."
        }
    }

    # Activate virtual environment
    # In PowerShell, we activate using the Activate.ps1 script in the Scripts folder
    $activateScript = Join-Path $VenvDir "Scripts\Activate.ps1"

    if (Test-Path $activateScript) {
        # Dot-sourcing the activation script to affect the current scope
        . $activateScript
    } else {
        Show-Error "Failed to activate Python virtual environment. Activation script not found at $activateScript"
    }

    # Upgrade pip
    Write-Host "Upgrading pip..."
    python -m pip install --upgrade pip
    if ($LASTEXITCODE -ne 0) {
        Show-Error "Failed to upgrade pip."
    }

    Write-Host "Python environment setup complete."
}

<#
.SYNOPSIS
Installs a specific version of Conan (or any pip package) if not already installed.
Assumes a Python environment is already active.

.PARAMETER ConanPackageName
The name of the conan package (e.g., "conan").

.PARAMETER ConanPackageVersion
The specific version of the conan package to install.

.EXAMPLE
Install-Conan "conan" "2.0.17"
#>
function Install-Conan {
    param (
        [string]$ConanPackageName,
        [string]$ConanPackageVersion
    )

    # --- Argument Validation ---
    if ([string]::IsNullOrWhiteSpace($ConanPackageName)) {
        Show-Error "Conan package name not provided."
    }
    if ([string]::IsNullOrWhiteSpace($ConanPackageVersion)) {
        Show-Error "Conan package version not provided."
    }

    # Verify pip is accessible (ensures environment is likely active)
    if (-not (Get-Command "pip" -ErrorAction SilentlyContinue)) {
        Show-Error "pip not found. Ensure the Python environment is activated before installing Conan."
    }

    Write-Host "Checking/installing $ConanPackageName version $ConanPackageVersion..."

    # Check installed version
    $installedVersion = $null
    try {
        $pipOutput = pip show "$ConanPackageName" 2>$null
    } catch {
        Write-Host "pip crashed with: " $_
    }

    if ($pipOutput) {
        # Parse the output to find the Version line
        $versionLine = $pipOutput | Select-String "Version:"
        if ($versionLine) {
            $installedVersion = $versionLine.ToString().Split(':')[1].Trim()
        }
    }

    if ($installedVersion -ne $ConanPackageVersion) {
        Write-Host "Installing $ConanPackageName==$ConanPackageVersion..."
        python -m pip install "$ConanPackageName==$ConanPackageVersion"
        if ($LASTEXITCODE -ne 0) {
            Show-Error "Failed to install $ConanPackageName version $ConanPackageVersion."
        }
    } else {
        Write-Host "$ConanPackageName version $ConanPackageVersion is already installed."
    }
}

<#
.SYNOPSIS
Installs a specific version of CMake via pip if not already installed.
Assumes a Python environment is already active.

.PARAMETER CmakeVersion
The specific version of CMake to install.

.EXAMPLE
Install-Cmake "3.27.7"
#>
function Install-Cmake {
    param (
        [string]$CmakeVersion
    )

    # --- Argument Validation ---
    if ([string]::IsNullOrWhiteSpace($CmakeVersion)) {
        Show-Error "CMake version not provided."
    }

    # Verify pip is accessible (ensures environment is likely active)
    if (-not (Get-Command "pip" -ErrorAction SilentlyContinue)) {
        Show-Error "pip not found. Ensure the Python environment is activated before installing CMake."
    }

    $cmakePackageName = "cmake"
    Write-Host "Checking/installing $cmakePackageName version $CmakeVersion..."

    # Check installed version via pip
    $installedVersion = $null
    try {
        $pipOutput = pip show "$cmakePackageName" 2>$null
    } catch {
        Write-Host "pip crashed with: " $_
    }

    if ($pipOutput) {
        # Parse the output to find the Version line
        $versionLine = $pipOutput | Select-String "Version:"
        if ($versionLine) {
            $installedVersion = $versionLine.ToString().Split(':')[1].Trim()
        }
    }

    if ($installedVersion -ne $CmakeVersion) {
        Write-Host "Installing $cmakePackageName==$CmakeVersion..."
        python -m pip install "$cmakePackageName==$CmakeVersion"
        if ($LASTEXITCODE -ne 0) {
            Show-Error "Failed to install $cmakePackageName version $CmakeVersion."
        }
    } else {
        Write-Host "$cmakePackageName version $CmakeVersion is already installed."
    }
}
