/**
 *
 */

#pragma once

#include "sc2/api/request/setup_requests.h"
#include "sc2/conversions/common_conversions.h"
#include "s2clientprotocol/sc2api.pb.h"

namespace astra::sc2
{

    inline void toProto(const PlayerSetup& in, SC2APIProtocol::PlayerSetup* out)
    {
        out->set_type(toProto(in.type));

        if(in.name) out->set_player_name(*in.name);
        if(in.race) out->set_race(toProto(*in.race));
        if(in.difficulty) out->set_difficulty(toProto(*in.difficulty));
        if(in.build) out->set_ai_build(toProto(*in.build));
    }

    inline void toProto(const PortSet& in, SC2APIProtocol::PortSet* out)
    {
        out->set_game_port(in.gamePort);
        out->set_base_port(in.basePort);
    }
}
