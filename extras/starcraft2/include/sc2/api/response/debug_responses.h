/*
 *
 */

#pragma once

#include <string>

namespace astra::sc2
{
    struct PingResponse
    {
        std::string gameVersion;
        std::string dataVersion;
        uint32_t dataBuild;
        uint32_t baseBuild;
    };
}
