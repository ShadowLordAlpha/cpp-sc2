/**
 *
 */

#pragma once

#include <vector>
#include <filesystem>
#include <variant>
#include <string>
#include <optional>
#include "sc2/api/common.h"
#include "sc2/api/request/setup_requests.h"

namespace astra::sc2
{
    enum class ActionResult
    {
        Success = 1,
        NotSupported = 2,
        Error = 3,
        CantQueueThatOrder = 4,
        Retry = 5,
        Cooldown = 6,
        QueueIsFull = 7,
        RallyQueueIsFull = 8,
        NotEnoughMinerals = 9,
        NotEnoughVespene = 10,
        NotEnoughTerrazine = 11,
        NotEnoughCustom = 12,
        NotEnoughFood = 13,
        FoodUsageImpossible = 14,
        NotEnoughLife = 15,
        NotEnoughShields = 16,
        NotEnoughEnergy = 17,
        LifeSuppressed = 18,
        ShieldsSuppressed = 19,
        EnergySuppressed = 20,
        NotEnoughCharges = 21,
        CantAddMoreCharges = 22,
        TooMuchMinerals = 23,
        TooMuchVespene = 24,
        TooMuchTerrazine = 25,
        TooMuchCustom = 26,
        TooMuchFood = 27,
        TooMuchLife = 28,
        TooMuchShields = 29,
        TooMuchEnergy = 30,
        MustTargetUnitWithLife = 31,
        MustTargetUnitWithShields = 32,
        MustTargetUnitWithEnergy = 33,
        CantTrade = 34,
        CantSpend = 35,
        CantTargetThatUnit = 36,
        CouldntAllocateUnit = 37,
        UnitCantMove = 38,
        TransportIsHoldingPosition = 39,
        BuildTechRequirementsNotMet = 40,
        CantFindPlacementLocation = 41,
        CantBuildOnThat = 42,
        CantBuildTooCloseToDropOff = 43,
        CantBuildLocationInvalid = 44,
        CantSeeBuildLocation = 45,
        CantBuildTooCloseToCreepSource = 46,
        CantBuildTooCloseToResources = 47,
        CantBuildTooFarFromWater = 48,
        CantBuildTooFarFromCreepSource = 49,
        CantBuildTooFarFromBuildPowerSource = 50,
        CantBuildOnDenseTerrain = 51,
        CantTrainTooFarFromTrainPowerSource = 52,
        CantLandLocationInvalid = 53,
        CantSeeLandLocation = 54,
        CantLandTooCloseToCreepSource = 55,
        CantLandTooCloseToResources = 56,
        CantLandTooFarFromWater = 57,
        CantLandTooFarFromCreepSource = 58,
        CantLandTooFarFromBuildPowerSource = 59,
        CantLandTooFarFromTrainPowerSource = 60,
        CantLandOnDenseTerrain = 61,
        AddOnTooFarFromBuilding = 62,
        MustBuildRefineryFirst = 63,
        BuildingIsUnderConstruction = 64,
        CantFindDropOff = 65,
        CantLoadOtherPlayersUnits = 66,
        NotEnoughRoomToLoadUnit = 67,
        CantUnloadUnitsThere = 68,
        CantWarpInUnitsThere = 69,
        CantLoadImmobileUnits = 70,
        CantRechargeImmobileUnits = 71,
        CantRechargeUnderConstructionUnits = 72,
        CantLoadThatUnit = 73,
        NoCargoToUnload = 74,
        LoadAllNoTargetsFound = 75,
        NotWhileOccupied = 76,
        CantAttackWithoutAmmo = 77,
        CantHoldAnyMoreAmmo = 78,
        TechRequirementsNotMet = 79,
        MustLockdownUnitFirst = 80,
        MustTargetUnit = 81,
        MustTargetInventory = 82,
        MustTargetVisibleUnit = 83,
        MustTargetVisibleLocation = 84,
        MustTargetWalkableLocation = 85,
        MustTargetPawnableUnit = 86,
        YouCantControlThatUnit = 87,
        YouCantIssueCommandsToThatUnit = 88,
        MustTargetResources = 89,
        RequiresHealTarget = 90,
        RequiresRepairTarget = 91,
        NoItemsToDrop = 92,
        CantHoldAnyMoreItems = 93,
        CantHoldThat = 94,
        TargetHasNoInventory = 95,
        CantDropThisItem = 96,
        CantMoveThisItem = 97,
        CantPawnThisUnit = 98,
        MustTargetCaster = 99,
        CantTargetCaster = 100,
        MustTargetOuter = 101,
        CantTargetOuter = 102,
        MustTargetYourOwnUnits = 103,
        CantTargetYourOwnUnits = 104,
        MustTargetFriendlyUnits = 105,
        CantTargetFriendlyUnits = 106,
        MustTargetNeutralUnits = 107,
        CantTargetNeutralUnits = 108,
        MustTargetEnemyUnits = 109,
        CantTargetEnemyUnits = 110,
        MustTargetAirUnits = 111,
        CantTargetAirUnits = 112,
        MustTargetGroundUnits = 113,
        CantTargetGroundUnits = 114,
        MustTargetStructures = 115,
        CantTargetStructures = 116,
        MustTargetLightUnits = 117,
        CantTargetLightUnits = 118,
        MustTargetArmoredUnits = 119,
        CantTargetArmoredUnits = 120,
        MustTargetBiologicalUnits = 121,
        CantTargetBiologicalUnits = 122,
        MustTargetHeroicUnits = 123,
        CantTargetHeroicUnits = 124,
        MustTargetRoboticUnits = 125,
        CantTargetRoboticUnits = 126,
        MustTargetMechanicalUnits = 127,
        CantTargetMechanicalUnits = 128,
        MustTargetPsionicUnits = 129,
        CantTargetPsionicUnits = 130,
        MustTargetMassiveUnits = 131,
        CantTargetMassiveUnits = 132,
        MustTargetMissile = 133,
        CantTargetMissile = 134,
        MustTargetWorkerUnits = 135,
        CantTargetWorkerUnits = 136,
        MustTargetEnergyCapableUnits = 137,
        CantTargetEnergyCapableUnits = 138,
        MustTargetShieldCapableUnits = 139,
        CantTargetShieldCapableUnits = 140,
        MustTargetFlyers = 141,
        CantTargetFlyers = 142,
        MustTargetBuriedUnits = 143,
        CantTargetBuriedUnits = 144,
        MustTargetCloakedUnits = 145,
        CantTargetCloakedUnits = 146,
        MustTargetUnitsInAStasisField = 147,
        CantTargetUnitsInAStasisField = 148,
        MustTargetUnderConstructionUnits = 149,
        CantTargetUnderConstructionUnits = 150,
        MustTargetDeadUnits = 151,
        CantTargetDeadUnits = 152,
        MustTargetRevivableUnits = 153,
        CantTargetRevivableUnits = 154,
        MustTargetHiddenUnits = 155,
        CantTargetHiddenUnits = 156,
        CantRechargeOtherPlayersUnits = 157,
        MustTargetHallucinations = 158,
        CantTargetHallucinations = 159,
        MustTargetInvulnerableUnits = 160,
        CantTargetInvulnerableUnits = 161,
        MustTargetDetectedUnits = 162,
        CantTargetDetectedUnits = 163,
        CantTargetUnitWithEnergy = 164,
        CantTargetUnitWithShields = 165,
        MustTargetUncommandableUnits = 166,
        CantTargetUncommandableUnits = 167,
        MustTargetPreventDefeatUnits = 168,
        CantTargetPreventDefeatUnits = 169,
        MustTargetPreventRevealUnits = 170,
        CantTargetPreventRevealUnits = 171,
        MustTargetPassiveUnits = 172,
        CantTargetPassiveUnits = 173,
        MustTargetStunnedUnits = 174,
        CantTargetStunnedUnits = 175,
        MustTargetSummonedUnits = 176,
        CantTargetSummonedUnits = 177,
        MustTargetUser1 = 178,
        CantTargetUser1 = 179,
        MustTargetUnstoppableUnits = 180,
        CantTargetUnstoppableUnits = 181,
        MustTargetResistantUnits = 182,
        CantTargetResistantUnits = 183,
        MustTargetDazedUnits = 184,
        CantTargetDazedUnits = 185,
        CantLockdown = 186,
        CantMindControl = 187,
        MustTargetDestructibles = 188,
        CantTargetDestructibles = 189,
        MustTargetItems = 190,
        CantTargetItems = 191,
        NoCalldownAvailable = 192,
        WaypointListFull = 193,
        MustTargetRace = 194,
        CantTargetRace = 195,
        MustTargetSimilarUnits = 196,
        CantTargetSimilarUnits = 197,
        CantFindEnoughTargets = 198,
        AlreadySpawningLarva = 199,
        CantTargetExhaustedResources = 200,
        CantUseMinimap = 201,
        CantUseInfoPanel = 202,
        OrderQueueIsFull = 203,
        CantHarvestThatResource = 204,
        HarvestersNotRequired = 205,
        AlreadyTargeted = 206,
        CantAttackWeaponsDisabled = 207,
        CouldntReachTarget = 208,
        TargetIsOutOfRange = 209,
        TargetIsTooClose = 210,
        TargetIsOutOfArc = 211,
        CantFindTeleportLocation = 212,
        InvalidItemClass = 213,
        CantFindCancelOrder = 214,
    };

