#include "sc2api/sc2_score.h"

#include <iostream>
#include <cassert>

#include "s2clientprotocol/sc2api.pb.h"

namespace sc2 {

ScoreDetails::ScoreDetails() :    
    idle_production_time(0.0f),
    idle_worker_time(0.0f),
    total_value_units(0.0f),
    total_value_structures(0.0f),
    killed_value_units(0.0f),
    killed_value_structures(0.0f),
    collected_minerals(0.0f),
    collected_vespene(0.0f),
    collection_rate_minerals(0.0f),
    collection_rate_vespene(0.0f),
    spent_minerals(0.0f),
    spent_vespene(0.0f) {
}

bool Score::IsEqual(const Score& other_score) const {
    if (score != other_score.score) {
        return false;
    }

    for (int i = 0; i < float_count_; ++i) {
        if (RawFloats()[i] != other_score.RawFloats()[i]) {
            return false;
        }
    }

    return true;
}

}
