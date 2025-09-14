/**
 *
 */

#pragma once

#include "sc2/api/response/game_responses.h"
#include "sc2/conversions/common_conversions.h"
#include "sc2/conversions/action_conversions.h"
#include "s2clientprotocol/sc2api.pb.h"

namespace astra::sc2
{
    inline StartRaw toDomain(const SC2APIProtocol::StartRaw& in)
    {
        StartRaw out{};
        out.mapSize = toDomain(in.map_size());
        out.pathingGrid = toDomain(in.pathing_grid());
        out.terrainHeight = toDomain(in.terrain_height());
        out.placementGrid = toDomain(in.placement_grid());
        out.playableArea = toDomain(in.playable_area());

        out.startLocations.reserve(in.start_locations_size());
        for(auto& loc: in.start_locations())
        {
            out.startLocations.push_back(toDomain(loc));
        }

        return out;
    }

    inline GameInfoResponse toDomain(const SC2APIProtocol::ResponseGameInfo& in)
    {
        GameInfoResponse out{};
        out.mapName = in.map_name();

        out.modNames.reserve(in.mod_names_size());
        for(auto& mod: in.mod_names())
        {
            out.modNames.push_back(mod);
        }

        out.localMapPath = in.local_map_path();

        out.playerInfo.resize(in.player_info_size());
        for(auto& player: in.player_info())
        {
            out.playerInfo.push_back(toDomain(player));
        }

        if(in.has_start_raw()) out.startRaw = toDomain(in.start_raw());
        out.options = toDomain(in.options());

        return out;
    }

    inline void toProto(const QueryPathing& in, SC2APIProtocol::RequestQueryPathing* out)
    {
        if(std::holds_alternative<Point2D>(in.start))
        {
            toProto(std::get<Point2D>(in.start), out->mutable_start_pos());
        }
        else
        {
            out->set_unit_tag(std::get<uint64_t>(in.start));
        }

        toProto(in.end, out->mutable_end_pos());

    }

    inline void toProto(const QueryPlacement& in, SC2APIProtocol::RequestQueryBuildingPlacement* out)
    {
        out->set_ability_id(in.abilityId);
        toProto(in.targetPos, out->mutable_target_pos());
        if(in.unitTag) out->set_placing_unit_tag(*in.unitTag);
    }

    inline ActionError toDomain(const SC2APIProtocol::ActionError& in)
    {
        ActionError out{};
        out.unitTag = in.unit_tag();
        out.abilityId = in.ability_id();
        out.result = toDomain(in.result());

        return out;
    }

    inline AvailableAbility toDomain(const SC2APIProtocol::AvailableAbility& in)
    {
        AvailableAbility out{};
        out.abilityId = in.ability_id();
        out.requiresPoint = in.requires_point();

        return out;
    }

    inline PlayerCommon toDomain(const SC2APIProtocol::PlayerCommon& in)
    {
        PlayerCommon out{};
        out.playerId = in.player_id();
        out.minerals = in.minerals();
        out.vespene = in.vespene();
        out.foodCap = in.food_cap();
        out.foodUsed = in.food_used();
        out.foodArmy = in.food_army();
        out.foodWorkers = in.food_workers();
        out.idleWorkerCount = in.idle_worker_count();
        out.warpGateCount = in.warp_gate_count();
        out.larvaCount = in.larva_count();

        return out;
    }

    inline CategoryScoreDetails toDomain(const SC2APIProtocol::CategoryScoreDetails& in)
    {
        CategoryScoreDetails out{};
        out.none = in.none();
        out.army = in.army();
        out.economy = in.economy();
        out.technology = in.technology();
        out.upgrade = in.upgrade();

        return out;
    }

    inline VitalScoreDetails toDomain(const SC2APIProtocol::VitalScoreDetails& in)
    {
        VitalScoreDetails out{};
        out.life = in.life();
        out.shields = in.shields();
        out.energy = in.energy();

        return out;
    }

