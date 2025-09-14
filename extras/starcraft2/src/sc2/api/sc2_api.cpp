/**
 *
 */

#include "sc2/api/sc2_api.h"
#include "s2clientprotocol/sc2api.pb.h"
#include <spdlog/spdlog.h>
#include <format>
#include "sc2/conversions/enums.h"
#include "sc2/conversions/request_builders.h"
#include "sc2/conversions/game_conversions.h"
#include "sc2/conversions/other_conversions.h"
#include "sc2/conversions/debug_conversions.h"

namespace astra::sc2
{

    StarCraft2Api::StarCraft2Api() = default;
    StarCraft2Api::~StarCraft2Api()
    {
        disconnect();
    }

    bool StarCraft2Api::connect(const std::string& address, uint32_t port)
    {
        SPDLOG_INFO(std::format("Starting Connection to: ws://{}:{}/sc2api", address, port));
        webSocket.setUrl(std::format("ws://{}:{}/sc2api", address, port));

        webSocket.setMessageCallback([this](int type, const std::string& message) {
            onMessage(type, message);
        });

        const bool webSocketConnected = webSocket.connect();
        connected.store(webSocketConnected, std::memory_order_relaxed);
        alive.store(webSocketConnected, std::memory_order_relaxed);
        return connected;
    }

    void StarCraft2Api::disconnect()
    {
        if(!connected.exchange(false))
        {
            webSocket.close();
            alive.store(false, std::memory_order_relaxed);
            return;
        }

        // Mark not alive first to avoid racing callbacks
        alive.store(false, std::memory_order_relaxed);

        // Close socket; no more callbacks after this returns
        webSocket.close();

        // Fail all pending promises so callers don't hang
        failAllPending({ErrorInfo::Error::TransportError, "Disconnected"});
    }

    void StarCraft2Api::onMessage(int type, const std::string &message)
    {
        if (!alive.load(std::memory_order_relaxed)) {
            return;
        }

        // Type 0 is a text or binary message. TODO we may want to handle close messages as well here
        if (type != 0) {
            return;
        }

        // Parse the message into a proper protobuf object
        SC2APIProtocol::Response response;
        if (!response.ParseFromString(message))
        {
            SPDLOG_WARN(std::format("Invalid Response (parse error), bytes={}", message.size()));
            return;
        }

        // Set the status the game is currentlt in. TODO: maybe use conversion here instead of static cast?
        status = static_cast<Status>(response.status());

        const auto id = response.id();
        if (id == 0) {
            // Unsolicited response; currently ignored or could be published to observers.
            SPDLOG_TRACE("Unsolicited SC2API response received (id=0)");
            return;
        }

        std::promise<SC2APIProtocol::Response> prom;
        {
            std::lock_guard<std::mutex> lock(this->mutex);
            auto it = this->waitingResponses.find(id);
            if (it == this->waitingResponses.end()) {
                SPDLOG_WARN(std::format("No pending waiter for SC2API response id={}", id));
                return;
            }
            prom = std::move(it->second);
            this->waitingResponses.erase(it);
        }

        try {
            prom.set_value(std::move(response));
        } catch (const std::exception& e) {
            SPDLOG_WARN(std::format("Failed to deliver response to waiter id={}, error={}", id, e.what()));
        }
    }

    void StarCraft2Api::sendAndIgnore(const SC2APIProtocol::Request& request)
    {
        // Best-effort fire-and-forget. If send fails, we just log.
        const auto ok = webSocket.send(request.SerializeAsString());
        if (!ok) {
            SPDLOG_WARN("WebSocket send failed (sendAndIgnore)");
        }
    }

