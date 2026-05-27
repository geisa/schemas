//-----------------------------------------------------------------------------
// Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
// Security Alliance (GEISA), a Series of LF Projects, LLC
//
// Licensed under the Apache License, Version 2.0. See LICENSE.
//-----------------------------------------------------------------------------
//
// Example GEISA application message decoder.
//
// This example decodes either GeisaAppMessage_Req or GeisaAppMessage_Rsp from
// a binary protobuf file and prints a JSON-like representation. Payload bytes
// are rendered as hex so the output stays readable in a terminal.
//-----------------------------------------------------------------------------

#include <cstdlib>
#include <sysexits.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "app-message.pb.h"
#include "helpers/example_utils.h"

// Renders the payload bytes in hex for a simple JSON-like preview
static std::string bytes_to_hex(const std::string &bytes)
{
    std::ostringstream out;
    out << std::hex << std::setfill('0');
    for (unsigned char b : bytes)
    {
        out << std::setw(2) << static_cast<unsigned int>(b);
    }
    return out.str();
}

// Common fields shared by both request and response examples.
static void print_common_fields(const std::string &request_id,
                                const std::string &priority,
                                const std::string &description_type,
                                const uint64_t timestamp_ms,
                                const uint64_t ttl_ms,
                                const std::string &content_type,
                                const std::string &payload)
{
    bool first_field = true;
    print_json_comma_if_needed(&first_field);
    print_json_key("request-id");
    print_json_escaped(request_id);
    print_json_comma_if_needed(&first_field);
    print_json_key("priority");
    print_json_escaped(priority);
    print_json_comma_if_needed(&first_field);
    print_json_key("description-type");
    print_json_escaped(description_type);
    std::cout << ",\"timestamp-ms\":" << timestamp_ms;
    std::cout << ",\"ttl-ms\":" << ttl_ms;

    if (!content_type.empty())
    {
        std::cout << ",\"content-type\":";
        print_json_escaped(content_type);
    }

    if (!payload.empty())
    {
        std::cout << ",\"payload-hex\":";
        print_json_escaped(bytes_to_hex(payload));
    }
}

// Prints a compact JSON-like view of the request payload
static void print_req(const GeisaAppMessage_Req &req)
{
    std::cout << "{\"geisa-app-message-req\":{";
    print_common_fields(req.request_id(),
                        GeisaAppMessagePriority_Name(req.priority()),
                        GeisaAppMessageDescriptionType_Name(req.description_type()),
                        req.timestamp_ms(),
                        req.ttl_ms(),
                        req.content_type(),
                        req.payload());
    std::cout << "}}\n";
}

// Print a compact JSON-like view of the response message.
// This response is per-request disposition, not quota/usage reporting
static void print_rsp(const GeisaAppMessage_Rsp &rsp)
{
    std::cout << "{\"geisa-app-message-rsp\":{";
    print_common_fields(rsp.request_id(),
                        GeisaAppMessagePriority_Name(rsp.priority()),
                        GeisaAppMessageDescriptionType_Name(rsp.description_type()),
                        rsp.timestamp_ms(),
                        rsp.ttl_ms(),
                        "",
                        "");

    std::cout << ",\"status\":";
    print_json_escaped(GeisaAppMessageStatus_Name(rsp.status()));

    if (!rsp.status_text().empty())
    {
        std::cout << ",\"status-text\":";
        print_json_escaped(rsp.status_text());
    }

    std::cout << "}}\n";
}

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // Require explicit request/response mode. Protobuf payloads are not
    // self-describing, and req/rsp wire compatibility can produce misleading
    // output if auto-detected.
    const bool req_mode = (argc == 3 && std::string(argv[1]) == "--req");
    const bool rsp_mode = (argc == 3 && std::string(argv[1]) == "--rsp");

    if (!req_mode && !rsp_mode)
    {
        return print_example_usage_and_return({
            std::string("usage: ") + argv[0]
                + " --req app-message-req.bin | --rsp app-message-rsp.bin"});
    }

    std::string bytes;
    const char *path = argv[2];
    // Read the serialized protobuf bytes from disk before decoding.
    if (!read_binary_file(path, &bytes))
    {
        return EXIT_FAILURE;
    }

    if (req_mode)
    {
        GeisaAppMessage_Req req;
        if (!req.ParseFromString(bytes) || req.request_id().empty())
        {
            std::cerr << "failed to decode GeisaAppMessage_Req\n";
            google::protobuf::ShutdownProtobufLibrary();
            return EXIT_FAILURE;
        }

        print_req(req);
        google::protobuf::ShutdownProtobufLibrary();
        return EXIT_SUCCESS;
    }

    if (rsp_mode)
    {
        GeisaAppMessage_Rsp rsp;
        if (!rsp.ParseFromString(bytes) || rsp.request_id().empty())
        {
            std::cerr << "failed to decode GeisaAppMessage_Rsp\n";
            google::protobuf::ShutdownProtobufLibrary();
            return EXIT_FAILURE;
        }

        print_rsp(rsp);
        google::protobuf::ShutdownProtobufLibrary();
        return EXIT_SUCCESS;
    }

    google::protobuf::ShutdownProtobufLibrary();
    return EX_USAGE;
}
