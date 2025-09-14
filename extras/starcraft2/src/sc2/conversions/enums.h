/**
 *
 */

#pragma once

#include <optional>
#include <array>
#include <cassert>
#include "sc2/api/common.h"
#include "sc2/actions/chat_actions.h"
#include "s2clientprotocol/sc2api.pb.h"

namespace astra::sc2
{

#ifndef NDEBUG
#define ASTRA_ENUM_REQUIRE(mappedOpt, what) do { assert((mappedOpt).has_value() && "Missing enum mapping: " what); } while(0)
#else
#define ASTRA_ENUM_REQUIRE(mappedOpt, what)
#endif

#define ASTRA_ENUM_CONVERSION_DOMAIN(from, to, default_to, map) \
    inline to toDomain(from value) { \
        auto m = enum_map_value(value, map); \
        ASTRA_ENUM_REQUIRE(m, "enum proto->domain"); \
        return m.value_or(default_to);       \
    }

// TODO: maybe we need to have optionals as well instead of just defaults?
#define ASTRA_ENUM_CONVERSION(from, to, default_to, default_from, map) \
    ASTRA_ENUM_CONVERSION_DOMAIN(from, to, default_to, map) \
    inline from toProto(to value) { \
        auto m = enum_map_reverse(value, map); \
        ASTRA_ENUM_REQUIRE(m, "enum proto->domain"); \
        return m.value_or(default_from); \
    }

    // Generic lookup using a constexpr mapping table (pairs of From -> To).
    template <class From, class To, size_t N>
    constexpr std::optional<To> enum_map_value(From value, const std::array<std::pair<From, To>, N>& table) {
        for (auto&& p : table) {
            if (p.first == value) return p.second;
        }
        return std::nullopt;
    }

    // Bidirectional lookup helpers, if you want reverse mapping without duplicating the table.
    // Note: O(N); if you care about hot-path performance, generate two tables or a switch for specific enums.
    template <class From, class To, size_t N>
    constexpr std::optional<From> enum_map_reverse(To value, const std::array<std::pair<From, To>, N>& table) {
        for (auto&& p : table) {
            if (p.second == value) return p.first;
        }
        return std::nullopt;
    }

    static constexpr std::array<std::pair<SC2APIProtocol::Race, Race>, 5> RACE_MAP {{
            {SC2APIProtocol::Race::NoRace, Race::NoRace},
            {SC2APIProtocol::Race::Terran, Race::Terran},
            {SC2APIProtocol::Race::Zerg, Race::Zerg},
            {SC2APIProtocol::Race::Protoss, Race::Protoss},
            {SC2APIProtocol::Race::Random, Race::Random}
    }};

    ASTRA_ENUM_CONVERSION(SC2APIProtocol::Race, Race, Race::NoRace, SC2APIProtocol::Race::NoRace, RACE_MAP)

    static constexpr std::array<std::pair<SC2APIProtocol::PlayerType, PlayerType>, 3> PLAYER_TYPE_MAP {{
            {SC2APIProtocol::PlayerType::Participant, PlayerType::Participant},
            {SC2APIProtocol::PlayerType::Computer, PlayerType::Computer},
            {SC2APIProtocol::PlayerType::Observer, PlayerType::Observer}
    }};

    ASTRA_ENUM_CONVERSION(SC2APIProtocol::PlayerType, PlayerType, PlayerType::Participant, SC2APIProtocol::PlayerType::Participant, PLAYER_TYPE_MAP)

    static constexpr std::array<std::pair<SC2APIProtocol::Difficulty, Difficulty>, 10> DIFFICULTY_MAP {{
        {SC2APIProtocol::Difficulty::VeryEasy, Difficulty::VeryEasy},
        {SC2APIProtocol::Difficulty::Easy, Difficulty::Easy},
        {SC2APIProtocol::Difficulty::Medium, Difficulty::Medium},
        {SC2APIProtocol::Difficulty::MediumHard, Difficulty::MediumHard},
        {SC2APIProtocol::Difficulty::Hard, Difficulty::Hard},
        {SC2APIProtocol::Difficulty::Harder, Difficulty::Harder},
        {SC2APIProtocol::Difficulty::VeryHard, Difficulty::VeryHard},
        {SC2APIProtocol::Difficulty::CheatVision, Difficulty::CheatVision},
        {SC2APIProtocol::Difficulty::CheatMoney, Difficulty::CheatMoney},
        {SC2APIProtocol::Difficulty::CheatInsane, Difficulty::CheatInsane}
    }};

    ASTRA_ENUM_CONVERSION(SC2APIProtocol::Difficulty, Difficulty, Difficulty::Medium, SC2APIProtocol::Difficulty::Medium, DIFFICULTY_MAP)

    static constexpr std::array<std::pair<SC2APIProtocol::Result, PlayerResult::Result>, 4> RESULT_MAP {{
        {SC2APIProtocol::Result::Victory, PlayerResult::Result::Victory},
        {SC2APIProtocol::Result::Defeat, PlayerResult::Result::Defeat},
        {SC2APIProtocol::Result::Tie, PlayerResult::Result::Tie},
        {SC2APIProtocol::Result::Undecided, PlayerResult::Result::Undecided}
    }};

    ASTRA_ENUM_CONVERSION(SC2APIProtocol::Result, PlayerResult::Result, PlayerResult::Result::Undecided, SC2APIProtocol::Result::Undecided, RESULT_MAP)

    static constexpr std::array<std::pair<SC2APIProtocol::AIBuild, AIBuild>, 6> AI_BUILD_MAP {{
        {SC2APIProtocol::AIBuild::RandomBuild, AIBuild::RandomBuild},
        {SC2APIProtocol::AIBuild::Rush, AIBuild::Rush},
        {SC2APIProtocol::AIBuild::Timing, AIBuild::Timing},
        {SC2APIProtocol::AIBuild::Power, AIBuild::Power},
        {SC2APIProtocol::AIBuild::Macro, AIBuild::Macro},
        {SC2APIProtocol::AIBuild::Air, AIBuild::Air}
    }};

