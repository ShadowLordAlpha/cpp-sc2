/**
 *
 */

#pragma once

#include <cstdint>
#include <utility>
#include <vector>
#include <variant>
#include <array>
#include <initializer_list>
#include <type_traits>
#include <span>
#include "sc2/api/common.h"
#include "astra/action.h"

namespace astra::sc2
{
    struct SpatialUnitCommandPayload
    {
        uint32_t abilityId;
        Point2I position; // This can be either Screen or Minimap, we represent that as a bool below
        bool screenTarget;
        bool queue; // Equivalent to shift+command.
    };

    struct SpatialMoveCameraPayload
    {
        Point2I position; // Center of the camera in minimap coordinates
    };

    struct SpatialUnitSelectionPointPayload
    {
        enum class SelectionType
        {
            Select = 1, // Equivalent to normal click. Changes selection to unit.
            Toggle = 2, // Equivalent to shift+click. Toggle selection of unit.
            AllType = 3, // Equivalent to control+click. Selects all units of a given type.
            AddAllType = 4, // Equivalent to shift+control+click. Selects all units of a given type.
        };
        Point2I position; // Screen coordinates we are clicking on
        SelectionType selectionType;
    };

    struct SpatialUnitSelectionPayload
    {
        std::vector<RectangleI> selection; // Rectangle on the screen to select
        bool addSelection;
    };

    using SpatialPayload = std::variant<
            SpatialUnitCommandPayload,
            SpatialMoveCameraPayload,
            SpatialUnitSelectionPointPayload,
            SpatialUnitSelectionPayload
    >;

    enum class SpatialActionType : uint8_t {
        SpatialUnitCommand,
        SpatialMoveCamera,
        SpatialUnitSelectionPoint,
        SpatialUnitSelection,
    };

    struct SpatialAction final : public astra::Action
    {
        SpatialPayload payload;

        SpatialAction() noexcept = default;
        explicit SpatialAction(SpatialPayload payload) noexcept : payload(std::move(payload)) {}

        ~SpatialAction() override = default;

        // Helper to map payload to a simple enum for quick switches
        [[nodiscard]] SpatialActionType type() const noexcept {
            return std::visit([](auto const& p) -> SpatialActionType {
                using T = std::decay_t<decltype(p)>;
                if constexpr (std::is_same_v<T, SpatialUnitCommandPayload>)        return SpatialActionType::SpatialUnitCommand;
                if constexpr (std::is_same_v<T, SpatialMoveCameraPayload>)         return SpatialActionType::SpatialMoveCamera;
                if constexpr (std::is_same_v<T, SpatialUnitSelectionPointPayload>) return SpatialActionType::SpatialUnitSelectionPoint;
                if constexpr (std::is_same_v<T, SpatialUnitSelectionPayload>)      return SpatialActionType::SpatialUnitSelection;
            }, payload);
        }

        static SpatialAction unitCommand(uint32_t abilityId, glm::ivec2 position, bool queue = false, bool screenTarget = false) noexcept
        {
            return SpatialAction(SpatialUnitCommandPayload{abilityId, position, screenTarget, queue});
        }

        static SpatialAction moveCamera(glm::ivec2 position) noexcept
        {
            return SpatialAction(SpatialMoveCameraPayload{position});
        }

        static SpatialAction selectUnit(glm::ivec2 position, SpatialUnitSelectionPointPayload::SelectionType selection = SpatialUnitSelectionPointPayload::SelectionType::Select) noexcept
        {
            return SpatialAction(SpatialUnitSelectionPointPayload{position, selection});
        }

        static SpatialAction selectUnits(std::initializer_list<RectangleI> rects, bool addSelection = false)
        {
            return SpatialAction(SpatialUnitSelectionPayload{std::vector<RectangleI>(rects), addSelection});
        }

        static SpatialAction selectUnits(std::span<const RectangleI> rects, bool addSelection = false)
        {
            return SpatialAction(SpatialUnitSelectionPayload{std::vector<RectangleI>(rects.begin(), rects.end()), addSelection});
        }
    };
}
