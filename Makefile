###############################################################################
# Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
# Security Alliance (GEISA), a Series of LF Projects, LLC
#
# Licensed under the Apache License, Version 2.0. See LICENSE.
###############################################################################
# Makefile for building:
#   - default: .binpb files from .proto files
#   - make c:    protobuf-c .pb-c.c / .pb-c.h
#   - make cpp:  C++ .pb.cc / .pb.h
#   - make java: Java .java files
#   - make python: Python .py files
###############################################################################
# NOTE: Ubuntu LTS 22.04 may require the
# --experimental_allow_proto3_optional flag for proto3 optional support.
###############################################################################

PROTOS ?= $(wildcard *.proto)

BUILDDIR = build
BINPBDIR = $(BUILDDIR)/binpb
CDIR = $(BUILDDIR)/c
CPPDIR = $(BUILDDIR)/cpp
JAVADIR = $(BUILDDIR)/java
PYTHONDIR = $(BUILDDIR)/python
EXAMPLESDIR = $(BUILDDIR)/examples
NANOPBDIR = $(BUILDDIR)/nanopb
PROTOC ?= protoc
# Prefer the repo venv for nanopb module generation when it exists.
PYTHON ?= $(if $(wildcard $(CURDIR)/venv/bin/python),$(CURDIR)/venv/bin/python,python3)
NANOPB_HOST_PYTHON ?= $(if $(wildcard /usr/local/bin/python3),/usr/local/bin/python3,python3)
NANOPB_GENERATOR ?= protoc-gen-nanopb
NANOPB_GENERATOR_MODULE ?=
NANOPB_DIR ?=
ifeq ($(strip $(NANOPB_DIR)),)
ifneq ($(and $(wildcard /tmp/nanopb/pb_common.c),$(wildcard /tmp/nanopb/pb_encode.c),$(wildcard /tmp/nanopb/pb_decode.c)),)
NANOPB_DIR := /tmp/nanopb
endif
endif
NANOPB_RUNTIME_DIR ?= $(NANOPB_DIR)
NANOPB_OPTIONS_DIR ?= nanopb_options
NANOPB_RUNTIME_DIR_EFFECTIVE = $(strip $(NANOPB_RUNTIME_DIR))
NANOPB_VENV_SITE_PACKAGES := $(firstword $(wildcard $(CURDIR)/venv/lib/python*/site-packages))

# Check protoc version for proto3 optional support
PROTOC_VERSION := $(shell $(PROTOC) --version | awk '{print $$2}')

PROTOC_FLAGS ?=
ifeq ($(shell printf "3.12.0\n$(PROTOC_VERSION)\n" | sort -V | head -n1),3.12.0)
  ifneq ($(shell printf "$(PROTOC_VERSION)\n3.15.0\n" | sort -V | head -n1),3.15.0)
    PROTOC_FLAGS += --experimental_allow_proto3_optional
  endif
endif

###############################################################################
# Default .binpb outputs
###############################################################################
BINPBS = $(patsubst %.proto,$(BINPBDIR)/%.binpb,$(PROTOS))

###############################################################################
# C outputs (protobuf-c)
###############################################################################
C_SRCS = $(patsubst %.proto,$(CDIR)/%.pb-c.c,$(PROTOS))
C_HDRS = $(patsubst %.proto,$(CDIR)/%.pb-c.h,$(PROTOS))

###############################################################################
# C++ outputs
###############################################################################
CPP_SRCS = $(patsubst %.proto,$(CPPDIR)/%.pb.cc,$(PROTOS))
CPP_HDRS = $(patsubst %.proto,$(CPPDIR)/%.pb.h,$(PROTOS))

###############################################################################
# Java outputs
###############################################################################
JAVA_STAMP = $(JAVADIR)/.java_generated

###############################################################################
# Python outputs
###############################################################################
PYTHON_STAMP = $(PYTHONDIR)/.python_generated

