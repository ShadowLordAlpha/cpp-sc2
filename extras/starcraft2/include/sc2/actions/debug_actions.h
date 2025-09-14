/**
 *
 */

#pragma once

#include <cstdint>
#include <string>
#include <filesystem>
#include <variant>
#include <array>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "astra/action.h"
#include "sc2/api/common.h"

namespace astra::sc2
{
    using Color = std::array<uint8_t, 3>;

    struct DebugDrawTextPayload
    {
        std::string text;
        std::variant<glm::vec2, glm::vec3> position;
        uint32_t size;
        Color color;
    };

    struct DebugDrawLinePayload
    {
        glm::vec3 start;
        glm::vec3 end;
        Color color;
    };

    struct DebugDrawBoxPayload
    {
        glm::vec3 min;
        glm::vec3 max;
        Color color;
    };

    struct DebugDrawSpherePayload
    {
        glm::vec3 center;
        float radius;
        Color color;
    };

    struct DebugGameStatePayload
    {
        enum class DebugGameState {
            show_map = 1,
            control_enemy = 2,
            food = 3,
            free = 4,
            all_resources = 5,
            god = 6,
            minerals = 7,
            gas = 8,
            cooldown = 9,
            tech_tree = 10,
            upgrade = 11,
            fast_build = 12,
        };

        DebugGameState state;
    };

    struct DebugCreateUnitPayload
    {
        uint32_t unit_type;
        int32_t owner;
        Point2D pos;
        uint32_t quantity;
    };

    struct DebugKillUnitPayload
    {
        std::vector<uint64_t> tag;
    };

    struct DebugTestProcessPayload
    {
        enum class Test {
            hang = 1,
            crash = 2,
            exit = 3,
        };

        Test test;
        int32_t delayMs;
    };

    struct DebugSetScorePayload
    {
        float score;
    };

    struct DebugEndGamePayload
    {
        enum class EndResult {
            Surrender = 1,                  // Default if nothing is set. The current player admits defeat.
            DeclareVictory = 2,
        };

        EndResult end_result;
    };

    struct DebugSetUnitValuePayload
    {
        enum class UnitValue {
            Energy = 1,
            Life = 2,
            Shields = 3,
        };

        UnitValue unit_value;
        float value;
        uint64_t unit_tag;
    };

    using DebugPayload = std::variant<
            DebugDrawTextPayload,
            DebugDrawLinePayload,
            DebugDrawBoxPayload,
            DebugDrawSpherePayload,
            DebugGameStatePayload,
            DebugCreateUnitPayload,
            DebugKillUnitPayload,
            DebugTestProcessPayload,
            DebugSetScorePayload,
            DebugEndGamePayload,
            DebugSetUnitValuePayload>;

    /**
     * Light-weight tag to identify observer action types.
     */
    enum class DebugActionType : uint8_t {
        DebugDrawText,
        DebugDrawLine,
        DebugDrawBox,
        DebugDrawSphere,
        DebugGameState,
        DebugCreateUnit,
        DebugKillUnit,
        DebugTestProcess,
        DebugSetScore,
        DebugEndGame,
        DebugSetUnitValue,
    };

    struct DebugAction final : public astra::Action
    {
        DebugPayload payload;

        DebugAction() noexcept = default;
        explicit DebugAction(DebugPayload payload) noexcept : payload(payload) {}

        ~DebugAction() = default;

        // Helper to map payload to a simple enum for quick switches
        [[nodiscard]] DebugActionType type() const noexcept
        {
            return std::visit([](auto const& p) -> DebugActionType {
                using T = std::decay_t<decltype(p)>;
                if constexpr (std::is_same_v<T, DebugDrawTextPayload>) return DebugActionType::DebugDrawText;
                if constexpr (std::is_same_v<T, DebugDrawLinePayload>) return DebugActionType::DebugDrawLine;
                if constexpr (std::is_same_v<T, DebugDrawBoxPayload>) return DebugActionType::DebugDrawBox;
                if constexpr (std::is_same_v<T, DebugDrawSpherePayload>) return DebugActionType::DebugDrawSphere;
                if constexpr (std::is_same_v<T, DebugGameStatePayload>) return DebugActionType::DebugGameState;
                if constexpr (std::is_same_v<T, DebugCreateUnitPayload>) return DebugActionType::DebugCreateUnit;
                if constexpr (std::is_same_v<T, DebugKillUnitPayload>) return DebugActionType::DebugKillUnit;
                if constexpr (std::is_same_v<T, DebugTestProcessPayload>) return DebugActionType::DebugTestProcess;
                if constexpr (std::is_same_v<T, DebugSetScorePayload>) return DebugActionType::DebugSetScore;
                if constexpr (std::is_same_v<T, DebugEndGamePayload>) return DebugActionType::DebugEndGame;
                if constexpr (std::is_same_v<T, DebugSetUnitValuePayload>) return DebugActionType::DebugSetUnitValue;
            }, payload);
        }
    };
}
