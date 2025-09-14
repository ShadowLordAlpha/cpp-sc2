/**
 *
 */

#pragma once

#include <string>
#include <memory>
#include <functional>

namespace ix
{
    class WebSocket;
}

namespace astra::network
{
    class WebSocket
    {
    public:
        using MessageCallback = std::function<void(int type, const std::string& message)>;

        WebSocket();
        ~WebSocket();

        void setUrl(const std::string& url);
        void setMessageCallback(const MessageCallback& callback);

        bool connect();
        bool send(const std::string& message);
        void close();

    private:
        std::string url;
        std::unique_ptr<ix::WebSocket> webSocket;
        MessageCallback messageCallback;
    };
}