###############################################################################
# Embedded C outputs
###############################################################################
NANOPB_STATUS_PROTO = geisa-status.proto
NANOPB_STATUS_OPTIONS = $(NANOPB_OPTIONS_DIR)/geisa-status.options
NANOPB_STATUS_GEN = \
	$(NANOPBDIR)/geisa-status.pb.c \
	$(NANOPBDIR)/geisa-status.pb.h
NANOPB_STATUS_STAMP = $(NANOPBDIR)/.geisa-status.nanopb.stamp

NANOPB_ACTUATOR_PROTO = actuator.proto
NANOPB_ACTUATOR_OPTIONS = \
	$(NANOPB_OPTIONS_DIR)/actuator.options
NANOPB_ACTUATOR_GEN = \
	$(NANOPBDIR)/actuator.pb.c \
	$(NANOPBDIR)/actuator.pb.h
NANOPB_ACTUATOR_STAMP = $(NANOPBDIR)/.actuator.nanopb.stamp
NANOPB_ACTUATOR_EXAMPLES = \
	$(EXAMPLESDIR)/actuator_write_response_example_c \
	$(EXAMPLESDIR)/actuator_read_example_c

NANOPB_APP_MESSAGE_PROTO = app-message.proto
NANOPB_APP_MESSAGE_OPTIONS = $(NANOPB_OPTIONS_DIR)/app-message.options
NANOPB_APP_MESSAGE_GEN = \
	$(NANOPBDIR)/app-message.pb.c \
	$(NANOPBDIR)/app-message.pb.h
NANOPB_APP_MESSAGE_STAMP = $(NANOPBDIR)/.app-message.nanopb.stamp
NANOPB_APP_MESSAGE_EXAMPLES = \
	$(EXAMPLESDIR)/app_message_write_response_example_c \
	$(EXAMPLESDIR)/app_message_read_example_c

NANOPB_SENSOR_PROTO = sensor.proto
NANOPB_SENSOR_OPTIONS = $(NANOPB_OPTIONS_DIR)/sensor.options
NANOPB_SENSOR_GEN = \
	$(NANOPBDIR)/sensor.pb.c \
	$(NANOPBDIR)/sensor.pb.h
NANOPB_SENSOR_STAMP = $(NANOPBDIR)/.sensor.nanopb.stamp
NANOPB_SENSOR_EXAMPLES = \
	$(EXAMPLESDIR)/sensor_write_response_example_c \
	$(EXAMPLESDIR)/sensor_read_example_c

NANOPB_WAVEFORM_PROTO = waveform.proto
NANOPB_WAVEFORM_GEN = \
	$(NANOPBDIR)/waveform.pb.c \
	$(NANOPBDIR)/waveform.pb.h
NANOPB_WAVEFORM_STAMP = $(NANOPBDIR)/.waveform.nanopb.stamp

NANOPB_DISCOVERY_PROTO = discovery.proto
NANOPB_DISCOVERY_OPTIONS = $(NANOPB_OPTIONS_DIR)/discovery.options
NANOPB_DISCOVERY_GEN = \
	$(NANOPBDIR)/discovery.pb.c \
	$(NANOPBDIR)/discovery.pb.h
NANOPB_DISCOVERY_STAMP = $(NANOPBDIR)/.discovery.nanopb.stamp
NANOPB_DISCOVERY_EXAMPLES = \
	$(EXAMPLESDIR)/discovery_write_request_example_c \
	$(EXAMPLESDIR)/discovery_write_response_example_c \
	$(EXAMPLESDIR)/discovery_read_example_c

