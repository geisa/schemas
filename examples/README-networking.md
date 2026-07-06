<!--
Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
Security Alliance (GEISA), a Series of LF Projects, LLC

Licensed under the Apache License, Version 2.0. See LICENSE.
-->

# Networking app-message examples

These examples demonstrate how to write and decode the GEISA app-message
protobuf envelopes for request submission and per-request disposition. The
message payload bytes are opaque and interpreted via `content-type`, not as
protobuf by default.

- `GeisaAppMessage_Req`
- `GeisaAppMessage_Rsp`

MQTT topics:

- App publishes request to `geisa/api/message/upstream/req/<userid>`
- Platform publishes per-request disposition to
  `geisa/api/message/upstream/rsp/<userid>`

Scope notes:

- `GEISA_APP_MESSAGE_STATUS_QUOTA_EXCEEDED` means the platform rejected the
  request because accepting it would exceed an applicable quota or policy
  limit.
- Quota counters, usage, remaining, and reset reporting remain out-of-band via
  Platform/App Status surfaces.
- Deployment Manifest and Platform Discovery cover static policy and allowance
  context.

## Build

From the repository root:

    make clean
    make examples

To build the embedded C app-message examples:

    make app-message-c NANOPB_GENERATOR=protoc-gen-nanopb
    make examples-app-message NANOPB_GENERATOR=protoc-gen-nanopb \
      NANOPB_DIR=/path/to/nanopb

Example binaries are written to:

    build/examples/

## C++ examples

- `build/examples/app_message_read_example`
- `build/examples/app_message_write_response_example`

Quick start:

    build/examples/app_message_write_response_example --demo

Manual write + read loop:

    build/examples/app_message_write_response_example \
      /tmp/app-message-req.bin /tmp/app-message-rsp.bin
    build/examples/app_message_read_example --req /tmp/app-message-req.bin
    build/examples/app_message_read_example --rsp /tmp/app-message-rsp.bin

## Embedded C examples

- `build/examples/app_message_read_example_c`
- `build/examples/app_message_write_response_example_c`

Quick start:

    build/examples/app_message_write_response_example_c --demo

Manual write + read loop:

    build/examples/app_message_write_response_example_c
    build/examples/app_message_read_example_c --req /tmp/app-message-req.bin
    build/examples/app_message_read_example_c --rsp /tmp/app-message-rsp.bin
