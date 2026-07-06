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
CDIR      = $(BUILDDIR)/c
CPPDIR    = $(BUILDDIR)/cpp
JAVADIR   = $(BUILDDIR)/java
PYTHONDIR   = $(BUILDDIR)/python
EXAMPLESDIR = $(BUILDDIR)/examples
NANOPBDIR = $(BUILDDIR)/nanopb
PROTOC ?= protoc
PYTHON ?= python3
NANOPB_GENERATOR ?= protoc-gen-nanopb
NANOPB_DIR ?=
NANOPB_RUNTIME_DIR ?= $(NANOPB_DIR)
NANOPB_OPTIONS_DIR ?= nanopb_options
NANOPB_RUNTIME_DIR_EFFECTIVE = $(strip $(NANOPB_RUNTIME_DIR))

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
# protoc may generate multiple .java files depending on package/options,
# so for Java we use a stamp file to let make know the command completed.
###############################################################################
JAVA_STAMP = $(JAVADIR)/.java_generated

###############################################################################
# Python outputs
# protoc may generate multiple .py files depending on package/options,
# so for Python we use a stamp file to let make know the command completed.
###############################################################################
PYTHON_STAMP = $(PYTHONDIR)/.python_generated

###############################################################################
# Embedded C outputs (metered_quantities)
###############################################################################
NANOPB_METERED_PROTO = metered_quantities.proto
NANOPB_METERED_OPTIONS = $(NANOPB_OPTIONS_DIR)/metered_quantities.options
NANOPB_METERED_SRCS = $(NANOPBDIR)/metered_quantities.pb.c
NANOPB_METERED_HDRS = $(NANOPBDIR)/metered_quantities.pb.h
NANOPB_METERED_EXAMPLES = \
	$(EXAMPLESDIR)/metered_quantities_write_example \
	$(EXAMPLESDIR)/metered_quantities_read_example

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

metered-c: $(NANOPB_METERED_SRCS) $(NANOPB_METERED_HDRS)

examples: examples-cpp
	@echo "Embedded C examples are available separately via: make examples-c NANOPB_DIR=/path/to/nanopb"

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

examples-metered: $(NANOPB_METERED_EXAMPLES)

examples-c: examples-metered

###############################################################################
# Nanopb generation and examples
###############################################################################
$(NANOPB_METERED_SRCS) $(NANOPB_METERED_HDRS): $(NANOPB_METERED_PROTO) $(NANOPB_METERED_OPTIONS)
	@mkdir -p $(NANOPBDIR)
	@plugin="$(NANOPB_GENERATOR)"; \
	if [ -x "$$plugin" ]; then \
	  :; \
	elif command -v "$$plugin" >/dev/null 2>&1; then \
	  plugin="$$(command -v "$$plugin")"; \
	else \
	  echo "Missing nanopb generator: $(NANOPB_GENERATOR)"; \
	  echo "Set NANOPB_GENERATOR to protoc-gen-nanopb or an explicit generator path, then rerun make metered-c."; \
	  exit 1; \
	fi; \
	$(PROTOC) $(PROTOC_FLAGS) -I. --plugin=protoc-gen-nanopb="$$plugin" --nanopb_out=$(NANOPBDIR) --nanopb_opt=-I$(NANOPB_OPTIONS_DIR) $(NANOPB_METERED_PROTO)

$(EXAMPLESDIR)/metered_quantities_write_example: examples/metered_quantities_write_example.c $(NANOPB_METERED_SRCS) $(NANOPB_METERED_HDRS)
	@mkdir -p $(EXAMPLESDIR)
	@if [ -z "$(NANOPB_RUNTIME_DIR_EFFECTIVE)" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_encode.c" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_decode.c" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_common.c" ]; then \
	  echo "NANOPB_DIR or NANOPB_RUNTIME_DIR must point to a nanopb source tree containing pb_common.c, pb_encode.c, and pb_decode.c."; \
	  exit 1; \
	fi
	cc -std=c11 -O2 \
	  -I$(NANOPBDIR) \
	  -I$(NANOPB_RUNTIME_DIR_EFFECTIVE) \
	  examples/metered_quantities_write_example.c \
	  $(NANOPBDIR)/metered_quantities.pb.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_common.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_encode.c \
	  $(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_decode.c \
	  -o $@

$(EXAMPLESDIR)/metered_quantities_read_example: examples/metered_quantities_read_example.c $(NANOPB_METERED_SRCS) $(NANOPB_METERED_HDRS)
	@mkdir -p $(EXAMPLESDIR)
	@if [ -z "$(NANOPB_RUNTIME_DIR_EFFECTIVE)" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_encode.c" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_decode.c" ] || [ ! -f "$(NANOPB_RUNTIME_DIR_EFFECTIVE)/pb_common.c" ]; then \
	  echo "NANOPB_DIR or NANOPB_RUNTIME_DIR must point to a nanopb source tree containing pb_common.c, pb_encode.c, and pb_decode.c."; \
	  exit 1; \
	fi
	cc -std=c11 -O2 \
	  -I$(NANOPBDIR) \
	  -I$(NANOPB_RUNTIME_DIR_EFFECTIVE) \
	  examples/metered_quantities_read_example.c \
	  $(NANOPBDIR)/metered_quantities.pb.c \
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
# One protoc invocation can generate multiple .java files, so use a stamp file.
###############################################################################
$(JAVA_STAMP): $(PROTOS)
	@mkdir -p $(JAVADIR)
	$(PROTOC) $(PROTOC_FLAGS) --java_out=$(JAVADIR) $(PROTOS)
	@touch $@

###############################################################################
# Python generation
# One protoc invocation can generate multiple .py files, so use a stamp file.
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

.PHONY: all c cpp java python metered-c examples examples-cpp examples-c examples-metered langs clean
