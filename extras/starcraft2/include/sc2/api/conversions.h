/**
*
*/

#pragma once

#include <array>
#include <format>
#include <optional>
#include <variant>
#include <memory>
#include <cassert>
#include "common.h"
#include "sc2/actions/chat_actions.h"
#include "sc2/actions/debug_actions.h"
#include "sc2/actions/observer_actions.h"
#include "sc2/actions/raw_actions.h"
#include "sc2/actions/spatial_actions.h"
#include "sc2/actions/ui_actions.h"
#include "s2clientprotocol/sc2api.pb.h"


namespace astra::sc2
{




    inline std::unique_ptr<ChatAction> toDomain(const SC2APIProtocol::ActionChat& in)
    {
        std::unique_ptr<ChatAction> out = std::make_unique<ChatAction>();
        out->payload = {toDomain(in.channel()), in.message()};

        return out;
    }

    inline RawUnitCommandPayload toDomain(const SC2APIProtocol::ActionRawUnitCommand& in)
    {
        RawUnitCommandPayload out{};
        out.abilityId = in.ability_id();

        if(in.has_target_unit_tag()) out.target = in.target_unit_tag();
        else if(in.has_target_world_space_pos()) out.target = toDomain(in.target_world_space_pos());

        out.unitTags.reserve(in.unit_tags_size());
        for(auto& tag: in.unit_tags())
        {
            out.unitTags.push_back(tag);
        }

        out.queue = in.queue_command();

        return out;
    }

    inline RawMoveCameraPayload toDomain(const SC2APIProtocol::ActionRawCameraMove& in)
    {
        RawMoveCameraPayload out{};
        out.position = toDomain(in.center_world_space());

        return out;
    }

    inline RawToggleAutocastPayload toDomain(const SC2APIProtocol::ActionRawToggleAutocast& in)
    {
        RawToggleAutocastPayload out{};
        out.abilityId = in.ability_id();

        out.unitTags.reserve(in.unit_tags_size());
        for(auto& tag: in.unit_tags())
        {
            out.unitTags.push_back(tag);
        }

        return out;
    }

    inline std::unique_ptr<RawAction> toDomain(const SC2APIProtocol::ActionRaw& in)
    {
        std::unique_ptr<RawAction> out = std::make_unique<RawAction>();

        if(in.has_unit_command()) out->payload = toDomain(in.unit_command());
        else if(in.has_camera_move()) out->payload = toDomain(in.camera_move());
        else if(in.has_toggle_autocast()) out->payload = toDomain(in.toggle_autocast());

        return out;
    }

    inline SpatialUnitCommandPayload toDomain(const SC2APIProtocol::ActionSpatialUnitCommand& in)
    {
        SpatialUnitCommandPayload out{};
        out.abilityId = in.ability_id();

        if(in.has_target_screen_coord())
        {
            out.position = toDomain(in.target_screen_coord());
            out.screenTarget = true;
        }
        else
        {
            out.position = toDomain(in.target_minimap_coord());
        }

        out.queue = in.queue_command();

        return out;
    }

    inline SpatialMoveCameraPayload toDomain(const SC2APIProtocol::ActionSpatialCameraMove& in)
    {
        SpatialMoveCameraPayload out{};
        out.position = toDomain(in.center_minimap());

        return out;
    }

    inline SpatialUnitSelectionPointPayload toDomain(const SC2APIProtocol::ActionSpatialUnitSelectionPoint& in)
    {
        SpatialUnitSelectionPointPayload out{};
        out.position = toDomain(in.selection_screen_coord());
        out.selectionType = toDomain(in.type());

        return out;
    }

    inline SpatialUnitSelectionPayload toDomain(const SC2APIProtocol::ActionSpatialUnitSelectionRect& in)
    {
        SpatialUnitSelectionPayload out{};

        out.selection.reserve(in.selection_screen_coord_size());
        for(auto& coord: in.selection_screen_coord()) out.selection.push_back(toDomain(coord));

        out.addSelection = in.selection_add();

        return out;
    }

    inline std::unique_ptr<SpatialAction> toDomain(const SC2APIProtocol::ActionSpatial& in)
    {
        std::unique_ptr<SpatialAction> out = std::make_unique<SpatialAction>();

        if(in.has_unit_command()) out->payload = toDomain(in.unit_command());
        else if(in.has_camera_move()) out->payload = toDomain(in.camera_move());
        else if(in.has_unit_selection_point()) out->payload = toDomain(in.unit_selection_point());
        else if(in.has_unit_selection_rect()) out->payload = toDomain(in.unit_selection_rect());

        return out;
    }

    inline UiControlGroupPayload toDomain(const SC2APIProtocol::ActionControlGroup& in)
    {
        UiControlGroupPayload out{};
        out.action = toDomain(in.action());
        out.controlGroupIndex = in.control_group_index();

        return out;
    }

    inline std::unique_ptr<UiAction> toDomain(const SC2APIProtocol::ActionUI& in)
    {
        std::unique_ptr<UiAction> out = std::make_unique<UiAction>();

        if(in.has_control_group()) out->payload = toDomain(in.control_group());
        else if(in.has_select_army()) out->payload = toDomain(in.select_army());
        else if(in.has_select_warp_gates()) out->payload = toDomain(in.select_warp_gates());
        else if(in.has_select_larva()) out->payload = toDomain(in.select_larva());
        else if(in.has_select_idle_worker()) out->payload = toDomain(in.select_idle_worker());
        else if(in.has_multi_panel()) out->payload = toDomain(in.multi_panel());
        else if(in.has_cargo_panel()) out->payload = toDomain(in.cargo_panel());
        else if(in.has_production_panel()) out->payload = toDomain(in.production_panel());
        else if(in.has_toggle_autocast()) out->payload = toDomain(in.toggle_autocast());

        return out;
    }

    inline std::unique_ptr<Action> toDomain(const SC2APIProtocol::Action& in)
    {
        std::unique_ptr<Action> out;
        // TODO: technically this also has game loop, we may need to add that in

        if(in.has_action_chat()) out = toDomain(in.action_chat());
        else if(in.has_action_raw()) out = toDomain(in.action_raw());
        else if(in.has_action_feature_layer()) out = toDomain(in.action_feature_layer());
        else if(in.has_action_render()) out = toDomain(in.action_render());
        else if(in.has_action_ui()) out = toDomain(in.action_ui());

        return out;
    }
}
