//-----------------------------------------------------------------------------
// Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
// Security Alliance (GEISA), a Series of LF Projects, LLC
// This file is licensed under the Community Specification License 1.0
// available at:
// https://github.com/geisa/specification/blob/main/LICENSE.md or
// https://github.com/CommunitySpecification/Community_Specification/blob/main/
// 1._Community_Specification_License-v1.md
//-----------------------------------------------------------------------------
//
// Example GEISA sensor response encoder.
//
// This example creates a GeisaSensorReadings_Rsp protobuf payload containing:
// - one sensor reading with multiple values
// - one additional sensor/value pair
//
// The generated binary payload can be decoded with sensor_read_example.cpp.
//
//-----------------------------------------------------------------------------

#include <fstream>
#include <iostream>
#include <string>

#include "geisa-status.pb.h"
#include "sensor.pb.h"

static bool write_file(const char *path,
                       const GeisaSensorReadings_Rsp &response)
{
    std::ofstream output(path, std::ios::binary);
    if (!output)
    {
        std::cerr << "failed to open output file: " << path << '\n';
        return false;
    }

    if (!response.SerializeToOstream(&output))
    {
        std::cerr << "failed to serialize GeisaSensorReadings_Rsp\n";
        return false;
    }

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

    GeisaSensorReadings_Rsp response;

    response.mutable_status()->set_code(GEISA_STATUS_SUCCESS);
    response.mutable_status()->set_message("ok");

    GeisaSensorReading *board_reading = response.add_readings();
    board_reading->set_sensor_id("board_temp1");
    board_reading->set_timestamp_ms(1735689600000ULL);
    board_reading->set_unit("C");
    board_reading->set_quality("GOOD");
    board_reading->set_status("OK");

    GeisaSensorValue *board_value_1 = board_reading->add_values();
    board_value_1->set_double_value(47.25);

    GeisaSensorValue *board_value_2 = board_reading->add_values();
    board_value_2->set_double_value(47.38);

    GeisaSensorReading *ambient_reading = response.add_readings();
    ambient_reading->set_sensor_id("ambient_temp1");
    ambient_reading->set_timestamp_ms(1735689600000ULL);
    ambient_reading->set_unit("C");
    ambient_reading->set_quality("GOOD");
    ambient_reading->set_status("OK");

    GeisaSensorValue *ambient_value = ambient_reading->add_values();
    ambient_value->set_double_value(33.0);

    if (!write_file(argv[1], response))
    {
        return 1;
    }

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
