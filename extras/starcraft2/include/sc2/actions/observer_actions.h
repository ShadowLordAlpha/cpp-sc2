/**
 *
 */

#pragma once

#include <cstdint>
#include <span>
#include <vector>
#include <initializer_list>
#include <glm/vec2.hpp>
#include "astra/action.h"

namespace astra::sc2
{
    // Helper constants for clarity and validation
    constexpr uint32_t OBSERVER_EVERYONE_PLAYER_ID = 0;
    constexpr uint32_t OBSERVER_MIN_PLAYER_ID      = 1;
    constexpr uint32_t OBSERVER_MAX_PLAYER_ID      = 15;

    inline constexpr bool isValidObservedPlayerId(uint32_t id) noexcept {
        return id == OBSERVER_EVERYONE_PLAYER_ID || (id >= OBSERVER_MIN_PLAYER_ID && id <= OBSERVER_MAX_PLAYER_ID);
    }

    // Payloads
    /**
     * Switch to a given player's perspective (0 = Everyone).
     */
    struct ObserverSetPerspectivePayload {
        uint32_t playerId = OBSERVER_EVERYONE_PLAYER_ID; //< The player id to observe. Use 0 (OBSERVER_EVERYONE_PLAYER_ID) for Everyone.
    };

    /**
     * Move the observer camera to a world position, optionally adjusting distance (zoom).
     */
    struct ObserverMoveCameraPayload {
        glm::vec2 worldPosition{0.0f, 0.0f}; //< World-space position to place the camera focus.
        float distance = 0.0f; //< 0 = default camera distance
    };

    /**
     * Follow a specific player's camera (id must be 1..15).
     */
    struct ObserverFollowPlayerPayload {
        uint32_t playerId = OBSERVER_MIN_PLAYER_ID;
    };

    /**
     * Follow specific units by their tags.
     */
    struct ObserverFollowUnitsPayload {
        std::vector<uint32_t> unitTags;
    };

    using ObserverPayload = std::variant<
            ObserverSetPerspectivePayload,
            ObserverMoveCameraPayload,
            ObserverFollowPlayerPayload,
            ObserverFollowUnitsPayload>;

    /**
     * Light-weight tag to identify observer action types.
     */
    enum class ObserverActionType : uint8_t {
        SetPerspective,
        CameraMove,
        CameraFollowPlayer,
        CameraFollowUnits,
    };

    /**
     * Base of all observer actions. Explicitly derives from astra::Action.
     */
    struct ObserverAction final : public astra::Action
    {
        ObserverPayload payload;

        ObserverAction() noexcept = default;
        explicit ObserverAction(ObserverPayload payload) noexcept : payload(payload) {}

        ~ObserverAction() = default;

        // Helper to map payload to a simple enum for quick switches
        [[nodiscard]] ObserverActionType type() const noexcept
        {
            return std::visit([](auto const& p) -> ObserverActionType {
                using T = std::decay_t<decltype(p)>;
                if constexpr (std::is_same_v<T, ObserverSetPerspectivePayload>) return ObserverActionType::SetPerspective;
                if constexpr (std::is_same_v<T, ObserverMoveCameraPayload>)     return ObserverActionType::CameraMove;
                if constexpr (std::is_same_v<T, ObserverFollowPlayerPayload>)   return ObserverActionType::CameraFollowPlayer;
                if constexpr (std::is_same_v<T, ObserverFollowUnitsPayload>)    return ObserverActionType::CameraFollowUnits;
            }, payload);
        }

        // Convenience factories (keep call sites terse)
        static ObserverAction setPerspective(uint32_t playerId) noexcept
        {
            return ObserverAction(ObserverSetPerspectivePayload{playerId});
        }

        static ObserverAction moveCamera(glm::vec2 pos, float distance = 0.0f) noexcept
        {
            return ObserverAction(ObserverMoveCameraPayload{pos, distance});
        }

        static ObserverAction followPlayer(uint32_t playerId) noexcept
        {
            return ObserverAction(ObserverFollowPlayerPayload{playerId});
        }

        static ObserverAction followUnits(std::initializer_list<uint32_t> tags)
        {
            return ObserverAction(ObserverFollowUnitsPayload{std::vector<uint32_t>(tags)});
        }

        static ObserverAction followUnits(std::span<const uint32_t> tags)
        {
            return ObserverAction(ObserverFollowUnitsPayload{std::vector<uint32_t>(tags.begin(), tags.end())});
        }
    };
}
