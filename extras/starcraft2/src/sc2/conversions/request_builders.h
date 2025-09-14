/**
 *
 */

#pragma once

#include <array>
#include <format>
#include <optional>
#include <variant>
#include <memory>
#include <cassert>
#include "sc2/api/common.h"
#include "sc2/actions/chat_actions.h"
#include "sc2/actions/debug_actions.h"
#include "sc2/actions/observer_actions.h"
#include "sc2/actions/raw_actions.h"
#include "sc2/actions/spatial_actions.h"
#include "sc2/actions/ui_actions.h"
#include "enums.h"
#include "debug_conversions.h"
#include "setup_conversions.h"
#include "game_conversions.h"
#include "s2clientprotocol/sc2api.pb.h"

namespace astra::sc2
{
    void fillCreateGameRequest(SC2APIProtocol::Request* req, const std::variant<std::string, char*>& map,const std::vector<PlayerSetup>& players, bool disableFog, const std::optional<uint32_t>& randomSeed, bool realtime)
    {
        auto createGame = req->mutable_create_game();

        // Map
        if(std::holds_alternative<std::string>(map))
        {
            auto& name = std::get<std::string>(map);
            if(name.ends_with("SC2MAP"))
            {
                createGame->mutable_local_map()->set_map_path(name);
            }
            else
            {
                createGame->set_battlenet_map_name(name);
            }
        }
        else
        {
            auto* local = createGame->mutable_local_map();
            local->set_map_data(std::get<char*>(map));
        }

        // Players
        for(auto& p : players) {
            toProto(p, createGame->add_player_setup());
        }

        // Flags
        createGame->set_disable_fog(disableFog);
        if(randomSeed) createGame->set_random_seed(*randomSeed);
        createGame->set_realtime(realtime);
    }

    void fillJoinGameRequest(SC2APIProtocol::Request* req, const std::variant<Race, uint32_t>& participation, const InterfaceOptions& options, std::optional<PortSet>& serverPorts, std::optional<std::vector<PortSet>>& clientPorts, int sharedPort, std::optional<std::string_view>& playerName, std::string_view hostIp)
    {
        auto joinGame = req->mutable_join_game();

        if(std::holds_alternative<Race>(participation))
        {
            joinGame->set_race(toProto(std::get<Race>(participation)));
        }
        else
        {
            joinGame->set_observed_player_id(std::get<uint32_t>(participation));
        }

        toProto(options, joinGame->mutable_options());

        if(serverPorts) toProto(*serverPorts, joinGame->mutable_server_ports());
        if(clientPorts) for(auto& port: *clientPorts) { toProto(port, joinGame->add_client_ports()); }

        joinGame->set_shared_port(sharedPort);
        if(playerName) joinGame->set_player_name(*playerName);
        joinGame->set_host_ip(hostIp);
    }

    void fillStartReplayRequest(SC2APIProtocol::Request* req, const std::variant<std::string, char*>& replay, uint32_t observerPlayerId, const InterfaceOptions& options, bool disableFog, bool realtime, bool recordReplay)
    {
        auto startReplay = req->mutable_start_replay();

        if(std::holds_alternative<std::string>(replay))
        {
            startReplay->set_replay_path(std::get<std::string>(replay));
        }
        else
        {
            startReplay->set_replay_data(std::get<char*>(replay));
        }

        // TODO: If we want to support map data then it goes here
        startReplay->set_observed_player_id(observerPlayerId);

        toProto(options, startReplay->mutable_options());

        startReplay->set_disable_fog(disableFog);
        startReplay->set_realtime(realtime);
        startReplay->set_record_replay(recordReplay);
    }

    void fillObservationRequest(SC2APIProtocol::Request* req, bool disableFog, uint32_t gameLoop)
    {
        auto observation = req->mutable_observation();

        if(disableFog) observation->set_disable_fog(disableFog);
        if(gameLoop > 0) observation->set_game_loop(gameLoop);
    }

    void fillActionRequest(SC2APIProtocol::Request* req, const std::vector<Action*>& actions)
    {
        auto debug = req->mutable_action();

        for(const auto action: actions)
        {
            toProto(action, debug->add_actions());
        }
    }

    void fillObsActionRequest(SC2APIProtocol::Request* req, const std::vector<ObserverAction*>& actions)
    {
        auto debug = req->mutable_obs_action();

        for(const auto action: actions)
        {
            toProto(action, debug->add_actions());
        }
    }

    void fillStepRequest(SC2APIProtocol::Request* req, uint32_t count)
    {
        auto step = req->mutable_step();

        step->set_count(count);
    }

    void fillDataRequest(SC2APIProtocol::Request* req, bool abilityData, bool unitTypeData, bool upgradeData, bool buffData, bool effectData)
    {
        auto data = req->mutable_data();

        if(abilityData) data->set_ability_id(abilityData);
        if(unitTypeData) data->set_unit_type_id(unitTypeData);
        if(upgradeData) data->set_upgrade_id(upgradeData);
        if(buffData) data->set_buff_id(buffData);
        if(effectData) data->set_effect_id(effectData);
    }

    void fillQueryRequest(SC2APIProtocol::Request* req, const std::vector<QueryPathing>& pathing, const std::vector<uint64_t>& abilities, const std::vector<QueryPlacement>& placements, bool ignoreResourceRequirements)
    {
        auto query = req->mutable_query();

        if(!pathing.empty())
        {
            for(const auto& p: pathing)
            {
                toProto(p, query->add_pathing());
            }
        }

        if(!abilities.empty())
        {
            for(const auto& a: abilities)
            {
                auto ability = query->add_abilities();
                ability->set_unit_tag(a);
            }
        }

        if(!placements.empty())
        {
            for(const auto& p: placements)
            {
                toProto(p, query->add_placements());
            }
        }

        query->set_ignore_resource_requirements(ignoreResourceRequirements);
    }

    void fillMapCommandRequest(SC2APIProtocol::Request* req, const std::string& command)
    {
        auto mapCommand = req->mutable_map_command();

        mapCommand->set_trigger_cmd(command);
    }

    void fillReplayInfoRequest(SC2APIProtocol::Request* req, std::variant<std::filesystem::path, char*> replay, bool downloadData)
    {
        auto replayInfo = req->mutable_replay_info();

        if(std::holds_alternative<std::filesystem::path>(replay))
        {
            replayInfo->set_replay_path(std::get<0>(replay).string());
        }
        else
        {
            replayInfo->set_replay_data(std::get<1>(replay));
        }

        if(downloadData) replayInfo->set_download_data(downloadData);
    }

    void fillSaveMapRequest(SC2APIProtocol::Request* req, const std::filesystem::path& savePath, char* mapData)
    {
        auto saveMap = req->mutable_save_map();

        saveMap->set_map_path(savePath.string());
        saveMap->set_map_data(mapData);
    }

    void fillDebugRequest(SC2APIProtocol::Request* req, const std::vector<DebugAction*>& actions)
    {
        auto debug = req->mutable_debug();

        for(const auto action: actions)
        {
            toProto(action, debug->add_debug());
        }
    }
}
