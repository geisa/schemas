# GEISA schemas

This repository contains the [protobuf](https://protobuf.dev/) and
[JSON Schema](https://json-schema.org/) schemas used by the
[Grid Edge Interoperability and Security Alliance
Specification](https://github.com/geisa/specification).

These schemas define message structures used by the GEISA API, message bus,
application manifests, profiles, and related examples.

For full documentation and context, refer to the
[GEISA Specification][geisa-spec].

## Repository contents

At a high level, this repository contains:

- `*.proto` files for protobuf message definitions
- `geisa-*-schema.json` files for JSON Schema definitions
- `profiles/*.json` files for additive conformance and capability profile
  overlays
- `examples/*.json` example payloads
- `examples/*.c` embedded C protobuf examples using nanopb.
- `examples/*.cpp` files with C++ protobuf encode/decode examples.
- `examples/helpers/*.h` files with shared example-only helper code.
- `examples/README*.md` example-specific build and run notes.

This schemas repository SHALL be used in conjunction with the GEISA
specification to enable creation of GEISA conformant implementations. The
content is intended to help users understand the structure, conformance, and
intent of GEISA messages and payloads.

Most API message payloads are defined in the GEISA specification as well as
here in proto files, documented in JSON schemas, implemented with example
code, and further documented with example payload contents.

While efforts have been made to keep items provided in multiple places,
formats, or examples consistent and free of ambiguity, an implementer should
be able to resolve conflicts or discrepancies found within this content.

For items with proto files, the proto files are authoritative, while JSON
schemas, examples, and lists in the GEISA specification for the same item are
not.

Profile schemas under `profiles/` are additive overlays on top of the base JSON
schemas. They are intended for conformance validation and implementation
guidance for a specific device/device class or capability profile without
implying that all GEISA-conformant devices must implement all APIs or expose
all measurements.

## Prerequisites

The repository build targets assume these tools are available on `PATH`:

- `make`
- `protoc` (Protocol Buffers compiler)

The C++ example build target also requires:

- a C++17 compiler, such as `c++`, `g++`, or `clang++`
- `pkg-config`
- protobuf development headers and libraries discoverable through
  `pkg-config protobuf`

The legacy C generation target (`make c`) requires protobuf-c support,
including `protoc-gen-c` on `PATH`. It is a best-effort generator path and is
not the supported embedded C path for protos that use proto3 optional fields.

The embedded C example path requires:

- a nanopb generator available either as `protoc-gen-nanopb` or through
  `NANOPB_GENERATOR_MODULE=nanopb.generator.nanopb_generator`
- a nanopb source checkout containing `pb_common.c`, `pb_encode.c`,
  `pb_decode.c`, and the matching headers, passed via `NANOPB_DIR=...` or
  `NANOPB_RUNTIME_DIR=...`

`NANOPB_DIR` or `NANOPB_RUNTIME_DIR` should point at the root of an external
nanopb source checkout that contains the runtime C sources and headers such as
`pb_common.c`, `pb_encode.c`, `pb_decode.c`, `pb.h`, `pb_encode.h`, and
`pb_decode.h`. Pointing only at a pip-installed Python package is not
sufficient because it does not include those runtime C source files.

The repository convention is to keep nanopb sidecar generator settings under
`nanopb_options/`. In this repository:

- `*.proto` files define the GEISA protobuf wire/API contract
- `geisa-*-schema.json` files define the JSON validation contract
- `nanopb_options/*.options` files define nanopb sidecar generation settings
  for this repository's embedded C examples

The `.options` bounds support generated example code and embedded C memory
layout. They are not GEISA protocol limits and do not define universal device
requirements.

For this branch, the supported nanopb generator environment is the repository
`venv`. Create or refresh it with:

    bash scripts/setup-dev-venv.sh

The embedded C runtime C files still come from an external nanopb source
checkout passed through `NANOPB_DIR` or `NANOPB_RUNTIME_DIR`.

Linting uses Node.js and `npm`. Install the repository Node dependencies before
running lint commands:

    npm ci

Generated outputs are written under `build/`.

## Building protobuf outputs

The Makefile generates protobuf outputs into `build/`.

Generate the default binary protobuf descriptor outputs:

    make

Generate C++ protobuf bindings:

    make cpp

Generate C protobuf bindings using `protoc-c`:

    make c

Generate Java protobuf bindings:

    make java

Generate Python protobuf bindings:

    make python

Generate actuator embedded C bindings only:

    make actuator-c

Generate app-message embedded C bindings only:

    make app-message-c

Generate conn-status embedded C bindings only:

    make conn-status-c

Generate sensor embedded C bindings only:

    make sensor-c

Generate Platform Discovery embedded C bindings only:

    make discovery-c

Build the default example set:

    make examples

Build the embedded C example set explicitly:

    make examples-c NANOPB_DIR=/path/to/nanopb

Build the C++ example set explicitly:

    make examples-cpp

Generate all supported language outputs:

    make langs

Clean generated outputs:

    make clean

When `/tmp/nanopb` already exists as a valid nanopb checkout, `make examples`
builds both the C++ and embedded C example sets. Otherwise it builds the C++
examples and prints the exact primary setup command for the embedded C path.

The `make langs` target intentionally omits legacy protobuf-c generation so it
does not fail on repo protos that use proto3 optional. Legacy protobuf-c code
generation remains available separately through `make c`.

For embedded C, waveform remains on the C++ example path for GEISA 0.9. The
embedded C examples currently cover actuator, app-message, conn-status,
sensor, and Platform Discovery payloads.

## Examples

Example-specific build and run instructions are in the `examples/` directory:

- `examples/README.md`
- `examples/README-actuators.md`
- `examples/README-conn-status.md`
- `examples/README-discovery.md`
- `examples/README-networking.md`
- `examples/README-waveform.md`
- `examples/README-sensors.md`

You can build the default example workflow directly from the repository root:

    make examples

To build the C++ examples explicitly:

    make examples-cpp

To build the embedded C example set explicitly:

    make examples-c NANOPB_DIR=/path/to/nanopb

This compiles the example binaries into `build/examples/`.

A typical C++ example workflow is:

    make clean
    make examples

A typical embedded C workflow is the verified module path from the repository
root:

    bash scripts/setup-dev-venv.sh
    test -d /tmp/nanopb/.git || git clone https://github.com/nanopb/nanopb /tmp/nanopb
    make clean
    PYTHON="$(pwd)/venv/bin/python" \
    NANOPB_GENERATOR_MODULE=nanopb.generator.nanopb_generator \
    NANOPB_DIR=/tmp/nanopb \
    make examples-c

This is the supported generator path for this branch. The `PYTHON=...` setting
keeps generation anchored to the repository `venv` instead of whichever Python
interpreter happens to be first on `PATH`.  Once the venv has been set up along 
with the nanopb directory, subsequent make examples or examples-c will build
without issue.

`make setup-dev` runs the same bootstrap script if you prefer a Makefile
entrypoint.

If you already have a working `protoc-gen-nanopb` executable, you can use it
instead by setting `NANOPB_GENERATOR=protoc-gen-nanopb` or an explicit plugin
path as a shorter alternate path.

Then run the relevant binary from `build/examples/`. The topic-specific README
files include additional notes and, where useful, example command lines for the
available C++ and embedded C paths.

Where embedded C examples exist, they are the preferred example path for this
branch because the nanopb-based build is the path being kept aligned with
proto3 optional support and current embedded review needs. The C++ examples
remain available as reference examples and may be removed in a later cleanup.

For the embedded C examples, writer `--demo` modes are the primary end-to-end
walkthrough entry points: they generate the standard `/tmp` payloads and then
read them back through the companion decoder. Reader `--demo` modes are
decode-only and expect those standard `/tmp` payloads to already exist.

## Lint and validation

Linting is optional for consumers who only need to generate protobuf outputs,
but contributors should run it before opening pull requests.

After installing Node.js/`npm` and running `npm ci`, run all repository lint
checks with:

    npm run lint

To run markdown lint only:

    npm run lint:md

To run JSON syntax validation only:

    npm run lint:json

The lint baseline checks repository markdown and validates JSON files for
parseable JSON syntax. It does not perform JSON Schema semantic validation,
protobuf linting, or C/C++ formatting.

[geisa-spec]: https://github.com/geisa/specification
