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
for the GEISA Platform Discovery response message
(`GeisaPlatformDiscovery_Rsp`) and render them in a JSON-like form.

## Build

Generate C++ protobuf sources from the repository root:

    make clean
    make cpp

Compile the examples (portable):

Writer:

    c++ -std=c++17 -O2 \
      -Ibuild/cpp \
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
      -Ibuild/cpp \
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

Run an end-to-end demo (writes a binary discovery response and immediately
decodes it):

    /tmp/discovery_write_response_example --demo

## Manual write + read loop

Write a sample binary discovery response:

    /tmp/discovery_write_response_example /tmp/discovery-response.bin

Decode and print it:

    /tmp/discovery_read_example /tmp/discovery-response.bin
