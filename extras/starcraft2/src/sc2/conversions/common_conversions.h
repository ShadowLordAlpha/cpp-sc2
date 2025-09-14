/**
 *
 */

#pragma once

#include "sc2/api/request/setup_requests.h"
#include "s2clientprotocol/sc2api.pb.h"

namespace astra::sc2
{
    inline Point2D toDomain(const SC2APIProtocol::Point2D& in)
    {
        return {in.x(), in.y()};
    }

    inline void toProto(const Point2D& in, SC2APIProtocol::Point2D* out)
    {
        out->set_x(in.x);
        out->set_y(in.y);
    }

    inline Point3D toDomain(const SC2APIProtocol::Point& in)
    {
        return {in.x(), in.y(), in.z()};
    }

    inline Point2I toDomain(const SC2APIProtocol::PointI& in)
    {
        return {in.x(), in.y()};
    }

    inline RectangleI toDomain(const SC2APIProtocol::RectangleI& in)
    {
        return {toDomain(in.p0()), toDomain(in.p1())};
    }

    inline void toProto(const Point2I& in, SC2APIProtocol::Size2DI* out)
    {
        out->set_x(in.x);
        out->set_y(in.y);
    }

    inline Point2I toDomain(const SC2APIProtocol::Size2DI& in)
    {
        return {in.x(), in.y()};
    }

    inline ImageData toDomain(const SC2APIProtocol::ImageData& in)
    {
        ImageData out{};
        out.bitsPerPixel = in.bits_per_pixel();
        out.size = toDomain(in.size());
        out.data = in.data();

        return out;
    }

    inline void toProto(const SpatialCameraSetup& in, SC2APIProtocol::SpatialCameraSetup* out)
    {
        toProto(in.resolution, out->mutable_resolution());
        toProto(in.minimapResolution, out->mutable_minimap_resolution());

        if(in.width) out->set_width(*in.width);
        if(in.cropToPlayableArea) out->set_crop_to_playable_area(*in.cropToPlayableArea);
        if(in.allowCheatingLayers) out->set_allow_cheating_layers(*in.allowCheatingLayers);
    }

    inline SpatialCameraSetup toDomain(const SC2APIProtocol::SpatialCameraSetup& in)
    {
        SpatialCameraSetup out{};
        out.resolution = toDomain(in.resolution());
        out.minimapResolution = toDomain(in.minimap_resolution());

        if(in.has_width()) out.width = in.width();
        if(in.has_crop_to_playable_area()) out.cropToPlayableArea = in.crop_to_playable_area();
        if(in.has_allow_cheating_layers()) out.allowCheatingLayers = in.allow_cheating_layers();

        return out;
    }

    inline void toProto(const InterfaceOptions& in, SC2APIProtocol::InterfaceOptions* out)
    {
        out->set_raw(in.raw);
        out->set_score(in.score);

        if(in.featureLayer) toProto(*in.featureLayer, out->mutable_feature_layer());
        if(in.render) toProto(*in.render, out->mutable_render());

        out->set_show_cloaked(in.showCloaked);
        out->set_show_burrowed_shadows(in.showBurrowedShadows);
        out->set_show_placeholders(in.showPlaceholders);
        out->set_raw_affects_selection(in.rawAffectsSelections);
        out->set_raw_crop_to_playable_area(in.rawCropToPlayableArea);
    }

    inline InterfaceOptions toDomain(const SC2APIProtocol::InterfaceOptions& in)
    {
        InterfaceOptions out{};
        out.raw = in.raw();
        out.score = in.score();

        if(in.has_feature_layer()) out.featureLayer = toDomain(in.feature_layer());
        if(in.has_render()) out.render = toDomain(in.render());

        out.showCloaked = in.show_cloaked();
        out.showBurrowedShadows = in.show_burrowed_shadows();
        out.showPlaceholders = in.show_placeholders();
        out.rawAffectsSelections = in.raw_affects_selection();
        out.rawCropToPlayableArea = in.raw_crop_to_playable_area();

        return out;
    }

    inline PlayerResult toDomain(const SC2APIProtocol::PlayerResult& in)
    {
        PlayerResult out{};
        out.playerId = in.player_id();
        out.result = toDomain(in.result());

        return out;
    }

    inline PlayerInfo toDomain(const SC2APIProtocol::PlayerInfo& in)
    {
        PlayerInfo out{};
        out.playerId = in.player_id();
        out.type = toDomain(in.type());
        out.requestedRace = toDomain(in.race_requested());

        if(in.has_race_actual()) out.actualRace = toDomain(in.race_actual());
        if(in.has_difficulty()) out.difficulty = toDomain(in.difficulty());
        if(in.has_ai_build()) out.aiBuild = toDomain(in.ai_build());

        out.playerName = in.player_name();

        return out;
    }
}
