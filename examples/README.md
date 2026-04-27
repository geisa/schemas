# GEISA Schema Examples

This directory contains JSON examples and small source examples for GEISA schema/protobuf messages.

## Sensor read C++ example

`sensor_read_example.cpp` demonstrates decoding a binary `GeisaSensorReadings_Rsp`
protobuf payload and printing a JSON-like representation.

Build from the repository root:

    make clean
    make cpp
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
protobuf-c generator used by `make c` does not currently support proto3 optional
for this file. Use the C++ generation path for this example.
