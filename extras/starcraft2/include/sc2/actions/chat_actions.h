/**
 *
 */

// Yes, this is a dumb file but it allows everything to follow the same pattern so we will just leave it

#pragma once

#include <string>
#include "astra/action.h"

namespace astra::sc2
{
    struct ChatPayload
    {
        enum class Channel
        {
            Broadcast = 1,
            Team = 2,
        };
        Channel channel = Channel::Broadcast;
        std::string message;
    };

    struct ChatAction final : public astra::Action
    {
        ChatPayload payload;

        ChatAction() noexcept = default;
        explicit ChatAction(ChatPayload payload) noexcept : payload(std::move(payload)) {}

        ~ChatAction() override = default;

        static ChatAction broadcast(const std::string& message) noexcept
        {
            return ChatAction({ChatPayload::Channel::Broadcast, message});
        }

        static ChatAction team(const std::string& message) noexcept
        {
            return ChatAction({ChatPayload::Channel::Team, message});
        }
    };
}