    inline ScoreDetails toDomain(const SC2APIProtocol::ScoreDetails& in)
    {
        ScoreDetails out{};
        out.idleProductionTime = in.idle_production_time();
        out.idleWorkerTime = in.idle_worker_time();
        out.totalValueUnits = in.total_value_units();
        out.totalValueStructures = in.total_value_structures();
        out.killedValueUnits = in.killed_value_units();
        out.killedValueStructures = in.killed_value_structures();
        out.collectedMinerals = in.collected_minerals();
        out.collectedVespene = in.collected_vespene();
        out.collectionRateMinerals = in.collection_rate_minerals();
        out.collectionRateVespene = in.collection_rate_vespene();
        out.spentMinerals = in.spent_minerals();
        out.spentVespene = in.spent_vespene();

        out.foodUsed = toDomain(in.food_used());
        out.killedMinerals = toDomain(in.killed_minerals());
        out.killedVespene = toDomain(in.killed_vespene());
        out.lostMinerals = toDomain(in.lost_minerals());
        out.lostVespene = toDomain(in.lost_vespene());
        out.friendlyFireMinerals = toDomain(in.friendly_fire_minerals());
        out.friendlyFireVespene = toDomain(in.friendly_fire_vespene());
        out.usedMinerals = toDomain(in.used_minerals());
        out.usedVespene = toDomain(in.used_vespene());
        out.totalUsedMinerals = toDomain(in.total_used_minerals());
        out.totalUsedVespene = toDomain(in.total_used_vespene());
        out.totalDamageDealt = toDomain(in.total_damage_dealt());
        out.totalDamageTaken = toDomain(in.total_damage_taken());
        out.totalHealed = toDomain(in.total_healed());

        out.currentApm = in.current_apm();
        out.currentEffectiveApm = in.current_effective_apm();

        return out;
    }

    inline Score toDomain(const SC2APIProtocol::Score& in)
    {
        Score out{};
        out.type = toDomain(in.score_type());
        out.score = in.score();
        out.scoreDetails = toDomain(in.score_details());

        return out;
    }

    inline PowerSource toDomain(const SC2APIProtocol::PowerSource& in)
    {
        PowerSource out{};
        out.pos = toDomain(in.pos());
        out.radius = in.radius();
        out.tag = in.tag();

        return out;
    }

    inline PlayerRaw toDomain(const SC2APIProtocol::PlayerRaw& in)
    {
        PlayerRaw out{};

        out.powerSources.reserve(in.power_sources_size());
        for(auto& source: in.power_sources())
        {
            out.powerSources.push_back(toDomain(source));
        }

        out.camera = toDomain(in.camera());

        out.upgradeIds.reserve(in.upgrade_ids_size());
        for(auto& id: in.upgrade_ids())
        {
            out.upgradeIds.push_back(id);
        }

        return out;
    }

    inline MapState toDomain(const SC2APIProtocol::MapState& in)
    {
        MapState out{};
        out.visibility = toDomain(in.visibility());
        out.creep = toDomain(in.creep());

        return out;
    }

    inline Event toDomain(const SC2APIProtocol::Event& in)
    {
        Event out{};

        out.deadUnits.reserve(in.dead_units_size());
        for(auto& unit: in.dead_units())
        {
            out.deadUnits.push_back(unit);
        }

        return out;
    }

    inline Effect toDomain(const SC2APIProtocol::Effect& in)
    {
        Effect out{};
        out.effectId = in.effect_id();

        out.pos.reserve(in.pos_size());
        for(auto& pos: in.pos())
        {
            out.pos.push_back(toDomain(pos));
        }

        out.alliance = toDomain(in.alliance());
        out.owner = in.owner();
        out.radius = in.radius();

        return out;
    }