    enum class Alert
    {
        NuclearLaunchDetected = 1,
        NydusWormDetected = 2,
        AlertError = 3,
        AddOnComplete = 4,
        BuildingComplete = 5,
        BuildingUnderAttack = 6,
        LarvaHatched = 7,
        MergeComplete = 8,
        MineralsExhausted = 9,
        MorphComplete = 10,
        MothershipComplete = 11,
        MULEExpired = 12,
        NukeComplete = 13,
        ResearchComplete = 14,
        TrainError = 15,
        TrainUnitComplete = 16,
        TrainWorkerComplete = 17,
        TransformationComplete = 18,
        UnitUnderAttack = 19,
        UpgradeComplete = 20,
        VespeneExhausted = 21,
        WarpInComplete = 22,
    };

    enum class Alliance {
        Self = 1,
        Ally = 2,
        Neutral = 3,
        Enemy = 4,
    };

    enum class DisplayType {
        Visible = 1,                            // Fully visible
        Snapshot = 2,                           // Dimmed version of unit left behind after entering fog of war
        Hidden = 3,                             // Fully hidden
        Placeholder = 4,                        // Building that hasn't started construction.
    };

    enum class CloakState {
        CloakedUnknown = 0,  // Under the fog, so unknown whether it's cloaked or not.
        Cloaked = 1,
        CloakedDetected = 2,
        NotCloaked = 3,
        CloakedAllied = 4,
    };