define NANOPB_GENERATE
	@mkdir -p $(NANOPBDIR)
	@if [ -n "$(strip $(NANOPB_GENERATOR_MODULE))" ]; then \
	  module_status=0; \
	  $(PYTHON) -m $(NANOPB_GENERATOR_MODULE) -D $(NANOPBDIR) -I $(NANOPB_OPTIONS_DIR) --protoc-opt=-I. $(1) || module_status=$$?; \
	  if { [ $$module_status -ne 0 ] || [ ! -f "$(word 1,$(2))" ]; } && [ -n "$(NANOPB_VENV_SITE_PACKAGES)" ]; then \
	    PYTHONPATH="$(NANOPB_VENV_SITE_PACKAGES)$${PYTHONPATH:+:$$PYTHONPATH}" $(NANOPB_HOST_PYTHON) -m $(NANOPB_GENERATOR_MODULE) -D $(NANOPBDIR) -I $(NANOPB_OPTIONS_DIR) --protoc-opt=-I. $(1) || exit $$?; \
	  elif [ $$module_status -ne 0 ]; then \
	    exit $$module_status; \
	  fi; \
	else \
	  plugin="$(NANOPB_GENERATOR)"; \
	  if [ -x "$$plugin" ]; then \
	    plugin="$$(cd "$$(dirname "$$plugin")" && pwd)/$$(basename "$$plugin")"; \
	  elif command -v "$$plugin" >/dev/null 2>&1; then \
	    plugin="$$(command -v "$$plugin")"; \
	  else \
	    echo "Missing nanopb generator: $(NANOPB_GENERATOR)"; \
	    echo "Set NANOPB_GENERATOR to protoc-gen-nanopb or an explicit generator path."; \
	    echo "Alternatively, set NANOPB_GENERATOR_MODULE=nanopb.generator.nanopb_generator when the nanopb Python package is importable."; \
	    exit 1; \
	  fi; \
	  if head -1 "$$plugin" | grep -q '^#!'; then \
	    plugin_wrapper="$(NANOPBDIR)/protoc-gen-nanopb-wrapper.sh"; \
	    printf '%s\n' '#!/bin/sh' "PYTHONPATH=\"$(NANOPB_VENV_SITE_PACKAGES)\$${PYTHONPATH:+:\$$PYTHONPATH}\" exec $(NANOPB_HOST_PYTHON) \"$$plugin\" \"\$$@\"" > "$$plugin_wrapper"; \
	    chmod +x "$$plugin_wrapper"; \
	    plugin="$$plugin_wrapper"; \
	  fi; \
	  $(PROTOC) $(PROTOC_FLAGS) -I. --plugin=protoc-gen-nanopb="$$plugin" --nanopb_out=$(NANOPBDIR) --nanopb_opt=-I$(NANOPB_OPTIONS_DIR) $(1) || exit $$?; \
	fi; \
	for output in $(2); do \
	  if [ ! -f "$$output" ]; then \
	    echo "nanopb generation did not produce expected file: $$output"; \
	    exit 1; \
	  fi; \
	done
endef

###############################################################################
# Default target: keep original behavior
###############################################################################
all: $(BINPBS) examples

###############################################################################
# Named language targets
###############################################################################
c: $(C_SRCS) $(C_HDRS)

cpp: $(CPP_SRCS) $(CPP_HDRS)

java: $(JAVA_STAMP)

python: $(PYTHON_STAMP)

setup-dev:
	bash scripts/setup-dev-venv.sh

actuator-c: $(NANOPB_STATUS_STAMP) $(NANOPB_ACTUATOR_STAMP)

app-message-c: $(NANOPB_APP_MESSAGE_STAMP)

sensor-c: $(NANOPB_STATUS_STAMP) $(NANOPB_SENSOR_STAMP)

discovery-c: $(NANOPB_STATUS_STAMP) $(NANOPB_SENSOR_STAMP) $(NANOPB_WAVEFORM_STAMP) $(NANOPB_DISCOVERY_STAMP)