    inline RadarRing toDomain(const SC2APIProtocol::RadarRing& in)
    {
        RadarRing out{};
        out.pos = toDomain(in.pos());
        out.radius = in.radius();

        return out;
    }

    inline UnitOrder toDomain(const SC2APIProtocol::UnitOrder& in)
    {
        UnitOrder out{};
        out.abilityId = in.ability_id();

        if(in.has_target_unit_tag()) out.target = in.target_unit_tag();
        else if(in.has_target_world_space_pos()) out.target = toDomain(in.target_world_space_pos());

        if(in.has_progress()) out.progress = in.progress();

        return out;
    }

    inline PassengerUnit toDomain(const SC2APIProtocol::PassengerUnit& in)
    {
        PassengerUnit out{};
        out.tag = in.tag();
        out.health = in.health();
        out.healthMax = in.health_max();
        out.shield = in.shield();
        out.shieldMax = in.shield_max();
        out.energy = in.energy();
        out.energyMax = in.energy_max();
        out.unitType = in.unit_type();

        return out;
    }

    inline RallyTarget toDomain(const SC2APIProtocol::RallyTarget& in)
    {
        RallyTarget out{};
        out.point = toDomain(in.point());
        if(in.has_tag()) out.tag = in.tag();

        return out;
    }

    inline Unit toDomain(const SC2APIProtocol::Unit& in)
    {
        Unit out{};
        out.displayType = toDomain(in.display_type());
        out.alliance = toDomain(in.alliance());

        out.tag = in.tag();
        out.unitType = in.unit_type();
        out.owner = in.owner();

        out.pos = toDomain(in.pos());
        out.facing = in.facing();
        out.radius = in.radius();
        out.buildProgress = in.build_progress();
        out.cloak = toDomain(in.cloak());

        std::vector<uint32_t> buffIds;

        out.detectRange = in.detect_range();
        out.radarRange = in.radar_range();

        out.selected = in.is_selected();
        out.onScreen = in.is_on_screen();
        out.blip = in.is_blip();
        out.powered = in.is_powered();
        out.active = in.is_active();
        out.attackUpgradeLevel = in.attack_upgrade_level();
        out.armorUpgradeLevel = in.armor_upgrade_level();
        out.shieldUpgradeLevel = in.shield_upgrade_level();

        if(in.has_health()) out.health = in.health();
        if(in.has_health_max()) out.healthMax = in.health_max();
        if(in.has_shield()) out.shield = in.shield();
        if(in.has_shield_max()) out.shieldMax = in.shield_max();
        if(in.has_energy()) out.energy = in.energy();
        if(in.has_energy_max()) out.energyMax = in.energy_max();
        if(in.has_mineral_contents()) out.mineralContents = in.mineral_contents();
        if(in.has_vespene_contents()) out.vespeneContents = in.vespene_contents();
        if(in.has_is_flying()) out.flying = in.is_flying();
        if(in.has_is_burrowed()) out.burrowed = in.is_burrowed();
        if(in.has_is_hallucination()) out.hallucination = in.is_hallucination();

        // Not populated for enemies
        out.orders.reserve(in.orders_size());
        for(auto& order: in.orders())
        {
            out.orders.push_back(toDomain(order));
        }

        if(in.has_add_on_tag()) out.addOnTag = in.add_on_tag();

        out.passengers.reserve(in.passengers_size());
        for(auto& passenger: in.passengers())
        {
            out.passengers.push_back(toDomain(passenger));
        }

        if(in.has_cargo_space_taken()) out.cargoSpaceTaken = in.cargo_space_taken();
        if(in.has_cargo_space_max()) out.cargoSpaceMax = in.cargo_space_max();
        if(in.has_assigned_harvesters()) out.assignedHarvesters = in.assigned_harvesters();
        if(in.has_ideal_harvesters()) out.idealHarvesters = in.ideal_harvesters();
        if(in.has_weapon_cooldown()) out.weaponCooldown = in.weapon_cooldown();
        if(in.has_engaged_target_tag()) out.engagedTargetTag = in.engaged_target_tag();
        if(in.has_buff_duration_remain()) out.buffDurationRemain = in.buff_duration_remain();
        if(in.has_buff_duration_max()) out.buffDurationMax = in.buff_duration_max();

        out.rallyTargets.reserve(in.rally_targets_size());
        for(auto& rallyTarget: in.rally_targets())
        {
            out.rallyTargets.push_back(toDomain(rallyTarget));
        }

        return out;
    }