    ASTRA_ENUM_CONVERSION(SC2APIProtocol::AIBuild, AIBuild, AIBuild::RandomBuild, SC2APIProtocol::AIBuild::RandomBuild, AI_BUILD_MAP)

    static constexpr std::array<std::pair<SC2APIProtocol::ActionResult, ActionResult>, 214> ACTION_RESULT_MAP {{
        {SC2APIProtocol::Success, ActionResult::Success},
        {SC2APIProtocol::NotSupported, ActionResult::NotSupported},
        {SC2APIProtocol::Error, ActionResult::Error},
        {SC2APIProtocol::CantQueueThatOrder, ActionResult::CantQueueThatOrder},
        {SC2APIProtocol::Retry, ActionResult::Retry},
        {SC2APIProtocol::Cooldown, ActionResult::Cooldown},
        {SC2APIProtocol::QueueIsFull, ActionResult::QueueIsFull},
        {SC2APIProtocol::RallyQueueIsFull, ActionResult::RallyQueueIsFull},
        {SC2APIProtocol::NotEnoughMinerals, ActionResult::NotEnoughMinerals},
        {SC2APIProtocol::NotEnoughVespene, ActionResult::NotEnoughVespene},
        {SC2APIProtocol::NotEnoughTerrazine, ActionResult::NotEnoughTerrazine},
        {SC2APIProtocol::NotEnoughCustom, ActionResult::NotEnoughCustom},
        {SC2APIProtocol::NotEnoughFood, ActionResult::NotEnoughFood},
        {SC2APIProtocol::FoodUsageImpossible, ActionResult::FoodUsageImpossible},
        {SC2APIProtocol::NotEnoughLife, ActionResult::NotEnoughLife},
        {SC2APIProtocol::NotEnoughShields, ActionResult::NotEnoughShields},
        {SC2APIProtocol::NotEnoughEnergy, ActionResult::NotEnoughEnergy},
        {SC2APIProtocol::LifeSuppressed, ActionResult::LifeSuppressed},
        {SC2APIProtocol::ShieldsSuppressed, ActionResult::ShieldsSuppressed},
        {SC2APIProtocol::EnergySuppressed, ActionResult::EnergySuppressed},
        {SC2APIProtocol::NotEnoughCharges, ActionResult::NotEnoughCharges},
        {SC2APIProtocol::CantAddMoreCharges, ActionResult::CantAddMoreCharges},
        {SC2APIProtocol::TooMuchMinerals, ActionResult::TooMuchMinerals},
        {SC2APIProtocol::TooMuchVespene, ActionResult::TooMuchVespene},
        {SC2APIProtocol::TooMuchTerrazine, ActionResult::TooMuchTerrazine},
        {SC2APIProtocol::TooMuchCustom, ActionResult::TooMuchCustom},
        {SC2APIProtocol::TooMuchFood, ActionResult::TooMuchFood},
        {SC2APIProtocol::TooMuchLife, ActionResult::TooMuchLife},
        {SC2APIProtocol::TooMuchShields, ActionResult::TooMuchShields},
        {SC2APIProtocol::TooMuchEnergy, ActionResult::TooMuchEnergy},
        {SC2APIProtocol::MustTargetUnitWithLife, ActionResult::MustTargetUnitWithLife},
        {SC2APIProtocol::MustTargetUnitWithShields, ActionResult::MustTargetUnitWithShields},
        {SC2APIProtocol::MustTargetUnitWithEnergy, ActionResult::MustTargetUnitWithEnergy},
        {SC2APIProtocol::CantTrade, ActionResult::CantTrade},
        {SC2APIProtocol::CantSpend, ActionResult::CantSpend},
        {SC2APIProtocol::CantTargetThatUnit, ActionResult::CantTargetThatUnit},
        {SC2APIProtocol::CouldntAllocateUnit, ActionResult::CouldntAllocateUnit},
        {SC2APIProtocol::UnitCantMove, ActionResult::UnitCantMove},
        {SC2APIProtocol::TransportIsHoldingPosition, ActionResult::TransportIsHoldingPosition},
        {SC2APIProtocol::BuildTechRequirementsNotMet, ActionResult::BuildTechRequirementsNotMet},
        {SC2APIProtocol::CantFindPlacementLocation, ActionResult::CantFindPlacementLocation},
        {SC2APIProtocol::CantBuildOnThat, ActionResult::CantBuildOnThat},
        {SC2APIProtocol::CantBuildTooCloseToDropOff, ActionResult::CantBuildTooCloseToDropOff},
        {SC2APIProtocol::CantBuildLocationInvalid, ActionResult::CantBuildLocationInvalid},
        {SC2APIProtocol::CantSeeBuildLocation, ActionResult::CantSeeBuildLocation},
        {SC2APIProtocol::CantBuildTooCloseToCreepSource, ActionResult::CantBuildTooCloseToCreepSource},
        {SC2APIProtocol::CantBuildTooCloseToResources, ActionResult::CantBuildTooCloseToResources},
        {SC2APIProtocol::CantBuildTooFarFromWater, ActionResult::CantBuildTooFarFromWater},
        {SC2APIProtocol::CantBuildTooFarFromCreepSource, ActionResult::CantBuildTooFarFromCreepSource},
        {SC2APIProtocol::CantBuildTooFarFromBuildPowerSource, ActionResult::CantBuildTooFarFromBuildPowerSource},
        {SC2APIProtocol::CantBuildOnDenseTerrain, ActionResult::CantBuildOnDenseTerrain},
        {SC2APIProtocol::CantTrainTooFarFromTrainPowerSource, ActionResult::CantTrainTooFarFromTrainPowerSource},
        {SC2APIProtocol::CantLandLocationInvalid, ActionResult::CantLandLocationInvalid},
        {SC2APIProtocol::CantSeeLandLocation, ActionResult::CantSeeLandLocation},
        {SC2APIProtocol::CantLandTooCloseToCreepSource, ActionResult::CantLandTooCloseToCreepSource},
        {SC2APIProtocol::CantLandTooCloseToResources, ActionResult::CantLandTooCloseToResources},
        {SC2APIProtocol::CantLandTooFarFromWater, ActionResult::CantLandTooFarFromWater},
        {SC2APIProtocol::CantLandTooFarFromCreepSource, ActionResult::CantLandTooFarFromCreepSource},
        {SC2APIProtocol::CantLandTooFarFromBuildPowerSource, ActionResult::CantLandTooFarFromBuildPowerSource},
        {SC2APIProtocol::CantLandTooFarFromTrainPowerSource, ActionResult::CantLandTooFarFromTrainPowerSource},
        {SC2APIProtocol::CantLandOnDenseTerrain, ActionResult::CantLandOnDenseTerrain},
        {SC2APIProtocol::AddOnTooFarFromBuilding, ActionResult::AddOnTooFarFromBuilding},
        {SC2APIProtocol::MustBuildRefineryFirst, ActionResult::MustBuildRefineryFirst},
        {SC2APIProtocol::BuildingIsUnderConstruction, ActionResult::BuildingIsUnderConstruction},
        {SC2APIProtocol::CantFindDropOff, ActionResult::CantFindDropOff},
        {SC2APIProtocol::CantLoadOtherPlayersUnits, ActionResult::CantLoadOtherPlayersUnits},
        {SC2APIProtocol::NotEnoughRoomToLoadUnit, ActionResult::NotEnoughRoomToLoadUnit},
        {SC2APIProtocol::CantUnloadUnitsThere, ActionResult::CantUnloadUnitsThere},
        {SC2APIProtocol::CantWarpInUnitsThere, ActionResult::CantWarpInUnitsThere},
        {SC2APIProtocol::CantLoadImmobileUnits, ActionResult::CantLoadImmobileUnits},
        {SC2APIProtocol::CantRechargeImmobileUnits, ActionResult::CantRechargeImmobileUnits},
        {SC2APIProtocol::CantRechargeUnderConstructionUnits, ActionResult::CantRechargeUnderConstructionUnits},
        {SC2APIProtocol::CantLoadThatUnit, ActionResult::CantLoadThatUnit},
        {SC2APIProtocol::NoCargoToUnload, ActionResult::NoCargoToUnload},
        {SC2APIProtocol::LoadAllNoTargetsFound, ActionResult::LoadAllNoTargetsFound},
        {SC2APIProtocol::NotWhileOccupied, ActionResult::NotWhileOccupied},
        {SC2APIProtocol::CantAttackWithoutAmmo, ActionResult::CantAttackWithoutAmmo},
        {SC2APIProtocol::CantHoldAnyMoreAmmo, ActionResult::CantHoldAnyMoreAmmo},
        {SC2APIProtocol::TechRequirementsNotMet, ActionResult::TechRequirementsNotMet},
        {SC2APIProtocol::MustLockdownUnitFirst, ActionResult::MustLockdownUnitFirst},
        {SC2APIProtocol::MustTargetUnit, ActionResult::MustTargetUnit},
        {SC2APIProtocol::MustTargetInventory, ActionResult::MustTargetInventory},
        {SC2APIProtocol::MustTargetVisibleUnit, ActionResult::MustTargetVisibleUnit},
        {SC2APIProtocol::MustTargetVisibleLocation, ActionResult::MustTargetVisibleLocation},
        {SC2APIProtocol::MustTargetWalkableLocation, ActionResult::MustTargetWalkableLocation},
        {SC2APIProtocol::MustTargetPawnableUnit, ActionResult::MustTargetPawnableUnit},
        {SC2APIProtocol::YouCantControlThatUnit, ActionResult::YouCantControlThatUnit},
        {SC2APIProtocol::YouCantIssueCommandsToThatUnit, ActionResult::YouCantIssueCommandsToThatUnit},
        {SC2APIProtocol::MustTargetResources, ActionResult::MustTargetResources},
        {SC2APIProtocol::RequiresHealTarget, ActionResult::RequiresHealTarget},
        {SC2APIProtocol::RequiresRepairTarget, ActionResult::RequiresRepairTarget},
        {SC2APIProtocol::NoItemsToDrop, ActionResult::NoItemsToDrop},
        {SC2APIProtocol::CantHoldAnyMoreItems, ActionResult::CantHoldAnyMoreItems},
        {SC2APIProtocol::CantHoldThat, ActionResult::CantHoldThat},
        {SC2APIProtocol::TargetHasNoInventory, ActionResult::TargetHasNoInventory},
        {SC2APIProtocol::CantDropThisItem, ActionResult::CantDropThisItem},
        {SC2APIProtocol::CantMoveThisItem, ActionResult::CantMoveThisItem},
        {SC2APIProtocol::CantPawnThisUnit, ActionResult::CantPawnThisUnit},
        {SC2APIProtocol::MustTargetCaster, ActionResult::MustTargetCaster},
        {SC2APIProtocol::CantTargetCaster, ActionResult::CantTargetCaster},
        {SC2APIProtocol::MustTargetOuter, ActionResult::MustTargetOuter},
        {SC2APIProtocol::CantTargetOuter, ActionResult::CantTargetOuter},
        {SC2APIProtocol::MustTargetYourOwnUnits, ActionResult::MustTargetYourOwnUnits},
        {SC2APIProtocol::CantTargetYourOwnUnits, ActionResult::CantTargetYourOwnUnits},
        {SC2APIProtocol::MustTargetFriendlyUnits, ActionResult::MustTargetFriendlyUnits},
        {SC2APIProtocol::CantTargetFriendlyUnits, ActionResult::CantTargetFriendlyUnits},
        {SC2APIProtocol::MustTargetNeutralUnits, ActionResult::MustTargetNeutralUnits},
        {SC2APIProtocol::CantTargetNeutralUnits, ActionResult::CantTargetNeutralUnits},
        {SC2APIProtocol::MustTargetEnemyUnits, ActionResult::MustTargetEnemyUnits},
        {SC2APIProtocol::CantTargetEnemyUnits, ActionResult::CantTargetEnemyUnits},
        {SC2APIProtocol::MustTargetAirUnits, ActionResult::MustTargetAirUnits},
        {SC2APIProtocol::CantTargetAirUnits, ActionResult::CantTargetAirUnits},
        {SC2APIProtocol::MustTargetGroundUnits, ActionResult::MustTargetGroundUnits},
        {SC2APIProtocol::CantTargetGroundUnits, ActionResult::CantTargetGroundUnits},
        {SC2APIProtocol::MustTargetStructures, ActionResult::MustTargetStructures},
        {SC2APIProtocol::CantTargetStructures, ActionResult::CantTargetStructures},
        {SC2APIProtocol::MustTargetLightUnits, ActionResult::MustTargetLightUnits},
        {SC2APIProtocol::CantTargetLightUnits, ActionResult::CantTargetLightUnits},
        {SC2APIProtocol::MustTargetArmoredUnits, ActionResult::MustTargetArmoredUnits},
        {SC2APIProtocol::CantTargetArmoredUnits, ActionResult::CantTargetArmoredUnits},
        {SC2APIProtocol::MustTargetBiologicalUnits, ActionResult::MustTargetBiologicalUnits},
        {SC2APIProtocol::CantTargetBiologicalUnits, ActionResult::CantTargetBiologicalUnits},
        {SC2APIProtocol::MustTargetHeroicUnits, ActionResult::MustTargetHeroicUnits},
        {SC2APIProtocol::CantTargetHeroicUnits, ActionResult::CantTargetHeroicUnits},
        {SC2APIProtocol::MustTargetRoboticUnits, ActionResult::MustTargetRoboticUnits},
        {SC2APIProtocol::CantTargetRoboticUnits, ActionResult::CantTargetRoboticUnits},
        {SC2APIProtocol::MustTargetMechanicalUnits, ActionResult::MustTargetMechanicalUnits},
        {SC2APIProtocol::CantTargetMechanicalUnits, ActionResult::CantTargetMechanicalUnits},
        {SC2APIProtocol::MustTargetPsionicUnits, ActionResult::MustTargetPsionicUnits},
        {SC2APIProtocol::CantTargetPsionicUnits, ActionResult::CantTargetPsionicUnits},
        {SC2APIProtocol::MustTargetMassiveUnits, ActionResult::MustTargetMassiveUnits},
        {SC2APIProtocol::CantTargetMassiveUnits, ActionResult::CantTargetMassiveUnits},
        {SC2APIProtocol::MustTargetMissile, ActionResult::MustTargetMissile},
        {SC2APIProtocol::CantTargetMissile, ActionResult::CantTargetMissile},
        {SC2APIProtocol::MustTargetWorkerUnits, ActionResult::MustTargetWorkerUnits},
        {SC2APIProtocol::CantTargetWorkerUnits, ActionResult::CantTargetWorkerUnits},
        {SC2APIProtocol::MustTargetEnergyCapableUnits, ActionResult::MustTargetEnergyCapableUnits},
        {SC2APIProtocol::CantTargetEnergyCapableUnits, ActionResult::CantTargetEnergyCapableUnits},
        {SC2APIProtocol::MustTargetShieldCapableUnits, ActionResult::MustTargetShieldCapableUnits},
        {SC2APIProtocol::CantTargetShieldCapableUnits, ActionResult::CantTargetShieldCapableUnits},
        {SC2APIProtocol::MustTargetFlyers, ActionResult::MustTargetFlyers},
        {SC2APIProtocol::CantTargetFlyers, ActionResult::CantTargetFlyers},
        {SC2APIProtocol::MustTargetBuriedUnits, ActionResult::MustTargetBuriedUnits},
        {SC2APIProtocol::CantTargetBuriedUnits, ActionResult::CantTargetBuriedUnits},
        {SC2APIProtocol::MustTargetCloakedUnits, ActionResult::MustTargetCloakedUnits},
        {SC2APIProtocol::CantTargetCloakedUnits, ActionResult::CantTargetCloakedUnits},
        {SC2APIProtocol::MustTargetUnitsInAStasisField, ActionResult::MustTargetUnitsInAStasisField},
        {SC2APIProtocol::CantTargetUnitsInAStasisField, ActionResult::CantTargetUnitsInAStasisField},
        {SC2APIProtocol::MustTargetUnderConstructionUnits, ActionResult::MustTargetUnderConstructionUnits},
        {SC2APIProtocol::CantTargetUnderConstructionUnits, ActionResult::CantTargetUnderConstructionUnits},
        {SC2APIProtocol::MustTargetDeadUnits, ActionResult::MustTargetDeadUnits},
        {SC2APIProtocol::CantTargetDeadUnits, ActionResult::CantTargetDeadUnits},
        {SC2APIProtocol::MustTargetRevivableUnits, ActionResult::MustTargetRevivableUnits},
        {SC2APIProtocol::CantTargetRevivableUnits, ActionResult::CantTargetRevivableUnits},
        {SC2APIProtocol::MustTargetHiddenUnits, ActionResult::MustTargetHiddenUnits},
        {SC2APIProtocol::CantTargetHiddenUnits, ActionResult::CantTargetHiddenUnits},
        {SC2APIProtocol::CantRechargeOtherPlayersUnits, ActionResult::CantRechargeOtherPlayersUnits},
        {SC2APIProtocol::MustTargetHallucinations, ActionResult::MustTargetHallucinations},
        {SC2APIProtocol::CantTargetHallucinations, ActionResult::CantTargetHallucinations},
        {SC2APIProtocol::MustTargetInvulnerableUnits, ActionResult::MustTargetInvulnerableUnits},
        {SC2APIProtocol::CantTargetInvulnerableUnits, ActionResult::CantTargetInvulnerableUnits},
        {SC2APIProtocol::MustTargetDetectedUnits, ActionResult::MustTargetDetectedUnits},
        {SC2APIProtocol::CantTargetDetectedUnits, ActionResult::CantTargetDetectedUnits},
        {SC2APIProtocol::CantTargetUnitWithEnergy, ActionResult::CantTargetUnitWithEnergy},
        {SC2APIProtocol::CantTargetUnitWithShields, ActionResult::CantTargetUnitWithShields},
        {SC2APIProtocol::MustTargetUncommandableUnits, ActionResult::MustTargetUncommandableUnits},
        {SC2APIProtocol::CantTargetUncommandableUnits, ActionResult::CantTargetUncommandableUnits},
        {SC2APIProtocol::MustTargetPreventDefeatUnits, ActionResult::MustTargetPreventDefeatUnits},
        {SC2APIProtocol::CantTargetPreventDefeatUnits, ActionResult::CantTargetPreventDefeatUnits},
        {SC2APIProtocol::MustTargetPreventRevealUnits, ActionResult::MustTargetPreventRevealUnits},
        {SC2APIProtocol::CantTargetPreventRevealUnits, ActionResult::CantTargetPreventRevealUnits},
        {SC2APIProtocol::MustTargetPassiveUnits, ActionResult::MustTargetPassiveUnits},
        {SC2APIProtocol::CantTargetPassiveUnits, ActionResult::CantTargetPassiveUnits},
        {SC2APIProtocol::MustTargetStunnedUnits, ActionResult::MustTargetStunnedUnits},
        {SC2APIProtocol::CantTargetStunnedUnits, ActionResult::CantTargetStunnedUnits},
        {SC2APIProtocol::MustTargetSummonedUnits, ActionResult::MustTargetSummonedUnits},
        {SC2APIProtocol::CantTargetSummonedUnits, ActionResult::CantTargetSummonedUnits},
        {SC2APIProtocol::MustTargetUser1, ActionResult::MustTargetUser1},
        {SC2APIProtocol::CantTargetUser1, ActionResult::CantTargetUser1},
        {SC2APIProtocol::MustTargetUnstoppableUnits, ActionResult::MustTargetUnstoppableUnits},
        {SC2APIProtocol::CantTargetUnstoppableUnits, ActionResult::CantTargetUnstoppableUnits},
        {SC2APIProtocol::MustTargetResistantUnits, ActionResult::MustTargetResistantUnits},
        {SC2APIProtocol::CantTargetResistantUnits, ActionResult::CantTargetResistantUnits},
        {SC2APIProtocol::MustTargetDazedUnits, ActionResult::MustTargetDazedUnits},
        {SC2APIProtocol::CantTargetDazedUnits, ActionResult::CantTargetDazedUnits},
        {SC2APIProtocol::CantLockdown, ActionResult::CantLockdown},
        {SC2APIProtocol::CantMindControl, ActionResult::CantMindControl},
        {SC2APIProtocol::MustTargetDestructibles, ActionResult::MustTargetDestructibles},
        {SC2APIProtocol::CantTargetDestructibles, ActionResult::CantTargetDestructibles},
        {SC2APIProtocol::MustTargetItems, ActionResult::MustTargetItems},
        {SC2APIProtocol::CantTargetItems, ActionResult::CantTargetItems},
        {SC2APIProtocol::NoCalldownAvailable, ActionResult::NoCalldownAvailable},
        {SC2APIProtocol::WaypointListFull, ActionResult::WaypointListFull},
        {SC2APIProtocol::MustTargetRace, ActionResult::MustTargetRace},
        {SC2APIProtocol::CantTargetRace, ActionResult::CantTargetRace},
        {SC2APIProtocol::MustTargetSimilarUnits, ActionResult::MustTargetSimilarUnits},
        {SC2APIProtocol::CantTargetSimilarUnits, ActionResult::CantTargetSimilarUnits},
        {SC2APIProtocol::CantFindEnoughTargets, ActionResult::CantFindEnoughTargets},
        {SC2APIProtocol::AlreadySpawningLarva, ActionResult::AlreadySpawningLarva},
        {SC2APIProtocol::CantTargetExhaustedResources, ActionResult::CantTargetExhaustedResources},
        {SC2APIProtocol::CantUseMinimap, ActionResult::CantUseMinimap},
        {SC2APIProtocol::CantUseInfoPanel, ActionResult::CantUseInfoPanel},
        {SC2APIProtocol::OrderQueueIsFull, ActionResult::OrderQueueIsFull},
        {SC2APIProtocol::CantHarvestThatResource, ActionResult::CantHarvestThatResource},
        {SC2APIProtocol::HarvestersNotRequired, ActionResult::HarvestersNotRequired},
        {SC2APIProtocol::AlreadyTargeted, ActionResult::AlreadyTargeted},
        {SC2APIProtocol::CantAttackWeaponsDisabled, ActionResult::CantAttackWeaponsDisabled},
        {SC2APIProtocol::CouldntReachTarget, ActionResult::CouldntReachTarget},
        {SC2APIProtocol::TargetIsOutOfRange, ActionResult::TargetIsOutOfRange},
        {SC2APIProtocol::TargetIsTooClose, ActionResult::TargetIsTooClose},
        {SC2APIProtocol::TargetIsOutOfArc, ActionResult::TargetIsOutOfArc},
        {SC2APIProtocol::CantFindTeleportLocation, ActionResult::CantFindTeleportLocation},
        {SC2APIProtocol::InvalidItemClass, ActionResult::InvalidItemClass},
        {SC2APIProtocol::CantFindCancelOrder, ActionResult::CantFindCancelOrder},
    }};

