/**
 *
 */

#pragma once

#include <string>
#include <optional>
#include <glm/vec2.hpp>
#include "sc2/api/common.h"

namespace astra::sc2
{
    enum class Status {
        launched = 1,                                 // Game has been launch and is not yet doing anything.
        init_game = 2,                                // Create game has been called, and the host is awaiting players.
        in_game = 3,                                  // In a single or multiplayer game.
        in_replay = 4,                                // In a replay.
        ended = 5,                                    // Game has ended, can still request game info, but ready for a new game.
        quit = 6,                                     // Application is shutting down.
        unknown = 99,                                 // Should not happen, but indicates an error if it occurs.
    };

    struct PlayerSetup
    {
        PlayerType type = PlayerType::Participant;

        // None of these are needed for anything not a computer and are ignored
        std::optional<Race> race;
        std::optional<std::string> name;
        std::optional<Difficulty> difficulty;
        std::optional<AIBuild> build;
    };

    struct SpatialCameraSetup
    {
        Point2I resolution;
        Point2I minimapResolution;

        std::optional<float> width;
        std::optional<bool> cropToPlayableArea;
        std::optional<bool> allowCheatingLayers;
    };

    struct InterfaceOptions
    {
        bool raw = true;
        bool score;
        std::optional<SpatialCameraSetup> featureLayer;
        std::optional<SpatialCameraSetup> render;
        bool showCloaked = true;
        bool showBurrowedShadows = true;
        bool showPlaceholders = true;
        bool rawAffectsSelections;
        bool rawCropToPlayableArea;
    };

    struct PortSet
    {
        int gamePort;
        int basePort;
    };
}