    inline ObservationRaw toDomain(const SC2APIProtocol::ObservationRaw& in)
    {
        ObservationRaw out{};
        out.player = toDomain(in.player());

        out.units.reserve(in.units_size());
        for(auto& unit: in.units())
        {
            out.units.push_back(toDomain(unit));
        }

        out.mapState = toDomain(in.map_state());
        out.event = toDomain(in.event());

        out.effects.reserve(in.effects_size());
        for(auto& effect: in.effects())
        {
            out.effects.push_back(toDomain(effect));
        }

        out.radar.reserve(in.radar_size());
        for(auto& radar: in.radar())
        {
            out.radar.push_back(toDomain(radar));
        }

        return out;
    }

    inline FeatureLayers toDomain(const SC2APIProtocol::FeatureLayers& in)
    {
        FeatureLayers out{};
        out.height_map = toDomain(in.height_map());
        out.visibility_map = toDomain(in.visibility_map());
        out.creep = toDomain(in.creep());
        out.power = toDomain(in.power());
        out.player_id = toDomain(in.player_id());
        out.unit_type = toDomain(in.unit_type());
        out.selected = toDomain(in.selected());
        out.unit_hit_points = toDomain(in.unit_hit_points());
        out.unit_hit_points_ratio = toDomain(in.unit_hit_points_ratio());
        out.unit_energy = toDomain(in.unit_energy());
        out.unit_energy_ratio = toDomain(in.unit_energy_ratio());
        out.unit_shields = toDomain(in.unit_shields());
        out.unit_shields_ratio = toDomain(in.unit_shields_ratio());
        out.player_relative = toDomain(in.player_relative());
        out.unit_density_aa = toDomain(in.unit_density_aa());
        out.unit_density = toDomain(in.unit_density());
        out.effects = toDomain(in.effects());
        out.hallucinations = toDomain(in.hallucinations());
        out.cloaked = toDomain(in.cloaked());
        out.blip = toDomain(in.blip());
        out.buffs = toDomain(in.buffs());
        out.buff_duration = toDomain(in.buff_duration());
        out.active = toDomain(in.active());
        out.build_progress = toDomain(in.build_progress());
        out.buildable = toDomain(in.buildable());
        out.pathable = toDomain(in.pathable());
        out.placeholder = toDomain(in.placeholder());

        return out;
    }

    inline FeatureLayersMinimap toDomain(const SC2APIProtocol::FeatureLayersMinimap& in)
    {
        FeatureLayersMinimap out{};
        out.height_map = toDomain(in.height_map());
        out.visibility_map = toDomain(in.visibility_map());
        out.creep = toDomain(in.creep());
        out.camera = toDomain(in.camera());
        out.player_id = toDomain(in.player_id());
        out.player_relative = toDomain(in.player_relative());
        out.selected = toDomain(in.selected());
        out.alerts = toDomain(in.alerts());
        out.buildable = toDomain(in.buildable());
        out.pathable = toDomain(in.pathable());

        if(in.has_unit_type()) out.unit_type = toDomain(in.unit_type());

        return out;
    }

    inline ObservationFeatureLayer toDomain(const SC2APIProtocol::ObservationFeatureLayer& in)
    {
        ObservationFeatureLayer out{};
        out.renders = toDomain(in.renders());
        out.minimapRenders = toDomain(in.minimap_renders());

        return out;
    }