    std::expected<SC2APIProtocol::Response, ErrorInfo> StarCraft2Api::sendAwait(SC2APIProtocol::Request& request, std::chrono::milliseconds timeout)
    {
        uint32_t requestId = ++packageId;
        request.set_id(requestId);

        std::promise<SC2APIProtocol::Response> prom;
        auto fut = prom.get_future();

        {
            std::lock_guard<std::mutex> lock(mutex);
            waitingResponses.emplace(requestId, std::move(prom));
        }

        const auto payload = request.SerializeAsString();
        if (!webSocket.send(payload)) {
            // Clean up the waiter and return an error
            std::promise<SC2APIProtocol::Response> orphan;
            {
                std::lock_guard<std::mutex> lock(mutex);
                auto it = waitingResponses.find(requestId);
                if (it != waitingResponses.end()) {
                    orphan = std::move(it->second);
                    waitingResponses.erase(it);
                }
            }
            try { orphan.set_exception(std::make_exception_ptr(std::runtime_error("WebSocket send failed"))); } catch (...) {}
            return std::unexpected(ErrorInfo{ErrorInfo::Error::TransportError, "WebSocket send failed"});
        }

        if (fut.wait_for(timeout) != std::future_status::ready) {
            // Timeout: remove waiter and return an error
            std::promise<SC2APIProtocol::Response> orphan;
            {
                std::lock_guard<std::mutex> lock(mutex);
                auto it = waitingResponses.find(requestId);
                if (it != waitingResponses.end()) {
                    orphan = std::move(it->second);
                    waitingResponses.erase(it);
                }
            }
            try { orphan.set_exception(std::make_exception_ptr(std::runtime_error("Timed out waiting for SC2 response"))); } catch (...) {}
            return std::unexpected(ErrorInfo{ErrorInfo::Error::Timeout, "Timed out waiting for SC2 response"});
        }

        try {
            return fut.get();
        } catch (const std::exception& e) {
            return std::unexpected(ErrorInfo{ErrorInfo::Error::TransportError, e.what()});
        }
    }

    void StarCraft2Api::failAllPending(const ErrorInfo& error)
    {
        std::unordered_map<uint32_t, std::promise<SC2APIProtocol::Response>> pending;
        {
            std::lock_guard<std::mutex> lock(mutex);
            pending.swap(waitingResponses);
        }

        for (auto& [id, prom] : pending) {
            try {
                prom.set_exception(std::make_exception_ptr(std::runtime_error(error.message)));
            } catch (...) {
                // ignore
            }
        }
    }

    // Game Setup ======================================================================================================
    std::expected<void, ErrorInfo> StarCraft2Api::createGame(std::variant<std::string, char*> map, std::vector<PlayerSetup> playerSetup, bool disableFog, std::optional<uint32_t> randomSeed, bool realtime)
    {
        SC2APIProtocol::Request request;
        fillCreateGameRequest(&request, map, playerSetup, disableFog, randomSeed, realtime);

        auto responseOr = sendAwait(request, defaultTimeout);
        if(!responseOr) return std::unexpected(responseOr.error());

        auto& createGame = responseOr->create_game();
        if(createGame.has_error()) return std::unexpected(ErrorInfo{toDomain(createGame.error()), createGame.error_details()});

        return {};
    }

    std::expected<uint32_t, ErrorInfo> StarCraft2Api::joinGame(std::variant<Race, uint32_t> participation, InterfaceOptions options, std::optional<PortSet> serverPorts, std::optional<std::vector<PortSet>> clientPorts, int sharedPort, std::optional<std::string_view> playerName, std::string hostIp)
    {
        SC2APIProtocol::Request request;
        fillJoinGameRequest(&request, participation, options, serverPorts, clientPorts, sharedPort, playerName, hostIp);

        auto responseOr = sendAwait(request, defaultTimeout);
        if(!responseOr) return std::unexpected(responseOr.error());

        auto& joinGame = responseOr->join_game();
        if(joinGame.has_error()) return std::unexpected(ErrorInfo{toDomain(joinGame.error()), joinGame.error_details()});

        return joinGame.player_id();
    }

    std::expected<bool, ErrorInfo> StarCraft2Api::restartGame()
    {
        SC2APIProtocol::Request request;
        request.mutable_restart_game();

        auto responseOr = sendAwait(request, defaultTimeout);
        if(!responseOr) return std::unexpected(responseOr.error());

        auto& restartGame = responseOr->restart_game();
        if(restartGame.has_error()) return std::unexpected(ErrorInfo{toDomain(restartGame.error()), restartGame.error_details()});

        return restartGame.need_hard_reset();
    }

