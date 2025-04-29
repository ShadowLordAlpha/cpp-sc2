#include "sc2api/sc2_connection.h"

#include <iostream>
#include <cassert>
#include <chrono>
#include <format>
#include <IXNetSystem.h>
#include <string>
#include <spdlog/spdlog.h>

#include "s2clientprotocol/sc2api.pb.h"
#include "sc2utils/sc2_manage_process.h"

namespace sc2
{
    Connection::Connection() : connection(),
                               verbose_(false),
                               queue_(),
                               mutex_(),
                               condition_(),
                               has_response_(false) {}


    bool Connection::Connect(const std::string &address, int port, bool verbose)
    {
        ix::initNetSystem();
        verbose_ = verbose;

        connection.setUrl(std::format("ws://{}:{}/sc2api", address, port));

        connection.setHandshakeTimeout(120);
        connection.enableAutomaticReconnection();

        connection.disablePerMessageDeflate();

        connection.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg)
        {
            if (msg->type == ix::WebSocketMessageType::Message)
            {
                SC2APIProtocol::Response *response = new SC2APIProtocol::Response();
                if (!response->ParseFromString(msg->str))
                {
                    SPDLOG_WARN("[SERVER] Invalid Request: {}", msg->str);
                }
                else
                {
                    PushResponse(response);
                }
            }
            else if (msg->type == ix::WebSocketMessageType::Close)
            {
                if (connection_closed_callback_)
                {
                    connection_closed_callback_();
                }
            }
        });

        connection.start();

        int loop = 0;

        do
        {
            SleepFor(1000);
            ++loop;
        } while (connection.getReadyState() == ix::ReadyState::Connecting && loop < 30);

        if (connection.getReadyState() != ix::ReadyState::Open)
        {
            SPDLOG_ERROR("[CLIENT] Failed to connect {}", static_cast<int>(connection.getReadyState()));
            Disconnect();
            return false;
        }

        SPDLOG_TRACE("[CLIENT] Connected");
        return true;
    }

    Connection::~Connection()
    {
        Disconnect();
    }

    void Connection::Send(const SC2APIProtocol::Request *request)
    {
        if (!request)
        {
            return;
        }
        // Connection must be established before sending.
        assert(connection.getReadyState() == ix::ReadyState::Open);
        if (connection.getReadyState() != ix::ReadyState::Open)
        {
            return;
        }

        std::string output;
        request->SerializeToString(&output);

        connection.send(output, true);

        if (verbose_)
        {
            std::cout << "Sending: " << request->DebugString();
        }
    }

    bool Connection::Receive(
        SC2APIProtocol::Response *&response,
        unsigned int timeout_ms)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        // Block until a message is recieved.
        if (verbose_)
        {
            std::cout << "Waiting for response..." << std::endl;
        }
        auto now = std::chrono::system_clock::now();
        if (condition_.wait_until(
            lock,
            now + std::chrono::milliseconds(timeout_ms),
            [&] { return queue_.size() != 0; }))
        {
            lock.unlock();
            PopResponse(response);
            return true;
        }

        lock.unlock();
        response = nullptr;
        Disconnect();
        queue_.clear();

        // Execute the timeout callback if it exists.
        if (timeout_callback_)
        {
            timeout_callback_();
        }

        return false;
    }

    void Connection::PushResponse(SC2APIProtocol::Response *&response)
    {
        std::lock_guard<std::mutex> guard(mutex_);
        queue_.push_back(response);
        condition_.notify_one();
        has_response_ = true;
    }

    void Connection::PopResponse(SC2APIProtocol::Response *&response)
    {
        if (queue_.empty()) return;
        std::lock_guard<std::mutex> guard(mutex_);
        response = queue_.front();
        queue_.pop_front();
        if (queue_.empty())
        {
            has_response_ = false;
        }
    }

    void Connection::SetTimeoutCallback(std::function<void()> callback)
    {
        timeout_callback_ = callback;
    }

    void Connection::SetConnectionClosedCallback(std::function<void()> callback)
    {
        connection_closed_callback_ = callback;
    }

    bool Connection::HasConnection() const
    {
        return connection.getReadyState() == ix::ReadyState::Open;
    }

    void Connection::Disconnect()
    {
        if (connection.getReadyState() != ix::ReadyState::Closing && connection.getReadyState() != ix::ReadyState::Closed)
        {
            SPDLOG_INFO("[CLIENT] Disconnecting...");
            connection.stop();
        }
    }

    bool Connection::PollResponse()
    {
        return has_response_;
    }
}
