<!--
Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
Security Alliance (GEISA), a Series of LF Projects, LLC

Licensed under the Apache License, Version 2.0. See LICENSE.
-->

# Sensor examples (C++)

These examples demonstrate GEISA sensor request/response payload patterns using
JSON examples plus C++ protobuf writer/reader flows.

JSON examples in this directory include:

- explicit sensor readings request JSON:
  - `sensor-readings-request-example.json`
- sensor descriptor/temperature metadata JSON:
  - `sensor-temperature-example.json`
- sensor reading/response JSON:
  - `sensor-temperature-reading-example.json`
  - `sensor-temperature-response-example.json`

C++ examples:

- `sensor_write_response_example.cpp` writes a
  `GeisaSensorReadings_Rsp` binary protobuf payload
- `sensor_read_example.cpp` decodes a `GeisaSensorReadings_Rsp` binary
  protobuf payload and prints JSON-like diagnostic output

## Build

From the repository root:

    make clean
    make examples

Compiled binaries are written to:

    build/examples/

## Quick start

Run end-to-end demo mode:

    build/examples/sensor_write_response_example --demo

## Manual write + read loop

Write a sample sensor response payload:

    build/examples/sensor_write_response_example /tmp/sensor-response.bin

Decode that payload:

    build/examples/sensor_read_example /tmp/sensor-response.bin
