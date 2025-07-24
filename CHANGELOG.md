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
- Method to add a json interface as a string directly, without reading it from a file
- Method to convert an AstarteMessage and AstarteData that returns an std::option instead of an exception
- A new Qt sample showing how to use the Astarte device library in a Qt project.
- `ASTARTE_PUBLIC_PROTO_DEP` CMake options.
- `AstarteDatastreamIndividual`, `AstartePropertyIndividual` and `AstarteDatastreamObject` (renaming
  of `AstarteObject`) classes, representing the different Astarte data types.
- `get_starded.md`, a step by step guide useful to try the SDK functionalities
  guide.

### Changed
- Reference version of the Astarte message hub is v0.8.0
- Bump the Astarte message hub proto definitions to v0.8.4.
- The `USE_SYSTEM_GRPC` option has been moved to the Astarte message hub proto and prefixed with
  `ASTARTE_`.
- The `grpc` sample has been renamed to `simple`.
- The value contained in an Astarte message can now be directly converted to one of:
  `AstarteDatastreamIndividual`, `AstartePropertyIndividual` and `AstarteDatastreamObject` using the
  `into` method.
- The connection procedure is now asynchrnonous. When `connect` is called the device will attempt
  to connect untill the `disconnect` function is called or the object is destroyed. The device will
  use an exponential backoff to avoid network congestion.
- Enums elements are now named using a k prefix and camel case format.

## [0.5.0-beta] - 2025-03-20
### Added
- Initial SDK release, supports connecting to a message hub instance and publish/receive of basic
  Astarte types.