    enum class Attribute {
        Light = 1,
        Armored = 2,
        Biological = 3,
        Mechanical = 4,
        Robotic = 5,
        Psionic = 6,
        Massive = 7,
        Structure = 8,
        Hover = 9,
        Heroic = 10,
        Summoned = 11,
    };

    struct ImageData
    {
        uint32_t bitsPerPixel;
        Point2I size;
        std::string data;
    };

    struct StartRaw
    {
        Point2I mapSize;
        ImageData pathingGrid;
        ImageData terrainHeight;
        ImageData placementGrid;
        RectangleI playableArea;
        std::vector<Point2D> startLocations;
    };

    struct GameInfoResponse
    {
        std::string mapName;
        std::vector<std::string> modNames;
        std::optional<std::filesystem::path> localMapPath;
        std::vector<PlayerInfo> playerInfo;
        std::optional<StartRaw> startRaw;
        InterfaceOptions options;
    };

    struct ChatReceived
    {
        uint32_t playerId;
        std::string message;
    };

    struct ActionError
    {
        std::optional<uint64_t> unitTag;
        uint64_t abilityId;
        ActionResult result = ActionResult::Error;
    };

    struct PlayerCommon
    {
        uint32_t playerId;
        uint32_t minerals;
        uint32_t vespene;
        uint32_t foodCap;
        uint32_t foodUsed;
        uint32_t foodArmy;
        uint32_t foodWorkers;
        uint32_t idleWorkerCount;
        uint32_t armyCount;
        uint32_t warpGateCount;
        uint32_t larvaCount;
    };

    struct CategoryScoreDetails
    {
        float none;
        float army;
        float economy;
        float technology;
        float upgrade;
    };

    struct VitalScoreDetails
    {
        float life;
        float shields;
        float energy;
    };

    struct ScoreDetails
    {
        float idleProductionTime;
        float idleWorkerTime;
        float totalValueUnits;
        float totalValueStructures;
        float killedValueUnits;
        float killedValueStructures;
        float collectedMinerals;
        float collectedVespene;
        float collectionRateMinerals;
        float collectionRateVespene;
        float spentMinerals;
        float spentVespene;

