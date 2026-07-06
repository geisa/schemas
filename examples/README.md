<!--
Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
Security Alliance (GEISA), a Series of LF Projects, LLC

Licensed under the Apache License, Version 2.0. See LICENSE.
-->

# GEISA Schema Examples

This directory contains reference example code for GEISA schema and protobuf
envelope examples.

The C++ writer examples emit binary protobuf envelopes. The reader examples
decode those envelopes and print JSON-like diagnostic output. The embedded C
examples use nanopb for the same protobuf message contracts. Many examples
include a `--demo` mode for a quick local walkthrough.

## Building examples

From the repository root:

    make clean
    make examples

Compiled example binaries are written to:

    build/examples/

The embedded C build uses the repository `venv` for nanopb generation plus an
external nanopb runtime source tree:

    bash scripts/setup-dev-venv.sh
    test -d /tmp/nanopb/.git || git clone https://github.com/nanopb/nanopb /tmp/nanopb
    PYTHON="$(pwd)/venv/bin/python" \
    NANOPB_GENERATOR_MODULE=nanopb.generator.nanopb_generator \
    NANOPB_DIR=/tmp/nanopb \
    make examples-c

You can also run `make setup-dev` from the repository root to call the same
bootstrap script.

If you already have a working `protoc-gen-nanopb` executable, you can use it
instead by setting `NANOPB_GENERATOR=protoc-gen-nanopb` or an explicit plugin
path.

## Directory organization

- `*.cpp`: C++ protobuf writer/reader examples
- `*.c`: embedded C protobuf writer/reader examples using nanopb
- `*.json`: JSON examples aligned to schemas
- `helpers/*.h`: shared example-only helper utilities
- `README-*.md`: topic-specific guides

## Topic guides

- Platform Discovery: `README-discovery.md`
- Networking / app-message: `README-networking.md`
- Actuators: `README-actuators.md`
- Sensors: `README-sensors.md`
- Waveform: `README-waveform.md`
- Application manifests: `README-manifests.md`

## C++ and embedded C paths

The repository currently ships both C++ and embedded C example paths.

Embedded C examples are available for:

- actuators
- app-message / networking
- sensors
- Platform Discovery

Waveform remains on the C++ example path for GEISA 0.9. The Platform Discovery
embedded C example includes waveform metadata in the discovery response; it
does not convert the waveform frame example path.

When `/tmp/nanopb` already exists as a valid nanopb checkout, `make examples`
builds both the C++ and embedded C example sets. Use `make examples-c` when
you want the embedded C path explicitly and fail-fast on missing nanopb
prerequisites.

Note that these examples are not part of the GEISA API specification, and do
not imply that GEISA applications must be written in C or C++. GEISA APIs are
defined at the protocol level using MQTT topics and protobuf envelopes, with
some payloads carrying opaque application bytes selected by `content-type`.
Applications may use the MQTT/protobuf implementation that best fits their
execution environment, language, and platform constraints.
