//-----------------------------------------------------------------------------
// Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
// Security Alliance (GEISA), a Series of LF Projects, LLC
//
// Licensed under the Apache License, Version 2.0. See LICENSE.
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

#include <cstdlib>
#include <sysexits.h>
#include <iostream>
#include <string>

#include "geisa-status.pb.h"
#include "discovery.pb.h"
#include "helpers/example_utils.h"

static void print_module(const GeisaPlatformDiscovery_Module &module)
{
    std::cout << "{";
    print_json_key("type");
    print_json_escaped(GeisaPlatformDiscovery_DeviceType_Name(module.type()));

    if (!module.manufacturer().empty())
    {
        std::cout << ",";
        print_json_key("manufacturer");
        print_json_escaped(module.manufacturer());
    }

    if (!module.model().empty())
    {
        std::cout << ",";
        print_json_key("model");
        print_json_escaped(module.model());
    }

    if (!module.serial_number().empty())
    {
        std::cout << ",";
        print_json_key("serial-number");
        print_json_escaped(module.serial_number());
    }

    if (!module.hw_revision().empty())
    {
        std::cout << ",";
        print_json_key("hw-revision");
        print_json_escaped(module.hw_revision());
    }

    if (!module.fw_revision().empty())
    {
        std::cout << ",";
        print_json_key("fw-revision");
        print_json_escaped(module.fw_revision());
    }

    std::cout << '}';
}

static void print_waveform_stream(
    const GeisaPlatformDiscovery_Waveform_Instance &stream)
{
    std::cout << "{";
    print_json_key("stream-id");
    print_json_escaped(stream.stream_id());

    if (!stream.name().empty())
    {
        std::cout << ",";
        print_json_key("name");
        print_json_escaped(stream.name());
    }

    if (!stream.description().empty())
    {
        std::cout << ",";
        print_json_key("description");
        print_json_escaped(stream.description());
    }

    std::cout << ",";
    print_json_key("sample-type");
    print_json_escaped(GeisaWaveform_SampleType_Name(stream.sample_type()));

    std::cout << ",\"voltage-scale\":"
              << stream.voltage_scale();
    std::cout << ",\"current-scale\":"
              << stream.current_scale();
    std::cout << ",\"voltage-channel-count\":" << stream.voltage_channel_count();
    std::cout << ",\"current-channel-count\":" << stream.current_channel_count();
    std::cout << ",\"other-channel-count\":" << stream.other_channel_count();
    std::cout << ",\"total-channel-count\":"
              << stream.total_channel_count();
    std::cout << ",\"cycle-aligned\":"
              << (stream.cycle_aligned() ? "true" : "false");
    std::cout << ",\"zero-crossing-aligned\":"
              << (stream.zero_crossing_aligned() ? "true" : "false");
    std::cout << ",\"sample-rate-hz\":" << stream.sample_rate_hz();
    std::cout << ",\"samples-per-cycle\":"
              << stream.samples_per_cycle();
    std::cout << ",\"nominal-frequency-hz\":" << stream.nominal_frequency_hz();
    std::cout << ",\"expected-frame-period-ms\":"
              << stream.expected_frame_period_ms();

    std::cout << '}';
}

static void print_sensor_descriptor(const GeisaSensorDescriptor &sensor)
{
    std::cout << "{\"sensor-id\":";
    print_json_escaped(sensor.sensor_id());

    std::cout << ",\"sensor-type\":";
    print_json_escaped(GeisaSensorType_Name(sensor.sensor_type()));

    if (sensor.has_sensor_subtype())
    {
        std::cout << ",\"sensor-subtype\":";
        print_json_escaped(sensor.sensor_subtype());
    }

    if (sensor.has_name())
    {
        std::cout << ",\"name\":";
        print_json_escaped(sensor.name());
    }

    if (sensor.has_description())
    {
        std::cout << ",\"description\":";
        print_json_escaped(sensor.description());
    }

    if (sensor.has_manufacturer())
    {
        std::cout << ",\"manufacturer\":";
        print_json_escaped(sensor.manufacturer());
    }

    if (sensor.has_model())
    {
        std::cout << ",\"model\":";
        print_json_escaped(sensor.model());
    }

    std::cout << ",\"unit\":";
    print_json_escaped(sensor.unit());
    std::cout << ",\"supports-read\":"
              << (sensor.supports_read() ? "true" : "false");
    std::cout << ",\"supports-publish\":"
              << (sensor.supports_publish() ? "true" : "false");

    if (sensor.has_min_report_period_ms())
    {
        std::cout << ",\"min-report-period-ms\":"
                  << sensor.min_report_period_ms();
    }

    if (sensor.has_max_report_period_ms())
    {
        std::cout << ",\"max-report-period-ms\":"
                  << sensor.max_report_period_ms();
    }

    if (sensor.has_geolocation())
    {
        std::cout << ",\"geolocation\":{";
        std::cout << "\"latitude\":" << sensor.geolocation().latitude();
        std::cout << ",\"longitude\":" << sensor.geolocation().longitude();
        if (sensor.geolocation().has_altitude_m())
        {
            std::cout << ",\"altitude-m\":"
                      << sensor.geolocation().altitude_m();
        }
        std::cout << '}';
    }

    std::cout << '}';
}