    ASTRA_ENUM_CONVERSION_DOMAIN(SC2APIProtocol::ActionResult, ActionResult, ActionResult::Error, ACTION_RESULT_MAP)

    static constexpr std::array<std::pair<SC2APIProtocol::ActionChat::Channel, ChatPayload::Channel>, 2> CHAT_CHANNEL_MAP {{
        {SC2APIProtocol::ActionChat::Broadcast, ChatPayload::Channel::Broadcast},
        {SC2APIProtocol::ActionChat::Team, ChatPayload::Channel::Team},
    }};

    ASTRA_ENUM_CONVERSION(SC2APIProtocol::ActionChat::Channel, ChatPayload::Channel, ChatPayload::Channel::Broadcast, SC2APIProtocol::ActionChat::Broadcast, CHAT_CHANNEL_MAP)


    static constexpr std::array<std::pair<SC2APIProtocol::ResponseCreateGame::Error, ErrorInfo::Error>, 8> CREATE_GAME_ERROR_MAP {{
        {SC2APIProtocol::ResponseCreateGame::MissingMap, ErrorInfo::Error::MissingMap},
        {SC2APIProtocol::ResponseCreateGame::InvalidMapPath, ErrorInfo::Error::InvalidMapPath},
        {SC2APIProtocol::ResponseCreateGame::InvalidMapData, ErrorInfo::Error::InvalidMapData},
        {SC2APIProtocol::ResponseCreateGame::InvalidMapName, ErrorInfo::Error::InvalidMapName},
        {SC2APIProtocol::ResponseCreateGame::InvalidMapHandle, ErrorInfo::Error::InvalidMapHandle},
        {SC2APIProtocol::ResponseCreateGame::MissingPlayerSetup, ErrorInfo::Error::MissingPlayerSetup},
        {SC2APIProtocol::ResponseCreateGame::InvalidPlayerSetup, ErrorInfo::Error::InvalidPlayerSetup},
        {SC2APIProtocol::ResponseCreateGame::MultiplayerUnsupported, ErrorInfo::Error::MultiplayerUnsupported}
    }};

