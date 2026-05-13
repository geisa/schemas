<!--
Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
Security Alliance (GEISA), a Series of LF Projects, LLC

This file is licensed under the Community Specification License 1.0
available at:
https://github.com/geisa/specification/blob/main/LICENSE.md or
https://github.com/CommunitySpecification/
Community_Specification/blob/main/
1._Community_Specification_License-v1.md
-->

# Platform Discovery examples (C++)

These examples demonstrate how to write and decode binary protobuf payloads
for the GEISA Platform Discovery request/response messages:

- `GeisaPlatformDiscovery_Req`
- `GeisaPlatformDiscovery_Rsp`

The response reader renders `GeisaPlatformDiscovery_Rsp` in a JSON-like form.
The sample response includes static network capability descriptors in addition
to device, metrology, sensor, and waveform metadata.

Note: the C++ reader prints protobuf enum identifiers for diagnostic output.
JSON schema examples use the corresponding JSON enum strings.

## Build

Generate the C++ protobuf sources from the repository root:

    make clean
    make cpp

Compile the examples:

Request writer:

    c++ -std=c++17 -O2 \
      -I build/cpp \
      $(pkg-config --cflags protobuf) \
      examples/discovery_write_request_example.cpp \
      build/cpp/discovery.pb.cc \
      $(pkg-config --libs protobuf) \
      -pthread \
      -o /tmp/discovery_write_request_example

Writer:

    c++ -std=c++17 -O2 \
      -I build/cpp \
      $(pkg-config --cflags protobuf) \
      examples/discovery_write_response_example.cpp \
      build/cpp/discovery.pb.cc \
      build/cpp/sensor.pb.cc \
      build/cpp/geisa-status.pb.cc \
      $(pkg-config --libs protobuf) \
      -pthread \
      -o /tmp/discovery_write_response_example

Reader:

    c++ -std=c++17 -O2 \
      -I build/cpp \
      $(pkg-config --cflags protobuf) \
      examples/discovery_read_example.cpp \
      build/cpp/discovery.pb.cc \
      build/cpp/sensor.pb.cc \
      build/cpp/geisa-status.pb.cc \
      $(pkg-config --libs protobuf) \
      -pthread \
      -o /tmp/discovery_read_example

If protobuf headers/libs are already on default paths, you can omit the
pkg-config pieces and use `-lprotobuf` directly.

## Quick start (recommended)

Write a sample binary discovery request:

    /tmp/discovery_write_request_example --demo

Platform Discovery requests do not define payload fields in GEISA v0.9. The
requesting application instance is identified by the `<userid>` segment in the
MQTT topic (`geisa/api/platform/discovery/req/<userid>`), and the platform
responds on `geisa/api/platform/discovery/rsp/<userid>` with the current
Platform Discovery snapshot.

To run an end-to-end demo (writes a binary discovery response and immediately
decodes it):

    /tmp/discovery_write_response_example --demo

## Manual write + read loop

To write a sample binary discovery response:

    /tmp/discovery_write_response_example /tmp/discovery-response.bin

To decode the response:

    /tmp/discovery_read_example /tmp/discovery-response.bin
