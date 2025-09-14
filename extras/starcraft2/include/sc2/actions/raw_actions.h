/**
 *
 */

#pragma once

#include <cstdint>
#include <variant>
#include <span>
#include <vector>
#include <sc2/api/common.h>

namespace astra::sc2
{
    struct RawUnitCommandPayload
    {
        uint32_t abilityId;
        std::variant<Point2D, uint64_t> target;
        std::vector<uint64_t> unitTags;
        bool queue;
    };

    struct RawMoveCameraPayload
    {
        Point3D position;
    };

    struct RawToggleAutocastPayload
    {
        uint32_t abilityId;
        std::vector<uint64_t> unitTags;
    };

    using RawPayload = std::variant<
            RawUnitCommandPayload,
            RawMoveCameraPayload,
            RawToggleAutocastPayload>;

    enum class RawActionType : uint8_t {
        RawUnitCommand,
        RawMoveCamera,
        RawToggleAutocast,
    };

    struct RawAction final : public astra::Action
    {
        RawPayload payload;

        RawAction() noexcept = default;
        explicit RawAction(RawPayload payload) noexcept : payload(std::move(payload)) {}

        ~RawAction() override = default;

        // Helper to map payload to a simple enum for quick switches
        [[nodiscard]] RawActionType type() const noexcept
        {
            return std::visit([](auto const& p) -> RawActionType {
                using T = std::decay_t<decltype(p)>;
                if constexpr (std::is_same_v<T, RawUnitCommandPayload>) return RawActionType::RawUnitCommand;
                if constexpr (std::is_same_v<T, RawMoveCameraPayload>) return RawActionType::RawMoveCamera;
                if constexpr (std::is_same_v<T, RawToggleAutocastPayload>) return RawActionType::RawToggleAutocast;
            }, payload);
        }

        static RawAction unitCommand(uint32_t abilityId, Point2D target, std::span<uint64_t> unitTags, bool queue = false) noexcept
        {
            return RawAction(RawUnitCommandPayload{abilityId, target, std::vector<uint64_t>(unitTags.begin(), unitTags.end()), queue});
        }

        static RawAction unitCommand(uint32_t abilityId, uint64_t target, std::span<uint64_t> unitTags, bool queue = false) noexcept
        {
            return RawAction(RawUnitCommandPayload{abilityId, target, std::vector<uint64_t>(unitTags.begin(), unitTags.end()), queue});
        }

        static RawAction moveCamera(glm::vec3 position) noexcept
        {
            return RawAction(RawMoveCameraPayload{position});
        }

        static RawAction toggleAutocast(uint32_t abilityId, std::span<uint64_t> unitTags) noexcept
        {
            return RawAction(RawToggleAutocastPayload{abilityId, std::vector<uint64_t>(unitTags.begin(), unitTags.end())});
        }
    };
}