    ASTRA_ENUM_CONVERSION_DOMAIN(SC2APIProtocol::ResponseCreateGame::Error, ErrorInfo::Error, ErrorInfo::Error::UnknownError, CREATE_GAME_ERROR_MAP)

    static constexpr std::array<std::pair<SC2APIProtocol::ResponseJoinGame::Error, ErrorInfo::Error>, 13> JOIN_GAME_ERROR_MAP {{
        {SC2APIProtocol::ResponseJoinGame::MissingParticipation, ErrorInfo::Error::MissingParticipation},
        {SC2APIProtocol::ResponseJoinGame::InvalidObservedPlayerId, ErrorInfo::Error::InvalidObservedPlayerId},
        {SC2APIProtocol::ResponseJoinGame::MissingOptions, ErrorInfo::Error::MissingOptions},
        {SC2APIProtocol::ResponseJoinGame::MissingPorts, ErrorInfo::Error::MissingPorts},
        {SC2APIProtocol::ResponseJoinGame::GameFull, ErrorInfo::Error::GameFull},
        {SC2APIProtocol::ResponseJoinGame::LaunchError, ErrorInfo::Error::LaunchError},
        {SC2APIProtocol::ResponseJoinGame::FeatureUnsupported, ErrorInfo::Error::FeatureUnsupported},
        {SC2APIProtocol::ResponseJoinGame::NoSpaceForUser, ErrorInfo::Error::NoSpaceForUser},
        {SC2APIProtocol::ResponseJoinGame::MapDoesNotExist, ErrorInfo::Error::MapDoesNotExist},
        {SC2APIProtocol::ResponseJoinGame::CannotOpenMap, ErrorInfo::Error::CannotOpenMap},
        {SC2APIProtocol::ResponseJoinGame::ChecksumError, ErrorInfo::Error::ChecksumError},
        {SC2APIProtocol::ResponseJoinGame::NetworkError, ErrorInfo::Error::NetworkError},
        {SC2APIProtocol::ResponseJoinGame::OtherError, ErrorInfo::Error::OtherError}
    }};

