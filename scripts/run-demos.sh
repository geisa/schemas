#!/bin/sh
#-----------------------------------------------------------------------------
# Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
# Security Alliance (GEISA), a Series of LF Projects, LLC
#
# Licensed under the Apache License, Version 2.0. See LICENSE.
#-----------------------------------------------------------------------------

set -eu

ROOT_DIR="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$ROOT_DIR"

needs_build=no

section()
{
  printf '\n==> %s\n' "$1"
}

check_demo_binary()
{
  source_path="$1"
  binary_path="$2"

  if [ -f "$source_path" ] && [ ! -x "$binary_path" ]; then
    needs_build=yes
  fi
}

run_demo()
{
  label="$1"
  source_path="$2"
  binary_path="$3"
  shift 3

  section "$label"

  if [ ! -f "$source_path" ]; then
    printf 'SKIP %s (source not present on this branch)\n' "$binary_path"
    return
  fi

  if [ ! -x "$binary_path" ]; then
    printf 'ERROR: required demo binary missing after make examples: %s\n' "$binary_path" >&2
    exit 1
  fi

  "$binary_path" "$@"
}

check_demo_binary examples/actuator_write_response_example.c build/examples/actuator_write_response_example_c
check_demo_binary examples/app_message_write_response_example.c build/examples/app_message_write_response_example_c
check_demo_binary examples/conn_status_write_example.c build/examples/conn_status_write_example_c
check_demo_binary examples/discovery_write_request_example.c build/examples/discovery_write_request_example_c
check_demo_binary examples/discovery_write_response_example.c build/examples/discovery_write_response_example_c
check_demo_binary examples/sensor_write_response_example.c build/examples/sensor_write_response_example_c
check_demo_binary examples/metered_quantities_write_example.c build/examples/metered_quantities_write_example_c
check_demo_binary examples/waveform_subscribe_and_read.cpp build/examples/waveform_subscribe_and_read

if [ "$needs_build" = yes ]; then
  section "Building examples"
  make examples
fi

run_demo "Actuator writer demo" \
  examples/actuator_write_response_example.c \
  build/examples/actuator_write_response_example_c \
  --demo

run_demo "App-message writer demo" \
  examples/app_message_write_response_example.c \
  build/examples/app_message_write_response_example_c \
  --demo

run_demo "Connection status writer demo" \
  examples/conn_status_write_example.c \
  build/examples/conn_status_write_example_c \
  --demo

run_demo "Platform Discovery request writer demo" \
  examples/discovery_write_request_example.c \
  build/examples/discovery_write_request_example_c \
  --demo

run_demo "Platform Discovery response writer demo" \
  examples/discovery_write_response_example.c \
  build/examples/discovery_write_response_example_c \
  --demo

run_demo "Sensor writer demo" \
  examples/sensor_write_response_example.c \
  build/examples/sensor_write_response_example_c \
  --demo

run_demo "Metered quantities writer demo" \
  examples/metered_quantities_write_example.c \
  build/examples/metered_quantities_write_example_c \
  --demo

run_demo "Waveform demo" \
  examples/waveform_subscribe_and_read.cpp \
  build/examples/waveform_subscribe_and_read \
  --demo