        CategoryScoreDetails foodUsed;
        CategoryScoreDetails killedMinerals;
        CategoryScoreDetails killedVespene;
        CategoryScoreDetails lostMinerals;
        CategoryScoreDetails lostVespene;
        CategoryScoreDetails friendlyFireMinerals;
        CategoryScoreDetails friendlyFireVespene;
        CategoryScoreDetails usedMinerals;
        CategoryScoreDetails usedVespene;
        CategoryScoreDetails totalUsedMinerals;
        CategoryScoreDetails totalUsedVespene;
        VitalScoreDetails totalDamageDealt;
        VitalScoreDetails totalDamageTaken;
        VitalScoreDetails totalHealed;

        float currentApm;
        float currentEffectiveApm;
    };

    struct Score
    {
        enum class Type
        {
            Curriculum,
            Melee,
        };

        Type type;
        uint32_t score;
        ScoreDetails scoreDetails;
    };

    struct PowerSource
    {
        Point3D pos;
        float radius;
        uint64_t tag;
    };

    struct MapState
    {
        ImageData visibility;
        ImageData creep;
    };

    struct RadarRing
    {
        Point3D pos;
        float radius;
    };

    struct PlayerRaw
    {
        std::vector<PowerSource> powerSources;
        Point3D camera;
        std::vector<uint32_t> upgradeIds;
    };

    struct Event
    {
        std::vector<uint64_t> deadUnits;
    };

    struct Effect
    {
        uint32_t effectId;
        std::vector<Point2D> pos;
        Alliance alliance;
        uint32_t owner;
        float radius;
    };

    struct UnitOrder
    {
        uint32_t abilityId;
        std::variant<Point3D, uint64_t> target;
        std::optional<float> progress;
    };

    struct PassengerUnit
    {
        uint64_t tag;
        float health;
        float healthMax;
        float shield;
        float shieldMax;
        float energy;
        float energyMax;
        uint32_t unitType;
    };

    struct RallyTarget
    {
        Point3D point;
        std::optional<uint64_t> tag;
    };

    struct Unit
    {
        DisplayType displayType;
        Alliance alliance;

        uint64_t tag;
        uint32_t unitType;
        uint32_t owner;

        Point3D pos;
        float facing;
        float radius;
        float buildProgress;
        CloakState cloak;
        std::vector<uint32_t> buffIds;

        float detectRange;
        float radarRange;

        bool selected;
        bool onScreen;
        bool blip;
        bool powered;
        bool active;
        uint32_t attackUpgradeLevel;
        uint32_t armorUpgradeLevel;
        uint32_t shieldUpgradeLevel;

        // Not populated for snapshots
        std::optional<float> health;
        std::optional<float> healthMax;
        std::optional<float> shield;
        std::optional<float> shieldMax;
        std::optional<float> energy;
        std::optional<float> energyMax;
        std::optional<uint32_t> mineralContents;
        std::optional<uint32_t> vespeneContents;
        std::optional<bool> flying;
        std::optional<bool> burrowed;
        std::optional<bool> hallucination;

        // Not populated for enemies
        std::vector<UnitOrder> orders;
        std::optional<uint64_t> addOnTag;
        std::vector<PassengerUnit> passengers;
        std::optional<uint32_t> cargoSpaceTaken;
        std::optional<uint32_t> cargoSpaceMax;
        std::optional<uint32_t> assignedHarvesters;
        std::optional<uint32_t> idealHarvesters;
        std::optional<float> weaponCooldown;
        std::optional<uint64_t> engagedTargetTag;
        std::optional<uint32_t> buffDurationRemain;
        std::optional<uint32_t> buffDurationMax;
        std::vector<RallyTarget> rallyTargets;
    };

    struct  ObservationRaw
    {
        PlayerRaw player;
        std::vector<Unit> units;
        MapState mapState;
        Event event;
        std::vector<Effect> effects;
        std::vector<RadarRing> radar;
    };

