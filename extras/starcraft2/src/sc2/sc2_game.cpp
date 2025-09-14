//
// Created by Shadow on 8/30/2025.
//

#include "sc2/sc2_game.h"
#include "sc2/sc2_game_state.h"
#include <format>
#include <fstream>
#include <chrono>
#include <thread>
#include <spdlog/spdlog.h>

namespace astra::sc2
{
    std::filesystem::path getDefaultExecuteInfoPath() {
        std::filesystem::path executeInfo;
#ifdef _WIN32
        const char* user_profile = std::getenv("USERPROFILE");
        if (user_profile) {
            executeInfo = std::string(user_profile) + R"(\Documents\StarCraft II\ExecuteInfo.txt)";
        }
        else
        {
            executeInfo = R"(C:\Users\Default\Documents\StarCraft II\ExecuteInfo.txt)";
        }
#elif __APPLE__
        const char* home = std::getenv("HOME");
        if (home) {
            executeInfo = std::string(home) + "/Documents/StarCraft II/ExecuteInfo.txt";
        }
        else
        {
            executeInfo = "/Users/Shared/Documents/StarCraft II/ExecuteInfo.txt";
        }
#else // Linux
        const char* home = std::getenv("HOME");
        if (home) {
            const char* user = std::getenv("USER");
            if (user) {
                return std::string(home) + "/.wine/drive_c/users/" + user + "/Documents/StarCraft II/ExecuteInfo.txt";
            }
        }
        return "/home/user/.wine/drive_c/users/user/Documents/StarCraft II/ExecuteInfo.txt";
#endif

        if(!exists(executeInfo))
        {
            SPDLOG_ERROR(std::format("Unable to find StarCraft II ExecuteInfo.txt: {}", executeInfo.string()));
            return {};
        }

        // Read execute info file for the needed executable.
        std::ifstream inputFile(executeInfo);
        if(!inputFile.is_open())
        {
            SPDLOG_ERROR("Unable to open StarCraft II ExecuteInfo.txt");
            return {};
        }

        std::filesystem::path executable;
        std::string line;
        while(std::getline(inputFile, line))
        {
            if(line.starts_with("executable"))
            {
                executable = line.substr(13);
            }
        }
        inputFile.close();
        SPDLOG_INFO(std::format("Returning StarCraft II Executable: {}", executable.string()));

        return executable;
    }

    StarCraft2Game::~StarCraft2Game() = default;

    bool StarCraft2Game::launchProcess(const ProcessConfig* config)
    {
        // Convert the given ProcessConfig to the needed StarCraft2ProcessConfig
        const StarCraft2ProcessConfig* sc2Config;
        if(config == nullptr) {
            sc2Config = {};
        }
        else
        {
            sc2Config = dynamic_cast<const StarCraft2ProcessConfig*>(config);
        }

        if(!sc2Config)
        {
            SPDLOG_ERROR("Unable to parse Process config");
            return false;
        }

        // Start the actual process
        process->setExecutable(sc2Config->executablePath);
        process->setWorkingDirectory(sc2Config->executablePath.parent_path().parent_path().parent_path() / (sc2Config->executablePath.filename().string().find("_x64")  != std::string::npos ? "Support64" : "Support"));
        std::vector<std::string> arguments = {"-listen", sc2Config->address, "-port", std::to_string(sc2Config->port), "-displayMode", std::to_string(static_cast<int>(sc2Config->displayMode))};
        if(sc2Config->dataVersion.has_value()) {
            arguments.emplace_back("-dataVersion");
            arguments.push_back(sc2Config->dataVersion.value());
        }
        process->setArguments(arguments);
        process->start();

        // Connect the game API to this process. Once this is done we technically no longer need to have the processor connection at this time
        if(gameApi.connect(sc2Config->address, sc2Config->port))
        {
            auto ping = gameApi.ping();
            if(ping)
            {
                SPDLOG_INFO(std::format("StarCraft II Version {} Launched", ping->gameVersion));
                return true;
            }
        }

        return false;
    }

    bool StarCraft2Game::quitProcess()
    {
        gameApi.quit();
        process->stop();
        return true;
    }

