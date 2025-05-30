#!/bin/bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

BUILD_DIR="build"
SRC_DIR="$(pwd)"
QT_PATH="$HOME/Qt/6.8.1/gcc_64/lib/cmake/Qt6"
STD_CPP="20"
CLEAN=false
USE_QT6=true
USE_SYSTEM_GRPC=true

usage() {
  echo "Usage: $0 [OPTIONS]"
  echo ""
  echo "Options:"
  echo "  --qt_path <path>   Path to QtXConfig.cmake (e.g. ~/Qt/6.8.1/gcc_64/lib/cmake/Qt6)"
  echo "  --stdcpp <ver>     C++ standard to use (17 or 20). Default: 20"
  echo "  --fresh            Delete and recreate build folder before building"
  echo "  -h, --help         Show this help message"
  echo "  --qt_version <5|6> Qt version to use (default: 6)"
  echo "  --no_system_grpc   Do not use system gRPC, fetch and build gRPC manually"

  exit 1
}

while [[ "$#" -gt 0 ]]; do
  case $1 in
    --qt_path)
      QT_PATH="$2"
      shift
      ;;
    --stdcpp)
      STD_CPP="$2"
      shift
      ;;
    --fresh)
      CLEAN=true
      ;;
    --qt_version)
      if [[ "$2" == "6" ]]; then
        USE_QT6=true
      elif [[ "$2" == "5" ]]; then
        USE_QT6=false
      else
        echo "Invalid Qt version: $2 (expected 5 or 6)"
        exit 1
      fi
      shift
      ;;
    --no_system_grpc)
      USE_SYSTEM_GRPC=false
      ;;
    -h|--help)
      usage
      ;;
    *)
      echo "Unknown option: $1"
      usage
      ;;
  esac
  shift
done

if [[ -z "$QT_PATH" ]]; then
  echo "Error: --qt_path not specified."
  usage
fi

# Check consistency between qt_path and qt_version
if $USE_QT6 && [[ "$QT_PATH" != *"Qt6"* ]]; then
  echo "⚠️  Mismatch: --qt_version is 6 but qt_path does not look like Qt6."
  exit 1
elif ! $USE_QT6 && [[ "$QT_PATH" != *"Qt5"* ]]; then
  echo "⚠️  Mismatch: --qt_version is 5 but qt_path does not look like Qt5."
  exit 1
fi


if [ "$CLEAN" = true ]; then
  echo "Cleaning build directory..."
  rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR" || exit 1

cmake_options="-DCMAKE_PREFIX_PATH=$QT_PATH -DCMAKE_CXX_STANDARD=$STD_CPP -DCMAKE_CXX_STANDARD_REQUIRED=ON -DUSE_QT6=$USE_QT6"

if $USE_SYSTEM_GRPC; then
    cmake_options="$cmake_options -DUSE_SYSTEM_GRPC=ON"
fi

echo "Configuring project using CMake..."
cmake "$SRC_DIR" $cmake_options

echo "Building project..."
cmake --build .

echo "✅ Build complete. Executable is in: $BUILD_DIR/qt"
