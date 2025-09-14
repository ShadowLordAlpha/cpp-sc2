/**
 * @file gameState.cpp
 * @brief Example implementation of a replay observer in the Astra library.
 *
 * This example demonstrates how to create a custom replay observer that follows
 * a specific unit in a StarCraft II replay. The observer tracks the unit's position
 * and updates the camera accordingly until the unit dies. Upon the unit's death,
 * the observer switches to following another specified unit.
 *
 * Key features:
 * - Implements the ReplayObserver interface from the SC2 API.
 * - Handles unit tracking and camera control.
 * - Switches targets dynamically when the current unit is destroyed.
 * - Allows input after the replay is done for another to be started without needing to relaunch the game.
 *
 * Usage:
 * - Compile and run this example with a StarCraft II replay file.
 * - Specify the units to track via command-line arguments or configuration.
 *
 * @author ShadowLordAlpha
 * @date August 23, 2025
 * @version 1.0
 * @license MIT License
 */

#include <cstdlib>
#include <iostream>
#include <ranges>
#include <random>
#include <vector>

#include "astra.h"
#include "starcraft2.h"

class ObserverAgent : public astra::Agent
{
private:
    uint64_t followUnit = 0;

public:
    void onGameStart() final
    {
        std::cout << "Replay observer started." << std::endl;
    }

    void onGameEnd() final
    {
        std::cout << "Replay observer ended." << std::endl;
    }

    void onStep() final
    {
        std::vector<astra::Action*> actions;

        auto observation = gameState<astra::sc2::StarCraft2GameState>();

        // Get a random unit from the observation
        if(observation->observation.observation.rawData)
        {
            auto raw = *observation->observation.observation.rawData;

            if(followUnit == 0)
            {
                // Seed the random number generator
                std::mt19937 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());

                // Filter Units to only units that are one of the 3 races
                auto filteredUnits = raw.units
                        | std::views::filter([](const auto& unit) {
                            return unit.owner != 16;
                        })
                        | std::ranges::to<std::vector>();

                std::uniform_int_distribution<size_t> dist(0, filteredUnits.size() - 1);
                size_t randomIndex = dist(rng);

                const auto& unit = filteredUnits[randomIndex];
                followUnit = unit.tag;
            }

            if(followUnit > 0)
            {
                //std::cout << "Following unit: " << followUnit << std::endl;

                auto it = std::find_if(raw.units.begin(), raw.units.end(), [&](const auto& unit) { return unit.tag == followUnit; });

                if(it != raw.units.end())
                {
                    auto movement = astra::sc2::ObserverAction::moveCamera(it->pos);
                    actions.push_back(&movement);

                    sendActions(actions);
                }
                else
                {
                    std::cout << "Unit not found." << std::endl;
                    followUnit = 0;
                }

            }
        }
    }
};

int main(int argc, char* argv[])
{
    astra::Coordinator coordinator;
    coordinator.loadSettings(argc, argv);

    astra::sc2::StarCraft2ProcessConfig launchConfig;
    launchConfig.dataVersion = "B89B5D6FA7CBF6452E721311BFBC6CB2";
    coordinator.launchGame<astra::sc2::StarCraft2Game>(&launchConfig);

    std::string input; // = R"(C:\Users\Shadow\Downloads\4132129_BenBotBC_Caninana_TorchesAIE_v3.SC2Replay)"
    std::cout << "Enter replay path or exit: ";
    while(std::getline(std::cin, input) && input != "exit")
    {
        ObserverAgent agent;
        astra::sc2::StarCraft2MatchConfig matchConfig;
        matchConfig.realtimeSpeed = 5;
        matchConfig.replayPath = input;
        matchConfig.realtime = true;
        coordinator.setupGame(&matchConfig, &agent);

        coordinator.run();
        std::cout << "Enter replay path or exit: ";
    }

    return EXIT_SUCCESS;
}
