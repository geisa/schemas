<!--
Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
Security Alliance (GEISA), a Series of LF Projects, LLC

Licensed under the Apache License, Version 2.0. See LICENSE.
-->

# Actuator Examples

This guide covers GEISA actuator query and control example payloads and the
companion C++ protobuf reader/writer examples.

Actuator operations are permission-gated by deployment manifest and platform
policy. Examples here show payload structure and expected request/response
patterns only; they are not safety or authorization workflow definitions.

Note the JSON files are human-readable schema examples. Runtime GEISA actuator
messages use protobuf payloads; JSON examples are provided for review,
diagnostics, tooling, and tests that convert protobuf messages to JSON.

## Build

From the repository root:

    make clean
    make examples

Example binaries are written to:

    build/examples/

## Example JSON files

- `actuator-get-request-service-switch-example.json`
- `actuator-get-response-service-switch-closed-example.json`
- `actuator-set-request-load-control-relay-example.json`
- `actuator-set-response-accepted-example.json`

These examples use realistic utility-aligned scenarios:

- get request/response for `service-switch`
- set request for permission-gated `lc-relay-0`

## C++ examples

- `build/examples/actuator_read_example`
- `build/examples/actuator_write_response_example`

`actuator_read_example` requires an explicit decode mode:

    build/examples/actuator_read_example --get-req <file>
    build/examples/actuator_read_example --get-rsp <file>
    build/examples/actuator_read_example --set-req <file>
    build/examples/actuator_read_example --set-rsp <file>

Run the end-to-end demo writer flow:

    build/examples/actuator_write_response_example --demo

The writer emits representative get/set request and response binaries and then
invokes the reader to decode each output.

Helper code under `examples/helpers/example_utils.h` are intended as examples
only, and are not part of the GEISA APIs nor SDK.
