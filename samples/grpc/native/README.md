<!--
Copyright 2025 SECO Mind Srl

SPDX-License-Identifier: Apache-2.0
-->

# Astarte device SDK for C++: Basic gRPC sample

This sample demonstrates the core functionality of the Astarte Device SDK for C++ using a native gRPC connection.

## Configuration

This sample comes pre-configured to connect to an Astarte message hub running locally.

Ensure that the Astarte message hub is active on `localhost` at port `50051` before running the application.

## Building

To build the sample, run the `build_sample.sh` script located in the project root.

```bash
./build_sample.sh grpc/native
```

## Running

Once the build is complete, execute the generated binary:
```bash
./samples/grpc/native/build/app
```