    ASTRA_ENUM_CONVERSION_DOMAIN(SC2APIProtocol::ResponseJoinGame::Error, ErrorInfo::Error, ErrorInfo::Error::UnknownError, JOIN_GAME_ERROR_MAP)

    static constexpr std::array<std::pair<SC2APIProtocol::ResponseRestartGame::Error, ErrorInfo::Error>, 1> RESTART_GAME_ERROR_MAP {{
        {SC2APIProtocol::ResponseRestartGame::LaunchError, ErrorInfo::Error::LaunchError}
    }};

    ASTRA_ENUM_CONVERSION_DOMAIN(SC2APIProtocol::ResponseRestartGame::Error, ErrorInfo::Error, ErrorInfo::Error::UnknownError, RESTART_GAME_ERROR_MAP)

    static constexpr std::array<std::pair<SC2APIProtocol::ResponseStartReplay::Error, ErrorInfo::Error>, 7> START_REPLAY_ERROR_MAP {{
        {SC2APIProtocol::ResponseStartReplay::MissingReplay, ErrorInfo::Error::MissingReplay},
        {SC2APIProtocol::ResponseStartReplay::InvalidReplayPath, ErrorInfo::Error::InvalidReplayPath},
        {SC2APIProtocol::ResponseStartReplay::InvalidReplayData, ErrorInfo::Error::InvalidReplayData},
        {SC2APIProtocol::ResponseStartReplay::InvalidMapData, ErrorInfo::Error::InvalidMapData},
        {SC2APIProtocol::ResponseStartReplay::InvalidObservedPlayerId, ErrorInfo::Error::InvalidObservedPlayerId},
        {SC2APIProtocol::ResponseStartReplay::MissingOptions, ErrorInfo::Error::MissingOptions},
        {SC2APIProtocol::ResponseStartReplay::LaunchError, ErrorInfo::Error::LaunchError}
    }};

