#!/bin/bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

BUILD_DIR="build"
SRC_DIR="$(pwd)"
QT_PATH="$HOME/Qt/6.8.1/gcc_64/lib/cmake/Qt6"
STD_CPP="20"
CLEAN=false

usage() {
  echo "Usage: $0 [OPTIONS]"
  echo ""
  echo "Options:"
  echo "  --qt_path <path>   Path to QtXConfig.cmake (e.g. ~/Qt/6.8.1/gcc_64/lib/cmake/Qt6)"
  echo "  --stdcpp <ver>     C++ standard to use (17 or 20). Default: 20"
  echo "  --fresh            Delete and recreate build folder before building"
  echo "  -h, --help         Show this help message"
  exit 1
}

while [[ "$#" -gt 0 ]]; do
  case $1 in
    --qt_path) QT_PATH="$2"; shift ;;
    --stdcpp) STD_CPP="$2"; shift ;;
    --fresh) CLEAN=true ;;
    -h|--help) usage ;;
    *) echo "Unknown option: $1"; usage ;;
  esac
  shift
done

if [[ -z "$QT_PATH" ]]; then
  echo "Error: --qt_path not specified."
  usage
fi

if [ "$CLEAN" = true ]; then
  echo "Cleaning build directory..."
  rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR" || exit 1

echo "Configuring project using CMake..."
cmake "$SRC_DIR" -DCMAKE_PREFIX_PATH="$QT_PATH" -DCMAKE_CXX_STANDARD="$STD_CPP" -DCMAKE_CXX_STANDARD_REQUIRED=ON

echo "Building project..."
cmake --build .

echo "✅ Build complete. Executable is in: $BUILD_DIR/qt"
