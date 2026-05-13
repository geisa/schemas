<!--
Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
Security Alliance (GEISA), a Series of LF Projects, LLC

Licensed under the Community Specification License 1.0. See LICENSE.
-->

# Networking app-message examples (C++)

These examples demonstrate how to write and decode binary protobuf payloads
for GEISA app-message request submission and per-request disposition.
They are the GEISA message API flow, not direct IP socket policy.

- `GeisaAppMessage_Req`
- `GeisaAppMessage_Rsp`

MQTT topics:

- App publishes request to `geisa/api/message/req/<userid>`
- Platform publishes per-request disposition to
  `geisa/api/message/rsp/<userid>`

Scope notes:

- Quota counters and usage reporting are out-of-band via Platform/App Status.
- Deployment Manifest and Platform Discovery cover static policy/allowance
  context.
- Direct IP socket policy is out of scope for this message example.

Available response JSON examples:

- accepted
- queued
- quota-exceeded

Reader/output note:

- The C++ reader prints payload bytes as hex for terminal readability.
- JSON examples represent payload bytes as base64 when a payload is present.

## Build

Generate C++ protobuf sources from the repository root:

    make clean
    make cpp

Compile the examples (portable):

Writer:

    c++ -std=c++17 -O2 \
      -Ibuild/cpp \
      $(pkg-config --cflags protobuf) \
      examples/app_message_write_response_example.cpp \
      build/cpp/app-message.pb.cc \
      $(pkg-config --libs protobuf) \
      -pthread \
      -o /tmp/app_message_write_response_example

Reader:

    c++ -std=c++17 -O2 \
      -Ibuild/cpp \
      $(pkg-config --cflags protobuf) \
      examples/app_message_read_example.cpp \
      build/cpp/app-message.pb.cc \
      $(pkg-config --libs protobuf) \
      -pthread \
      -o /tmp/app_message_read_example

If protobuf headers/libs are already on default paths, you can omit the
pkg-config pieces and use `-lprotobuf` directly.

## Quick start (recommended)

Run end-to-end demo mode (writes request/response and immediately decodes both):

    /tmp/app_message_write_response_example --demo

## Manual write + read loop

Write request + response samples:

    /tmp/app_message_write_response_example /tmp/app-message-req.bin /tmp/app-message-rsp.bin

Decode and print request:

    /tmp/app_message_read_example --req /tmp/app-message-req.bin

Decode and print response:

    /tmp/app_message_read_example --rsp /tmp/app-message-rsp.bin