    inline ObservationRender toDomain(const SC2APIProtocol::ObservationRender& in)
    {
        ObservationRender out{};
        out.map = toDomain(in.map());
        out.minimap = toDomain(in.minimap());

        return out;
    }

    inline ControlGroup toDomain(const SC2APIProtocol::ControlGroup& in)
    {
        ControlGroup out{};
        out.controlGroupIndex = in.control_group_index();
        out.leaderUintType = in.leader_unit_type();
        out.count = in.count();

        return out;
    }

    inline UnitInfo toDomain(const SC2APIProtocol::UnitInfo& in)
    {
        UnitInfo out{};
        out.unitType = in.unit_type();
        out.playerRelative = in.player_relative();
        out.health = in.health();
        out.shield = in.shields();
        out.energy = in.energy();
        out.transportSlotsTaken = in.transport_slots_taken();
        out.buildProgress = in.build_progress();

        // if(in.has_add_on()) out.addOn = toDomain(in.add_on()); // TODO: actually allow this?

        out.healthMax = in.max_health();
        out.shieldMax = in.max_shields();
        out.energyMax = in.max_energy();

        return out;
    }

    inline SinglePanel toDomain(const SC2APIProtocol::SinglePanel& in)
    {
        SinglePanel out{};
        out.unit = toDomain(in.unit());
        out.attackUpgradeLevel = in.attack_upgrade_level();
        out.armorUpgradeLevel = in.armor_upgrade_level();
        out.shieldUpgradeLevel = in.shield_upgrade_level();

        out.buffs.reserve(in.buffs_size());
        for(auto& buff: in.buffs())
        {
            out.buffs.push_back(buff);
        }

        return out;
    }

    inline MultiPanel toDomain(const SC2APIProtocol::MultiPanel& in)
    {
        MultiPanel out{};

        out.units.reserve(in.units_size());
        for(auto& unit: in.units())
        {
            out.units.push_back(toDomain(unit));
        }

        return out;
    }

    inline CargoPanel toDomain(const SC2APIProtocol::CargoPanel& in)
    {
        CargoPanel out{};
        out.unit = toDomain(in.unit());

        out.passengers.reserve(in.passengers_size());
        for(auto& passenger: in.passengers())
        {
            out.passengers.push_back(toDomain(passenger));
        }

        out.slotsAvailable = in.slots_available();

        return out;
    }

    inline BuildItem toDomain(const SC2APIProtocol::BuildItem& in)
    {
        BuildItem out{};
        out.abilityId = in.ability_id();
        out.buildProgress = in.build_progress();

        return out;
    }

    inline ProductionPanel toDomain(const SC2APIProtocol::ProductionPanel& in)
    {
        ProductionPanel out{};
        out.unit = toDomain(in.unit());

        out.buildQueue.reserve(in.build_queue_size());
        for(auto& buildQueue: in.build_queue())
        {
            out.buildQueue.push_back(toDomain(buildQueue));
        }

        out.productionQueue.reserve(in.production_queue_size());
        for(auto& productionQueue: in.production_queue())
        {
            out.productionQueue.push_back(toDomain(productionQueue));
        }

        return out;
    }

    inline ObservationUI toDomain(const SC2APIProtocol::ObservationUI& in)
    {
        ObservationUI out{};

        out.groups.reserve(in.groups_size());
        for(auto& group: in.groups())
        {
            out.groups.push_back(toDomain(group));
        }

        switch (in.panel_case()) {
            case SC2APIProtocol::ObservationUI::kSingle:
                out.panel = toDomain(in.single());
                break;
            case SC2APIProtocol::ObservationUI::kMulti:
                out.panel = toDomain(in.multi());
                break;
            case SC2APIProtocol::ObservationUI::kCargo:
                out.panel = toDomain(in.cargo());
                break;
            case SC2APIProtocol::ObservationUI::kProduction:
                out.panel = toDomain(in.production());
                break;
            case SC2APIProtocol::ObservationUI::PANEL_NOT_SET:
                // nothing needed here
                break;
        }

        return out;
    }