    ASTRA_ENUM_CONVERSION_DOMAIN(SC2APIProtocol::ResponseStartReplay::Error, ErrorInfo::Error, ErrorInfo::Error::UnknownError, START_REPLAY_ERROR_MAP)

    static constexpr std::array<std::pair<SC2APIProtocol::ResponseReplayInfo::Error, ErrorInfo::Error>, 5> REPLAY_INFO_ERROR_MAP {{
        {SC2APIProtocol::ResponseReplayInfo::MissingReplay, ErrorInfo::Error::MissingReplay},
        {SC2APIProtocol::ResponseReplayInfo::InvalidReplayPath, ErrorInfo::Error::InvalidReplayPath},
        {SC2APIProtocol::ResponseReplayInfo::InvalidReplayData, ErrorInfo::Error::InvalidReplayData},
        {SC2APIProtocol::ResponseReplayInfo::ParsingError, ErrorInfo::Error::ParsingError},
        {SC2APIProtocol::ResponseReplayInfo::DownloadError, ErrorInfo::Error::DownloadError}
    }};

    ASTRA_ENUM_CONVERSION_DOMAIN(SC2APIProtocol::ResponseReplayInfo::Error, ErrorInfo::Error, ErrorInfo::Error::UnknownError, REPLAY_INFO_ERROR_MAP)

