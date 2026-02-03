#!/usr/bin/env bash

# (C) Copyright 2025, SECO Mind Srl
#
# SPDX-License-Identifier: Apache-2.0

set -exEuo pipefail

cmake -S ./grpc/ -B ./grpc/cmake/build -G Ninja \
    -DBUILD_SHARED_LIBS=ON \
    -DgRPC_INSTALL=ON \
    -DgRPC_BUILD_TESTS=OFF \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
    -DCMAKE_INSTALL_PREFIX="$MY_INSTALL_DIR"

cmake --build ./grpc/cmake/build

cmake --install ./grpc/cmake/build