    inline Observation toDomain(const SC2APIProtocol::Observation& in)
    {
        Observation out{};
        out.gameLoop = in.game_loop();
        out.playerCommon = toDomain(in.player_common());

        out.alerts.reserve(in.alerts_size());
        for(int i = 0; i < in.alerts_size(); ++i)
        {
            out.alerts.push_back(toDomain(in.alerts(i)));
        }

        out.score = toDomain(in.score());

        if(in.has_raw_data()) out.rawData = toDomain(in.raw_data());
        if(in.has_feature_layer_data()) out.featureLayerData = toDomain(in.feature_layer_data());
        if(in.has_render_data()) out.renderData = toDomain(in.render_data());
        if(in.has_ui_data()) out.uiData = toDomain(in.ui_data());

        return out;
    }

    inline ChatReceived toDomain(const SC2APIProtocol::ChatReceived& in)
    {
        ChatReceived out{};
        out.playerId = in.player_id();
        out.message = in.message();

        return out;
    }

    inline ObservationResponse toDomain(const SC2APIProtocol::ResponseObservation& in)
    {
        ObservationResponse out{};

        out.actions.reserve(in.actions_size());
        for(auto& action: in.actions())
        {
            out.actions.push_back(toDomain(action));
        }

        out.actionErrors.reserve(in.action_errors_size());
        for(auto& error: in.action_errors())
        {
            out.actionErrors.push_back(toDomain(error));
        }

        out.observation = toDomain(in.observation());

        out.playerResult.resize(in.player_result_size());
        for(auto& result: in.player_result())
        {
            out.playerResult.push_back(toDomain(result));
        }

        out.chat.reserve(in.chat_size());
        for(auto& chat: in.chat())
        {
            out.chat.push_back(toDomain(chat));
        }

        return out;
    }

    inline AbilityData toDomain(const SC2APIProtocol::AbilityData& in)
    {
        AbilityData out{};
        out.abilityId = in.ability_id();
        out.linkName = in.link_name();
        out.linkIndex = in.link_index();
        out.buttonName = in.button_name();
        out.friendlyName = in.friendly_name();
        out.hotkey = in.hotkey();
        out.remapAbilityId = in.remaps_to_ability_id();
        out.available = in.available();
        out.target = toDomain(in.target());
        out.allowMinimap = in.allow_minimap();
        out.allowAutocast = in.allow_autocast();
        out.isBuilding = in.is_building();
        out.footprintRadius = in.footprint_radius();
        out.instantPlacement = in.is_instant_placement();
        out.castRange = in.cast_range();

        return out;
    }

    inline DamageBonus toDomain(const SC2APIProtocol::DamageBonus& in)
    {
        DamageBonus out{};
        out.attribute = toDomain(in.attribute());
        out.bonus = in.bonus();

        return out;
    }

    inline Weapon toDomain(const SC2APIProtocol::Weapon& in)
    {
        Weapon out{};
        out.type = toDomain(in.type());
        out.damage = in.damage();

        out.damageBonus.reserve(in.damage_bonus_size());
        for(auto& damageBonus: in.damage_bonus())
        {
            out.damageBonus.push_back(toDomain(damageBonus));
        }

        out.attacks = in.attacks();
        out.range = in.range();
        out.speed = in.speed();

        return out;
    }

