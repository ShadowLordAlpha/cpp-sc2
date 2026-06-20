#include "sc2api/sc2_data.h"
#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_proto_to_pods.h"

#include <iostream>
#include <cassert>

#include "s2clientprotocol/sc2api.pb.h"

// TODO: Fill out the log functions

namespace sc2
{

    void AbilityData::ReadFromProto(const SC2APIProtocol::AbilityData &ability_data)
    {
        // ability_id_
        if (ability_data.has_ability_id())
        {
            ability_id = ability_data.ability_id();
        }

        // available_
        available = false;
        if (ability_data.has_available())
        {
            available = ability_data.available();
        }

        // link_name_
        if (ability_data.has_link_name())
        {
            link_name = ability_data.link_name();
        }

        // link_index_
        link_index = 0;
        if (ability_data.has_link_index())
        {
            link_index = ability_data.link_index();
        }

        // button_name_
        if (ability_data.has_button_name())
        {
            button_name = ability_data.button_name();
        }

        // friendly_name_
        if (ability_data.has_friendly_name())
        {
            friendly_name = ability_data.friendly_name();
        }

        // hotkey_
        if (ability_data.has_hotkey())
        {
            hotkey = ability_data.hotkey();
        }

        // Remaps.
        remaps_to_ability_id = 0;
        if (ability_data.has_remaps_to_ability_id())
        {
            remaps_to_ability_id = ability_data.remaps_to_ability_id();
        }

        // target_
        if (ability_data.has_target())
        {
            switch (ability_data.target())
            {
                case SC2APIProtocol::AbilityData_Target_Point:
                {
                    target = Target::Point;
                    break;
                }
                case SC2APIProtocol::AbilityData_Target_Unit:
                {
                    target = Target::Unit;
                    break;
                }
                case SC2APIProtocol::AbilityData_Target_PointOrUnit:
                {
                    target = Target::PointOrUnit;
                    break;
                }
                case SC2APIProtocol::AbilityData_Target_PointOrNone:
                {
                    target = Target::PointOrNone;
                    break;
                }
                case SC2APIProtocol::AbilityData_Target_None:
                default:
                {
                    target = Target::None;
                    break;
                }
            }
        }

        // allow_minimap_
        allow_minimap = 0;
        if (ability_data.has_allow_minimap())
        {
            allow_minimap = ability_data.allow_minimap();
        }

        // allow_minimap_
        allow_autocast = 0;
        if (ability_data.has_allow_autocast())
        {
            allow_autocast = ability_data.allow_autocast();
        }

        // is_building_
        is_building = false;
        if (ability_data.has_is_building())
        {
            is_building = ability_data.is_building();
        }

        // footprint_radius_
        footprint_radius = 0.0f;
        if (ability_data.has_footprint_radius())
        {
            footprint_radius = ability_data.footprint_radius();
        }

        is_instant_placement = false;
        if (ability_data.has_is_instant_placement())
        {
            is_instant_placement = ability_data.has_is_instant_placement();
        }

        cast_range = 0.0f;
        if (ability_data.has_cast_range())
        {
            cast_range = ability_data.cast_range();
        }
    }

    static Attribute ConvertAttributeEnum(SC2APIProtocol::Attribute attribute)
    {
        switch (attribute)
        {
            case SC2APIProtocol::Attribute::Light: return Attribute::Light;
            case SC2APIProtocol::Attribute::Armored: return Attribute::Armored;
            case SC2APIProtocol::Attribute::Biological: return Attribute::Biological;
            case SC2APIProtocol::Attribute::Mechanical: return Attribute::Mechanical;
            case SC2APIProtocol::Attribute::Robotic: return Attribute::Robotic;
            case SC2APIProtocol::Attribute::Psionic: return Attribute::Psionic;
            case SC2APIProtocol::Attribute::Massive: return Attribute::Massive;
            case SC2APIProtocol::Attribute::Structure: return Attribute::Structure;
            case SC2APIProtocol::Attribute::Hover: return Attribute::Hover;
            case SC2APIProtocol::Attribute::Heroic: return Attribute::Heroic;
            case SC2APIProtocol::Attribute::Summoned: return Attribute::Summoned;
            default: return Attribute::Invalid;
        }
    }

    static Weapon::TargetType ConvertTargetTypeEnum(SC2APIProtocol::Weapon::TargetType type)
    {
        switch (type)
        {
            case SC2APIProtocol::Weapon::Ground: return Weapon::TargetType::Ground;
            case SC2APIProtocol::Weapon::Air: return Weapon::TargetType::Air;
            case SC2APIProtocol::Weapon::Any: return Weapon::TargetType::Any;
            default: return Weapon::TargetType::Invalid;
        }
    }

