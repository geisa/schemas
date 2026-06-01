//-----------------------------------------------------------------------------
// Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
// Security Alliance (GEISA), a Series of LF Projects, LLC
//
// Licensed under the Apache License, Version 2.0. See LICENSE.
//-----------------------------------------------------------------------------

#include <cstdlib>
#include <string>
#include <sysexits.h>

#include "actuator.pb.h"
#include "geisa-status.pb.h"
#include "helpers/example_utils.h"

// Runs the actuator demo flow, writes representative payloads, and then
// decodes them through the reader to show the round trip end-to-end
int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    const bool demo_mode = (argc == 2 && std::string(argv[1]) == "--demo");

    if (!demo_mode)
    {
        return print_example_usage_and_return({
            std::string("usage: ") + argv[0] + " --demo"});
    }

    const std::string get_req_path = "/tmp/actuator-get-req.bin";
    const std::string get_rsp_path = "/tmp/actuator-get-rsp.bin";
    const std::string set_req_path = "/tmp/actuator-set-req.bin";
    const std::string set_rsp_path = "/tmp/actuator-set-rsp.bin";

    GeisaActuatorGet_Req get_req;
    get_req.set_actuator(GEISA_TYPE_ACTUATOR_SERVICE_SWITCH);

    GeisaActuatorGet_Rsp get_rsp;
    get_rsp.mutable_status()->set_code(GEISA_STATUS_SUCCESS);
    get_rsp.mutable_status()->set_message("ok");
    get_rsp.mutable_actuator_status()->set_actuator(
        GEISA_TYPE_ACTUATOR_SERVICE_SWITCH);
    get_rsp.mutable_actuator_status()->set_on(GEISA_TYPE_ON_OFF_ON);
    get_rsp.mutable_actuator_status()->set_position_present(false);

    GeisaActuatorSet_Req set_req;
    GeisaActuatorStatus *set_entry = set_req.add_new_settings();
    set_entry->set_actuator(GEISA_TYPE_ACTUATOR_LC_RELAY_0);
    set_entry->set_on(GEISA_TYPE_ON_OFF_OFF);
    set_entry->set_position_present(false);

    GeisaActuatorSet_Rsp set_rsp;
    set_rsp.mutable_status()->set_code(GEISA_STATUS_SUCCESS);
    set_rsp.mutable_status()->set_message("accepted");

    if (!serialize_protobuf_to_file(get_req_path.c_str(),
                                    get_req,
                                    "GeisaActuatorGet_Req") ||
        !serialize_protobuf_to_file(get_rsp_path.c_str(),
                                    get_rsp,
                                    "GeisaActuatorGet_Rsp") ||
        !serialize_protobuf_to_file(set_req_path.c_str(),
                                    set_req,
                                    "GeisaActuatorSet_Req") ||
        !serialize_protobuf_to_file(set_rsp_path.c_str(),
                                    set_rsp,
                                    "GeisaActuatorSet_Rsp"))
    {
        google::protobuf::ShutdownProtobufLibrary();
        return EXIT_FAILURE;
    }

    const std::string reader_path = resolve_demo_reader_path(
        argv[0], "actuator_read_example", "/tmp/actuator_read_example");

    print_example_info_lines({
        "Running actuator demo...",
        "Wrote /tmp/actuator-get-req.bin",
        "Wrote /tmp/actuator-get-rsp.bin",
        "Wrote /tmp/actuator-set-req.bin",
        "Wrote /tmp/actuator-set-rsp.bin",
        std::string("Reader: ") + reader_path});

    const int get_req_rc =
        run_demo_subprocess(reader_path + " --get-req " + get_req_path);
    const int get_rsp_rc =
        run_demo_subprocess(reader_path + " --get-rsp " + get_rsp_path);
    const int set_req_rc =
        run_demo_subprocess(reader_path + " --set-req " + set_req_path);
    const int set_rsp_rc =
        run_demo_subprocess(reader_path + " --set-rsp " + set_rsp_path);

    if (get_req_rc != 0 || get_rsp_rc != 0 || set_req_rc != 0 || set_rsp_rc != 0)
    {
        google::protobuf::ShutdownProtobufLibrary();
        return EXIT_FAILURE;
    }

    google::protobuf::ShutdownProtobufLibrary();
    return EXIT_SUCCESS;
}