    std::expected<void, ErrorInfo> StarCraft2Api::startReplay(std::variant<std::string, char*> replay, uint32_t observerPlayerId, InterfaceOptions options, bool disableFog, bool realtime, bool recordReplay)
    {
        SC2APIProtocol::Request request;
        fillStartReplayRequest(&request, replay, observerPlayerId, options, disableFog, realtime, recordReplay);

        auto responseOr = sendAwait(request, defaultTimeout);
        if(!responseOr) return std::unexpected(responseOr.error());

        auto& startReplay = responseOr->start_replay();
        if(startReplay.has_error()) return std::unexpected(ErrorInfo{toDomain(startReplay.error()), startReplay.error_details()});

        return {};
    }

    std::expected<void, ErrorInfo> StarCraft2Api::leaveGame()
    {
        SC2APIProtocol::Request request;
        request.mutable_leave_game();

        auto responseOr = sendAwait(request, defaultTimeout);
        if(!responseOr) return std::unexpected(responseOr.error());

        return {};
    }

    std::expected<void, ErrorInfo> StarCraft2Api::quickSave()
    {
        SC2APIProtocol::Request request;
        request.mutable_quick_save();

        auto responseOr = sendAwait(request, defaultTimeout);
        if(!responseOr) return std::unexpected(responseOr.error());

        return {};
    }

    std::expected<void, ErrorInfo> StarCraft2Api::quickLoad()
    {
        SC2APIProtocol::Request request;
        request.mutable_quick_load();

        auto responseOr = sendAwait(request, defaultTimeout);
        if(!responseOr) return std::unexpected(responseOr.error());

        return {};
    }

    std::expected<void, ErrorInfo> StarCraft2Api::quit()
    {
        SC2APIProtocol::Request request;
        request.mutable_quit();

        auto responseOr = sendAwait(request, defaultTimeout);
        if(!responseOr) return std::unexpected(responseOr.error());

        return {};
    }

    // During Game =====================================================================================================
    std::expected<GameInfoResponse, ErrorInfo> StarCraft2Api::gameInfo()
    {
        SC2APIProtocol::Request request;
        request.mutable_game_info();

        auto responseOr = sendAwait(request, defaultTimeout);
        if(!responseOr) return std::unexpected(responseOr.error());

        return toDomain(responseOr->game_info());
    }

    std::expected<ObservationResponse, ErrorInfo> StarCraft2Api::observation(bool disableFog, uint32_t gameLoop)
    {
        SC2APIProtocol::Request request;
        fillObservationRequest(&request, disableFog, gameLoop);

        auto responseOr = sendAwait(request, defaultTimeout);
        if(!responseOr) return std::unexpected(responseOr.error());

        return toDomain(responseOr->observation());
    }

    std::expected<std::vector<ActionResult>, ErrorInfo> StarCraft2Api::action(const std::vector<Action*>& actions)
    {
        SC2APIProtocol::Request request;
        fillActionRequest(&request, actions);

        auto responseOr = sendAwait(request, defaultTimeout);
        if(!responseOr) return std::unexpected(responseOr.error());

        auto& actionResponse = responseOr->action();

        std::vector<ActionResult> out;
        out.reserve(actionResponse.result_size());
        for(int i = 0; i < actionResponse.result_size(); ++i)
        {
            out.push_back(toDomain(actionResponse.result(i)));
        }

        return out;
    }

    std::expected<void, ErrorInfo> StarCraft2Api::obsAction(const std::vector<ObserverAction*>& actions)
    {
        SC2APIProtocol::Request request;
        fillObsActionRequest(&request, actions);

        sendAndIgnore(request);
        // TODO: probably want to check for an error with just itself

        return {};
    }

    std::expected<uint32_t, ErrorInfo> StarCraft2Api::step(uint32_t count)
    {
        SC2APIProtocol::Request request;
        fillStepRequest(&request, count);

        auto responseOr = sendAwait(request, defaultTimeout);
        if(!responseOr) return std::unexpected(responseOr.error());

        auto& stepResponse = responseOr->step();
        return stepResponse.simulation_loop();
    }

