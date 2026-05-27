//-----------------------------------------------------------------------------
// Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
// Security Alliance (GEISA), a Series of LF Projects, LLC
//
// Licensed under the Community Specification License 1.0. See LICENSE.
//-----------------------------------------------------------------------------
//
// Example GEISA application message req/rsp encoder.
//
// This example writes one request payload and one response message and can run
// an end-to-end demo by immediately decoding both with app_message_read_example.
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <cstdlib>
#include <sysexits.h>
#include <string>

#include "app-message.pb.h"
#include "helpers/example_utils.h"

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // Support either explicit output paths or the default --demo flow.
    const bool demo_mode = (argc == 2 && std::string(argv[1]) == "--demo");
    const bool positional = (argc == 3 && !demo_mode);

    if (!demo_mode && !positional)
    {
        return print_example_usage_and_return({
            std::string("usage: ") + argv[0] + " req.bin rsp.bin | --demo"});
    }

    const std::string req_path =
        demo_mode ? std::string("/tmp/app-message-req.bin") : std::string(argv[1]);
    const std::string rsp_path =
        demo_mode ? std::string("/tmp/app-message-rsp.bin") : std::string(argv[2]);

    GeisaAppMessage_Req req;
    // Write a representative app-data request payload.
    req.set_request_id("req-0001");
    req.set_priority(GEISA_APP_MESSAGE_PRIORITY_IMMEDIATE);
    req.set_description_type(GEISA_APP_MESSAGE_DESCRIPTION_TYPE_APP_DATA);
    req.set_timestamp_ms(1762862400000ULL);
    req.set_ttl_ms(300000ULL);
    req.set_content_type("application/json");
    req.set_payload("{\"event\":\"meter-snapshot\",\"kwh\":1234567}");

    if (!serialize_protobuf_to_file(req_path.c_str(),
                                    req,
                                    "GeisaAppMessage_Req"))
    {
        return EXIT_FAILURE;
    }

    GeisaAppMessage_Rsp rsp;
    // Write a representative platform response for the request above.
    // Quota counters and usage are intentionally out-of-band in status APIs.
    rsp.set_request_id("req-0001");
    rsp.set_status(GEISA_APP_MESSAGE_STATUS_ACCEPTED);
    rsp.set_status_text("accepted for processing");
    rsp.set_priority(GEISA_APP_MESSAGE_PRIORITY_IMMEDIATE);
    rsp.set_description_type(GEISA_APP_MESSAGE_DESCRIPTION_TYPE_APP_DATA);
    rsp.set_timestamp_ms(1762862400100ULL);
    rsp.set_ttl_ms(300000ULL);

    if (!serialize_protobuf_to_file(rsp_path.c_str(),
                                    rsp,
                                    "GeisaAppMessage_Rsp"))
    {
        return EXIT_FAILURE;
    }

    if (demo_mode)
    {
        const std::string reader_path = resolve_demo_reader_path(
            argv[0], "app_message_read_example", "/tmp/app_message_read_example");
        // Show the files and immediately decode them so the flow is visible.
        print_example_info_lines({
            "Running app-message demo...",
            "Wrote app message request to /tmp/app-message-req.bin",
            "Wrote app message response to /tmp/app-message-rsp.bin",
            std::string("Decode with: ") + reader_path
                + " --req /tmp/app-message-req.bin",
            std::string("Decode with: ") + reader_path
                + " --rsp /tmp/app-message-rsp.bin"});

        int rc = run_demo_subprocess(reader_path + " --req " + req_path);
        if (rc != 0)
        {
            google::protobuf::ShutdownProtobufLibrary();
            return rc;
        }

        rc = run_demo_subprocess(reader_path + " --rsp " + rsp_path);
        if (rc != 0)
        {
            google::protobuf::ShutdownProtobufLibrary();
            return rc;
        }
    }

    google::protobuf::ShutdownProtobufLibrary();
    return EXIT_SUCCESS;
}