    static constexpr std::array<std::pair<SC2APIProtocol::ResponseMapCommand::Error, ErrorInfo::Error>, 1> MAP_COMMAND_ERROR_MAP {{
        {SC2APIProtocol::ResponseMapCommand::NoTriggerError, ErrorInfo::Error::NoTriggerError}
    }};

    ASTRA_ENUM_CONVERSION_DOMAIN(SC2APIProtocol::ResponseMapCommand::Error, ErrorInfo::Error, ErrorInfo::Error::UnknownError, MAP_COMMAND_ERROR_MAP)

    static constexpr std::array<std::pair<SC2APIProtocol::ResponseSaveMap::Error, ErrorInfo::Error>, 1> SAVE_MAP_ERROR_MAP {{
        {SC2APIProtocol::ResponseSaveMap::InvalidMapData, ErrorInfo::Error::InvalidMapData}
    }};

    ASTRA_ENUM_CONVERSION_DOMAIN(SC2APIProtocol::ResponseSaveMap::Error, ErrorInfo::Error, ErrorInfo::Error::UnknownError, SAVE_MAP_ERROR_MAP)

    static constexpr std::array<std::pair<SC2APIProtocol::Alert, Alert>, 22> ALERT_MAP {{
        {SC2APIProtocol::Alert::NuclearLaunchDetected, Alert::NuclearLaunchDetected},
        {SC2APIProtocol::Alert::NydusWormDetected, Alert::NydusWormDetected},
        {SC2APIProtocol::Alert::AlertError, Alert::AlertError},
        {SC2APIProtocol::Alert::AddOnComplete, Alert::AddOnComplete},
        {SC2APIProtocol::Alert::BuildingComplete, Alert::BuildingComplete},
        {SC2APIProtocol::Alert::BuildingUnderAttack, Alert::BuildingUnderAttack},
        {SC2APIProtocol::Alert::LarvaHatched, Alert::LarvaHatched},
        {SC2APIProtocol::Alert::MergeComplete, Alert::MergeComplete},
        {SC2APIProtocol::Alert::MineralsExhausted, Alert::MineralsExhausted},
        {SC2APIProtocol::Alert::MorphComplete, Alert::MorphComplete},
        {SC2APIProtocol::Alert::MothershipComplete, Alert::MothershipComplete},
        {SC2APIProtocol::Alert::MULEExpired, Alert::MULEExpired},
        {SC2APIProtocol::Alert::NukeComplete, Alert::NukeComplete},
        {SC2APIProtocol::Alert::ResearchComplete, Alert::ResearchComplete},
        {SC2APIProtocol::Alert::TrainError, Alert::TrainError},
        {SC2APIProtocol::Alert::TrainUnitComplete, Alert::TrainUnitComplete},
        {SC2APIProtocol::Alert::TrainWorkerComplete, Alert::TrainWorkerComplete},
        {SC2APIProtocol::Alert::TransformationComplete, Alert::TransformationComplete},
        {SC2APIProtocol::Alert::UnitUnderAttack, Alert::UnitUnderAttack},
        {SC2APIProtocol::Alert::UpgradeComplete, Alert::UpgradeComplete},
        {SC2APIProtocol::Alert::VespeneExhausted, Alert::VespeneExhausted},
        {SC2APIProtocol::Alert::WarpInComplete, Alert::WarpInComplete},
    }};

