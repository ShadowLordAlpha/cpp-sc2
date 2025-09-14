/**
 *
 */

#include <format>
#include "astra/websocket.h"
#include "IXNetSystem.h"
#include <spdlog/spdlog.h>
#include <future>
#include <ixwebsocket/IXWebSocket.h>

namespace astra::network
{
    WebSocket::WebSocket() : webSocket(std::make_unique<ix::WebSocket>()) {};

    WebSocket::~WebSocket()
    {
        close();
    }

    void WebSocket::setUrl(const std::string& urlConnect)
    {
        url = urlConnect;
    }

    void WebSocket::setMessageCallback(const MessageCallback& callback)
    {
        messageCallback = callback;
    }

    bool WebSocket::connect()
    {
        ix::initNetSystem(); // Required on Windows

        auto promise = std::promise<bool>();
        auto future = promise.get_future();

        webSocket->setUrl(url);
        webSocket->setOnMessageCallback([this, &promise](const ix::WebSocketMessagePtr& message) {
            if(message->type == ix::WebSocketMessageType::Message)
            {
                this->messageCallback(static_cast<int>(message->type), message->str);
            }
            else if(message->type == ix::WebSocketMessageType::Open)
            {
                SPDLOG_INFO("Open");
                this->messageCallback(static_cast<int>(message->type), "Open");
                promise.set_value(true);
            }
            else if(message->type == ix::WebSocketMessageType::Close)
            {
                SPDLOG_INFO("Close");
                this->messageCallback(static_cast<int>(message->type), "Close");
            }
            else if(message->type == ix::WebSocketMessageType::Error)
            {
                SPDLOG_INFO("Error");
                this->messageCallback(static_cast<int>(message->type), message->errorInfo.reason);
            }
            else if(message->type == ix::WebSocketMessageType::Ping)
            {
                this->messageCallback(static_cast<int>(message->type), "Ping");
            }
            else if(message->type == ix::WebSocketMessageType::Pong)
            {
                this->messageCallback(static_cast<int>(message->type), "Pong");
            }
            else
            {
                SPDLOG_INFO("Other");
            }
        });

        webSocket->setHandshakeTimeout(120);
        webSocket->enableAutomaticReconnection();
        webSocket->disablePerMessageDeflate();

        webSocket->start();

        future.get(); // TODO: have a timeout?
        return true;
    }

    bool WebSocket::send(const std::string& message)
    {
        auto sendInfo = webSocket->send(message, true);
        return sendInfo.success;
    }

    void WebSocket::close()
    {
        webSocket->stop();
        ix::uninitNetSystem();
    }
}
