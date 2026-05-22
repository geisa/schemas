//-----------------------------------------------------------------------------
// Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
// Security Alliance (GEISA), a Series of LF Projects, LLC
//
// Licensed under the Community Specification License 1.0. See LICENSE.
//-----------------------------------------------------------------------------
//
// Example GEISA application message req/rsp encoder.
//
// This example writes one request payload and one response payload and can run
// an end-to-end demo by immediately decoding both with app_message_read_example.
//
//-----------------------------------------------------------------------------

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <sys/wait.h>

#include "app-message.pb.h"

static bool write_file(const char *path, const std::string &bytes)
{
    // Persists the serialized protobuf bytes for the demo and manual decode..
    std::ofstream output(path, std::ios::binary);
    if (!output)
    {
        std::cerr << "failed to open output file: " << path << '\n';
        return false;
    }

    output.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
    if (!output.good())
    {
        std::cerr << "failed to write output file: " << path << '\n';
        return false;
    }

    return true;
}

static int run_reader_subprocess(const std::string &mode,
                                 const std::string &path)
{
    // Reuse the reader example so the demo exercises the same decode path.
    const std::string command =
        "/tmp/app_message_read_example " + mode + " " + path;
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

    // Support either explicit output paths or the default --demo flow.
    const bool demo_mode = (argc == 2 && std::string(argv[1]) == "--demo");
    const bool positional = (argc == 3 && !demo_mode);

    if (!demo_mode && !positional)
    {
        std::cerr << "usage: " << argv[0]
                  << " req.bin rsp.bin | --demo\n";
        return 2;
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

    std::string req_bytes;
    if (!req.SerializeToString(&req_bytes))
    {
        std::cerr << "failed to serialize GeisaAppMessage_Req\n";
        return 1;
    }

    if (!write_file(req_path.c_str(), req_bytes))
    {
        return 1;
    }

    GeisaAppMessage_Rsp rsp;
    // Write a representative platform response for the request above.
    // Quota counters and usage are intentionally out-of-band in status APIs.
    rsp.set_request_id("req-0001");
    rsp.set_status(GEISA_APP_MESSAGE_STATUS_ACCEPTED);
    rsp.set_status_text("accepted for delivery");
    rsp.set_priority(GEISA_APP_MESSAGE_PRIORITY_IMMEDIATE);
    rsp.set_description_type(GEISA_APP_MESSAGE_DESCRIPTION_TYPE_APP_DATA);
    rsp.set_timestamp_ms(1762862400100ULL);
    rsp.set_ttl_ms(300000ULL);

    std::string rsp_bytes;
    if (!rsp.SerializeToString(&rsp_bytes))
    {
        std::cerr << "failed to serialize GeisaAppMessage_Rsp\n";
        return 1;
    }

    if (!write_file(rsp_path.c_str(), rsp_bytes))
    {
        return 1;
    }

    if (demo_mode)
    {
        // Show the files and immediately decode them so the flow is visible.
        std::cout << "Running app-message demo...\n";
        std::cout << "Wrote app message request to /tmp/app-message-req.bin\n";
        std::cout << "Wrote app message response to /tmp/app-message-rsp.bin\n";
        std::cout << "Decode with: /tmp/app_message_read_example --req /tmp/app-message-req.bin\n";
        std::cout << "Decode with: /tmp/app_message_read_example --rsp /tmp/app-message-rsp.bin\n";

        int rc = run_reader_subprocess("--req", req_path);
        if (rc != 0)
        {
            google::protobuf::ShutdownProtobufLibrary();
            return rc;
        }

        rc = run_reader_subprocess("--rsp", rsp_path);
        if (rc != 0)
        {
            google::protobuf::ShutdownProtobufLibrary();
            return rc;
        }
    }

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
