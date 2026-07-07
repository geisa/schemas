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
examples use nanopb for the same protobuf message contracts. For the embedded C
path, writer `--demo` modes are the primary end-to-end walkthrough entry
points: they generate the standard `/tmp` payloads and then decode them
immediately. Reader `--demo` modes are decode-only and expect those standard
`/tmp` payloads to already exist.

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

After the repo `venv` exists, direct embedded C targets such as
`make examples-c` and `make examples-metered` use it automatically unless you
override the nanopb generator settings explicitly.

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
- Connection status: `README-conn-status.md`
- Networking / app-message: `README-networking.md`
- Actuators: `README-actuators.md`
- Sensors: `README-sensors.md`
- Waveform: `README-waveform.md`
- Metered quantities: `README-metered-quantities.md`
- Application manifests: `README-manifests.md`

## C++ and embedded C paths

The repository currently ships both C++ and embedded C example paths.

Embedded C examples are available for:

- actuators
- app-message / networking
- conn-status
- metered quantities
- sensors
- Platform Discovery

Where embedded C examples exist, they are the preferred example path because
the nanopb-based build is the path being kept aligned with proto3 optional
support and current embedded review needs. The C++ examples remain available
as reference examples and may be removed in a later cleanup.

The legacy `make c` protobuf-c generation path remains available for standalone
code generation, but it is not the primary C example path.

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
