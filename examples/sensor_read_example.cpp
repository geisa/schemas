//-----------------------------------------------------------------------------
// Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
// Security Alliance (GEISA), a Series of LF Projects, LLC
// This file is licensed under the Community Specification License 1.0
// available at:
// https://github.com/geisa/specification/blob/main/LICENSE.md or
// https://github.com/CommunitySpecification/Community_Specification/blob/main/1._Community_Specification_License-v1.md
//-----------------------------------------------------------------------------
//
// Example GEISA sensor response decoder.
//
// This example demonstrates how an application can decode a
// GeisaSensorReadings_Rsp protobuf payload and print a JSON-like representation
// for logging, diagnostics, or handoff to application-local parsing logic.
//
// Build example:
//
//   make cpp
//   g++ -std=c++17 \
//     -Ibuild/cpp \
//     examples/sensor_read_example.cpp \
//     build/cpp/sensor.pb.cc \
//     build/cpp/geisa-status.pb.cc \
//     -lprotobuf -pthread \
//     -o /tmp/sensor_read_example
//
// Run with a binary GeisaSensorReadings_Rsp payload:
//
//   /tmp/sensor_read_example sensor-response.bin
//
//-----------------------------------------------------------------------------

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

#include "geisa-status.pb.h"
#include "sensor.pb.h"

static void print_json_escaped(const std::string &value)
{
    std::cout << '"';

    for (const char ch : value)
    {
        switch (ch)
        {
        case '\\':
            std::cout << "\\\\";
            break;
        case '"':
            std::cout << "\\\"";
            break;
        case '\n':
            std::cout << "\\n";
            break;
        case '\r':
            std::cout << "\\r";
            break;
        case '\t':
            std::cout << "\\t";
            break;
        default:
            std::cout << ch;
            break;
        }
    }

    std::cout << '"';
}

static void print_sensor_value(const GeisaSensorValue &value)
{
    switch (value.value_case())
    {
    case GeisaSensorValue::kDoubleValue:
        std::cout << "{\"double-value\":" << value.double_value() << '}';
        break;
    case GeisaSensorValue::kInt64Value:
        std::cout << "{\"int64-value\":" << value.int64_value() << '}';
        break;
    case GeisaSensorValue::kBoolValue:
        std::cout << "{\"bool-value\":"
                  << (value.bool_value() ? "true" : "false") << '}';
        break;
    case GeisaSensorValue::kStringValue:
        std::cout << "{\"string-value\":";
        print_json_escaped(value.string_value());
        std::cout << '}';
        break;
    case GeisaSensorValue::VALUE_NOT_SET:
    default:
        std::cout << "{\"unset\":true}";
        break;
    }
}

static void print_sensor_reading(const GeisaSensorReading &reading)
{
    std::cout << "{\"sensor-id\":";
    print_json_escaped(reading.sensor_id());

    std::cout << ",\"timestamp-ms\":" << reading.timestamp_ms();

    std::cout << ",\"values\":[";
    for (int i = 0; i < reading.values_size(); ++i)
    {
        if (i > 0)
        {
            std::cout << ',';
        }
        print_sensor_value(reading.values(i));
    }
    std::cout << ']';

    if (reading.has_unit())
    {
        std::cout << ",\"unit\":";
        print_json_escaped(reading.unit());
    }

    if (reading.has_quality())
    {
        std::cout << ",\"quality\":";
        print_json_escaped(reading.quality());
    }

    if (reading.has_status())
    {
        std::cout << ",\"status\":";
        print_json_escaped(reading.status());
    }

    std::cout << '}';
}

static void print_sensor_readings_response(const GeisaSensorReadings_Rsp &rsp)
{
    std::cout << "{\"geisa-sensor-readings-rsp\":{";

    std::cout << "\"status\":{";
    std::cout << "\"code\":";
    print_json_escaped(GeisaStatusCode_Name(rsp.status().code()));

    if (!rsp.status().message().empty())
    {
        std::cout << ",\"message\":";
        print_json_escaped(rsp.status().message());
    }

    if (!rsp.status().details().empty())
    {
        std::cout << ",\"details\":";
        print_json_escaped(rsp.status().details());
    }

    std::cout << '}';

    std::cout << ",\"readings\":[";
    for (int i = 0; i < rsp.readings_size(); ++i)
    {
        if (i > 0)
        {
            std::cout << ',';
        }
        print_sensor_reading(rsp.readings(i));
    }
    std::cout << "]}}\n";
}

static bool read_file(const char *path, std::string *contents)
{
    std::ifstream input(path, std::ios::binary);
    if (!input)
    {
        std::cerr << "failed to open input file: " << path << '\n';
        return false;
    }

    contents->assign(std::istreambuf_iterator<char>(input),
                     std::istreambuf_iterator<char>());
    return true;
}

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    if (argc != 2)
    {
        std::cerr << "usage: " << argv[0] << " sensor-response.bin\n";
        return 2;
    }

    std::string payload;
    if (!read_file(argv[1], &payload))
    {
        return 1;
    }

    GeisaSensorReadings_Rsp response;
    if (!response.ParseFromString(payload))
    {
        std::cerr << "failed to decode GeisaSensorReadings_Rsp payload\n";
        return 1;
    }

    print_sensor_readings_response(response);

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
