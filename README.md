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
- `examples/*.json` files with example payloads.
- `examples/*.cpp` files with small C++ protobuf encode/decode examples.
- `examples/helpers/example_utils.h` for shared example-only C++ helper code
  (not a GEISA SDK).
- `examples/README*.md` files with example-specific build and run notes.

This schemas repository SHALL be used in conjunction with the GEISA
specification to enable creation of GEISA conformant implementations. The
content within has been provided with the intent for the user to fully
understand the structure of, conformance with, and the intent of GEISA
messages and payloads.

Most API message payloads are defined in the GEISA specification as well as here
in proto files, documented in JSON schemas, implemented with example code, and
further documented with example payload contents.

While efforts have been made to make sure that items provided in multiple
places, formats, or examples are consistent and free of ambiguity, an
implementer should be able to resolve conflicts or discrepancies found within
this content.

For items with proto files, the proto files are authoritative, while JSON
schemas, examples, and lists in the GEISA specification for the same item are
not.

For items without proto files, the JSON schema files are authoritative, while
examples and lists in the GEISA specification for the same item are not.

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

Generate all language outputs:

    make langs

Clean generated outputs:

    make clean

Note: Most current examples use the C++ generation path. Some messages use
proto3 optional fields, and the protobuf-c generator used by `make c` may not
support those fields on all platforms.  The GEISA schemas repo is currently
transitioning to use nanopb to support optionality.

## Examples

Example-specific build and run instructions are in the `examples/` directory:

- `examples/README.md`
- `examples/README-actuators.md`
- `examples/README-discovery.md`
- `examples/README-waveform.md`
- `examples/README-sensors.md`

You can build the C++ example binaries directly from the repository root:

    make examples

(Alias: `make examples-cpp`.)

This compiles the examples into `build/examples/`.

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

To run JSON Schema semantic validation for the checked example payloads:

    npm run lint:schema

A clean rebuild that also refreshes generated artifacts is:

    make clean all

The lint baseline checks repository markdown, validates repository JSON files
for parseable syntax, and validates checked example payloads against their
matching JSON Schemas. It does not perform protobuf linting or C/C++
formatting at this time.

[geisa-spec]: https://github.com/geisa/specification
