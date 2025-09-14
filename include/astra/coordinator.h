/**
 *
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <filesystem>
#include "game.h"

namespace astra
{
    class Agent;

    /**
     * @class Coordinator
     * @brief
     *
     */
    class Coordinator
    {
    public:
        Coordinator() = default;
        ~Coordinator() = default;

        void loadSettings(int argc, char *argv[])
        {

        }

        template<typename T>
        bool launchGame(const ProcessConfig* config = nullptr)
        {
            // TODO: automatically launch a game per participant? for(auto participant: participants)
            game = std::make_shared<T>();
            return game->launchProcess(config);
        }

        bool setupGame(const GameMatchConfig* config, Agent* agent)
        {
            // Only one game can create the game
            game->setupGame(config, agent);
            return true;
        }

        bool joinGame()
        {
            // All games must join
            game->joinGame();
            return true;
        }

        void run()
        {
            // Start ever game running and then wait for them all to join back together, possibly with a time limit
            game->runGame();
        }

    private:
        std::shared_ptr<Game> game;
    };
}
