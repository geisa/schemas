<!--
Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
Security Alliance (GEISA), a Series of LF Projects, LLC

Licensed under the Apache License, Version 2.0. See LICENSE.
-->

# Metered quantities examples

These examples write and read metered quantity protobuf payloads using the
embedded C generation path (nanopb).

## What the example covers

- GEISA 0.9 AC meter profile required instantaneous scalar fields
- GEISA 0.9 billing profile required billing fields
- proto3 optional scalar presence
- fixed-point electrical values in micro-units
- power factor as a ratio
- bounded harmonic arrays suitable for nanopb static allocation

Note that the writer `--demo` mode writes both standard `/tmp` payloads and
immediately walks through the instantaneous and billing decode output. The
demo values model a plausible shoulder-season residential snapshot with a
~1,000 kWh/month baseline. Reader `--demo` decodes those same standard `/tmp`
payloads and prints both sections directly.

## Build

From the repository root:

    make metered-c

To also compile the embedded C metered examples, point `NANOPB_DIR` or
`NANOPB_RUNTIME_DIR` at a nanopb source tree containing `pb_common.c`,
`pb_encode.c`, and `pb_decode.c`:

    make examples-metered NANOPB_DIR=/path/to/nanopb

Or use the aggregate embedded C example path:

    make examples-c NANOPB_DIR=/path/to/nanopb

If `protoc-gen-nanopb` is not on `PATH`, set:

    make metered-c NANOPB_GENERATOR=/path/to/protoc-gen-nanopb

The C build uses `protoc-gen-nanopb` and an external nanopb source tree
supplied through `NANOPB_DIR` or `NANOPB_RUNTIME_DIR`. The pip-installed
`nanopb` package provides the generator, but not the runtime C source files
used by these examples.

These metered embedded C examples do not currently require or provide a
repo-managed Python virtual environment. Any working `protoc-gen-nanopb`
executable is acceptable, whether installed system-wide or provided by a
user-managed local environment.

`NANOPB_DIR` or `NANOPB_RUNTIME_DIR` should point at the root of an external
nanopb source checkout that contains the runtime C sources and headers such as
`pb_common.c`, `pb_encode.c`, `pb_decode.c`, `pb.h`, `pb_encode.h`, and
`pb_decode.h`. Pointing only at a pip-installed Python package is not
sufficient because it does not include those runtime C source files.

A typical quick-start workflow is:

    make metered-c NANOPB_GENERATOR=protoc-gen-nanopb
    make examples-metered NANOPB_GENERATOR=protoc-gen-nanopb \
      NANOPB_DIR=/path/to/nanopb-checkout
    build/examples/metered_quantities_write_example --demo

## Run

Write the standard demo payloads to `/tmp`:

    build/examples/metered_quantities_write_example

Run the writer quick-start walkthrough:

    build/examples/metered_quantities_write_example --demo

Run the reader quick-start walkthrough:

    build/examples/metered_quantities_read_example --demo

Decode the instantaneous payload:

    build/examples/metered_quantities_read_example instantaneous \
      /tmp/geisa-metered-instantaneous.bin

Decode the billing payload:

    build/examples/metered_quantities_read_example billing \
      /tmp/geisa-metered-billing.bin