// Diagnostic output prints protobuf enum identifiers; schema examples use
// normalized JSON enum strings.
static void print_network_interface(
    const GeisaPlatformDiscovery_Network_Instance &network)
{
    std::cout << "{\"interface-id\":";
    print_json_escaped(network.interface_id());
    std::cout << ",\"network-class\":";
    print_json_escaped(
        GeisaPlatformDiscovery_NetworkClass_Name(network.network_class()));
    std::cout << ",\"owner\":";
    print_json_escaped(
        GeisaPlatformDiscovery_NetworkOwner_Name(network.owner()));
    std::cout << ",\"technology\":";
    print_json_escaped(
        GeisaPlatformDiscovery_NetworkTechnology_Name(network.technology()));
    std::cout << ",\"supports-ipv4\":"
              << (network.supports_ipv4() ? "true" : "false");
    std::cout << ",\"supports-ipv6\":"
              << (network.supports_ipv6() ? "true" : "false");

    if (network.has_name())
    {
        std::cout << ",\"name\":";
        print_json_escaped(network.name());
    }

    if (network.has_description())
    {
        std::cout << ",\"description\":";
        print_json_escaped(network.description());
    }

    std::cout << '}';
}

// Parses GeisaPlatformDiscovery_Rsp and prints a JSON-like representation,
// including enum names for status and waveform/device fields.
static void print_discovery_response(const GeisaPlatformDiscovery_Rsp &rsp)
{
    // Discovery is static/semi-static capability metadata for apps to read
    // on startup
    std::cout << "{\"geisa-platform-discovery-rsp\":{";

    std::cout << "\"status\":";
    print_geisa_status_json(rsp.status());

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
        print_json_comma_if_needed(&first_operator_field);
        std::cout << "\"operator-name\":";
        print_json_escaped(rsp.operator_().operator_name());
    }

    if (!rsp.operator_().operator_identifier().empty())
    {
        print_json_comma_if_needed(&first_operator_field);
        std::cout << "\"operator-identifier\":";
        print_json_escaped(rsp.operator_().operator_identifier());
    }
    std::cout << '}';

    std::cout << ",\"metrology\":{";
    bool first_metrology_field = true;
    if (!rsp.metrology().meter_form().empty())
    {
        print_json_comma_if_needed(&first_metrology_field);
        std::cout << "\"meter-form\":";
        print_json_escaped(rsp.metrology().meter_form());
    }
    print_json_comma_if_needed(&first_metrology_field);
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
    std::cout << ",\"sensors\":[";
    for (int i = 0; i < rsp.sensor().sensors_size(); ++i)
    {
        if (i > 0)
        {
            std::cout << ',';
        }
        print_sensor_descriptor(rsp.sensor().sensors(i));
    }
    std::cout << ']';
    std::cout << '}';

    std::cout << ",\"network\":{";
    std::cout << "\"interface-count\":"
              << rsp.network().interfaces_size();
    std::cout << ",\"interfaces\":[";
    for (int i = 0; i < rsp.network().interfaces_size(); ++i)
    {
        if (i > 0)
        {
            std::cout << ',';
        }
        print_network_interface(rsp.network().interfaces(i));
    }
    std::cout << ']';
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
        return print_example_usage_and_return({
            std::string("usage: ") + argv[0]
                + " discovery-response.bin | --read-rsp discovery-response.bin"});
    }

    const char *input_path = positional ? argv[1] : argv[2];

    std::string payload;
    // Reads a binary protobuf discovery payload captured from a response path
    if (!read_binary_file(input_path, &payload))
    {
        return EXIT_FAILURE;
    }

    GeisaPlatformDiscovery_Rsp response;
    // Parses the payload into the typed response and renders a JSON-like
    // diagnostic view
    if (!response.ParseFromString(payload))
    {
        std::cerr << "Failed to decode GeisaPlatformDiscovery_Rsp "
                     "payload\n";
        return EXIT_FAILURE;
    }

    print_discovery_response(response);

    google::protobuf::ShutdownProtobufLibrary();
    return EXIT_SUCCESS;
}
