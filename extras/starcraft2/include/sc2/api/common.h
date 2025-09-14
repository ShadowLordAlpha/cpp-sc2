/**
 *
 */

#pragma once

#include <string>
#include <optional>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace astra::sc2
{
    using Point2I = glm::ivec2;
    using Point2D = glm::vec2;
    using Point3D = glm::vec3;

    struct RectangleI
    {
        Point2I min;
        Point2I max;
    };

    enum class PlayerType {
        Participant = 1,
        Computer = 2,
        Observer = 3,
    };

    enum class Difficulty {
        VeryEasy = 1,
        Easy = 2,
        Medium = 3,
        MediumHard = 4,
        Hard = 5,
        Harder = 6,
        VeryHard = 7,
        CheatVision = 8,
        CheatMoney = 9,
        CheatInsane = 10,
    };

    enum class AIBuild {
        RandomBuild = 1,
        Rush = 2,
        Timing = 3,
        Power = 4,
        Macro = 5,
        Air = 6,
    };

    enum class Race {
        NoRace = 0,
        Terran = 1,
        Zerg = 2,
        Protoss = 3,
        Random = 4,
    };

    struct PlayerInfo
    {
        uint32_t playerId;
        PlayerType type;
        Race requestedRace;
        std::optional<Race> actualRace;
        std::optional<Difficulty> difficulty;
        std::optional<AIBuild> aiBuild;
        std::string playerName;
    };

    struct PlayerResult
    {
        enum class Result {
            Victory = 1,
            Defeat = 2,
            Tie = 3,
            Undecided = 4,
        };

        uint32_t playerId = 0;
        Result result = Result::Undecided;
    };

    struct AvailableAbility
    {
        uint32_t abilityId;
        bool requiresPoint;
    };
}
