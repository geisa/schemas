<!--
Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
Security Alliance (GEISA), a Series of LF Projects, LLC

This file is licensed under the Community Specification License 1.0
available at:
https://github.com/geisa/specification/blob/main/LICENSE.md or
https://github.com/CommunitySpecification/Community_Specification/blob/main/1._Community_Specification_License-v1.md
-->

# GEISA Schema Examples

This directory contains JSON examples and small source examples for GEISA 
schema/protobuf messages.

## Sensor read C++ example

`sensor_read_example.cpp` demonstrates decoding a binary 
`GeisaSensorReadings_Rsp` protobuf payload and printing a JSON-like 
representation.

Build from the repository root:

    make clean
    make cpp
    c++ -std=c++17 -O2 \
      -Ibuild/cpp \
      $(pkg-config --cflags protobuf) \
      examples/sensor_read_example.cpp \
      build/cpp/sensor.pb.cc \
      build/cpp/geisa-status.pb.cc \
      $(pkg-config --libs protobuf) \
      -pthread \
      -o /tmp/sensor_read_example

If protobuf headers/libs are already on default paths, this also works:

    g++ -std=c++17 \
      -Ibuild/cpp \
      examples/sensor_read_example.cpp \
      build/cpp/sensor.pb.cc \
      build/cpp/geisa-status.pb.cc \
      -lprotobuf -pthread \
      -o /tmp/sensor_read_example

Run:

    /tmp/sensor_read_example sensor-response.bin

Note: `sensor.proto` uses proto3 optional fields for presence semantics. The
protobuf-c generator used by `make c` does not currently support proto3 
optional for this file. Use the C++ generation path for this example.

## Platform Discovery C++ examples

`discovery_write_response_example.cpp` writes a sample binary 
`GeisaPlatformDiscovery_Rsp` protobuf payload and can run an end-to-end demo 
that immediately decodes it.

`discovery_read_example.cpp` decodes a binary `GeisaPlatformDiscovery_Rsp` 
protobuf payload and prints a JSON-like representation.

See `README-discovery.md` for build and run instructions.

## Waveform subscribe + frame decode C++ example

`waveform_subscribe_and_read.cpp` demonstrates GEISA waveform subscribe 
response decoding and waveform frame parsing.

Build from the repository root:

    make clean
    make cpp

Compile (portable):

    c++ -std=c++17 -O2 \
      -Ibuild/cpp \
      $(pkg-config --cflags protobuf) \
      examples/waveform_subscribe_and_read.cpp \
      build/cpp/waveform.pb.cc \
      build/cpp/geisa-status.pb.cc \
      $(pkg-config --libs protobuf) \
      -pthread \
      -o /tmp/waveform_subscribe_and_read

Run:

    /tmp/waveform_subscribe_and_read --demo

See README-waveform.md for full details.