    std::expected<DataResponse, ErrorInfo> StarCraft2Api::data(bool abilityData, bool unitTypeData, bool upgradeData, bool buffData, bool effectData)
    {
        SC2APIProtocol::Request request;
        fillDataRequest(&request, abilityData, unitTypeData, upgradeData, buffData, effectData);

        auto responseOr = sendAwait(request, defaultTimeout);
        if(!responseOr) return std::unexpected(responseOr.error());

        return toDomain(responseOr->data());
    }

    std::expected<QueryResponse, ErrorInfo> StarCraft2Api::query(const std::vector<QueryPathing>& pathing, const std::vector<uint64_t>& abilities, const std::vector<QueryPlacement>& placements, bool ignoreResourceRequirements)
    {
        SC2APIProtocol::Request request;
        fillQueryRequest(&request, pathing, abilities, placements, ignoreResourceRequirements);

        auto responseOr = sendAwait(request, defaultTimeout);
        if(!responseOr) return std::unexpected(responseOr.error());

        return toDomain(responseOr->query());
    }

    std::expected<std::string, ErrorInfo> StarCraft2Api::saveReplay()
    {
        SC2APIProtocol::Request request;
        request.mutable_save_replay();

        auto responseOr = sendAwait(request, defaultTimeout);
        if(!responseOr) return std::unexpected(responseOr.error());

        return responseOr->save_replay().data();
    }

    std::expected<void, ErrorInfo> StarCraft2Api::mapCommand(const std::string& command)
    {
        SC2APIProtocol::Request request;
        fillMapCommandRequest(&request, command);

        auto responseOr = sendAwait(request, defaultTimeout);
        if(!responseOr) return std::unexpected(responseOr.error());

        auto& mapCommand = responseOr->map_command();
        if(mapCommand.has_error()) return std::unexpected(ErrorInfo{toDomain(mapCommand.error()), mapCommand.error_details()});

        return {};
    }

    // Other ===========================================================================================================
    std::expected<ReplayInfoResponse, ErrorInfo> StarCraft2Api::replayInfo(std::variant<std::filesystem::path, char*> replay, bool downloadData)
    {
        SC2APIProtocol::Request request;
        fillReplayInfoRequest(&request, replay, downloadData);

        auto responseOr = sendAwait(request, defaultTimeout);
        if(!responseOr) return std::unexpected(responseOr.error());

        auto& replayInfo = responseOr->replay_info();
        if(replayInfo.has_error()) return std::unexpected(ErrorInfo{toDomain(replayInfo.error()), replayInfo.error_details()});

        return toDomain(replayInfo);
    }

    std::expected<std::vector<std::string>, ErrorInfo> StarCraft2Api::availableMaps()
    {
        SC2APIProtocol::Request request;
        request.mutable_available_maps();

        auto responseOr = sendAwait(request, defaultTimeout);
        if(!responseOr) return std::unexpected(responseOr.error());

        return toDomain(responseOr->available_maps());
    }

    std::expected<void, ErrorInfo> StarCraft2Api::saveMap(std::filesystem::path savePath, char* mapData)
    {
        SC2APIProtocol::Request request;
        fillSaveMapRequest(&request, savePath, mapData);

        auto responseOr = sendAwait(request, defaultTimeout);
        if(!responseOr) return std::unexpected(responseOr.error());

        auto& saveMap = responseOr->save_map();
        if(saveMap.has_error()) return std::unexpected(ErrorInfo{toDomain(saveMap.error()), "Invalid Map Data"});

        return {};
    }

    // Debugging =======================================================================================================
    std::expected<PingResponse, ErrorInfo> StarCraft2Api::ping()
    {
        SC2APIProtocol::Request request;
        request.mutable_ping();

        auto responseOr = sendAwait(request, defaultTimeout);
        if(!responseOr) return std::unexpected(responseOr.error());

        return toDomain(responseOr->ping());
    }

    std::expected<void, ErrorInfo> StarCraft2Api::debug(const std::vector<DebugAction*>& actions)
    {
        SC2APIProtocol::Request request;
        fillDebugRequest(&request, actions);

        sendAndIgnore(request);
        // TODO: check for ErrorInfo here, we still don't need to wait for a response

        return {};
    }
}
