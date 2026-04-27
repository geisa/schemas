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
// Example GEISA platform discovery response decoder.
//
// This example demonstrates how an application can decode a
// GeisaPlatformDiscovery_Rsp protobuf payload and print a JSON-like
// representation for logging, diagnostics, or handoff to application-local
// parsing logic.
//
//-----------------------------------------------------------------------------

#include <fstream>
#include <iostream>
#include <string>

#include "geisa-status.pb.h"
#include "discovery.pb.h"

// Escapes strings so the JSON-like output stays readable and valid for
// control characters, quotes, and backslashes.
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

// Reads a binary protobuf payload from disk into memory for decoding.
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

static void print_module(const GeisaPlatformDiscovery_Module &module)
{
    std::cout << "{\"type\":";
    print_json_escaped(GeisaPlatformDiscovery_DeviceType_Name(module.type()));

    if (!module.manufacturer().empty())
    {
        std::cout << ",\"manufacturer\":";
        print_json_escaped(module.manufacturer());
    }

    if (!module.model().empty())
    {
        std::cout << ",\"model\":";
        print_json_escaped(module.model());
    }

    if (!module.serial_number().empty())
    {
        std::cout << ",\"serial-number\":";
        print_json_escaped(module.serial_number());
    }

    if (!module.hw_revision().empty())
    {
        std::cout << ",\"hw-revision\":";
        print_json_escaped(module.hw_revision());
    }

    if (!module.fw_revision().empty())
    {
        std::cout << ",\"fw-revision\":";
        print_json_escaped(module.fw_revision());
    }

    std::cout << '}';
}

static void print_waveform_stream(
    const GeisaPlatformDiscovery_Waveform_Instance &stream)
{
    std::cout << "{\"stream-id\":";
    print_json_escaped(stream.stream_id());

    if (!stream.name().empty())
    {
        std::cout << ",\"name\":";
        print_json_escaped(stream.name());
    }

    if (!stream.description().empty())
    {
        std::cout << ",\"description\":";
        print_json_escaped(stream.description());
    }

    std::cout << ",\"datatype\":";
    print_json_escaped(GeisaWaveform_Datatype_Name(stream.datatype()));

    std::cout << ",\"voltage-multiplier\":"
              << stream.voltage_multiplier();
    std::cout << ",\"current-multiplier\":"
              << stream.current_multiplier();
    std::cout << ",\"num-voltage-ch\":" << stream.num_voltage_ch();
    std::cout << ",\"num-current-ch\":" << stream.num_current_ch();
    std::cout << ",\"num-other-ch\":" << stream.num_other_ch();
    std::cout << ",\"total-channel-count\":"
              << stream.total_channel_count();
    std::cout << ",\"cycle-aligned\":"
              << (stream.cycle_aligned() ? "true" : "false");
    std::cout << ",\"zero-crossing-aligned\":"
              << (stream.zero_crossing_aligned() ? "true" : "false");
    std::cout << ",\"sample-rate\":" << stream.sample_rate();
    std::cout << ",\"samples-per-cycle\":"
              << stream.samples_per_cycle();
    std::cout << ",\"nominal-frequency-hz\":" << stream.nominal_frequency_hz();
    std::cout << ",\"expected-frame-period-ms\":"
              << stream.expected_frame_period_ms();

    std::cout << '}';
}

// Parses GeisaPlatformDiscovery_Rsp and prints a JSON-like representation,
// including enum names for status and waveform/device fields.
static void print_discovery_response(const GeisaPlatformDiscovery_Rsp &rsp)
{
    std::cout << "{\"geisa-platform-discovery-rsp\":{";

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

    std::cout << ",\"geisa\":{";
    std::cout << "\"ver-major\":" << rsp.geisa().ver_major();
    std::cout << ",\"ver-minor\":" << rsp.geisa().ver_minor();
    std::cout << ",\"ver-rev\":" << rsp.geisa().ver_rev();
    std::cout << ",\"pillar-adm\":"
              << (rsp.geisa().pillar_adm() ? "true" : "false");
    std::cout << ",\"pillar-api\":"
              << (rsp.geisa().pillar_api() ? "true" : "false");
    std::cout << ",\"pillar-lee\":"
              << (rsp.geisa().pillar_lee() ? "true" : "false");
    std::cout << ",\"pillar-vee\":"
              << (rsp.geisa().pillar_vee() ? "true" : "false");
    std::cout << '}';

    std::cout << ",\"device\":{";
    std::cout << "\"top-module\":";
    print_module(rsp.device().top_module());
    std::cout << '}';

    std::cout << ",\"operator\":{";
    bool first_operator_field = true;
    if (!rsp.operator_().operator_name().empty())
    {
        std::cout << "\"operator-name\":";
        print_json_escaped(rsp.operator_().operator_name());
        first_operator_field = false;
    }

    if (!rsp.operator_().operator_identifier().empty())
    {
        if (!first_operator_field)
        {
            std::cout << ',';
        }
        std::cout << "\"operator-identifier\":";
        print_json_escaped(rsp.operator_().operator_identifier());
    }
    std::cout << '}';

    std::cout << ",\"metrology\":{";
    bool first_metrology_field = true;
    if (!rsp.metrology().meter_form().empty())
    {
        std::cout << "\"meter-form\":";
        print_json_escaped(rsp.metrology().meter_form());
        first_metrology_field = false;
    }

    if (!first_metrology_field)
    {
        std::cout << ',';
    }
    std::cout << "\"phase-count\":" << rsp.metrology().phase_count();
    std::cout << ",\"neutral-connected\":"
              << (rsp.metrology().neutral_connected() ? "true" : "false");
    std::cout << ",\"nominal-frequency-hz\":"
              << rsp.metrology().nominal_frequency_hz();
    std::cout << ",\"nominal-phase-to-neutral-voltage-v\":"
              << rsp.metrology().nominal_phase_to_neutral_voltage_v();
    std::cout << ",\"nominal-phase-to-phase-voltage-v\":"
              << rsp.metrology().nominal_phase_to_phase_voltage_v();
    std::cout << '}';

    std::cout << ",\"sensor\":{";
    std::cout << "\"sensor-count\":" << rsp.sensor().sensors_size();
    std::cout << '}';

    std::cout << ",\"waveform\":{";
    std::cout << "\"stream-count\":" << rsp.waveform().streams_size();
    std::cout << ",\"streams\":[";
    for (int i = 0; i < rsp.waveform().streams_size(); ++i)
    {
        if (i > 0)
        {
            std::cout << ',';
        }
        print_waveform_stream(rsp.waveform().streams(i));
    }
    std::cout << "]}}";
    std::cout << '\n';
}

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    const bool positional = (argc == 2);
    const bool explicit_flag =
        (argc == 3 && std::string(argv[1]) == "--read-rsp");
    if (!positional && !explicit_flag)
    {
        std::cerr << "usage: " << argv[0]
                  << " discovery-response.bin | --read-rsp "
                  << "discovery-response.bin\n";
        return 2;
    }

    const char *input_path = positional ? argv[1] : argv[2];

    std::string payload;
    if (!read_file(input_path, &payload))
    {
        return 1;
    }

    GeisaPlatformDiscovery_Rsp response;
    if (!response.ParseFromString(payload))
    {
        std::cerr << "failed to decode GeisaPlatformDiscovery_Rsp "
                     "payload\n";
        return 1;
    }

    print_discovery_response(response);

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
