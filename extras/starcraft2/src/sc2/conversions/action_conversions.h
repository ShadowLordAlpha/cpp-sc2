/**
 *
 */

#pragma once

#include <memory>
#include <variant>
#include "sc2/actions/observer_actions.h"
#include "s2clientprotocol/sc2api.pb.h"

namespace astra::sc2
{
    inline void toProto(const ObserverSetPerspectivePayload& in, SC2APIProtocol::ActionObserverPlayerPerspective* out)
    {
        out->set_player_id(in.playerId);
    }

    inline void toProto(const ObserverMoveCameraPayload& in, SC2APIProtocol::ActionObserverCameraMove* out)
    {
        toProto(in.worldPosition, out->mutable_world_pos());
        out->set_distance(in.distance);
    }

    inline void toProto(const ObserverFollowPlayerPayload& in, SC2APIProtocol::ActionObserverCameraFollowPlayer* out)
    {
        out->set_player_id(in.playerId);
    }

    inline void toProto(const ObserverFollowUnitsPayload& in, SC2APIProtocol::ActionObserverCameraFollowUnits* out)
    {
        for(auto& unitTag : in.unitTags)
        {
            out->add_unit_tags(unitTag);
        }
    }

    inline void toProto(ObserverAction* in, SC2APIProtocol::ObserverAction* out)
    {
        switch (in->type()) {

            case ObserverActionType::SetPerspective:
                toProto(std::get<ObserverSetPerspectivePayload>(in->payload), out->mutable_player_perspective());
                break;
            case ObserverActionType::CameraMove:
                toProto(std::get<ObserverMoveCameraPayload>(in->payload), out->mutable_camera_move());
                break;
            case ObserverActionType::CameraFollowPlayer:
                toProto(std::get<ObserverFollowPlayerPayload>(in->payload), out->mutable_camera_follow_player());
                break;
            case ObserverActionType::CameraFollowUnits:
                toProto(std::get<ObserverFollowUnitsPayload>(in->payload), out->mutable_camera_follow_units());
                break;
        }
        // TODO: implement actions
    }

    inline void toProto(const DebugAction* in, SC2APIProtocol::DebugCommand* out)
    {
        // TODO: actually implement debug commands, there are a lot of them...
    }

    inline std::unique_ptr<Action> toDomain(const SC2APIProtocol::Action& in)
    {
        std::unique_ptr<Action> out;

        return out;
    }

    inline void toProto(Action* in, SC2APIProtocol::Action* out)
    {
        // TODO: implement actions
    }
}
