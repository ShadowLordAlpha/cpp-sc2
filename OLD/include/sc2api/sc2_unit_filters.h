/**
 * @file sc2_unit_filters.h
 * @brief Simple unit filtering functions for StarCraft II API
 *
 * This header provides filtering functions that work with the Filter typedef:
 * `typedef std::function<bool(const Unit&)> Filter;`
 *
 * All functions can be used directly with GetUnits() and other filtering operations.
 *
 * @example
 * ```cpp
 * // Simple usage
 * auto workers = observation->GetUnits(Alliance::Self, IsWorker);
 * auto scvs = observation->GetUnits(Alliance::Self, IsUnit(UNIT_TYPEID::TERRAN_SCV));
 * auto minerals = observation->GetUnits(Alliance::Neutral, IsVisibleMineralPatch);
 *
 * // Combining filters with lambdas
 * auto idle_workers = observation->GetUnits(Alliance::Self, [](const Unit& u) {
 *     return IsWorker(u) && u.orders.empty();
 * });
 * ```
 */
#pragma once

#include "sc2_typeenums.h"
#include "sc2_unit.h"

#include <vector>
#include <algorithm>
#include <functional>
#include <ranges>

namespace sc2
{

    // Basic Type Filters
    // Simple functions for common filtering needs

    /**
     * Create a filter for a specific unit type
     * @param type The unit type to filter for
     * @return Filter function that matches the specified type
     *
     * @example
     * ```cpp
     * auto scvs = observation->GetUnits(Alliance::Self, isUnit(UNIT_TYPEID::TERRAN_SCV));
     * ```
     */
    [[nodiscard]] constexpr auto isUnit(UNIT_TYPEID type) noexcept {
        return [type](const Unit& unit) noexcept -> bool {
            return unit.unit_type == type;
        };
    }

    /**
     * Create a filter for multiple unit types
     * @param types Vector of unit types to match
     * @return Filter function that matches any of the specified types
     *
     * @example
     * ```cpp
     * auto workers = observation->GetUnits(Alliance::Self,
     *     isUnits({UNIT_TYPEID::TERRAN_SCV, UNIT_TYPEID::PROTOSS_PROBE, UNIT_TYPEID::ZERG_DRONE}));
     * ```
     */
    [[nodiscard]] inline auto isUnits(std::vector<UNIT_TYPEID> types) noexcept {
        return [types = std::move(types)](const Unit& unit) noexcept -> bool {
            return std::ranges::any_of(types, [&unit](UNIT_TYPEID type) {
                return unit.unit_type == type;
            });
        };
    }

    /**
     * Create a filter for multiple unit types (initializer list version)
     * @param types Initializer list of unit types
     * @return Filter function that matches any of the specified types
     */
    [[nodiscard]] inline auto isUnits(std::initializer_list<UNIT_TYPEID> types) noexcept {
        return isUnits(std::vector<UNIT_TYPEID>(types));
    }

    // Building Category Filters
    // Functions for specific building categories

    /**
     * Check if a unit type is a town hall
     * @param type Unit type to test
     * @return true if type is a town hall
     */
    [[nodiscard]] constexpr bool isTownHall(UNIT_TYPEID type) noexcept {
        return type == UNIT_TYPEID::PROTOSS_NEXUS ||
               type == UNIT_TYPEID::TERRAN_COMMANDCENTER ||
               type == UNIT_TYPEID::TERRAN_COMMANDCENTERFLYING ||
               type == UNIT_TYPEID::TERRAN_ORBITALCOMMAND ||
               type == UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING ||
               type == UNIT_TYPEID::TERRAN_PLANETARYFORTRESS ||
               type == UNIT_TYPEID::ZERG_HATCHERY ||
               type == UNIT_TYPEID::ZERG_HIVE ||
               type == UNIT_TYPEID::ZERG_LAIR;
    }

    /**
     * Check if a unit is a town hall (command center, nexus, hatchery, etc.)
     * @param unit Unit to test
     * @return true if unit is a town hall
     *
     * @example
     * ```cpp
     * auto bases = observation->GetUnits(Alliance::Self, isTownHall);
     * ```
     */
    [[nodiscard]] constexpr bool isTownHall(const Unit& unit) noexcept {
        return isTownHall(unit.unit_type);
    }

