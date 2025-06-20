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
- A new Qt sample. The Qt sample shows how to use the Astarte device library in a Qt project.
- The CMake options: `ASTARTE_ENABLE_FORMAT` and `ASTARTE_PUBLIC_PROTO_DEP`.
- The new classes: `AstarteDatastreamIndividual`, `AstartePropertyIndividual` and
  `AstarteDatastreamObject` (renaming of `AstarteObject`).
- A `get_starded.md` file has been added. Users can follow it for a step by step getting started
  guide.

### Changed
- Bump the Astarte message hub proto definitions to v0.8.0. With this new proto version the
  probuffer files are compiled together with the Astarte device library instead of being
  pre-compiled.
- With the bump of the Astarte message hub proto to v0.8.0 the `USE_SYSTEM_GRPC` option has been
  moved from this library to the Astarte message hub proto and prefixed with `ASTARTE_`.
- The `grpc` sample has been renamed to `simple`.
- The value contained in an Astarte message can now be directly converted to one of:
  `AstarteDatastreamIndividual`, `AstartePropertyIndividual` and `AstarteDatastreamObject` using the
  `into` method.
- The connection procedure is now asynchrnonous. When `connect` is called the device will attempt
  to connect untill the `disconnect` function is called or the object is destroyed. The device will
  use an exponential backoff to avoid network congestion.
- Enums elements are now named using a k prefix and camel case format. This affects the
  `AstarteType`.

## [0.5.0-beta] - 2025-03-20
### Added
- Initial SDK release, supports connecting to a message hub instance and publish/receive of basic
  Astarte types.
