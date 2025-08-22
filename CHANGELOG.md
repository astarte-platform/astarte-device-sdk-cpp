<!--
Copyright 2025 SECO Mind Srl

SPDX-License-Identifier: Apache-2.0
-->

# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.1.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [unreleased]
### Added
- Implement Astarte pairing mechanism to register a device, retrieve MQTT broker URL and retrieve and check
a device certificate.

### Changed
- Introduced an `AstarteDeviceMQTT` class that uses the MQTT protocol as a transport layer.
- The `simple` sample has been renamed to `grpc_native`.
- The `qt` sample has been renamed to `grpc_qt`.
- Use C++20 as the minimum required library version.

### Removed
- Avoid using timeout to check the device connection status.

## [0.6.1] - 2025-08-11
### Added
- Astarte classes can now be formatted, using the standard library's formatter by default. If it's not available, the feature uses the `fmtlib` library from `spdlog` as a fallback. To use this fallback, you must enable the `ASTARTE_PUBLIC_SPDLOG_DEP` option in CMake.

### Fixed
- Qt sample compilation errors on windows machines.

## [0.6.0] - 2025-07-21
### Added
- Added `is_connected` method to the Astarte device. Users can evaluate through that method if the device is connected to the Astarte message hub.
- All gRPC messages can be logged usign spdlog with trace level logging enabled.
- Method to add a json interface as a string directly, without reading it from a file
- Method to convert an AstarteMessage and AstarteData that returns an std::option instead of an exception
- A new Qt sample showing how to use the Astarte device library in a Qt project.
- `ASTARTE_PUBLIC_PROTO_DEP` CMake option.
- `AstarteDatastreamIndividual`, `AstartePropertyIndividual` and `AstarteDatastreamObject` (renaming of `AstarteObject`) classes, representing the different Astarte data types.
- `get_starded.md`, a step by step guide useful to try the SDK functionalities guide.

### Changed
- The `AstarteDevice` is now an abstract class transport independent. Users should instantiate the `AstarteDeviceGRPC` class which is the gRPC specific Astarte device class.
- Astarte device poll requires a timeout parameter.
- Reference version of the Astarte message hub is v0.8.0
- Bump the Astarte message hub proto definitions to v0.8.4.
- The `USE_SYSTEM_GRPC` option has been moved to the Astarte message hub proto and prefixed with `ASTARTE_`.
- The `grpc` sample has been renamed to `simple`.
- The value contained in an Astarte message can now be directly converted to one of: `AstarteDatastreamIndividual`, `AstartePropertyIndividual` and `AstarteDatastreamObject` using the `into` method.
- The connection procedure is now asynchrnonous. When `connect` is called the device will attempt to connect untill the `disconnect` function is called or the object is destroyed. The device will use an exponential backoff to avoid network congestion.
- Enums elements are now named using a k prefix and camel case format.

### Removed
- `base64` has been fully removed as a dependency.

## [0.5.0-beta] - 2025-03-20
### Added
- Initial SDK release, supports connecting to a message hub instance and publish/receive of basic Astarte types.
