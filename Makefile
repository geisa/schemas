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

PROTOC ?= protoc

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
# Default target: keep original behavior
###############################################################################
all: $(BINPBS)

###############################################################################
# Named language targets
###############################################################################
c: $(C_SRCS) $(C_HDRS)

cpp: $(CPP_SRCS) $(CPP_HDRS)

java: $(JAVA_STAMP)

python: $(PYTHON_STAMP)

examples: cpp
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
	  $(CPPDIR)/discovery.pb.cc $(CPPDIR)/sensor.pb.cc $(CPPDIR)/geisa-status.pb.cc \
	  $$(pkg-config --libs protobuf) -pthread \
	  -o $(EXAMPLESDIR)/discovery_read_example
	c++ -std=c++17 -O2 -I$(CPPDIR) $$(pkg-config --cflags protobuf) \
	  examples/discovery_write_request_example.cpp \
	  $(CPPDIR)/discovery.pb.cc $(CPPDIR)/sensor.pb.cc $(CPPDIR)/geisa-status.pb.cc \
	  $$(pkg-config --libs protobuf) -pthread \
	  -o $(EXAMPLESDIR)/discovery_write_request_example
	c++ -std=c++17 -O2 -I$(CPPDIR) $$(pkg-config --cflags protobuf) \
	  examples/discovery_write_response_example.cpp \
	  $(CPPDIR)/discovery.pb.cc $(CPPDIR)/sensor.pb.cc $(CPPDIR)/geisa-status.pb.cc \
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

examples-cpp: examples

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

.PHONY: all c cpp java python examples examples-cpp langs clean
