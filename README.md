<!--
Copyright 2025 SECO Mind Srl

SPDX-License-Identifier: Apache-2.0
-->

# Astarte device SDK for C++

This repo contains an implementation of an Astarte device SDK for C++.

> **N.B.** At the current state this SDK only provides connectivity to Astarte through the
[Astarte message hub](https://github.com/astarte-platform/astarte-message-hub).

## Minimum C++ version

This library requires at least C++ 17.

## Supported Astarte message hub versions

This library relies on the Astarte message hub for connectivity with Astarte.
The message hub should be at least at version X.X for this library to function properly.

## Formatting Astarte data

Astarte data, both in transmission and reception is encapsulated in C++ objects.
It is possible to pretty print data objects. The resulting string will be in a JSON compabile format and
can be used to interact with the Astarte REST APIs during developement.

## Optional features

Some features of the library can be enabled or disabled using CMake options.


### Public spdlog dependency

The spdlog libary is the logger used by the Astarte device library.
By the default it's a private dependency, checked our with fetch content. However, the user can
choose to link the library as a public dependency using the dedicated option.

The option to enable this feature is: `ASTARTE_PUBLIC_SPDLOG_DEP`.

### Public message hub proto dependency

The [Astarte message hub proto](https://github.com/astarte-platform/astarte-message-hub-proto) is a
separate CMake project that contains the protobuffer definitions of the gRPC used by this library
to connect with the Astarte message hub.
This feature makes the Astarte message hub proto project a public dependency of this library.
The user can subsequently access the gRPC stubs directly from within C++ code.

The option to enable this feature is: `ASTARTE_PUBLIC_PROTO_DEP`.

> **N.B.** This option should only be enabled during developement.

## Dependencies

For this library to properly work some dependencies are required. All of them can be imported
directly by the library using fetch content.
However, the user may choose to use system installed ones setting some cmake options.

### gRPC

The most importand and large depencency for this library is [gRPC](https://github.com/grpc/grpc).
The officially supported version of gRPC is `v0.69.0`, but any gRPC version following that one
should work just fine.

By default gRPC will be imported using FetchContent. Users can choose to use their system gRPC
by setting the CMake option `ASTARTE_USE_SYSTEM_GRPC`.

> **N.B.** Importing gRPC with FetchContent requires a re-compilation of the gRPC library
leading to a much longer build time.
> We suggest using your system gRPC for fast developement.

### Astarte message hub proto

The [Astarte message hub proto](https://github.com/astarte-platform/astarte-message-hub-proto) is
a separate project that contains the protobuffer definitions of the gRPC used by this library
to connect with the Astarte message hub.
This dependency is imported using fetch content and it's necessary for the proper functioning of
the library.

### spdlog

This library uses [spdlog](https://github.com/gabime/spdlog) to log information.
The logging utility is imported using FetchContent and is an essential component of this library.

## Get started with the samples

Various samples have been added in the `samples` folder of this project.
An utility bash script has also been added, named `build_sample.sh`. It can be used to build one of
the samples without having to deal directly with CMake.

## Importing the library in an external project

This library supports CMake as the default build system.
It can be imported adding the following lines to your application CMakeList file.

```CMake
include(FetchContent)
FetchContent_Declare(
    astarte_device_sdk
  GIT_REPOSITORY git@github.com:secomind/astarte-device-sdk-cpp.git
  GIT_TAG v0.5.0-beta
)
FetchContent_MakeAvailable(astarte_device_sdk)
```

Additionally remember to link the library with your executable.
```CMake
target_link_libraries(app
    PRIVATE astarte_device_sdk)
```
