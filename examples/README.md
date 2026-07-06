<!--
Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
Security Alliance (GEISA), a Series of LF Projects, LLC

Licensed under the Apache License, Version 2.0. See LICENSE.
-->

# GEISA Schema Examples

This directory contains reference example code for GEISA schema and
protobuf envelope examples.

The C++ writer examples emit binary protobuf envelopes. The reader examples
decode those envelopes and print JSON-like diagnostic output. Many examples
include a `--demo` mode for a quick local walkthrough.

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
- Actuators: `README-actuators.md`
- Sensors: `README-sensors.md`
- Waveform: `README-waveform.md`
- Metered quantities: `README-metered-quantities.md`
- Application manifests: `README-manifests.md`

## Why are most examples in C++?

Most example programs are written in C++ because they are intended to
cover the active schema set consistently, and some GEISA protobuf files use
proto3 `optional` fields.

The protobuf toolchain supports proto3 optional field presence, but the
language generator plugins must also support that feature. The C toolchain in
use by this repo relies on protobuf-c / `protoc-gen-c`, which does not support
the proto3 `optional` fields used by some schemas. As a result, examples that
depend on those schemas cannot be built through the C generation path. Open
issue on
protobuf-c: [Add support for "optional" field for proto3 files][protobuf-c-476].

The `metered_quantities` contract also includes an embedded C example
path built with nanopb. Most examples still use C++ so
that the repository has one working example path across the broader schema
set, including schemas that use proto3 optional fields.

Embedded C examples are available separately through:

    make examples-c NANOPB_DIR=/path/to/nanopb

The aggregate `examples-c` target builds the metered quantities examples.

Note that these examples are not part of the GEISA API specification, and do
not imply that GEISA applications must be written in C++. GEISA APIs are
defined at the protocol level using MQTT topics and protobuf envelopes, with
some payloads carrying opaque application bytes selected by `content-type`.
Applications may use the MQTT/protobuf implementation that best fits their
execution environment, language, and platform constraints.

[protobuf-c-476]: https://github.com/protobuf-c/protobuf-c/issues/476