    /**
     * Check if a unit type is a building
     * @param type Unit type to test
     * @return true if type is a building
     */
    [[nodiscard]] constexpr bool isBuilding(UNIT_TYPEID type) noexcept {
        switch (type) {
            // Terran buildings
            case UNIT_TYPEID::TERRAN_ARMORY:
            case UNIT_TYPEID::TERRAN_BARRACKS:
            case UNIT_TYPEID::TERRAN_BARRACKSFLYING:
            case UNIT_TYPEID::TERRAN_BARRACKSREACTOR:
            case UNIT_TYPEID::TERRAN_BARRACKSTECHLAB:
            case UNIT_TYPEID::TERRAN_BUNKER:
            case UNIT_TYPEID::TERRAN_COMMANDCENTER:
            case UNIT_TYPEID::TERRAN_COMMANDCENTERFLYING:
            case UNIT_TYPEID::TERRAN_ENGINEERINGBAY:
            case UNIT_TYPEID::TERRAN_FACTORY:
            case UNIT_TYPEID::TERRAN_FACTORYFLYING:
            case UNIT_TYPEID::TERRAN_FACTORYREACTOR:
            case UNIT_TYPEID::TERRAN_FACTORYTECHLAB:
            case UNIT_TYPEID::TERRAN_FUSIONCORE:
            case UNIT_TYPEID::TERRAN_GHOSTACADEMY:
            case UNIT_TYPEID::TERRAN_MISSILETURRET:
            case UNIT_TYPEID::TERRAN_ORBITALCOMMAND:
            case UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING:
            case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
            case UNIT_TYPEID::TERRAN_REFINERY:
            case UNIT_TYPEID::TERRAN_SENSORTOWER:
            case UNIT_TYPEID::TERRAN_STARPORT:
            case UNIT_TYPEID::TERRAN_STARPORTFLYING:
            case UNIT_TYPEID::TERRAN_STARPORTREACTOR:
            case UNIT_TYPEID::TERRAN_STARPORTTECHLAB:
            case UNIT_TYPEID::TERRAN_SUPPLYDEPOT:
            case UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED:
            case UNIT_TYPEID::TERRAN_REACTOR:
            case UNIT_TYPEID::TERRAN_TECHLAB:

                // Zerg buildings
            case UNIT_TYPEID::ZERG_BANELINGNEST:
            case UNIT_TYPEID::ZERG_CREEPTUMOR:
            case UNIT_TYPEID::ZERG_CREEPTUMORBURROWED:
            case UNIT_TYPEID::ZERG_CREEPTUMORQUEEN:
            case UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER:
            case UNIT_TYPEID::ZERG_EXTRACTOR:
            case UNIT_TYPEID::ZERG_GREATERSPIRE:
            case UNIT_TYPEID::ZERG_HATCHERY:
            case UNIT_TYPEID::ZERG_HIVE:
            case UNIT_TYPEID::ZERG_HYDRALISKDEN:
            case UNIT_TYPEID::ZERG_INFESTATIONPIT:
            case UNIT_TYPEID::ZERG_LAIR:
            case UNIT_TYPEID::ZERG_LURKERDENMP:
            case UNIT_TYPEID::ZERG_NYDUSCANAL:
            case UNIT_TYPEID::ZERG_NYDUSNETWORK:
            case UNIT_TYPEID::ZERG_ROACHWARREN:
            case UNIT_TYPEID::ZERG_SPAWNINGPOOL:
            case UNIT_TYPEID::ZERG_SPINECRAWLER:
            case UNIT_TYPEID::ZERG_SPINECRAWLERUPROOTED:
            case UNIT_TYPEID::ZERG_SPIRE:
            case UNIT_TYPEID::ZERG_SPORECRAWLER:
            case UNIT_TYPEID::ZERG_SPORECRAWLERUPROOTED:
            case UNIT_TYPEID::ZERG_ULTRALISKCAVERN:

                // Protoss buildings
            case UNIT_TYPEID::PROTOSS_ASSIMILATOR:
            case UNIT_TYPEID::PROTOSS_CYBERNETICSCORE:
            case UNIT_TYPEID::PROTOSS_DARKSHRINE:
            case UNIT_TYPEID::PROTOSS_FLEETBEACON:
            case UNIT_TYPEID::PROTOSS_FORGE:
            case UNIT_TYPEID::PROTOSS_GATEWAY:
            case UNIT_TYPEID::PROTOSS_NEXUS:
            case UNIT_TYPEID::PROTOSS_PHOTONCANNON:
            case UNIT_TYPEID::PROTOSS_PYLON:
            case UNIT_TYPEID::PROTOSS_PYLONOVERCHARGED:
            case UNIT_TYPEID::PROTOSS_ROBOTICSBAY:
            case UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY:
            case UNIT_TYPEID::PROTOSS_STARGATE:
            case UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE:
            case UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL:
            case UNIT_TYPEID::PROTOSS_WARPGATE:
            case UNIT_TYPEID::PROTOSS_SHIELDBATTERY:
                return true;

            default:
                return false;
        }
    }