    struct FeatureLayers
    {
        ImageData height_map;               // uint8. Terrain height. World space units of [-200, 200] encoded into [0, 255].
        ImageData visibility_map;           // uint8. 0=Hidden, 1=Fogged, 2=Visible, 3=FullHidden
        ImageData creep;                    // 1-bit. Zerg creep.
        ImageData power;                    // 1-bit. Protoss power.
        ImageData player_id;                // uint8. Participants: [1, 15] Neutral: 16
        ImageData unit_type;                // int32. Unique identifier for type of unit.
        ImageData selected;                 // 1-bit. Selected units.
        ImageData unit_hit_points;          // int32.
        ImageData unit_hit_points_ratio;    // uint8. Ratio of current health to max health. [0%, 100%] encoded into [0, 255].
        ImageData unit_energy;              // int32.
        ImageData unit_energy_ratio;        // uint8. Ratio of current energy to max energy. [0%, 100%] encoded into [0, 255].
        ImageData unit_shields;             // int32.
        ImageData unit_shields_ratio;       // uint8. Ratio of current shields to max shields. [0%, 100%] encoded into [0, 255].
        ImageData player_relative;          // uint8. See "Alliance" enum in raw.proto. Range: [1, 4]
        ImageData unit_density_aa;          // uint8. Density of units overlapping a pixel, anti-aliased. [0.0, 16.0f] encoded into [0, 255].
        ImageData unit_density;             // uint8. Count of units overlapping a pixel.
        ImageData effects;                  // uint8. Visuals of persistent abilities. (eg. Psistorm)
        ImageData hallucinations;           // 1-bit. Whether the unit here is a hallucination.
        ImageData cloaked;                  // 1-bit. Whether the unit here is cloaked. Hidden units will show up too, but with less details in other layers.
        ImageData blip;                     // 1-bit. Whether the unit here is a blip.
        ImageData buffs;                    // int32. One of the buffs applied to this unit. Extras are ignored.
        ImageData buff_duration;            // uint8. Ratio of buff remaining. [0%, 100%] encoded into [0, 255].
        ImageData active;                   // 1-bit. Whether the unit here is active.
        ImageData build_progress;           // uint8. How far along the building is building something. [0%, 100%] encoded into [0, 255].
        ImageData buildable;                // 1-bit. Whether a building can be built here.
        ImageData pathable;                 // 1-bit. Whether a unit can walk here.
        ImageData placeholder;              // 1-bit. Whether the unit here is a placeholder building to be constructed.
    };

    struct FeatureLayersMinimap {
        ImageData height_map;                // uint8. Terrain height. World space units of [-200, 200] encoded into [0, 255].
        ImageData visibility_map;            // uint8. 0=Hidden, 1=Fogged, 2=Visible, 3=FullHidden
        ImageData creep;                     // 1-bit. Zerg creep.
        ImageData camera;                    // 1-bit. Area covered by the camera.
        ImageData player_id;                 // uint8. Participants: [1, 15] Neutral: 16
        ImageData player_relative;           // uint8. See "Alliance" enum in raw.proto. Range: [1, 4]
        ImageData selected;                  // 1-bit. Selected units.
        ImageData alerts;                    // 1-bit. Shows 'UnitAttacked' alert location.
        ImageData buildable;                 // 1-bit. Whether a building can be built here.
        ImageData pathable;                  // 1-bit. Whether a unit can walk here.

        // Cheat layers, enable with SpatialCameraSetup.allow_cheating_layers.
        std::optional<ImageData> unit_type;                 // int32. Unique identifier for type of unit.
    };


struct ObservationFeatureLayer
    {
        FeatureLayers renders;
        FeatureLayersMinimap minimapRenders;
    };

    struct ObservationRender
    {
        ImageData map;
        ImageData minimap;
    };

    struct ControlGroup
    {
        uint32_t controlGroupIndex;
        uint32_t leaderUintType;
        uint32_t count;
    };

    struct UnitInfo
    {
        uint32_t unitType;
        uint32_t playerRelative;
        uint32_t health;
        uint32_t shield;
        uint32_t energy;
        uint32_t transportSlotsTaken;
        float buildProgress;
        std::optional<std::unique_ptr<UnitInfo>> addOn;
        uint32_t healthMax;
        uint32_t shieldMax;
        uint32_t energyMax;
    };

    struct SinglePanel
    {
        UnitInfo unit;
        uint32_t attackUpgradeLevel;
        uint32_t armorUpgradeLevel;
        uint32_t shieldUpgradeLevel;
        std::vector<uint32_t> buffs;
    };

    struct MultiPanel
    {
        std::vector<UnitInfo> units;
    };

    struct CargoPanel
    {
        UnitInfo unit;
        std::vector<UnitInfo> passengers;
        uint32_t slotsAvailable;
    };

    struct BuildItem
    {
        uint32_t abilityId;
        float buildProgress;
    };

    struct ProductionPanel
    {
        UnitInfo unit;

        std::vector<UnitInfo> buildQueue;
        std::vector<BuildItem> productionQueue;
    };

