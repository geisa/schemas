//-----------------------------------------------------------------------------
// Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
// Security Alliance (GEISA), a Series of LF Projects, LLC
//
// Licensed under the Apache License, Version 2.0. See LICENSE.
//-----------------------------------------------------------------------------
//
// Example GEISA sensor response encoder
//
// This example creates a GeisaSensorReadings_Rsp protobuf payload containing:
// - one sensor reading with multiple values
// - one additional sensor/value pair
//
// The generated binary payload can be decoded with sensor_read_example.cpp
//-----------------------------------------------------------------------------

#include <iostream>
#include <cstdlib>
#include <sysexits.h>
#include <string>

#include "geisa-status.pb.h"
#include "sensor.pb.h"
#include "helpers/example_utils.h"

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    const bool demo_mode = (argc == 2 && std::string(argv[1]) == "--demo");
    const bool positional = (argc == 2 && !demo_mode);

    if (!demo_mode && !positional)
    {
        return print_example_usage_and_return({
            std::string("usage: ") + argv[0] + " sensor-response.bin | --demo"});
    }

    const std::string output_path =
        demo_mode ? std::string("/tmp/sensor-response.bin")
                  : std::string(argv[1]);

    GeisaSensorReadings_Rsp response;

    // Populate top-level GEISA status envelope.
    response.mutable_status()->set_code(GEISA_STATUS_SUCCESS);
    response.mutable_status()->set_message("ok");

    // First reading demonstrates repeated values in one observation
    GeisaSensorReading *board_reading = response.add_readings();
    board_reading->set_sensor_id("board_temp1");
    board_reading->set_timestamp_ms(1735689600000ULL);
    board_reading->set_unit("Cel");
    board_reading->set_quality("GOOD");
    board_reading->set_status("OK");

    GeisaSensorValue *board_value_1 = board_reading->add_values();
    board_value_1->set_double_value(47.25);

    GeisaSensorValue *board_value_2 = board_reading->add_values();
    board_value_2->set_double_value(47.38);

    // Second reading demonstrates repeated reading entries in one response
    GeisaSensorReading *ambient_reading = response.add_readings();
    ambient_reading->set_sensor_id("ambient_temp1");
    ambient_reading->set_timestamp_ms(1735689600000ULL);
    ambient_reading->set_unit("Cel");
    ambient_reading->set_quality("GOOD");
    ambient_reading->set_status("OK");

    GeisaSensorValue *ambient_value = ambient_reading->add_values();
    ambient_value->set_double_value(33.0);

    if (!serialize_protobuf_to_file(output_path.c_str(),
                                    response,
                                    "GeisaSensorReadings_Rsp"))
    {
        return EXIT_FAILURE;
    }

    if (demo_mode)
    {
        const std::string reader_path = resolve_demo_reader_path(
            argv[0], "sensor_read_example", "/tmp/sensor_read_example");
        // Demo mode immediately runs the companion reader for end-to-end flow
        print_example_info_lines({
            "Running sensor demo...",
            "Wrote sensor response to /tmp/sensor-response.bin",
            std::string("Decode with: ") + reader_path
                + " /tmp/sensor-response.bin"});

        const int reader_rc =
            run_demo_subprocess(reader_path + " " + output_path);
        if (reader_rc != 0)
        {
            google::protobuf::ShutdownProtobufLibrary();
            return reader_rc;
        }
    }

    google::protobuf::ShutdownProtobufLibrary();
    return EXIT_SUCCESS;
}
