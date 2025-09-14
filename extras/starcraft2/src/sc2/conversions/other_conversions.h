/**
 *
 */

#pragma once

#include <string>
#include <vector>
#include "sc2/conversions/common_conversions.h"
#include "s2clientprotocol/sc2api.pb.h"

namespace astra::sc2
{
    inline PlayerInfoExtra toDomain(const SC2APIProtocol::PlayerInfoExtra& in)
    {
        PlayerInfoExtra out{};
        out.playerInfo = toDomain(in.player_info());
        out.playerResult = toDomain(in.player_result());
        out.playerMMR = in.player_mmr();
        out.playerAPM = in.player_apm();

        return out;
    }

    inline ReplayInfoResponse toDomain(const SC2APIProtocol::ResponseReplayInfo& in)
    {
        ReplayInfoResponse out{};
        out.mapName = in.map_name();
        out.localMapPath = in.local_map_path();

        out.playerInfo.reserve(in.player_info_size());
        for(auto& player: in.player_info())
        {
            out.playerInfo.push_back(toDomain(player));
        }

        out.gameDurationLoops = in.game_duration_loops();
        out.gameDurationSeconds = in.game_duration_seconds();
        out.gameVersion = in.game_version();
        out.dataVersion = in.data_version();
        out.dataBuild = in.data_build();
        out.baseBuild = in.base_build();

        return out;
    }

    inline std::vector<std::string> toDomain(const SC2APIProtocol::ResponseAvailableMaps& in)
    {
        std::vector<std::string> out;

        out.reserve(in.local_map_paths_size() + in.battlenet_map_names_size());
        for(auto& map: in.local_map_paths())
        {
            out.push_back(map);
        }

        for(auto& map: in.battlenet_map_names())
        {
            out.push_back(map);
        }

        return out;
    }
}
