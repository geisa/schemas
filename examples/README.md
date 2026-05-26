<!--
Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
Security Alliance (GEISA), a Series of LF Projects, LLC

Licensed under the Apache License, Version 2.0. See LICENSE.
-->

# GEISA Schema Examples

This directory contains small source code examples and JSON examples for GEISA
schema/protobuf messages.

These examples are intended as learning references: each writer emits a
binary protobuf payload, and each reader decodes that payload into a concise
JSON-like diagnostic view. Most include a --demo mode which will output
both the writer and the receiving reader outputs.

## Sensor reading C++ example

`sensor_read_example.cpp` demonstrates decoding a binary
`GeisaSensorReadings_Rsp` protobuf payload and printing a JSON-like
representation.

Build from the repository root:

    make clean
    make examples

This generates protobuf sources and compiles the C++ examples into
`build/examples/`.

For manual or single-example builds, use the commands below.

Manual build from the repository root:

    make clean
    make cpp
    c++ -std=c++17 -O2 \
      -I build/cpp \
      $(pkg-config --cflags protobuf) \
      examples/sensor_read_example.cpp \
      build/cpp/sensor.pb.cc \
      build/cpp/geisa-status.pb.cc \
      $(pkg-config --libs protobuf) \
      -pthread \
      -o /tmp/sensor_read_example

If protobuf headers/libs are already on default paths, this also works:

    g++ -std=c++17 \
      -I build/cpp \
      examples/sensor_read_example.cpp \
      build/cpp/sensor.pb.cc \
      build/cpp/geisa-status.pb.cc \
      -lprotobuf -pthread \
      -o /tmp/sensor_read_example

Run:

    build/examples/sensor_read_example sensor-response.bin

If you used the manual compile commands below, use:

    /tmp/sensor_read_example sensor-response.bin

Note: `sensor.proto` uses proto3 optional fields for presence semantics. The
protobuf-c generator used by `make c` does not currently support proto3
`optional` for this file. Use the C++ generation path for this example.

## Platform Discovery C++ examples

`discovery_write_request_example.cpp` writes a binary
`GeisaPlatformDiscovery_Req` protobuf payload.

`discovery_write_response_example.cpp` writes a sample binary
`GeisaPlatformDiscovery_Rsp` protobuf payload and can run an end-to-end demo
that immediately decodes it.

`discovery_read_example.cpp` decodes a binary `GeisaPlatformDiscovery_Rsp`
protobuf payload and prints a JSON-like representation.

See `README-discovery.md` for build and run instructions.

## Networking / app-message C++ examples

`app_message_write_response_example.cpp` writes one
`GeisaAppMessage_Req` payload and one `GeisaAppMessage_Rsp` payload.
In `--demo` mode it immediately decodes both files through the companion
reader.

`app_message_read_example.cpp` decodes either `GeisaAppMessage_Req` or
`GeisaAppMessage_Rsp` and prints a JSON-like representation.

See `README-networking.md` for build and run instructions.

## Waveform subscribe + frame decode C++ example

`waveform_subscribe_and_read.cpp` demonstrates GEISA waveform subscribe
response decoding and waveform frame parsing.

Build from the repository root:

    make clean
    make examples

This generates protobuf sources and compiles the C++ examples into
`build/examples/`.

For manual or single-example builds, use the commands below.

Manual build from the repository root:

    make clean
    make cpp

Compile (portable):

    c++ -std=c++17 -O2 \
      -I build/cpp \
      $(pkg-config --cflags protobuf) \
      examples/waveform_subscribe_and_read.cpp \
      build/cpp/waveform.pb.cc \
      build/cpp/geisa-status.pb.cc \
      $(pkg-config --libs protobuf) \
      -pthread \
      -o /tmp/waveform_subscribe_and_read

Run:

    build/examples/waveform_subscribe_and_read --demo

If you used the manual compile commands below, use:

    /tmp/waveform_subscribe_and_read --demo

See README-waveform.md for full details.
