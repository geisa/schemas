<!--
Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
Security Alliance (GEISA), a Series of LF Projects, LLC

Licensed under the Apache License, Version 2.0. See LICENSE.
-->

# GEISA Schema Examples

This directory contains learning/reference artifacts for GEISA schema and
protobuf payloads.

The C++ writer examples emit binary protobuf payloads. C++ reader examples
decode those payloads and print JSON-like diagnostic output. Many examples
include a
`--demo` mode for an end-to-end local walkthrough.

## Building the C++ examples

From the repository root:

    make clean
    make examples

Compiled example binaries are written to:

    build/examples/

## Directory organization

- `*.cpp`: C++ protobuf writer/reader examples
- `*.json`: JSON examples aligned to schemas
- `helpers/example_utils.h`: shared example-only helper utilities
  (not part of the GEISA APIs or specification)
- `README-*.md`: topic-specific guides

## Topic guides

- Platform Discovery: `README-discovery.md`
- Networking / app-message: `README-networking.md`
- Sensors: `README-sensors.md`
- Waveform: `README-waveform.md`
- Application manifests: `README-manifests.md`

## Why are the examples in C++?

The example programs are currently written in C++ because they are intended to
cover the active schema set consistently, and some GEISA protobuf files use
proto3 `optional` fields.

The current protobuf toolchain supports proto3 optional field presence,
but the language generator plugins must also support that feature. The C
toolchain currently in use by this repo relies on protobuf-c /
`protoc-gen-c`, which does not currently support the proto3 `optional` fields
used by some schemas. As a result, examples that depend on those schemas
cannot be built through the current C generation path. Open issue on
protobuf-c: [Add support for "optional" field for proto3 files][protobuf-c-476].

A future set of examples may transition to use nanopb, which supports proto3
`optional` fields and is more embedded-focused. Meanwhile, the current
examples use C++ so that the repository has one working example path across
all current schemas, including the schemas that use proto3 optional fields.

Note that these examples are not part of the GEISA API specification, and do
not imply that GEISA applications must be written in C++. GEISA APIs are
defined at the protocol level using MQTT topics and protobuf payloads.
Applications may use the MQTT/protobuf implementation that best fits their
execution environment, language, and platform constraints.

[protobuf-c-476]: https://github.com/protobuf-c/protobuf-c/issues/476
