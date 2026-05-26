//-----------------------------------------------------------------------------
// Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
// Security Alliance (GEISA), a Series of LF Projects, LLC
//
// Licensed under the Community Specification License 1.0. See LICENSE.
//-----------------------------------------------------------------------------
//
// Example GEISA platform discovery request encoder.
//
// This example writes a binary GeisaPlatformDiscovery_Req protobuf payload.
// GeisaPlatformDiscovery_Req currently has no payload fields.
//
//-----------------------------------------------------------------------------

#include <fstream>
#include <cstdlib>
#include <sysexits.h>
#include <iostream>
#include <string>

#include "discovery.pb.h"

static bool write_file(const char *path, const GeisaPlatformDiscovery_Req &request)
{
    std::ofstream output(path, std::ios::binary);
    if (!output)
    {
        std::cerr << "failed to open output file: " << path << '\n';
        return false;
    }

    if (!request.SerializeToOstream(&output))
    {
        std::cerr << "failed to serialize GeisaPlatformDiscovery_Req\n";
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    const bool demo_mode = (argc == 2 && std::string(argv[1]) == "--demo");
    const bool positional = (argc == 2 && !demo_mode);

    if (!demo_mode && !positional)
    {
        std::cerr << "usage: " << argv[0]
                  << " discovery-request.bin | --demo\n";
        return EX_USAGE;
    }

    const std::string output_path =
        demo_mode ? std::string("/tmp/discovery-request.bin")
                  : std::string(argv[1]);

    // GeisaPlatformDiscovery_Req currently has no payload fields.
    // The requesting application instance is identified by the MQTT topic
    // <userid> segment. Future versions may add request parameters if needed.
    GeisaPlatformDiscovery_Req request;

    if (!write_file(output_path.c_str(), request))
    {
        return EXIT_FAILURE;
    }

    if (demo_mode)
    {
        std::cout << "Running discovery request demo...\n";
    }

    std::cout << "Wrote discovery request to: " << output_path << '\n';
    std::cout
        << "The output file contains raw bytes of a serialized "
        << "GeisaPlatformDiscovery_Req protobuf payload\n";
    std::cout << "Message topic: geisa/api/platform/discovery/req/<userid>\n";

    google::protobuf::ShutdownProtobufLibrary();
    return EXIT_SUCCESS;
}
