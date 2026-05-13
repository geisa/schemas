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
// Example GEISA sensor response encoder
//
// This example creates a GeisaSensorReadings_Rsp protobuf payload containing:
// - one sensor reading with multiple values
// - one additional sensor/value pair
//
// The generated binary payload can be decoded with sensor_read_example.cpp
//-----------------------------------------------------------------------------

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <sys/wait.h>

#include "geisa-status.pb.h"
#include "sensor.pb.h"

static bool write_file(const char *path,
                       const GeisaSensorReadings_Rsp &response)
{
    // Persist a typed response as raw protobuf bytes for demo/fixture use
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

static int run_reader_subprocess(const std::string &output_path)
{
    const std::string command = "/tmp/sensor_read_example " + output_path;
    const int rc = std::system(command.c_str());
    if (rc == -1)
    {
        std::cerr << "Failed to launch reader subprocess\n";
        return 1;
    }

    if (WIFEXITED(rc))
    {
        return WEXITSTATUS(rc);
    }

    if (WIFSIGNALED(rc))
    {
        std::cerr << "Reader subprocess terminated by signal "
                  << WTERMSIG(rc) << '\n';
        return 1;
    }

    std::cerr << "Reader subprocess failed\n";
    return 1;
}

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    const bool demo_mode = (argc == 2 && std::string(argv[1]) == "--demo");
    const bool positional = (argc == 2 && !demo_mode);

    if (!demo_mode && !positional)
    {
        std::cerr << "usage: " << argv[0]
                  << " sensor-response.bin | --demo\n";
        return 2;
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
    board_reading->set_unit("C");
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
    ambient_reading->set_unit("C");
    ambient_reading->set_quality("GOOD");
    ambient_reading->set_status("OK");

    GeisaSensorValue *ambient_value = ambient_reading->add_values();
    ambient_value->set_double_value(33.0);

    if (!write_file(output_path.c_str(), response))
    {
        return 1;
    }

    if (demo_mode)
    {
        // Demo mode immediately runs the companion reader for end-to-end flow
        std::cout << "Running sensor demo...\n";
        std::cout << "Wrote sensor response to /tmp/sensor-response.bin\n";
        std::cout << "Decode with: /tmp/sensor_read_example "
                     "/tmp/sensor-response.bin\n";

        const int reader_rc = run_reader_subprocess(output_path);
        if (reader_rc != 0)
        {
            google::protobuf::ShutdownProtobufLibrary();
            return reader_rc;
        }
    }

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
