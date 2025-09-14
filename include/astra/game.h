/**
 *
 */

#pragma once

#include "action.h"
#include "game_state.h"
#include <vector>
#include <memory>

namespace astra
{
    class Agent;

    struct ProcessConfig
    {
        virtual ~ProcessConfig() = default;
    };

    struct GameMatchConfig
    {
        virtual ~GameMatchConfig() = default;
    };

    /**
     * @class Game
     * @brief Base class for all games.
     *
     */
    class Game
    {
    public:
        virtual ~Game() = default;

        /**
         * @brief Launches the needed process.
         */
        virtual bool launchProcess(const ProcessConfig* processConfig) = 0;

        /**
         * @brief Closes the game executable if needed.
         */
        virtual bool quitProcess() = 0;

        /**
         * @brief Start a game with settings.
         */
        virtual void setupGame(const GameMatchConfig* config, Agent* botAgent = nullptr) = 0;

        /**
         * @brief Join an existing game.
         */
        virtual void joinGame() = 0;

        /**
         * @brief Run through a game loop as needed
         */
        virtual void runGame() = 0;

        /**
         * @brief Stop the current ongoing game.
         */
        virtual void stopGame() = 0;

        virtual std::shared_ptr<GameState> gameState() = 0;
        virtual void sendActions(const std::vector<Action*>& actions) = 0;
    };
}
