/**
 *
 */

#pragma once

#include <filesystem>
#include <glm/vec2.hpp>
#include "astra/agent.h"
#include "astra/game.h"
#include "astra/process.h"
#include "sc2/api/sc2_api.h"

namespace astra::sc2
{
    std::filesystem::path getDefaultExecuteInfoPath();

    struct StarCraft2ProcessConfig : public astra::ProcessConfig
    {
        enum class DisplayMode
        {
            Windowed = 0,
            Fullscreen = 1,
        };

        std::filesystem::path executablePath = getDefaultExecuteInfoPath();
        std::string address = "127.0.0.1";
        uint32_t port = 5679;
        DisplayMode displayMode = DisplayMode::Windowed;
        std::optional<glm::ivec2> screenPosition;
        std::optional<glm::ivec2> screenSize;
        std::optional<std::string> dataVersion;
        std::optional<std::filesystem::path> dataDirectory;
        std::optional<std::filesystem::path> tempDirectory;
        std::optional<bool> verbose;
        std::optional<std::filesystem::path> eglPath;
        std::optional<std::filesystem::path> osmesaPath;
    };

    struct StarCraft2MatchConfig : public GameMatchConfig
    {
        // Common Settings

        // Game-specific settings

        // Replay specific settings
        std::optional<std::filesystem::path> replayPath;
        // TODO: replay data instead of path?
        // TODO: map data instead of using the replays map path
        uint32_t observedPlayerId = 0;
        InterfaceOptions interfaceOptions;
        bool disableFog = false;
        bool realtime = true;
        bool recordReplay = false;
        float realtimeSpeed = 2.0;
    };

    class StarCraft2Game : public astra::Game
    {
    public:
        ~StarCraft2Game() override;

        bool launchProcess(const ProcessConfig* config = nullptr) override;
        bool quitProcess() override;

        void setupGame(const GameMatchConfig* config, Agent* botAgent = nullptr) override;
        void joinGame() override;
        void stopGame() override;
        void runGame() override;

        std::shared_ptr<GameState> gameState() override;
        void sendActions(const std::vector<Action*>& actions) override;

    private:
        StarCraft2Api gameApi;

        std::unique_ptr<process::Process> process = std::make_unique<process::Process>();
        Agent* agent;
        uint32_t endGameLoop; //< We will always break out of the game loop at this point, even if the game technically has not ended. There is no way to disable this and we will set to 1 hour (80640) for a game or the length of a replay
        float fakeRealtimeSpeed = 10; // The speed to fake realtime at. 1 = 22.4 steps per second or as close as we can get
        bool fakeRealtime; // This is specifically for replay as replay does not support realtime
    };
}