    struct ObservationUI
    {
        std::vector<ControlGroup> groups;
        std::optional<std::variant<SinglePanel, MultiPanel, CargoPanel, ProductionPanel>> panel;
    };

    struct Observation
    {
        uint32_t gameLoop;
        PlayerCommon playerCommon;
        std::vector<Alert> alerts;
        std::vector<AvailableAbility> availableAbilities;
        Score score;
        std::optional<ObservationRaw> rawData;
        std::optional<ObservationFeatureLayer> featureLayerData;
        std::optional<ObservationRender> renderData;
        std::optional<ObservationUI> uiData;
    };

    struct ObservationResponse
    {
        std::vector<std::unique_ptr<Action>> actions;
        std::vector<ActionError> actionErrors;
        Observation observation;
        std::vector<PlayerResult> playerResult;
        std::vector<ChatReceived> chat;
    };

    struct QueryPlacement
    {
        uint32_t abilityId;
        Point2D targetPos;
        std::optional<uint64_t> unitTag;
    };

    struct QueryPathing
    {
        std::variant<Point2D, uint64_t> start;
        Point2D end;
    };

    struct QueryAvailableAbilities
    {
        std::vector<AvailableAbility> abilities;
        uint64_t tag;
        uint32_t unitTypeId;
    };

    struct QueryResponse
    {
        std::vector<float> pathing;
        std::vector<QueryAvailableAbilities> abilities;
        std::vector<ActionResult> placement;
    };

    struct AbilityData
    {
        enum class Target {
            None = 1,                                                     // Does not require a target.
            Point = 2,                                                    // Requires a target position.
            Unit = 3,                                                     // Requires a unit to target. Given by position using feature layers.
            PointOrUnit = 4,                                              // Requires either a target point or target unit.
            PointOrNone = 5,                                              // Requires either a target point or no target. (eg. building add-ons)
        };

        uint32_t abilityId;
        std::string linkName;
        uint32_t linkIndex;
        std::string buttonName;
        std::string friendlyName;
        std::string hotkey;
        uint32_t remapAbilityId;
        bool available;
        Target target;
        bool allowMinimap;
        bool allowAutocast;
        bool isBuilding;
        float footprintRadius;
        bool instantPlacement;
        float castRange;
    };

    struct DamageBonus {
        Attribute attribute;
        float bonus;
    };

    struct Weapon {
        enum class TargetType {
            Ground = 1,
            Air = 2,
            Any = 3,
        };

        TargetType type;
        float damage;
        std::vector<DamageBonus> damageBonus;
        uint32_t attacks;              // Number of hits per attack. (eg. Colossus has 2 beams)
        float range;
        float speed;                 // Time between attacks.
    };

    struct UnitTypeData
    {
        uint32_t unitId;                                    // Stable ID.
        std::string name;                                       // Catalog name of the unit.
        bool available;                                    // If true, the ability may be used by this set of mods/map.
        uint32_t cargoSize;                                 // Number of cargo slots it occupies in transports.
        uint32_t mineralCost;
        uint32_t vespeneCost;
        float foodRequired;
        float foodProvided;
        uint32_t abilityId;                                // The ability that builds this unit.
        Race race;
        float buildTime;
        bool hasVespene;
        bool hasMinerals;
        float sightRange;                                // Range unit reveals vision.

        std::vector<uint32_t> techAlias;                                // Other units that satisfy the same tech requirement.
        uint32_t unitAlias;                                // The morphed variant of this unit.

        uint32_t techRequirement;                          // Structure required to build this unit. (Or any with the same tech_alias)
        bool requireAttached;                            // Whether tech_requirement is an add-on.

        // Values include changes from upgrades
        std::vector<Attribute> attributes;
        float movementSpeed;
        float armor;
        std::vector<Weapon> weapons;
    };

    struct UpgradeData
    {
        uint32_t upgradeId;
        std::string name;
        uint32_t mineralCost;
        uint32_t vespeneCost;
        float researchTime;
        uint32_t abilityId;
    };

    struct BuffData
    {
        uint32_t buffId;
        std::string name;
    };

    struct EffectData
    {
        uint32_t effectId;
        std::string name;
        std::string friendlyName;
        float radius;
    };

    struct DataResponse
    {
        std::vector<AbilityData> abilities;
        std::vector<UnitTypeData> units;
        std::vector<UpgradeData> upgrades;
        std::vector<BuffData> buffs;
        std::vector<EffectData> effects;
    };
}