examples: examples-cpp
	@if [ -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_common.c" ] && [ -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_encode.c" ] && [ -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_decode.c" ] && [ -n "$(NANOPB_VENV_SITE_PACKAGES)" ]; then \
	  echo "Building embedded C examples with nanopb runtime at $(NANOPB_RUNTIME_DIR_EFFECTIVE)"; \
	  $(MAKE) examples-c PYTHON="$(PYTHON)" NANOPB_GENERATOR_MODULE=nanopb.generator.nanopb_generator NANOPB_DIR="$(NANOPB_DIR)" NANOPB_RUNTIME_DIR="$(NANOPB_RUNTIME_DIR)"; \
	else \
	  echo "Skipping embedded C examples."; \
	  echo "Primary setup:"; \
	  echo "  test -d /tmp/nanopb/.git || git clone https://github.com/nanopb/nanopb /tmp/nanopb"; \
	  echo "  make clean"; \
	  echo "  PYTHON=\"\`pwd\`/venv/bin/python\" NANOPB_GENERATOR_MODULE=nanopb.generator.nanopb_generator NANOPB_DIR=/tmp/nanopb make examples-c"; \
	fi

examples-cpp: cpp
	@mkdir -p $(EXAMPLESDIR)
	c++ -std=c++17 -O2 -I$(CPPDIR) $$(pkg-config --cflags protobuf) \
	  examples/actuator_read_example.cpp \
	  $(CPPDIR)/actuator.pb.cc $(CPPDIR)/geisa-status.pb.cc \
	  $$(pkg-config --libs protobuf) -pthread \
	  -o $(EXAMPLESDIR)/actuator_read_example
	c++ -std=c++17 -O2 -I$(CPPDIR) $$(pkg-config --cflags protobuf) \
	  examples/actuator_write_response_example.cpp \
	  $(CPPDIR)/actuator.pb.cc $(CPPDIR)/geisa-status.pb.cc \
	  $$(pkg-config --libs protobuf) -pthread \
	  -o $(EXAMPLESDIR)/actuator_write_response_example
	c++ -std=c++17 -O2 -I$(CPPDIR) $$(pkg-config --cflags protobuf) \
	  examples/app_message_read_example.cpp \
	  $(CPPDIR)/app-message.pb.cc \
	  $$(pkg-config --libs protobuf) -pthread \
	  -o $(EXAMPLESDIR)/app_message_read_example
	c++ -std=c++17 -O2 -I$(CPPDIR) $$(pkg-config --cflags protobuf) \
	  examples/app_message_write_response_example.cpp \
	  $(CPPDIR)/app-message.pb.cc \
	  $$(pkg-config --libs protobuf) -pthread \
	  -o $(EXAMPLESDIR)/app_message_write_response_example
	c++ -std=c++17 -O2 -I$(CPPDIR) $$(pkg-config --cflags protobuf) \
	  examples/discovery_read_example.cpp \
	  $(CPPDIR)/discovery.pb.cc $(CPPDIR)/sensor.pb.cc $(CPPDIR)/waveform.pb.cc $(CPPDIR)/geisa-status.pb.cc \
	  $$(pkg-config --libs protobuf) -pthread \
	  -o $(EXAMPLESDIR)/discovery_read_example
	c++ -std=c++17 -O2 -I$(CPPDIR) $$(pkg-config --cflags protobuf) \
	  examples/discovery_write_request_example.cpp \
	  $(CPPDIR)/discovery.pb.cc $(CPPDIR)/sensor.pb.cc $(CPPDIR)/waveform.pb.cc $(CPPDIR)/geisa-status.pb.cc \
	  $$(pkg-config --libs protobuf) -pthread \
	  -o $(EXAMPLESDIR)/discovery_write_request_example
	c++ -std=c++17 -O2 -I$(CPPDIR) $$(pkg-config --cflags protobuf) \
	  examples/discovery_write_response_example.cpp \
	  $(CPPDIR)/discovery.pb.cc $(CPPDIR)/sensor.pb.cc $(CPPDIR)/waveform.pb.cc $(CPPDIR)/geisa-status.pb.cc \
	  $$(pkg-config --libs protobuf) -pthread \
	  -o $(EXAMPLESDIR)/discovery_write_response_example
	c++ -std=c++17 -O2 -I$(CPPDIR) $$(pkg-config --cflags protobuf) \
	  examples/sensor_read_example.cpp \
	  $(CPPDIR)/sensor.pb.cc $(CPPDIR)/geisa-status.pb.cc \
	  $$(pkg-config --libs protobuf) -pthread \
	  -o $(EXAMPLESDIR)/sensor_read_example
	c++ -std=c++17 -O2 -I$(CPPDIR) $$(pkg-config --cflags protobuf) \
	  examples/sensor_write_response_example.cpp \
	  $(CPPDIR)/sensor.pb.cc $(CPPDIR)/geisa-status.pb.cc \
	  $$(pkg-config --libs protobuf) -pthread \
	  -o $(EXAMPLESDIR)/sensor_write_response_example
	c++ -std=c++17 -O2 -I$(CPPDIR) $$(pkg-config --cflags protobuf) \
	  examples/waveform_subscribe_and_read.cpp \
	  $(CPPDIR)/waveform.pb.cc $(CPPDIR)/geisa-status.pb.cc \
	  $$(pkg-config --libs protobuf) -pthread \
	  -o $(EXAMPLESDIR)/waveform_subscribe_and_read

examples-actuator: $(NANOPB_ACTUATOR_EXAMPLES)

examples-app-message: $(NANOPB_APP_MESSAGE_EXAMPLES)

examples-sensor: $(NANOPB_SENSOR_EXAMPLES)

examples-discovery: $(NANOPB_DISCOVERY_EXAMPLES)

examples-c: examples-actuator examples-app-message examples-sensor examples-discovery
	@echo "Built embedded C examples in $(EXAMPLESDIR)"

###############################################################################
# Nanopb generation and examples
###############################################################################
$(NANOPB_STATUS_STAMP): $(NANOPB_STATUS_PROTO) $(NANOPB_STATUS_OPTIONS)
	$(call NANOPB_GENERATE,$(NANOPB_STATUS_PROTO),$(NANOPB_STATUS_GEN))
	@touch $@

$(NANOPB_ACTUATOR_STAMP): $(NANOPB_ACTUATOR_PROTO) $(NANOPB_ACTUATOR_OPTIONS) $(NANOPB_STATUS_STAMP)
	$(call NANOPB_GENERATE,$(NANOPB_ACTUATOR_PROTO),$(NANOPB_ACTUATOR_GEN))
	@touch $@

$(NANOPB_APP_MESSAGE_STAMP): $(NANOPB_APP_MESSAGE_PROTO) $(NANOPB_APP_MESSAGE_OPTIONS)
	$(call NANOPB_GENERATE,$(NANOPB_APP_MESSAGE_PROTO),$(NANOPB_APP_MESSAGE_GEN))
	@touch $@

$(NANOPB_SENSOR_STAMP): $(NANOPB_SENSOR_PROTO) $(NANOPB_SENSOR_OPTIONS) $(NANOPB_STATUS_STAMP)
	$(call NANOPB_GENERATE,$(NANOPB_SENSOR_PROTO),$(NANOPB_SENSOR_GEN))
	@touch $@

$(NANOPB_WAVEFORM_STAMP): $(NANOPB_WAVEFORM_PROTO) $(NANOPB_STATUS_STAMP)
	$(call NANOPB_GENERATE,$(NANOPB_WAVEFORM_PROTO),$(NANOPB_WAVEFORM_GEN))
	@touch $@

$(NANOPB_DISCOVERY_STAMP): $(NANOPB_DISCOVERY_PROTO) $(NANOPB_DISCOVERY_OPTIONS) $(NANOPB_STATUS_STAMP) $(NANOPB_SENSOR_STAMP) $(NANOPB_WAVEFORM_STAMP)
	$(call NANOPB_GENERATE,$(NANOPB_DISCOVERY_PROTO),$(NANOPB_DISCOVERY_GEN))
	@touch $@

$(EXAMPLESDIR)/actuator_write_response_example_c: examples/actuator_write_response_example.c examples/helpers/actuator_example_utils.h examples/helpers/nanopb_example_utils.h $(NANOPB_STATUS_STAMP) $(NANOPB_ACTUATOR_STAMP)
	@mkdir -p $(EXAMPLESDIR)
	@if [ -z "$(NANOPB_RUNTIME_DIR_EFFECTIVE)" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_encode.c" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_decode.c" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_common.c" ]; then \
	  echo "NANOPB_DIR or NANOPB_RUNTIME_DIR must point to a nanopb source tree containing pb_common.c, pb_encode.c, and pb_decode.c."; \
	  exit 1; \
	fi
	cc -std=c11 -O2 \
	  -I$(NANOPBDIR) \
	  -I$(NANOPB_RUNTIME_DIR_EFFECTIVE) \
	  examples/actuator_write_response_example.c \
	  $(NANOPBDIR)/geisa-status.pb.c \
	  $(NANOPBDIR)/actuator.pb.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_common.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_encode.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_decode.c \
	  -o $@

$(EXAMPLESDIR)/actuator_read_example_c: examples/actuator_read_example.c examples/helpers/actuator_example_utils.h examples/helpers/nanopb_example_utils.h $(NANOPB_STATUS_STAMP) $(NANOPB_ACTUATOR_STAMP)
	@mkdir -p $(EXAMPLESDIR)
	@if [ -z "$(NANOPB_RUNTIME_DIR_EFFECTIVE)" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_encode.c" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_decode.c" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_common.c" ]; then \
	  echo "NANOPB_DIR or NANOPB_RUNTIME_DIR must point to a nanopb source tree containing pb_common.c, pb_encode.c, and pb_decode.c."; \
	  exit 1; \
	fi
	cc -std=c11 -O2 \
	  -I$(NANOPBDIR) \
	  -I$(NANOPB_RUNTIME_DIR_EFFECTIVE) \
	  examples/actuator_read_example.c \
	  $(NANOPBDIR)/geisa-status.pb.c \
	  $(NANOPBDIR)/actuator.pb.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_common.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_encode.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_decode.c \
	  -o $@

$(EXAMPLESDIR)/app_message_write_response_example_c: examples/app_message_write_response_example.c examples/helpers/app_message_example_utils.h examples/helpers/nanopb_example_utils.h $(NANOPB_APP_MESSAGE_STAMP)
	@mkdir -p $(EXAMPLESDIR)
	@if [ -z "$(NANOPB_RUNTIME_DIR_EFFECTIVE)" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_encode.c" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_decode.c" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_common.c" ]; then \
	  echo "NANOPB_DIR or NANOPB_RUNTIME_DIR must point to a nanopb source tree containing pb_common.c, pb_encode.c, and pb_decode.c."; \
	  exit 1; \
	fi
	cc -std=c11 -O2 \
	  -I$(NANOPBDIR) \
	  -I$(NANOPB_RUNTIME_DIR_EFFECTIVE) \
	  examples/app_message_write_response_example.c \
	  $(NANOPBDIR)/app-message.pb.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_common.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_encode.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_decode.c \
	  -o $@

$(EXAMPLESDIR)/app_message_read_example_c: examples/app_message_read_example.c examples/helpers/app_message_example_utils.h examples/helpers/nanopb_example_utils.h $(NANOPB_APP_MESSAGE_STAMP)
	@mkdir -p $(EXAMPLESDIR)
	@if [ -z "$(NANOPB_RUNTIME_DIR_EFFECTIVE)" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_encode.c" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_decode.c" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_common.c" ]; then \
	  echo "NANOPB_DIR or NANOPB_RUNTIME_DIR must point to a nanopb source tree containing pb_common.c, pb_encode.c, and pb_decode.c."; \
	  exit 1; \
	fi
	cc -std=c11 -O2 \
	  -I$(NANOPBDIR) \
	  -I$(NANOPB_RUNTIME_DIR_EFFECTIVE) \
	  examples/app_message_read_example.c \
	  $(NANOPBDIR)/app-message.pb.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_common.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_encode.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_decode.c \
	  -o $@

$(EXAMPLESDIR)/sensor_write_response_example_c: examples/sensor_write_response_example.c examples/helpers/sensor_example_utils.h examples/helpers/nanopb_example_utils.h $(NANOPB_STATUS_STAMP) $(NANOPB_SENSOR_STAMP)
	@mkdir -p $(EXAMPLESDIR)
	@if [ -z "$(NANOPB_RUNTIME_DIR_EFFECTIVE)" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_encode.c" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_decode.c" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_common.c" ]; then \
	  echo "NANOPB_DIR or NANOPB_RUNTIME_DIR must point to a nanopb source tree containing pb_common.c, pb_encode.c, and pb_decode.c."; \
	  exit 1; \
	fi
	cc -std=c11 -O2 \
	  -I$(NANOPBDIR) \
	  -I$(NANOPB_RUNTIME_DIR_EFFECTIVE) \
	  examples/sensor_write_response_example.c \
	  $(NANOPBDIR)/geisa-status.pb.c \
	  $(NANOPBDIR)/sensor.pb.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_common.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_encode.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_decode.c \
	  -o $@

$(EXAMPLESDIR)/sensor_read_example_c: examples/sensor_read_example.c examples/helpers/sensor_example_utils.h examples/helpers/nanopb_example_utils.h $(NANOPB_STATUS_STAMP) $(NANOPB_SENSOR_STAMP)
	@mkdir -p $(EXAMPLESDIR)
	@if [ -z "$(NANOPB_RUNTIME_DIR_EFFECTIVE)" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_encode.c" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_decode.c" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_common.c" ]; then \
	  echo "NANOPB_DIR or NANOPB_RUNTIME_DIR must point to a nanopb source tree containing pb_common.c, pb_encode.c, and pb_decode.c."; \
	  exit 1; \
	fi
	cc -std=c11 -O2 \
	  -I$(NANOPBDIR) \
	  -I$(NANOPB_RUNTIME_DIR_EFFECTIVE) \
	  examples/sensor_read_example.c \
	  $(NANOPBDIR)/geisa-status.pb.c \
	  $(NANOPBDIR)/sensor.pb.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_common.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_encode.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_decode.c \
	  -o $@

$(EXAMPLESDIR)/discovery_write_request_example_c: examples/discovery_write_request_example.c examples/helpers/discovery_example_utils.h examples/helpers/nanopb_example_utils.h $(NANOPB_STATUS_STAMP) $(NANOPB_SENSOR_STAMP) $(NANOPB_WAVEFORM_STAMP) $(NANOPB_DISCOVERY_STAMP)
	@mkdir -p $(EXAMPLESDIR)
	@if [ -z "$(NANOPB_RUNTIME_DIR_EFFECTIVE)" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_encode.c" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_decode.c" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_common.c" ]; then \
	  echo "NANOPB_DIR or NANOPB_RUNTIME_DIR must point to a nanopb source tree containing pb_common.c, pb_encode.c, and pb_decode.c."; \
	  exit 1; \
	fi
	cc -std=c11 -O2 \
	  -I$(NANOPBDIR) \
	  -I$(NANOPB_RUNTIME_DIR_EFFECTIVE) \
	  examples/discovery_write_request_example.c \
	  $(NANOPBDIR)/geisa-status.pb.c \
	  $(NANOPBDIR)/sensor.pb.c \
	  $(NANOPBDIR)/waveform.pb.c \
	  $(NANOPBDIR)/discovery.pb.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_common.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_encode.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_decode.c \
	  -o $@

$(EXAMPLESDIR)/discovery_write_response_example_c: examples/discovery_write_response_example.c examples/helpers/discovery_example_utils.h examples/helpers/nanopb_example_utils.h $(NANOPB_STATUS_STAMP) $(NANOPB_SENSOR_STAMP) $(NANOPB_WAVEFORM_STAMP) $(NANOPB_DISCOVERY_STAMP)
	@mkdir -p $(EXAMPLESDIR)
	@if [ -z "$(NANOPB_RUNTIME_DIR_EFFECTIVE)" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_encode.c" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_decode.c" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_common.c" ]; then \
	  echo "NANOPB_DIR or NANOPB_RUNTIME_DIR must point to a nanopb source tree containing pb_common.c, pb_encode.c, and pb_decode.c."; \
	  exit 1; \
	fi
	cc -std=c11 -O2 \
	  -I$(NANOPBDIR) \
	  -I$(NANOPB_RUNTIME_DIR_EFFECTIVE) \
	  examples/discovery_write_response_example.c \
	  $(NANOPBDIR)/geisa-status.pb.c \
	  $(NANOPBDIR)/sensor.pb.c \
	  $(NANOPBDIR)/waveform.pb.c \
	  $(NANOPBDIR)/discovery.pb.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_common.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_encode.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_decode.c \
	  -o $@

$(EXAMPLESDIR)/discovery_read_example_c: examples/discovery_read_example.c examples/helpers/discovery_example_utils.h examples/helpers/nanopb_example_utils.h $(NANOPB_STATUS_STAMP) $(NANOPB_SENSOR_STAMP) $(NANOPB_WAVEFORM_STAMP) $(NANOPB_DISCOVERY_STAMP)
	@mkdir -p $(EXAMPLESDIR)
	@if [ -z "$(NANOPB_RUNTIME_DIR_EFFECTIVE)" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_encode.c" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_decode.c" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_common.c" ]; then \
	  echo "NANOPB_DIR or NANOPB_RUNTIME_DIR must point to a nanopb source tree containing pb_common.c, pb_encode.c, and pb_decode.c."; \
	  exit 1; \
	fi
	cc -std=c11 -O2 \
	  -I$(NANOPBDIR) \
	  -I$(NANOPB_RUNTIME_DIR_EFFECTIVE) \
	  examples/discovery_read_example.c \
	  $(NANOPBDIR)/geisa-status.pb.c \
	  $(NANOPBDIR)/sensor.pb.c \
	  $(NANOPBDIR)/waveform.pb.c \
	  $(NANOPBDIR)/discovery.pb.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_common.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_encode.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_decode.c \
	  -o $@

###############################################################################
# .binpb generation
###############################################################################
$(BINPBDIR)/%.binpb: %.proto
	@mkdir -p $(@D)
	$(PROTOC) $(PROTOC_FLAGS) -I. -o $@ $<

###############################################################################
# C generation using protoc-c plugin
###############################################################################
$(CDIR)/%.pb-c.c $(CDIR)/%.pb-c.h: %.proto
	@mkdir -p $(CDIR)
	$(PROTOC) $(PROTOC_FLAGS) --c_out=$(CDIR) $<

###############################################################################
# C++ generation
###############################################################################
$(CPPDIR)/%.pb.cc $(CPPDIR)/%.pb.h: %.proto
	@mkdir -p $(CPPDIR)
	$(PROTOC) $(PROTOC_FLAGS) --cpp_out=$(CPPDIR) $<

###############################################################################
# Java generation
###############################################################################
$(JAVA_STAMP): $(PROTOS)
	@mkdir -p $(JAVADIR)
	$(PROTOC) $(PROTOC_FLAGS) --java_out=$(JAVADIR) $(PROTOS)
	@touch $@

###############################################################################
# Python generation
###############################################################################
$(PYTHON_STAMP): $(PROTOS)
	@mkdir -p $(PYTHONDIR)
	$(PROTOC) $(PROTOC_FLAGS) --python_out=$(PYTHONDIR) $(PROTOS)
	@touch $@

###############################################################################
# Convenience target to generate everything (except examples)
###############################################################################
langs: c cpp java python

###############################################################################
# Clean
###############################################################################
clean:
	rm -rf $(BUILDDIR)

.PHONY: all c cpp java python actuator-c app-message-c sensor-c discovery-c \
	examples examples-cpp examples-c examples-actuator examples-app-message \
	examples-sensor examples-discovery langs clean
