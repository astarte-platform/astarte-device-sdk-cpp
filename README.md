<!--
Copyright 2025 SECO Mind Srl

SPDX-License-Identifier: Apache-2.0
-->

# Astarte device SDK for C++

This repo contains an implementation of an Astarte device SDK for C++.

> **N.B.** At the current state this SDK only provides connectivity to Astarte through the
[Astarte message hub](https://github.com/astarte-platform/astarte-message-hub).

## Required dependencies

This library has only one notable dependency, gRPC.
You can choose to use your system gRPC installation, or to let the library check out gRPC
internally.
When building with CMake use the option `USE_SYSTEM_GRPC` to select if building with the system
gRPC or if to fetch it at build time.

> **N.B.** Importing gRPC internally to the library requires a re-compilation of the gRPC library
leading to a much longer build time.
> We suggest using your system gRPC for fast developement.

The officially supported version of gRPC is `v0.69.0`.

## Required C++ version

This library requires at least C++ 17.

## Get started with the samples

Check out the sample folder and get started using the library.

## Importing the library in an external project

This library supports CMake as the default build system.
It can be imported adding the following lines to your application CMakeList file.

```CMake
include(FetchContent)
FetchContent_Declare(
    astarte_device_sdk
  GIT_REPOSITORY git@github.com:secomind/astarte-device-sdk-cpp.git
  GIT_TAG main
)
FetchContent_MakeAvailable(astarte_device_sdk)
```

Additionally remember to link the library with your executable.
```CMake
target_link_libraries(app
    PRIVATE astarte_device_sdk)
```
