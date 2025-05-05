#include "sc2api/sc2_api.h"
#include "sc2utils/sc2_utils.h"

#include <cstdlib>

#if defined(_WIN32)
    #include <windows.h>
    #include <shlobj.h>      // For SHGetKnownFolderPath
#endif

namespace sc2
{
    Point2D FindRandomLocation(const Point2D &min, const Point2D &max)
    {
        Point2D target_pos;
        float playable_w = max.x - min.x;
        float playable_h = max.y - min.y;
        target_pos.x = playable_w * GetRandomFraction() + min.x;
        target_pos.y = playable_h * GetRandomFraction() + min.y;
        return target_pos;
    }

    Point2D FindRandomLocation(const GameInfo &game_info)
    {
        return FindRandomLocation(game_info.playable_min, game_info.playable_max);
    }

    Point2D FindCenterOfMap(const GameInfo &game_info)
    {
        Point2D target_pos;
        target_pos.x = game_info.playable_max.x / 2.0f;
        target_pos.y = game_info.playable_max.y / 2.0f;
        return target_pos;
    }

    static const float PI = 3.1415927f;

    size_t CalculateQueries(float radius, float step_size, const Point2D &center,
                            std::vector<QueryInterface::PlacementQuery> &queries)
    {
        Point2D current_grid, previous_grid(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        size_t valid_queries = 0;
        // Find a buildable location on the circumference of the sphere
        float loc = 0.0f;
        while (loc < 360.0f)
        {
            Point2D point = Point2D(
                (radius * std::cos((loc * PI) / 180.0f)) + center.x,
                (radius * std::sin((loc * PI) / 180.0f)) + center.y);

            QueryInterface::PlacementQuery query(ABILITY_ID::BUILD_COMMANDCENTER, point);

            current_grid = Point2D(std::floor(point.x), std::floor(point.y));

            if (previous_grid != current_grid)
            {
                queries.push_back(query);
                ++valid_queries;
            }

            previous_grid = current_grid;
            loc += step_size;
        }

        return valid_queries;
    }

    std::vector<std::pair<Point3D, std::vector<Unit> > > Cluster(const Units &units, float distance_apart)
    {
        float squared_distance_apart = distance_apart * distance_apart;
        std::vector<std::pair<Point3D, std::vector<Unit> > > clusters;
        for (size_t i = 0, e = units.size(); i < e; ++i)
        {
            const Unit &u = *units[i];

            float distance = std::numeric_limits<float>::max();
            std::pair<Point3D, std::vector<Unit> > *target_cluster = nullptr;
            // Find the cluster this mineral patch is closest to.
            for (auto &cluster: clusters)
            {
                float d = DistanceSquared3D(u.pos, cluster.first);
                if (d < distance)
                {
                    distance = d;
                    target_cluster = &cluster;
                }
            }

            // If the target cluster is some distance away don't use it.
            if (distance > squared_distance_apart)
            {
                clusters.push_back(std::pair<Point3D, std::vector<Unit> >(u.pos, std::vector<Unit>{u}));
                continue;
            }

            // Otherwise append to that cluster and update it's center of mass.
            target_cluster->second.push_back(u);
            size_t size = target_cluster->second.size();
            target_cluster->first = ((target_cluster->first * (float(size) - 1)) + u.pos) / float(size);
        }

        return clusters;
    }


    std::vector<Point3D> CalculateExpansionLocations(const ObservationInterface *observation, QueryInterface *query,
                                                     ExpansionParameters parameters)
    {
        Units resources = observation->GetUnits(
            [](const Unit &unit)
            {
                return unit.unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD || unit.unit_type ==
                       UNIT_TYPEID::NEUTRAL_MINERALFIELD750 ||
                       unit.unit_type == UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD || unit.unit_type ==
                       UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD750 ||
                       unit.unit_type == UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD || unit.unit_type ==
                       UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD750 ||
                       unit.unit_type == UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD || unit.unit_type ==
                       UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD750 ||
                       unit.unit_type == UNIT_TYPEID::NEUTRAL_LABMINERALFIELD || unit.unit_type ==
                       UNIT_TYPEID::NEUTRAL_LABMINERALFIELD750 ||
                       unit.unit_type == UNIT_TYPEID::NEUTRAL_BATTLESTATIONMINERALFIELD || unit.unit_type ==
                       UNIT_TYPEID::NEUTRAL_BATTLESTATIONMINERALFIELD750 ||
                       unit.unit_type == UNIT_TYPEID::NEUTRAL_VESPENEGEYSER || unit.unit_type ==
                       UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER ||
                       unit.unit_type == UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER || unit.unit_type ==
                       UNIT_TYPEID::NEUTRAL_PURIFIERVESPENEGEYSER ||
                       unit.unit_type == UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER || unit.unit_type ==
                       UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER;
            }
        );

        std::vector<Point3D> expansion_locations;
        std::vector<std::pair<Point3D, std::vector<Unit> > > clusters =
                Cluster(resources, parameters.cluster_distance_);

        std::vector<size_t> query_size;
        std::vector<QueryInterface::PlacementQuery> queries;
        for (size_t i = 0; i < clusters.size(); ++i)
        {
            std::pair<Point3D, std::vector<Unit> > &cluster = clusters[i];
            if (parameters.debug_)
            {
                for (auto r: parameters.radiuses_)
                {
                    parameters.debug_->DebugSphereOut(cluster.first, r, Colors::Green);
                }
            }

            // Get the required queries for this cluster.
            size_t query_count = 0;
            for (auto r: parameters.radiuses_)
            {
                query_count += CalculateQueries(r, parameters.circle_step_size_, cluster.first, queries);
            }

            query_size.push_back(query_count);
        }

        std::vector<bool> results = query->Placement(queries);
        size_t start_index = 0;
        for (int i = 0; i < clusters.size(); ++i)
        {
            std::pair<Point3D, std::vector<Unit> > &cluster = clusters[i];
            float distance = std::numeric_limits<float>::max();
            Point2D closest;

            // For each query for the cluster minimum distance location that is valid.
            for (size_t j = start_index, e = start_index + query_size[i]; j < e; ++j)
            {
                if (!results[j])
                {
                    continue;
                }

                Point2D &p = queries[j].target_pos;

                float d = Distance2D(p, cluster.first);
                if (d < distance)
                {
                    distance = d;
                    closest = p;
                }
            }

            Point3D expansion(closest.x, closest.y, cluster.second.begin()->pos.z);

            if (parameters.debug_)
            {
                parameters.debug_->DebugSphereOut(expansion, 0.35f, Colors::Red);
            }

            expansion_locations.push_back(expansion);
            start_index += query_size[i];
        }

        return expansion_locations;
    }

    std::optional<std::filesystem::path> folderExistsWithName(const std::filesystem::path &parent, const std::string &folder_name)
    {
        auto checking = parent;
        while (checking.has_parent_path())
        {
            checking = checking.parent_path();
            if (checking.filename() == folder_name)
            {
                return checking;
            }
        }
        return std::nullopt;
    }

    std::optional<std::filesystem::path> getNewestFolder(const std::vector<std::filesystem::path>& folders) {
        std::optional<std::filesystem::path> newest;
        std::filesystem::file_time_type newest_time;

        for (const auto& folder : folders) {
            if (std::filesystem::is_directory(folder)) {
                auto mod_time = std::filesystem::last_write_time(folder);
                if (!newest || mod_time > newest_time) {
                    newest = folder;
                    newest_time = mod_time;
                }
            }
        }
        return newest;
    }

    bool FindSC2VersionExe(std::filesystem::path &path, uint32_t base_build)
    {
        const std::string base_folder = "Base" + std::to_string(base_build);

        // Get the versions path.
        auto versionFolder = folderExistsWithName(path, "Versions");
        if (!versionFolder)
        {
            return true; // TODO: return folder instead? maybe we need to return false here instead?
        }

        path = versionFolder.value() / base_folder / path.filename();
        return std::filesystem::exists(path);
    }

    bool FindLatestSC2Exe(std::filesystem::path &path)
    {
        if (path.empty() || !std::filesystem::exists(path))
            return false;

        // Get the versions path.
        auto versionFolder = folderExistsWithName(path, "Versions");
        if (!versionFolder)
        {
            return true; // TODO: return folder instead? maybe we need to return false here instead?
        }

        // Get a list of all subfolders.
        auto subfolders = fs::ScanDirectory(versionFolder.value(), true, true);
        if (!subfolders)
        {
            return false;
        }

        // Find the newest one
        auto baseFolder = getNewestFolder(subfolders.value());
        if (!baseFolder)
        {
            return false;
        }

        // Add the executable name
        path = baseFolder.value() / path.filename();

        // Check if it exists and return
        return std::filesystem::exists(path);
    }

    std::filesystem::path GetGameDataDirectory()
    {
#if defined(_WIN32)
        PWSTR path_tmp = nullptr;
        // FOLDERID_Documents is the modern way to get “My Documents”
        HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &path_tmp);
        if (SUCCEEDED(hr) && path_tmp) {
            std::filesystem::path result = path_tmp;
            CoTaskMemFree(path_tmp);
            return result;
        }
#else
        std::string result = std::getenv("HOME");
        result += "/Blizzard";
        if (!result.empty())
            return std::filesystem::path(result);
#endif

        return std::filesystem::current_path();
    }

    std::filesystem::path GetLibraryMapsDirectory()
    {
        auto currentPath = std::filesystem::current_path();
        return currentPath.parent_path().parent_path().parent_path() / "maps";
    }

    std::filesystem::path GetGameMapsDirectory(const std::filesystem::path &path)
    {
        return path.parent_path().parent_path().parent_path() / "maps";
    }
}
