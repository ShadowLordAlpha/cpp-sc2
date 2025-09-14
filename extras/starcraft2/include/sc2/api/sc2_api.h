/**
 *
 */

#pragma once

#include <string>
#include <future>
#include <expected>
#include "astra/websocket.h"

#include "sc2/actions/debug_actions.h"
#include "sc2/actions/observer_actions.h"

#include "sc2/api/request/setup_requests.h"

#include "sc2/api/response/setup_responses.h"
#include "sc2/api/response/game_responses.h"
#include "sc2/api/response/other_responses.h"
#include "sc2/api/response/error_responses.h"
#include "sc2/api/response/debug_responses.h"

namespace SC2APIProtocol
{
    class Request;
    class Response;
}

namespace astra::sc2
{
    class StarCraft2Api
    {
    public:
        StarCraft2Api();
        ~StarCraft2Api();

        StarCraft2Api(const StarCraft2Api&) = delete;
        StarCraft2Api& operator=(const StarCraft2Api&) = delete;
        StarCraft2Api(StarCraft2Api&&) = delete;
        StarCraft2Api& operator=(StarCraft2Api&&) = delete;

        bool connect(const std::string& address, uint32_t port);
        void disconnect();

        // Game Setup ==================================================================================================
        std::expected<void, ErrorInfo> createGame(std::variant<std::string, char*> map, std::vector<PlayerSetup> playerSetup, bool disableFog = false, std::optional<uint32_t> randomSeed = std::nullopt, bool realtime = false);
        std::expected<uint32_t, ErrorInfo> joinGame(std::variant<Race, uint32_t> participation, InterfaceOptions options, std::optional<PortSet> serverPorts, std::optional<std::vector<PortSet>> clientPorts, int sharedPort, std::optional<std::string_view> playerName, std::string hostIp = "localhost");
        std::expected<bool, ErrorInfo> restartGame();
        std::expected<void, ErrorInfo> startReplay(std::variant<std::string, char*> replay, uint32_t observerPlayerId, InterfaceOptions options, bool disableFog = false, bool realtime = false, bool recordReplay = false);
        std::expected<void, ErrorInfo> leaveGame();

        std::expected<void, ErrorInfo> quickSave();
        std::expected<void, ErrorInfo> quickLoad();

        std::expected<void, ErrorInfo> quit();

        // In Progress
        // During Game =================================================================================================
        std::expected<GameInfoResponse, ErrorInfo> gameInfo();
        std::expected<ObservationResponse, ErrorInfo> observation(bool disableFog = false, uint32_t gameLoop = 0);
        std::expected<std::vector<ActionResult>, ErrorInfo> action(const std::vector<Action*>& actions);
        std::expected<void, ErrorInfo> obsAction(const std::vector<ObserverAction*>& actions);
        std::expected<uint32_t, ErrorInfo> step(uint32_t count = 1);
        std::expected<DataResponse, ErrorInfo> data(bool abilityData, bool unitTypeData, bool upgradeData, bool buffData, bool effectData);
        std::expected<QueryResponse, ErrorInfo> query(const std::vector<QueryPathing>& pathing, const std::vector<uint64_t>& abilities, const std::vector<QueryPlacement>& placements, bool ignoreResourceRequirements);
        std::expected<std::string, ErrorInfo> saveReplay();
        std::expected<void, ErrorInfo> mapCommand(const std::string& command);

        // Other =======================================================================================================
        std::expected<ReplayInfoResponse, ErrorInfo> replayInfo(std::variant<std::filesystem::path, char*> replay, bool downloadData = true);
        std::expected<std::vector<std::string>, ErrorInfo> availableMaps();
        std::expected<void, ErrorInfo> saveMap(std::filesystem::path savePath, char* mapData = nullptr);

        // Debugging ===================================================================================================
        std::expected<PingResponse, ErrorInfo> ping();
        std::expected<void, ErrorInfo> debug(const std::vector<DebugAction*>& actions);

    protected:
        /**
         * @brief Send a request and do not wait for the response.
         * @param request
         */
        void sendAndIgnore(const SC2APIProtocol::Request& request);

        /**
         * @brief Send a request and wait for a response with timeout and error handling.
         * Returns std::expected with either the response or ErrorInfo on failure.
         */
        std::expected<SC2APIProtocol::Response, ErrorInfo> sendAwait(SC2APIProtocol::Request& request, std::chrono::milliseconds timeout);

    private:
        void onMessage(int type, const std::string& message);
        void failAllPending(const ErrorInfo& error);

        std::atomic<uint32_t> packageId = 0; //< Incrementing ID for each network request we make.
        std::mutex mutex; //< Mutex for synchronizing access to the waitingResponses.
        std::unordered_map<uint32_t, std::promise<SC2APIProtocol::Response>> waitingResponses; //< Map of request IDs to promises for syncing responses.
        Status status = Status::unknown;

        std::atomic<bool> connected{false};
        std::atomic<bool> alive{false};
        std::chrono::milliseconds defaultTimeout{std::chrono::seconds(120)}; // configurable default

        astra::network::WebSocket webSocket; //< WebSocket for sending and receiving messages.
    };
}