    inline UnitTypeData toDomain(const SC2APIProtocol::UnitTypeData& in)
    {
        UnitTypeData out{};
        out.unitId = in.unit_id();
        out.name = in.name();
        out.available = in.available();
        out.cargoSize = in.cargo_size();
        out.mineralCost = in.mineral_cost();
        out.vespeneCost = in.vespene_cost();
        out.foodRequired = in.food_required();
        out.foodProvided = in.food_provided();
        out.abilityId = in.ability_id();
        out.race = toDomain(in.race());
        out.buildTime = in.build_time();
        out.hasVespene = in.has_vespene();
        out.hasMinerals = in.has_minerals();
        out.sightRange = in.sight_range();

        out.techAlias.reserve(in.tech_alias_size());
        for(auto& techAlias: in.tech_alias())
        {
            out.techAlias.push_back(techAlias);
        }

        out.unitAlias = in.unit_alias();
        out.techRequirement = in.tech_requirement();
        out.requireAttached = in.require_attached();

        out.attributes.reserve(in.attributes_size());
        for(int i = 0; i < in.attributes_size(); ++i)
        {
            out.attributes.push_back(toDomain(in.attributes(i)));
        }

        out.movementSpeed = in.movement_speed();
        out.armor = in.armor();

        out.weapons.reserve(in.weapons_size());
        for(auto& weapon: in.weapons())
        {
            out.weapons.push_back(toDomain(weapon));
        }

        return out;
    }

    inline UpgradeData toDomain(const SC2APIProtocol::UpgradeData& in)
    {
        UpgradeData out{};
        out.upgradeId = in.upgrade_id();
        out.name = in.name();
        out.mineralCost = in.mineral_cost();
        out.vespeneCost = in.vespene_cost();
        out.researchTime = in.research_time();
        out.abilityId = in.ability_id();

        return out;
    }

    inline BuffData toDomain(const SC2APIProtocol::BuffData& in)
    {
        BuffData out{};
        out.buffId = in.buff_id();
        out.name = in.name();

        return out;
    }

    inline EffectData toDomain(const SC2APIProtocol::EffectData& in)
    {
        EffectData out{};
        out.effectId = in.effect_id();
        out.name = in.name();
        out.friendlyName = in.friendly_name();
        out.radius = in.radius();

        return out;
    }

    inline DataResponse toDomain(const SC2APIProtocol::ResponseData& in)
    {
        DataResponse out{};

        out.abilities.reserve(in.abilities_size());
        for(auto& ability: in.abilities())
        {
            out.abilities.push_back(toDomain(ability));
        }

        out.units.reserve(in.units_size());
        for(auto& unit: in.units())
        {
            out.units.push_back(toDomain(unit));
        }

        out.upgrades.reserve(in.upgrades_size());
        for(auto& upgrade: in.upgrades())
        {
            out.upgrades.push_back(toDomain(upgrade));
        }

        out.buffs.reserve(in.buffs_size());
        for(auto& buff: in.buffs())
        {
            out.buffs.push_back(toDomain(buff));
        }

        out.effects.reserve(in.effects_size());
        for(auto& effect: in.effects())
        {
            out.effects.push_back(toDomain(effect));
        }

        return out;
    }

    inline QueryAvailableAbilities toDomain(const SC2APIProtocol::ResponseQueryAvailableAbilities& in)
    {
        QueryAvailableAbilities out{};

        out.abilities.reserve(in.abilities_size());
        for(auto& ability: in.abilities())
        {
            out.abilities.push_back(toDomain(ability));
        }

        out.tag = in.unit_tag();
        out.unitTypeId = in.unit_type_id();

        return out;
    }

    inline QueryResponse toDomain(const SC2APIProtocol::ResponseQuery& in)
    {
        QueryResponse out{};

        out.pathing.reserve(in.pathing_size());
        for(auto& path: in.pathing())
        {
            out.pathing.push_back(path.distance());
        }

        out.abilities.reserve(in.abilities_size());
        for(auto& ability: in.abilities())
        {
            out.abilities.push_back(toDomain(ability));
        }

        out.placement.reserve(in.placements_size());
        for(auto& placement: in.placements())
        {
            out.placement.push_back(toDomain(placement.result()));
        }

        return out;
    }
}
