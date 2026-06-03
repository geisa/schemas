//-----------------------------------------------------------------------------
// Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
// Security Alliance (GEISA), a Series of LF Projects, LLC
//
// Licensed under the Apache License, Version 2.0. See LICENSE.
//-----------------------------------------------------------------------------

#include <cstdlib>
#include <iostream>
#include <string>
#include <sysexits.h>

#include "actuator.pb.h"
#include "geisa-status.pb.h"
#include "helpers/example_utils.h"

// Converts the actuator enum to the normalized JSON string used by the schema
// examples and JSON-like diagnostic output.
static const char *actuator_to_json(const GeisaTypeActuator value)
{
    switch (value)
    {
    case GEISA_TYPE_ACTUATOR_SERVICE_SWITCH:
        return "service-switch";
    case GEISA_TYPE_ACTUATOR_DER_SWITCH:
        return "der-switch";
    case GEISA_TYPE_ACTUATOR_LC_RELAY_0:
        return "lc-relay-0";
    case GEISA_TYPE_ACTUATOR_LC_RELAY_1:
        return "lc-relay-1";
    case GEISA_TYPE_ACTUATOR_LC_RELAY_2:
        return "lc-relay-2";
    case GEISA_TYPE_ACTUATOR_LC_RELAY_3:
        return "lc-relay-3";
    case GEISA_TYPE_ACTUATOR_UNSPECIFIED:
    default:
        return "unspecified";
    }
}

// Converts the on/off enum to the normalized JSON string used by the schema
// examples and JSON-like diagnostic output.
static const char *state_to_json(const GeisaTypeOnOff value)
{
    switch (value)
    {
    case GEISA_TYPE_ON_OFF_ON:
        return "on";
    case GEISA_TYPE_ON_OFF_OFF:
        return "off";
    case GEISA_TYPE_ON_OFF_UNSPECIFIED:
    default:
        return "unspecified";
    }
}

// Renders a GeisaActuatorStatus message as JSON-like output for the actuator
// get/set examples and diagnostic reading..
static void print_actuator_status(const GeisaActuatorStatus &status)
{
    std::cout << "{";
    print_json_key("actuator");
    print_json_escaped(actuator_to_json(status.actuator()));
    std::cout << ",";
    print_json_key("state");
    print_json_escaped(state_to_json(status.on()));
    std::cout << ",\"position-present\":"
              << (status.position_present() ? "true" : "false");
    if (status.position_present())
    {
        std::cout << ",\"position\":" << status.position();
    }
    std::cout << "}";
}

// Prints a GeisaActuatorGet_Req payload using the schema's top-level wrapper.
static void print_get_req(const GeisaActuatorGet_Req &req)
{
    std::cout << "{\"geisa-actuator-get-req\":{";
    print_json_key("actuator");
    print_json_escaped(actuator_to_json(req.actuator()));
    std::cout << "}}\n";
}

// Prints a GeisaActuatorGet_Rsp payload with status and current actuator state.
static void print_get_rsp(const GeisaActuatorGet_Rsp &rsp)
{
    std::cout << "{\"geisa-actuator-get-rsp\":{";
    print_json_key("status");
    print_geisa_status_json(rsp.status());
    std::cout << ",\"actuator-status\":";
    print_actuator_status(rsp.actuator_status());
    std::cout << "}}\n";
}

// Prints a GeisaActuatorSet_Req payload with the repeated new_settings entries.
static void print_set_req(const GeisaActuatorSet_Req &req)
{
    std::cout << "{\"geisa-actuator-set-req\":{\"new-settings\":[";
    for (int i = 0; i < req.new_settings_size(); ++i)
    {
        if (i > 0)
        {
            std::cout << ",";
        }
        print_actuator_status(req.new_settings(i));
    }
    std::cout << "]}}\n";
}

// Prints a GeisaActuatorSet_Rsp payload with the GEISA status envelope.
static void print_set_rsp(const GeisaActuatorSet_Rsp &rsp)
{
    std::cout << "{\"geisa-actuator-set-rsp\":{";
    print_json_key("status");
    print_geisa_status_json(rsp.status());
    std::cout << "}}\n";
}

// Parses explicit actuator decode modes so req/rsp payloads are never guessed
// from the wire bytes.
int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    const bool get_req_mode = (argc == 3 && std::string(argv[1]) == "--get-req");
    const bool get_rsp_mode = (argc == 3 && std::string(argv[1]) == "--get-rsp");
    const bool set_req_mode = (argc == 3 && std::string(argv[1]) == "--set-req");
    const bool set_rsp_mode = (argc == 3 && std::string(argv[1]) == "--set-rsp");

    if (!get_req_mode && !get_rsp_mode && !set_req_mode && !set_rsp_mode)
    {
        return print_example_usage_and_return({
            std::string("usage: ") + argv[0]
                + " --get-req file.bin | --get-rsp file.bin | --set-req file.bin | --set-rsp file.bin"});
    }

    std::string bytes;
    if (!read_binary_file(argv[2], &bytes))
    {
        return EXIT_FAILURE;
    }

    if (get_req_mode)
    {
        GeisaActuatorGet_Req req;
        if (!req.ParseFromString(bytes))
        {
            std::cerr << "failed to decode GeisaActuatorGet_Req\n";
            google::protobuf::ShutdownProtobufLibrary();
            return EXIT_FAILURE;
        }
        print_get_req(req);
    }
    else if (get_rsp_mode)
    {
        GeisaActuatorGet_Rsp rsp;
        if (!rsp.ParseFromString(bytes))
        {
            std::cerr << "failed to decode GeisaActuatorGet_Rsp\n";
            google::protobuf::ShutdownProtobufLibrary();
            return EXIT_FAILURE;
        }
        print_get_rsp(rsp);
    }
    else if (set_req_mode)
    {
        GeisaActuatorSet_Req req;
        if (!req.ParseFromString(bytes))
        {
            std::cerr << "failed to decode GeisaActuatorSet_Req\n";
            google::protobuf::ShutdownProtobufLibrary();
            return EXIT_FAILURE;
        }
        print_set_req(req);
    }
    else
    {
        GeisaActuatorSet_Rsp rsp;
        if (!rsp.ParseFromString(bytes))
        {
            std::cerr << "failed to decode GeisaActuatorSet_Rsp\n";
            google::protobuf::ShutdownProtobufLibrary();
            return EXIT_FAILURE;
        }
        print_set_rsp(rsp);
    }

    google::protobuf::ShutdownProtobufLibrary();
    return EXIT_SUCCESS;
}
