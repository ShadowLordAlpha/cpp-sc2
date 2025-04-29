/*! \file sc2_server.h
    \brief A basic websocket server for sc2.
*/

#pragma once

#include <utility>
#include <queue>
#include <vector>
#include <mutex>
#include <memory>

#include <ixwebsocket/IXWebSocketServer.h>

namespace SC2APIProtocol
{
    class Request;
    class Response;
}

namespace sc2
{
    using RequestData = std::pair<ix::WebSocket *, SC2APIProtocol::Request *>;
    using ResponseData = std::pair<ix::WebSocket *, SC2APIProtocol::Response *>;

    class Server
    {
    public:
        Server() = default;

        ~Server();

        bool Listen(int listeningPort, const char *requestTimeoutMs, const char *websocketTimeoutMs,
                    const char *numThreads);

        void QueueRequest(ix::WebSocket *conn, SC2APIProtocol::Request *&request);

        void QueueResponse(ix::WebSocket *conn, SC2APIProtocol::Response *&response);

        // If no connection is provided send it to the first connection attained.
        void SendRequest(ix::WebSocket *conn = nullptr);

        void SendResponse(ix::WebSocket *conn = nullptr);

        bool HasRequest();

        bool HasResponse();

        const RequestData &PeekRequest();

        const ResponseData &PeekResponse();

    private:
        std::unique_ptr<ix::WebSocketServer> webSocketServer;
        std::vector<ix::WebSocket *> clients;

        std::queue<RequestData> requests;
        std::queue<ResponseData> responses;

        std::mutex request_mutex;
        std::mutex response_mutex;
    };
}
