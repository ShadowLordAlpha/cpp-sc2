/**
 *
 */

#pragma once

#include <optional>
#include <string>
#include <filesystem>
#include <vector>
#include <sc2/api/common.h>

namespace astra::sc2
{
    struct PlayerInfoExtra
    {
        PlayerInfo playerInfo;
        PlayerResult playerResult;
        int playerMMR = 0;
        int playerAPM = 0;
    };

    struct ReplayInfoResponse
    {
        std::string mapName;
        std::filesystem::path localMapPath;
        std::vector<PlayerInfoExtra> playerInfo;
        uint32_t gameDurationLoops;
        float gameDurationSeconds;
        std::string gameVersion;
        std::string dataVersion;
        uint32_t dataBuild;
        uint32_t baseBuild;
    };
}
