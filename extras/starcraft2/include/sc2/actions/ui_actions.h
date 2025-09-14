/**
 *
 */

#pragma once

namespace astra::sc2
{

    struct UiControlGroupPayload
    {
        enum class ControlGroupAction {
            Recall = 1,             // Equivalent to number hotkey. Replaces current selection with control group.
            Set = 2,                // Equivalent to Control + number hotkey. Sets control group to current selection.
            Append = 3,             // Equivalent to Shift + number hotkey. Adds current selection into control group.
            SetAndSteal = 4,        // Equivalent to Control + Alt + number hotkey. Sets control group to current selection. Units are removed from other control groups.
            AppendAndSteal = 5,     // Equivalent to Shift + Alt + number hotkey. Adds current selection into control group. Units are removed from other control groups.
        };

        ControlGroupAction action;
        uint32_t controlGroupIndex;
    };

    struct UiSelectArmyPayload
    {
        bool selectionAdd;
    };

    struct UiSelectWarpGatesPayload
    {
        bool selectionAdd;
    };

    struct UiSelectLarvaPayload
    {

    };

    struct UiSelectIdleWorkerPayload
    {
        enum class Type {
            Set = 1,        // Equivalent to click with no modifiers. Replaces selection with single idle worker.
            Add = 2,        // Equivalent to shift+click. Adds single idle worker to current selection.
            All = 3,        // Equivalent to control+click. Selects all idle workers.
            AddAll = 4,     // Equivalent to shift+control+click. Adds all idle workers to current selection.
        };

        Type type;
    };

    struct UiMultiPanelPayload
    {
        enum class Type {
            SingleSelect = 1,         // Click on icon
            DeselectUnit = 2,         // Shift Click on icon
            SelectAllOfType = 3,      // Control Click on icon.
            DeselectAllOfType = 4,    // Control+Shift Click on icon.
        };

        Type type;
        uint32_t unitIndex;
    };

    struct UiCargoPanelUnloadPayload
    {
        uint32_t unitIndex;
    };

    struct UiProductionPanelRemoveFromQueuePayload
    {
        uint32_t unitIndex;
    };

    struct UiToggleAutocastPayload
    {
        uint32_t abilityId;
    };

    using UiPayload = std::variant<
            UiControlGroupPayload,
            UiSelectArmyPayload,
            UiSelectWarpGatesPayload,
            UiSelectLarvaPayload,
            UiSelectIdleWorkerPayload,
            UiMultiPanelPayload,
            UiCargoPanelUnloadPayload,
            UiProductionPanelRemoveFromQueuePayload,
            UiToggleAutocastPayload>;

    enum class UiActionType : uint8_t {
        UiControlGroup,
        UiSelectArmy,
        UiSelectWarpGates,
        UiSelectLarva,
        UiSelectIdleWorker,
        UiMultiPanel,
        UiCargoPanelUnload,
        UiProductionPanelRemoveFromQueue,
        UiToggleAutocast,
    };

    struct UiAction final : public astra::Action
    {
        UiPayload payload;

        UiAction() noexcept = default;
        explicit UiAction(UiPayload payload) noexcept : payload(std::move(payload)) {}

        ~UiAction() override = default;

        // Helper to map payload to a simple enum for quick switches
        [[nodiscard]] UiActionType type() const noexcept
        {
            return std::visit([](auto const& p) -> UiActionType {
                using T = std::decay_t<decltype(p)>;
                if constexpr (std::is_same_v<T, UiControlGroupPayload>) return UiActionType::UiControlGroup;
                if constexpr (std::is_same_v<T, UiSelectArmyPayload>) return UiActionType::UiSelectArmy;
                if constexpr (std::is_same_v<T, UiSelectWarpGatesPayload>) return UiActionType::UiSelectWarpGates;
                if constexpr (std::is_same_v<T, UiSelectLarvaPayload>) return UiActionType::UiSelectLarva;
                if constexpr (std::is_same_v<T, UiSelectIdleWorkerPayload>) return UiActionType::UiSelectIdleWorker;
                if constexpr (std::is_same_v<T, UiMultiPanelPayload>) return UiActionType::UiMultiPanel;
                if constexpr (std::is_same_v<T, UiCargoPanelUnloadPayload>) return UiActionType::UiCargoPanelUnload;
                if constexpr (std::is_same_v<T, UiProductionPanelRemoveFromQueuePayload>) return UiActionType::UiProductionPanelRemoveFromQueue;
                if constexpr (std::is_same_v<T, UiToggleAutocastPayload>) return UiActionType::UiToggleAutocast;
            }, payload);
        }

        // TODO: Static helper methods for each action type. Not technically needed but nice to have.
    };
}
