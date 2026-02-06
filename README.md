<!--
Copyright 2025 SECO Mind Srl

SPDX-License-Identifier: Apache-2.0
-->

# Astarte device SDK for C++

This repository contains the implementation of the Astarte device SDK for C++. This library enables devices to communicate with an Astarte instance using one of two supported transport layers:

* **MQTT:** The device connects directly to the Astarte instance.
* **gRPC:** The device connects to Astarte through the Astarte message hub.

## Requirements

### Minimum C++ version
This library requires **C++ 20** or later.

### Astarte message hub
If using **gRPC** as the transport layer, this library relies on the Astarte message hub for connectivity. The message hub must be version **0.8.0** or higher.

## Dependencies

This library requires several dependencies to function. By default, the build system imports them automatically using CMake's `FetchContent`. Alternatively, you can configure the build to use system-installed versions or manage dependencies via [Conan](https://conan.io/).

### spdlog
The library uses [spdlog](https://github.com/gabime/spdlog) for logging.

* **Default behavior:** Imported via `FetchContent`.
* **System install:** Set the CMake option `ASTARTE_USE_SYSTEM_SPDLOG` to use a local installation.

### MQTT transport dependencies
* **Default behavior:** All MQTT-related dependencies are imported via `FetchContent`.
* **System install:** Set the CMake option `ASTARTE_USE_SYSTEM_MQTT` to use system-installed dependencies.

#### Paho MQTT C++
For MQTT communication, the SDK uses the [paho MQTT C++ library](https://github.com/eclipse-paho/paho.mqtt.cpp). This is a wrapper around the Paho MQTT C library that enables direct connectivity with Astarte.

### gRPC transport dependencies

#### gRPC
When gRPC is selected as the transport layer, the primary dependency is [gRPC](https://github.com/grpc/grpc). While the officially supported version is `v1.72.0`, most recent versions should work.

* **Default behavior:** Imported via `FetchContent`.
* **System install:** Set the CMake option `ASTARTE_USE_SYSTEM_GRPC` to use your system's gRPC installation.

> **Note:** Importing gRPC via `FetchContent` requires recompiling the gRPC library, which significantly increases build time. We recommend using a system-installed gRPC for faster development.

#### Astarte message hub proto
The [Astarte message hub proto](https://github.com/astarte-platform/astarte-message-hub-proto) repository contains the `.proto` files and CMake functions required for gRPC communication with the message hub.

* **Default behavior:** Imported via `FetchContent`.
* **Local checkout:** Set the CMake option `ASTARTE_MESSAGE_HUB_PROTO_DIR` to the path of your local repository checkout.

## Getting started with samples

The `samples` folder contains various examples demonstrating how to use the SDK. A utility script, `build_sample.sh`, is provided to build these samples without interacting directly with CMake.

## Integrating into an external project

This library is designed to be integrated using CMake. Add the following to your `CMakeLists.txt`:

```cmake
include(FetchContent)
FetchContent_Declare(
    astarte_device_sdk
    GIT_REPOSITORY https://github.com/astarte-platform/astarte-device-sdk-cpp.git
    GIT_TAG v0.8.1
)
FetchContent_MakeAvailable(astarte_device_sdk)
```

Ensure you link the library to your executable:
```CMake
target_link_libraries(app
    PRIVATE astarte_device_sdk)
```

## Conan integration

This library supports the [Conan](https://conan.io/) package manager. Although the package is not yet available on the Conan Center Index, you can install it into your local cache by cloning this repository and running:
```bash
conan create . --build=missing --settings=compiler.cppstd=20
```
Add the requirement to your project's `conanfile`:
```toml
[requires]
astarte-device-sdk/0.8.1
```
Then, import and link the library in your CMake configuration:
```CMake
find_package(astarte_device_sdk)
...
target_link_libraries(app
    PRIVATE astarte_device_sdk::astarte_device_sdk)
```

## Extending public interfaces

You can expose certain internal dependencies as part of the library's public interface using specific CMake options.

### Public spdlog dependency

To link `spdlog` as a public dependency, enable the option: `ASTARTE_PUBLIC_SPDLOG_DEP`.

### Public message hub proto dependency

To expose the generated gRPC stubs and Protobuf messages in the public interface (allowing direct access from your C++ code), enable the option: `ASTARTE_PUBLIC_PROTO_DEP`.

> **N.B.** This option is intended primarily for development purposes.
