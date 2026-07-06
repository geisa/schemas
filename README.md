# GEISA schemas

This repository contains the [protobuf](https://protobuf.dev/) and
[JSON Schema](https://json-schema.org/) schemas used by the
[Grid Edge Interoperability and Security Alliance
Specification](https://github.com/geisa/specification).

These schemas define message structures used by the GEISA API, message bus,
application manifests, and related examples.

For full documentation and context, refer to the
[GEISA Specification][geisa-spec].

## Repository contents

At a high level, this repository contains:

- `*.proto` files for protobuf message definitions.
- `geisa-*-schema.json` files for JSON Schema definitions.
- `profiles/*.json` files for additive conformance and capability profile
  overlays.
- `examples/*.json` files with example payloads.
- `examples/*.cpp` files with small C++ protobuf encode/decode examples.
- `examples/helpers/example_utils.h` for shared example-only C++ helper code
  (not a GEISA SDK).
- `examples/README*.md` files with example-specific build and run notes.

This schemas repository SHALL be used in conjunction with the GEISA
specification to enable creation of GEISA conformant implementations. The
content is intended to help users understand the structure, conformance, and
intent of GEISA
messages and payloads.

Most API message payloads are defined in the GEISA specification as well as here
in proto files, documented in JSON schemas, implemented with example code, and
further documented with example payload contents.

While efforts have been made to keep items provided in multiple places,
formats, or examples consistent and free of ambiguity, an
implementer should be able to resolve conflicts or discrepancies found within
this content.

For items with proto files, the proto files are authoritative, while JSON
schemas, examples, and lists in the GEISA specification for the same item are
not.

For items without proto files, the JSON schema files are authoritative, while
examples and lists in the GEISA specification for the same item are not.

Profile schemas under `profiles/` are additive overlays on top of the base JSON
schemas. They are intended for conformance validation and implementation
guidance for a claimed device or capability profile without implying that all
GEISA devices must implement all APIs or expose all measurements.

## Prerequisites

The repository build targets assume these tools are available on `PATH`:

- `make`
- `protoc` (Protocol Buffers compiler)

The C++ example build target also requires:

- a C++17 compiler, such as `c++`, `g++`, or `clang++`
- `pkg-config`
- protobuf development headers and libraries discoverable through
  `pkg-config protobuf`

The optional C generation target requires protobuf-c support, including
`protoc-gen-c`, available on `PATH`.

The embedded C example path requires:

- a nanopb generator executable available as `protoc-gen-nanopb` or passed via
  `NANOPB_GENERATOR=...`
- a nanopb source checkout containing `pb_common.c`, `pb_encode.c`,
  `pb_decode.c`, and the matching headers, passed via `NANOPB_DIR=...` or
  `NANOPB_RUNTIME_DIR=...`

The embedded C example path does not currently require or provide a
repo-managed Python virtual environment. Any working
`protoc-gen-nanopb` executable is acceptable, whether installed system-wide
or provided by a user-managed local environment.

`NANOPB_DIR` or `NANOPB_RUNTIME_DIR` should point at the root of an external
nanopb source checkout that contains the runtime C sources and headers such as
`pb_common.c`, `pb_encode.c`, `pb_decode.c`, `pb.h`, `pb_encode.h`, and
`pb_decode.h`. Pointing only at a pip-installed Python package is not
sufficient because it does not include those runtime C source files.

The repository convention is to keep nanopb sidecar generator settings under
`nanopb_options/`. In this repository:

- `*.proto` files define the GEISA protobuf wire/API contract
- `geisa-*-schema.json` files define the JSON validation contract
- `nanopb_options/*.options` files define nanopb C generation and allocation
  constraints such as bounded repeated fields or other C memory-layout choices

Some GEISA proto files use proto3 optional fields, bounded repeated fields,
strings, bytes, callbacks, static allocation, or related C-oriented generation
decisions that require nanopb sidecar options. The pip-installed `nanopb`
package provides the generator, but it does not
provide the nanopb runtime C source files (`pb_common.c`, `pb_encode.c`,
`pb_decode.c`) used by the example build.

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

Generate metered quantities embedded C bindings only:

    make metered-c

Build the default example set:

    make examples

Build the C++ example set explicitly:

    make examples-cpp

Build the embedded C example set after setting an external nanopb source tree:

    make examples-c NANOPB_DIR=/path/to/nanopb

A typical embedded C metered quantities workflow is:

    make metered-c NANOPB_GENERATOR=protoc-gen-nanopb
    make examples-metered NANOPB_GENERATOR=protoc-gen-nanopb \
      NANOPB_DIR=/path/to/nanopb-checkout
    build/examples/metered_quantities_write_example --demo

Generate all language outputs:

    make langs

Clean generated outputs:

    make clean

Note: the default examples use the C++ generation path. Some messages use proto3
optional fields, and the protobuf-c generator used by `make c` may not support
those fields on all platforms.

## Examples

Example-specific build and run instructions are in the `examples/` directory:

- `examples/README.md`
- `examples/README-actuators.md`
- `examples/README-discovery.md`
- `examples/README-metered-quantities.md`
- `examples/README-waveform.md`
- `examples/README-sensors.md`

You can build the default example workflow directly from the repository root:

    make examples

By default, `make examples` builds the C++ example set and prints a note
explaining that embedded C examples are available separately through
`make examples-c NANOPB_DIR=/path/to/nanopb`.

To build the C++ examples explicitly:

    make examples-cpp

To build the embedded C example set explicitly:

    make examples-c NANOPB_DIR=/path/to/nanopb

This compiles the example binaries into `build/examples/`.

A typical C++ example workflow is:

    make clean
    make examples

Then run the relevant binary from `build/examples/`. The topic-specific README
files include additional notes and, where useful, manual single-example
compile commands for developers who want to inspect the exact compiler inputs.

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

The lint baseline checks repository markdown and validates JSON
files for parseable JSON syntax. It does not perform JSON Schema semantic
validation, protobuf linting, or C/C++ formatting.

[geisa-spec]: https://github.com/geisa/specification