    void StarCraft2Game::setupGame(const GameMatchConfig* config, Agent* botAgent)
    {
        auto sc2Config = dynamic_cast<const StarCraft2MatchConfig*>(config);
        if(!sc2Config)
        {
            SPDLOG_ERROR("Unable to parse game config");
            return;
        }

        agent = botAgent;

        if(sc2Config->replayPath.has_value())
        {
            SPDLOG_INFO(std::format("Checking Replay: {}", sc2Config->replayPath->string()));

            if(!sc2Config->replayPath->string().ends_with(".SC2Replay"))
            {
                SPDLOG_WARN("Given Replay was not a .SC2Replay file");
                return;
            }

            // Get the needed info about the replay so we can verify that we are actually able to run it
            auto replayInfo = gameApi.replayInfo(sc2Config->replayPath.value());
            if(!replayInfo)
            {
                SPDLOG_WARN(std::format("Unable to get Replay Info: {} - {}", std::to_string(static_cast<int>(replayInfo.error().error)), replayInfo.error().message));
                return;
            }

            auto ping = gameApi.ping();

            // Check to make sure we are running the correct version of the game
            // TODO: implement checks

            fakeRealtime = sc2Config->realtime;
            fakeRealtimeSpeed = sc2Config->realtimeSpeed;
            endGameLoop = replayInfo->gameDurationLoops;

            // Launch replay
            auto info = gameApi.startReplay(sc2Config->replayPath.value().string(), sc2Config->observedPlayerId, sc2Config->interfaceOptions, sc2Config->disableFog, sc2Config->realtime, sc2Config->recordReplay);
        }
        else
        {

        }
    }

    void StarCraft2Game::joinGame()
    {

    }

    void StarCraft2Game::stopGame()
    {

    }

    void StarCraft2Game::runGame()
    {
        using dseconds = std::chrono::duration<double>;

        // Set the target frame rate
        constexpr double TARGET_FPS = 22.4;
        // Calculate the target frame duration
        const dseconds TARGET_FRAME_DURATION{1.0 / (TARGET_FPS * fakeRealtimeSpeed)};

        agent->setGame(this);
        agent->onGameStart();
        uint32_t previousStep = 0;
        uint32_t currentStep = 0;
        auto last_frame_start = std::chrono::steady_clock::now();
        while(currentStep >= previousStep && currentStep != endGameLoop)
        {
            auto current_frame_start = std::chrono::steady_clock::now();

            // Calculate the time elapsed since the last frame began
            dseconds elapsed_time = current_frame_start - last_frame_start;

            if(fakeRealtime && elapsed_time < TARGET_FRAME_DURATION)
            {
                // Calculate the remaining time to sleep
                auto sleep_duration = TARGET_FRAME_DURATION - elapsed_time;

                // Sleep until the next frame should begin
                std::this_thread::sleep_for(sleep_duration);
            }

            // Record the start time for the next frame
            last_frame_start = std::chrono::steady_clock::now();

            previousStep = currentStep;
            auto step = gameApi.step();
            if(step)
            {
                currentStep = *step;
                agent->onStep();
            }
        }
        agent->onGameEnd();
        gameApi.leaveGame();
        agent->setGame(nullptr);
    }

    std::shared_ptr<GameState> StarCraft2Game::gameState()
    {
        auto out = std::make_shared<StarCraft2GameState>();
        out->observation = *gameApi.observation();

        return out;
    }

    void StarCraft2Game::sendActions(const std::vector<Action*>& actions)
    {
        // Create vectors internally
        std::vector<ObserverAction*> observerActions;
        std::vector<DebugAction*> debugActions;
        std::vector<Action*> otherActions;

        // Reserve space to avoid reallocations
        observerActions.reserve(actions.size());
        debugActions.reserve(actions.size());
        otherActions.reserve(actions.size());

        // Iterate through input vector
        for (auto action : actions) {
            if (auto* observer = dynamic_cast<ObserverAction*>(action)) {
                observerActions.push_back(observer);
            } else if (auto* debug = dynamic_cast<DebugAction*>(action)) {
                debugActions.push_back(debug);
            } else {
                otherActions.push_back(action);
            }
        }

        // Process each group of actions, Observer Actions and other actions should not exist in the same group, though we don't actually restrict them currently
        if(!observerActions.empty())
        {
            gameApi.obsAction(observerActions);
        }
        if(!otherActions.empty())
        {
            gameApi.action(otherActions);
        }
        if(!debugActions.empty())
        {
            gameApi.debug(debugActions);
        }
    }
}