    static Weapon weaponFromProto(const SC2APIProtocol::Weapon &weapon)
    {
        std::vector<DamageBonus> bonuses;
        bonuses.reserve(weapon.damage_bonus_size());
        for (int i = 0; i < weapon.damage_bonus_size(); ++i)
        {
            const auto& bonus = weapon.damage_bonus(i);
            bonuses.emplace_back(ConvertAttributeEnum(bonus.attribute()), bonus.bonus());
        }

        return {ConvertTargetTypeEnum(weapon.type()), weapon.damage(), bonuses, weapon.attacks(), weapon.range(), weapon.speed()};
    }

    UnitTypeData::UnitTypeData() {}

    void UnitTypeData::ReadFromProto(const SC2APIProtocol::UnitTypeData &unit_data)
    {
        // unit_type_id_
        unit_type_id = unit_data.unit_id();

        // name_
        name = unit_data.name();

        // available_
        available = unit_data.available();

        // cargo_size_
        cargo_size = unit_data.cargo_size();

        // mineral_cost_
        mineral_cost = unit_data.mineral_cost();

        // vespene_cost_
        vespene_cost = unit_data.vespene_cost();

        // attribute_
        for (int i = 0; i < unit_data.attributes_size(); ++i)
        {
            attributes.push_back(ConvertAttributeEnum(unit_data.attributes(i)));
        }

        // movement_speed_
        movement_speed = unit_data.movement_speed();

        // armor_
        armor = unit_data.armor();

        // weapons_
        weapons.reserve(unit_data.weapons_size());
        for (int i = 0; i < unit_data.weapons_size(); ++i)
        {
            weapons.emplace_back(weaponFromProto(unit_data.weapons(i)));
        }

        food_provided = unit_data.food_provided();

        food_required = unit_data.food_required();

        ability_id = unit_data.ability_id();

        race = ConvertRaceFromProto(unit_data.race());

        build_time = unit_data.build_time();

        has_minerals = unit_data.has_minerals();

        has_vespene = unit_data.has_vespene();

        sight_range = unit_data.sight_range();

        for (int i = 0; i < unit_data.tech_alias_size(); ++i)
        {
            tech_alias.push_back(unit_data.tech_alias(i));
        }

        unit_alias = unit_data.unit_alias();

        tech_requirement = unit_data.tech_requirement();

        require_attached = unit_data.require_attached();
    }

    std::string UnitTypeData::Log() const
    {
        std::string str_out;

        str_out = unit_type_id.to_string() + ":\n";
        str_out += "  " + (name.length() > 0 ? name : "Null") + "\n";

        return str_out;
    }

    void UpgradeData::ReadFromProto(const SC2APIProtocol::UpgradeData &upgrade_data)
    {
        // upgrade_id_
        upgrade_id = upgrade_data.upgrade_id();

        // name_
        name = upgrade_data.name();

        mineral_cost = upgrade_data.mineral_cost();
        vespene_cost = upgrade_data.vespene_cost();
        ability_id = upgrade_data.ability_id();
        research_time = upgrade_data.research_time();
    }

    std::string UpgradeData::Log() const
    {
        std::string str_out;

        return str_out;
    }

    void BuffData::ReadFromProto(const SC2APIProtocol::BuffData &buff_data)
    {
        // upgrade_id_
        buff_id = buff_data.buff_id();

        // name_
        name = buff_data.name();
    }

    std::string BuffData::Log() const
    {
        std::string str_out;

        return str_out;
    }

    void EffectData::ReadFromProto(const SC2APIProtocol::EffectData &effect_data)
    {
        effect_id = effect_data.effect_id();
        name = effect_data.name();
        friendly_name = effect_data.friendly_name();
        radius = effect_data.radius();
    }

    std::string EffectData::Log() const
    {
        std::string str_out;

        return str_out;
    }

    void Effect::ReadFromProto(const SC2APIProtocol::Effect &effect)
    {
        effect_id = effect.effect_id();
        for (int i = 0; i < effect.pos_size(); ++i)
        {
            const SC2APIProtocol::Point2D &pos = effect.pos(i);
            positions.push_back(Point2D(pos.x(), pos.y()));
        }
    }

    AbilityID GetGeneralizedAbilityID(uint32_t ability_id, const ObservationInterface &observation)
    {
        if (ability_id == 0)
        {
            return AbilityID(ability_id);
        }

        const Abilities &abilities = observation.GetAbilityData();
        if (ability_id >= abilities.size())
        {
            assert(0);
            return AbilityID(ability_id);
        }

        const AbilityData &ability = abilities[ability_id];
        if (ability.remaps_to_ability_id != 0)
        {
            return AbilityID(ability.remaps_to_ability_id);
        }

        return AbilityID(ability_id);
    }
}
