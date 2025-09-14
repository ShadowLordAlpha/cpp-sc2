/**
 *
 */

#pragma once

#include <vector>
#include <memory>
#include "action.h"
#include "astra/game.h"

namespace astra
{
    class Agent
    {
    public:
        virtual ~Agent() = default;

        virtual void onGameStart() {}
        virtual void onGameEnd() {}

        virtual void onStep() {}

        template <class T>
        std::shared_ptr<T> gameState()
        {
            return std::dynamic_pointer_cast<T>(game->gameState());
        }

        void sendActions(const std::vector<Action*>& actions)
        {
            game->sendActions(actions);
        }

        void setGame(Game* activeGame)
        {
            game = activeGame;
        }

    private:
        Game* game = nullptr;
    };
}