    /**
     * Check if a unit is a building structure
     * @param unit Unit to test
     * @return true if unit is a building
     *
     * @example
     * ```cpp
     * auto buildings = observation->GetUnits(Alliance::Self, isBuilding);
     * ```
     */
    [[nodiscard]] constexpr bool isBuilding(const Unit& unit) noexcept {
        return isBuilding(unit.unit_type);
    }

    // Resource Filters
    // Functions for resource nodes and gatherers

    /**
     * Check if a unit type is a mineral patch
     * @param type Unit type to test
     * @return true if type is a mineral patch
     */
    [[nodiscard]] constexpr bool isMineralPatch(UNIT_TYPEID type) noexcept {
        return type == UNIT_TYPEID::NEUTRAL_BATTLESTATIONMINERALFIELD750 ||
               type == UNIT_TYPEID::NEUTRAL_BATTLESTATIONMINERALFIELD ||
               type == UNIT_TYPEID::NEUTRAL_LABMINERALFIELD750 ||
               type == UNIT_TYPEID::NEUTRAL_LABMINERALFIELD ||
               type == UNIT_TYPEID::NEUTRAL_MINERALFIELD750 ||
               type == UNIT_TYPEID::NEUTRAL_MINERALFIELD ||
               type == UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD750 ||
               type == UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD ||
               type == UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD750 ||
               type == UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD ||
               type == UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD750 ||
               type == UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD;
    }

    /**
     * Check if a unit is a mineral patch (including those under fog of war)
     * @param unit Unit to test
     * @return true if unit is a mineral patch
     *
     * @example
     * ```cpp
     * auto all_minerals = observation->GetUnits(Alliance::Neutral, isMineralPatch);
     * ```
     */
    [[nodiscard]] constexpr bool isMineralPatch(const Unit& unit) noexcept {
        return isMineralPatch(unit.unit_type);
    }

    /**
     * Check if a unit is a visible mineral patch with harvestable resources
     * @param unit Unit to test
     * @return true if unit has mineral contents > 0
     *
     * @example
     * ```cpp
     * auto harvestable_minerals = observation->GetUnits(Alliance::Neutral, isVisibleMineralPatch);
     * ```
     */
    [[nodiscard]] constexpr bool isVisibleMineralPatch(const Unit& unit) noexcept {
        return unit.mineral_contents > 0;
    }

    /**
     * Check if a unit type is a vespene geyser
     * @param type Unit type to test
     * @return true if type is a geyser
     */
    [[nodiscard]] constexpr bool isGeyser(UNIT_TYPEID type) noexcept {
        return type == UNIT_TYPEID::NEUTRAL_VESPENEGEYSER ||
               type == UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER ||
               type == UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER ||
               type == UNIT_TYPEID::NEUTRAL_PURIFIERVESPENEGEYSER ||
               type == UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER ||
               type == UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER;
    }

