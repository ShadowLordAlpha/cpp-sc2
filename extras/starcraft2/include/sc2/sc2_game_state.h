/**
 *
 */

#pragma once

#include "astra/game_state.h"
#include "sc2/api/response/game_responses.h"

namespace astra::sc2
{
    class StarCraft2GameState : public astra::GameState
    {
    public:
        ObservationResponse observation;
    };
}