    ASTRA_ENUM_CONVERSION_DOMAIN(SC2APIProtocol::Alert, Alert, Alert::AlertError, ALERT_MAP)

    static constexpr std::array<std::pair<SC2APIProtocol::Score::ScoreType, Score::Type>, 2> SCORE_MAP {{
        {SC2APIProtocol::Score::ScoreType::Score_ScoreType_Curriculum, Score::Type::Curriculum},
        {SC2APIProtocol::Score::ScoreType::Score_ScoreType_Melee, Score::Type::Melee}
    }};

    ASTRA_ENUM_CONVERSION_DOMAIN(SC2APIProtocol::Score::ScoreType, Score::Type, Score::Type::Curriculum, SCORE_MAP)

    static constexpr std::array<std::pair<SC2APIProtocol::Alliance, Alliance>, 4> ALLIANCE_MAP {{
        {SC2APIProtocol::Alliance::Self, Alliance::Self},
        {SC2APIProtocol::Alliance::Ally, Alliance::Ally},
        {SC2APIProtocol::Alliance::Neutral, Alliance::Neutral},
        {SC2APIProtocol::Alliance::Enemy, Alliance::Enemy}
    }};

    ASTRA_ENUM_CONVERSION_DOMAIN(SC2APIProtocol::Alliance, Alliance, Alliance::Neutral, ALLIANCE_MAP)

    static constexpr std::array<std::pair<SC2APIProtocol::DisplayType, DisplayType>, 4> DISPLAY_MAP {{
        {SC2APIProtocol::DisplayType::Visible, DisplayType::Visible},
        {SC2APIProtocol::DisplayType::Snapshot, DisplayType::Snapshot},
        {SC2APIProtocol::DisplayType::Hidden, DisplayType::Hidden},
        {SC2APIProtocol::DisplayType::Placeholder, DisplayType::Placeholder}
    }};

    ASTRA_ENUM_CONVERSION_DOMAIN(SC2APIProtocol::DisplayType, DisplayType, DisplayType::Snapshot, DISPLAY_MAP)

    static constexpr std::array<std::pair<SC2APIProtocol::CloakState, CloakState>, 5> CLOAK_MAP {{
        {SC2APIProtocol::CloakState::CloakedUnknown, CloakState::CloakedUnknown},
        {SC2APIProtocol::CloakState::Cloaked, CloakState::Cloaked},
        {SC2APIProtocol::CloakState::CloakedDetected, CloakState::CloakedDetected},
        {SC2APIProtocol::CloakState::NotCloaked, CloakState::NotCloaked},
        {SC2APIProtocol::CloakState::CloakedAllied, CloakState::CloakedAllied}
    }};

    ASTRA_ENUM_CONVERSION_DOMAIN(SC2APIProtocol::CloakState, CloakState, CloakState::NotCloaked, CLOAK_MAP)

    static constexpr std::array<std::pair<SC2APIProtocol::AbilityData::Target, AbilityData::Target>, 5> TARGET_MAP {{
        {SC2APIProtocol::AbilityData::Target::AbilityData_Target_None, AbilityData::Target::None},
        {SC2APIProtocol::AbilityData::Target::AbilityData_Target_Point, AbilityData::Target::Point},
        {SC2APIProtocol::AbilityData::Target::AbilityData_Target_Unit, AbilityData::Target::Unit},
        {SC2APIProtocol::AbilityData::Target::AbilityData_Target_PointOrUnit, AbilityData::Target::PointOrUnit},
        {SC2APIProtocol::AbilityData::Target::AbilityData_Target_PointOrNone, AbilityData::Target::PointOrNone}
    }};

    ASTRA_ENUM_CONVERSION(SC2APIProtocol::AbilityData::Target, AbilityData::Target, AbilityData::Target::None, SC2APIProtocol::AbilityData::Target::AbilityData_Target_None, TARGET_MAP)

    static constexpr std::array<std::pair<SC2APIProtocol::Attribute, Attribute>, 11> ATTRIBUTE_MAP {{
        {SC2APIProtocol::Attribute::Light, Attribute::Light},
        {SC2APIProtocol::Attribute::Armored, Attribute::Armored},
        {SC2APIProtocol::Attribute::Biological, Attribute::Biological},
        {SC2APIProtocol::Attribute::Mechanical, Attribute::Mechanical},
        {SC2APIProtocol::Attribute::Robotic, Attribute::Robotic},
        {SC2APIProtocol::Attribute::Psionic, Attribute::Psionic},
        {SC2APIProtocol::Attribute::Massive, Attribute::Massive},
        {SC2APIProtocol::Attribute::Structure, Attribute::Structure},
        {SC2APIProtocol::Attribute::Hover, Attribute::Hover},
        {SC2APIProtocol::Attribute::Heroic, Attribute::Heroic},
        {SC2APIProtocol::Attribute::Summoned, Attribute::Summoned}
    }};

    ASTRA_ENUM_CONVERSION_DOMAIN(SC2APIProtocol::Attribute, Attribute, Attribute::Light, ATTRIBUTE_MAP)

    static constexpr std::array<std::pair<SC2APIProtocol::Weapon::TargetType, Weapon::TargetType>, 3> WEAPON_TYPE_MAP {{
        {SC2APIProtocol::Weapon::TargetType::Weapon_TargetType_Ground, Weapon::TargetType::Ground},
        {SC2APIProtocol::Weapon::TargetType::Weapon_TargetType_Air, Weapon::TargetType::Air},
        {SC2APIProtocol::Weapon::TargetType::Weapon_TargetType_Any, Weapon::TargetType::Any}
    }};

    ASTRA_ENUM_CONVERSION_DOMAIN(SC2APIProtocol::Weapon::TargetType, Weapon::TargetType, Weapon::TargetType::Ground, WEAPON_TYPE_MAP)
}