    /**
     * Check if a unit is a vespene geyser (including those under fog of war)
     * @param unit Unit to test
     * @return true if unit is a geyser
     *
     * @example
     * ```cpp
     * auto all_geysers = observation->GetUnits(Alliance::Neutral, isGeyser);
     * ```
     */
    [[nodiscard]] constexpr bool isGeyser(const Unit& unit) noexcept {
        return isGeyser(unit.unit_type);
    }

    /**
     * Check if a unit is a visible geyser with harvestable vespene
     * @param unit Unit to test
     * @return true if unit has vespene contents > 0
     *
     * @example
     * ```cpp
     * auto harvestable_geysers = observation->GetUnits(Alliance::Neutral, isVisibleGeyser);
     * ```
     */
    [[nodiscard]] constexpr bool isVisibleGeyser(const Unit& unit) noexcept {
        return unit.vespene_contents > 0;
    }

    // Unit Category Filters
    // Functions for broad unit categories

    /**
     * Check if a unit type is a worker
     * @param type Unit type to test
     * @return true if type is a worker
     */
    [[nodiscard]] constexpr bool isWorker(UNIT_TYPEID type) noexcept {
        return type == UNIT_TYPEID::TERRAN_SCV ||
               type == UNIT_TYPEID::ZERG_DRONE ||
               type == UNIT_TYPEID::PROTOSS_PROBE;
    }

    /**
     * Check if a unit is a worker (SCV, Probe, or Drone)
     * @param unit Unit to test
     * @return true if unit is a worker
     *
     * @example
     * ```cpp
     * auto workers = observation->GetUnits(Alliance::Self, isWorker);
     * ```
     */
    [[nodiscard]] constexpr bool isWorker(const Unit& unit) noexcept {
        return isWorker(unit.unit_type);
    }

    // Visibility Filters
    // Functions for unit visibility status

    /**
     * Check if a unit is visible (not a snapshot or placeholder)
     * @param unit Unit to test
     * @return true if unit display type is Visible
     *
     * @example
     * ```cpp
     * auto visible_enemies = observation->GetUnits(Alliance::Enemy, isVisible);
     * ```
     */
    [[nodiscard]] constexpr bool isVisible(const Unit& unit) noexcept {
        return unit.display_type == Unit::Visible;
    }

    // Resource Carrying Status
    // Functions for checking what units are carrying

    /**
     * Check if a unit is carrying minerals
     * @param unit Unit to test
     * @return true if the unit is carrying minerals
     *
     * @example
     * ```cpp
     * auto mineral_carriers = observation->GetUnits(Alliance::Self, isCarryingMinerals);
     * ```
     */
    [[nodiscard]] inline bool isCarryingMinerals(const Unit& unit) noexcept {
        // Note: lambda cannot be constexpr because BuffID is not a literal type
        auto is_mineral_buff = [](BuffID buff) noexcept -> bool {
            return buff == BUFF_ID::CARRYMINERALFIELDMINERALS ||
                   buff == BUFF_ID::CARRYHIGHYIELDMINERALFIELDMINERALS;
        };

        return std::ranges::any_of(unit.buffs, is_mineral_buff);
    }

    /**
     * Check if a unit is carrying vespene gas
     * @param unit Unit to test
     * @return true if the unit is carrying vespene
     *
     * @example
     * ```cpp
     * auto gas_carriers = observation->GetUnits(Alliance::Self, isCarryingVespene);
     * ```
     */
    [[nodiscard]] inline bool isCarryingVespene(const Unit& unit) noexcept {
        // Note: lambda cannot be constexpr because BuffID is not a literal type
        auto is_vespene_buff = [](BuffID buff) noexcept -> bool {
            return buff == BUFF_ID::CARRYHARVESTABLEVESPENEGEYSERGAS ||
                   buff == BUFF_ID::CARRYHARVESTABLEVESPENEGEYSERGASPROTOSS ||
                   buff == BUFF_ID::CARRYHARVESTABLEVESPENEGEYSERGASZERG;
        };

        return std::ranges::any_of(unit.buffs, is_vespene_buff);
    }
} // namespace sc2
