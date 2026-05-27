<!--
Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
Security Alliance (GEISA), a Series of LF Projects, LLC

Licensed under the Apache License, Version 2.0. See LICENSE.
-->

# Networking app-message examples (C++)

These examples demonstrate how to write and decode binary protobuf payloads
for GEISA app-message request submission and per-request disposition.
They are the GEISA message API flow, not direct IP socket policy.

- `GeisaAppMessage_Req`
- `GeisaAppMessage_Rsp`

MQTT topics:

- App publishes request to `geisa/api/message/upstream/req/<userid>`
- Platform publishes per-request disposition to
  `geisa/api/message/upstream/rsp/<userid>`

Scope notes:

- `GEISA_APP_MESSAGE_STATUS_QUOTA_EXCEEDED` means the platform rejected the
  request because accepting it would exceed an applicable quota or policy
  limit (for example message-count, byte/volume, or network policy limits).
- Quota counters/usage/remaining/reset reporting remains out-of-band via
  Platform/App Status (`conn_msg`, `conn_oper`, `conn_inet`, `conn_local`,
  and `GeisaConnAppInfo` surfaces).
- Deployment Manifest and Platform Discovery cover static policy/allowance
  context.
- Direct IP socket policy is out of scope for this message example.

Available response JSON examples:

- accepted
- unavailable
- quota-exceeded

Reader/output note:

- The C++ reader prints payload bytes as hex for terminal readability.
- JSON examples represent payload bytes as base64 when a payload is present.
- Shared reader formatting helpers live in
  `examples/helpers/example_utils.h` so the app-message examples can stay
  focused on message semantics. This helper is example support code, not a
  GEISA SDK.

## Build

From the repository root, the recommended build path is:

    make clean
    make examples

This generates protobuf sources and compiles the C++ examples into
`build/examples/`.

The manual commands below are retained for developers who want to compile a
single example directly or inspect the exact compiler inputs.

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

Using the Makefile-built binaries:

    build/examples/app_message_write_response_example --demo

## Manual write + read loop

Write request + response samples:

    build/examples/app_message_write_response_example /tmp/app-message-req.bin /tmp/app-message-rsp.bin

Decode and print request:

    build/examples/app_message_read_example --req /tmp/app-message-req.bin

Decode and print response:

    build/examples/app_message_read_example --rsp /tmp/app-message-rsp.bin
