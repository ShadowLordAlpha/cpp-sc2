
#include "sc2api/sc2_server.h"
#include "s2clientprotocol/sc2api.pb.h"

#include <spdlog/spdlog.h>

namespace sc2
{
    template<class T>
    static void SendMessage(ix::WebSocket *client, std::queue<T> &message_queue)
    {
        if (message_queue.empty())
        {
            return;
        }

        google::protobuf::Message *message = message_queue.front().second;

        std::string output;
        message->SerializeToString(&output); // TODO: validate we actually have something to send
        SPDLOG_TRACE("[SERVER] Sending Message: {}\n", output);

        client->send(output, true);

        message_queue.pop();
        delete message;
    }

    Server::~Server()
    {
        webSocketServer->stop();
    }

    bool Server::Listen(int listeningPort, const char *requestTimeoutMs, const char *websocketTimeoutMs, const char *numThreads)
    {
        ix::initNetSystem();
        if (webSocketServer)
        {
            SPDLOG_ERROR("[SERVER] already started");
            return false;
        }

        webSocketServer = std::make_unique<ix::WebSocketServer>(listeningPort);

        webSocketServer->setOnClientMessageCallback(
            [this](const std::shared_ptr<ix::ConnectionState> &connectionState, ix::WebSocket &webSocket, const ix::WebSocketMessagePtr &msg)
            {
                if (msg->type == ix::WebSocketMessageType::Open)
                {
                    SPDLOG_TRACE("[SERVER] Client Connected: {}:{}", connectionState->getRemoteIp(), connectionState->getRemotePort());
                    clients.push_back(&webSocket); // TODO: check why these are actually needed...
                } else if (msg->type == ix::WebSocketMessageType::Message)
                {
                    SPDLOG_TRACE("[SERVER] Message Recieved From: {}:{} - {}", connectionState->getRemoteIp(), connectionState->getRemotePort(), msg->str);

                    auto *request = new SC2APIProtocol::Request();
                    if (!request->ParseFromString(msg->str))
                    {
                        // The server can only receive valid requests off civetweb threads. Otherwise, die.
                        SPDLOG_WARN("[SERVER] Invalid Request: {}", msg->str);
                    }
                    else
                    {
                        QueueRequest(&webSocket, request);
                    }
                } else if (msg->type == ix::WebSocketMessageType::Close)
                {
                    SPDLOG_TRACE("[SERVER] Client Closed: {}:{}", connectionState->getRemoteIp(), connectionState->getRemotePort());
                    clients.erase(std::remove(clients.begin(), clients.end(), &webSocket), clients.end());
                    // TODO: remove from request and responce queues as well?
                }
            });

        if (webSocketServer->listenAndStart())
        {
            SPDLOG_INFO("[SERVER] Server has started on port: {}", webSocketServer->getPort());
            return true;
        }

        SPDLOG_ERROR("[SERVER] Failed to start server");
        return false;
    }

    void Server::QueueRequest(ix::WebSocket *conn, SC2APIProtocol::Request *&request)
    {
        request_mutex.lock();
        requests.emplace(conn, request);
        request_mutex.unlock();
    }

    void Server::QueueResponse(ix::WebSocket *conn, SC2APIProtocol::Response *&response)
    {
        response_mutex.lock();
        responses.emplace(conn, response);
        response_mutex.unlock();
    }

    void Server::SendRequest(ix::WebSocket *conn)
    {
        request_mutex.lock();
        SendMessage(conn ? conn : clients.front(), requests);
        request_mutex.unlock();
    }

    void Server::SendResponse(ix::WebSocket *conn)
    {
        response_mutex.lock();
        SendMessage(conn ? conn : clients.front(), responses);
        response_mutex.unlock();
    }

    bool Server::HasRequest()
    {
        request_mutex.lock();
        bool empty = requests.empty();
        request_mutex.unlock();
        return !empty;
    }

    bool Server::HasResponse()
    {
        response_mutex.lock();
        bool empty = responses.empty();
        response_mutex.unlock();
        return !empty;
    }

    const RequestData &Server::PeekRequest()
    {
        return requests.front();
    }

    const ResponseData &Server::PeekResponse()
    {
        return responses.front();
    }
}
