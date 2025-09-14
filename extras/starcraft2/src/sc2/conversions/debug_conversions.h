/**
 *
 */

#pragma once

#include "sc2/api/response/debug_responses.h"
#include "s2clientprotocol/sc2api.pb.h"

namespace astra::sc2
{
    inline PingResponse toDomain(const SC2APIProtocol::ResponsePing& in)
    {
        PingResponse out{};
        out.gameVersion = in.game_version();
        out.dataVersion = in.data_version();
        out.dataBuild = in.data_build();
        out.baseBuild = in.base_build();

        return out;
    }
}
