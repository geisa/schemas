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
// Example GEISA platform discovery response encoder.
//
// This example creates a sample GeisaPlatformDiscovery_Rsp protobuf payload
// and writes it to a binary file for decoding with the companion reader.
//-----------------------------------------------------------------------------

#include <iostream>
#include <cstdlib>
#include <sysexits.h>
#include <string>

#include "geisa-status.pb.h"
#include "discovery.pb.h"
#include "helpers/example_utils.h"

static void print_post_write_guidance(const std::string &output_path,
                                      const std::string &reader_path)
{
    std::cout << "Wrote discovery response to: " << output_path << '\n';
    std::cout
        << "The output file contains raw bytes of a serialized "
        << "GeisaPlatformDiscovery_Rsp protobuf payload\n";
    std::cout << "Output path: " << output_path << '\n';
    std::cout << "Decode with: " << reader_path << " "
              << output_path << '\n';
    std::cout
        << "Integration note (secondary): In an operational GEISA "
        << "conformant platform, these protobuf bytes are carried via the "
        << "Platform Discovery response path and applications typically decode"
        << " protobuf and/or convert to JSON for processing if/as desired.\n";
}

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    const bool positional = (argc == 2);
    const bool explicit_write =
        (argc == 3 && std::string(argv[1]) == "--write-rsp-sample");
    const bool demo_mode = (argc == 2 && std::string(argv[1]) == "--demo");

    if (!positional && !explicit_write && !demo_mode)
    {
        return print_example_usage_and_return({
            std::string("usage: ") + argv[0]
                + " discovery-response.bin | --write-rsp-sample "
                + "discovery-response.bin | --demo"});
    }

    const std::string output_path =
        demo_mode ? std::string("/tmp/discovery-response.bin")
                  : std::string(positional ? argv[1] : argv[2]);

    // Populates a sample discovery response with representative
    // meter metrology, one sensor descriptor, static network capability
    // descriptors, and one baseline waveform stream.
    GeisaPlatformDiscovery_Rsp response;

    // Populates a top-level GEISA status + version/pillar metadata.
    response.mutable_status()->set_code(GEISA_STATUS_SUCCESS);
    response.mutable_status()->set_message("ok");

    response.mutable_geisa()->set_ver_major(0);
    response.mutable_geisa()->set_ver_minor(9);
    response.mutable_geisa()->set_ver_rev(0);
    response.mutable_geisa()->set_pillar_api(true);
    response.mutable_geisa()->set_pillar_adm(false);
    response.mutable_geisa()->set_pillar_lee(false);
    response.mutable_geisa()->set_pillar_vee(false);

    // Populates one representative descriptor for each discovery section
    GeisaPlatformDiscovery_Module *top_module =
        response.mutable_device()->mutable_top_module();
    top_module->set_type(TYPE_ELECTRIC_METER);
    top_module->set_manufacturer("ExampleCompany");
    top_module->set_model("ExampleMeter");
    top_module->set_serial_number("SN123456");
    top_module->set_hw_revision("A");
    top_module->set_fw_revision("0.9-demo");

    response.mutable_operator_()->set_operator_name("Example Utility");
    response.mutable_operator_()->set_operator_identifier("MTR-0001");

    response.mutable_metrology()->set_meter_form("2S");
    response.mutable_metrology()->set_phase_count(1);
    response.mutable_metrology()->set_neutral_connected(true);
    response.mutable_metrology()->set_nominal_frequency_hz(60);
    response.mutable_metrology()->set_nominal_phase_to_neutral_voltage_v(120);
    response.mutable_metrology()->set_nominal_phase_to_phase_voltage_v(240);

    GeisaSensorDescriptor *sensor = response.mutable_sensor()->add_sensors();
    sensor->set_sensor_id("board_temp1");
    sensor->set_sensor_type(GEISA_SENSOR_TYPE_TEMPERATURE);
    sensor->set_sensor_subtype("board");
    sensor->set_name("Board Temperature Sensor 1");
    sensor->set_description(
        "Internal board or electronics temperature sensor.");
    sensor->set_manufacturer("Example Sensor Vendor");
    sensor->set_model("TMP1075");
    sensor->set_unit("C");
    sensor->set_supports_read(true);
    sensor->set_supports_publish(true);
    sensor->set_min_report_period_ms(1000);
    sensor->set_max_report_period_ms(60000);

    GeisaPlatformDiscovery_Network_Instance *network_operator =
        response.mutable_network()->add_interfaces();
    network_operator->set_interface_id("wan-operator-rfmesh-1");
    network_operator->set_network_class(NETWORK_CLASS_OPERATOR);
    network_operator->set_owner(NETWORK_OWNER_OPERATOR);
    network_operator->set_technology(NETWORK_TECHNOLOGY_RF_MESH);
    network_operator->set_supports_ipv4(true);
    network_operator->set_supports_ipv6(true);
    network_operator->set_name("Utility FAN");
    network_operator->set_description(
        "Utility-operated RF mesh field area network.");

    GeisaPlatformDiscovery_Network_Instance *network_internet =
        response.mutable_network()->add_interfaces();
    network_internet->set_interface_id("wan-internet-cellular-1");
    network_internet->set_network_class(NETWORK_CLASS_INTERNET);
    network_internet->set_owner(NETWORK_OWNER_OPERATOR);
    network_internet->set_technology(NETWORK_TECHNOLOGY_CELLULAR);
    network_internet->set_supports_ipv4(true);
    network_internet->set_supports_ipv6(false);
    network_internet->set_name("Public Internet APN");
    network_internet->set_description(
        "Public internet access path provided through cellular WAN.");

    GeisaPlatformDiscovery_Network_Instance *network_local =
        response.mutable_network()->add_interfaces();
    network_local->set_interface_id("lan-consumer-wifi-1");
    network_local->set_network_class(NETWORK_CLASS_LOCAL);
    network_local->set_owner(NETWORK_OWNER_CONSUMER);
    network_local->set_technology(NETWORK_TECHNOLOGY_WIFI);
    network_local->set_supports_ipv4(true);
    network_local->set_supports_ipv6(true);
    network_local->set_name("Home Wi-Fi");
    network_local->set_description(
        "Consumer local network capability for LAN/HAN access.");

    GeisaPlatformDiscovery_Waveform_Instance *stream =
        response.mutable_waveform()->add_streams();
    stream->set_stream_id("waveform-base");
    stream->set_name("Baseline Waveform Stream");
    stream->set_description(
        "Baseline interoperable waveform stream for GEISA applications");
    stream->set_datatype(DATA_INT16);
    stream->set_voltage_multiplier(1.0);
    stream->set_current_multiplier(1.0);
    stream->set_num_voltage_ch(1);
    stream->set_num_current_ch(2);
    stream->set_num_other_ch(0);
    stream->set_total_channel_count(3);
    stream->set_cycle_aligned(true);
    stream->set_zero_crossing_aligned(true);
    stream->set_sample_rate(7680);
    stream->set_samples_per_cycle(128);
    stream->set_nominal_frequency_hz(60);
    stream->set_expected_frame_period_ms(200);

    if (!serialize_protobuf_to_file(output_path.c_str(),
                                    response,
                                    "GeisaPlatformDiscovery_Rsp"))
    {
        return EXIT_FAILURE;
    }

    if (demo_mode)
    {
        const std::string reader_path = resolve_demo_reader_path(
            argv[0], "discovery_read_example", "/tmp/discovery_read_example");
        // Demo mode immediately decodes via reader so the output is visible
        print_example_info_lines({"Running GEISA discovery demo..."});
        print_post_write_guidance(output_path, reader_path);
    }
    else
    {
        print_post_write_guidance(output_path, "/tmp/discovery_read_example");
    }

    // In demo mode, the sample binary payload is immediately decoded by the
    // companion reader so the end-to-end write/decode flow is visible in one
    // run
    if (demo_mode)
    {
        const std::string reader_path = resolve_demo_reader_path(
            argv[0], "discovery_read_example", "/tmp/discovery_read_example");
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
