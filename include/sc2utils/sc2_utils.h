#pragma once

#include "sc2api/sc2_common.h"
#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_unit.h"
#include "sc2api/sc2_map_info.h"

#include <filesystem>

namespace sc2 {

    std::filesystem::path GetGameDataDirectory(); ///< Get the GameData directory (the one with ExecuteInfo.txt
    std::filesystem::path GetLibraryMapsDirectory(); ///< Get the path to the maps folder from teh library running
    std::filesystem::path GetGameMapsDirectory(const std::filesystem::path &path); ///< Get the maps from the processes path

    bool FindLatestSC2Exe(std::filesystem::path &path); // TODO: return path
    bool FindSC2VersionExe(std::filesystem::path &path, uint32_t base_build); // TODO: return path

    struct ExpansionParameters {
        // Some nice parameters that generally work but may require tuning for certain maps.
        ExpansionParameters() :
            radiuses_({ 6.4f, 5.3f }),
            circle_step_size_(0.5f),
            cluster_distance_(15.0f),
            debug_(nullptr) {
        }

        // The various radius to check at from the center of an expansion.
        std::vector<float> radiuses_;

        // With what granularity to step the circumference of the circle.
        float circle_step_size_;

        // With what distance to cluster mineral/vespene in, this will be used for center of mass calulcation.
        float cluster_distance_;

        // If filled out CalculateExpansionLocations will render spheres to show what it calculated.
        DebugInterface* debug_;
    };

    Point2D FindRandomLocation(const Point2D& min, const Point2D& max);
    Point2D FindRandomLocation(const GameInfo& game_info);
    Point2D FindCenterOfMap(const GameInfo& game_info);

    // Clusters units within some distance of each other and returns a list of them and their center of mass.
    std::vector<std::pair<Point3D, std::vector<Unit>>> Cluster(const Units& units, float distance_apart);

    // Calculates expansion locations, this call can take on the order of 100ms since it makes blocking queries to SC2 so call it once and cache the reults.
    std::vector<Point3D> CalculateExpansionLocations(const ObservationInterface* observation, QueryInterface* query, ExpansionParameters parameters=ExpansionParameters());
}