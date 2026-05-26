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
- `examples/README*.md` files with example-specific build and run notes.

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

Note: current examples use the C++ generation path. Some messages use proto3
optional fields, and the protobuf-c generator used by `make c` may not support
those fields on all platforms.

## Examples

Example-specific build and run instructions are in the `examples/` directory:

- `examples/README.md`
- `examples/README-discovery.md`
- `examples/README-waveform.md`

You can build the C++ example binaries directly from the repository root:

    make examples

(Alias: `make examples-cpp`.)

This compiles the examples into `build/examples/`.

A typical C++ example workflow is:

    make clean
    make cpp

Then compile the desired example using the command shown in the relevant
example README.

## Lint and validation

Lint is currently enabled for markup/MD and JSON only at this time.
To run lint, follow the below instructions.

Install Node dependencies:

    npm ci

To run all repository lint checks:

    npm run lint

To run markdown (.md files) lint only:

    npm run lint:md

To run JSON syntax validation only:

    npm run lint:json

The lint baseline checks repository markdown and validates JSON
files for parseable JSON syntax. It does not perform JSON Schema semantic
validation, protobuf linting, or C/C++ formatting at this time.

[geisa-spec]: https://github.com/geisa/specification
