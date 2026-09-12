#include "sc2_search.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>
#include <utility>
#include <vector>

#include "sc2api/sc2_map_info.h"
#include "sc2api/sc2_typeenums.h"
#include "sc2api/sc2_unit_filters.h"

namespace {
const float PI = 3.1415927F;
// python-sc2 compares raw 0-255 height bytes with delta <= 10 (~1.25 world units).
const float kHeightMergeDelta = 10.0F / 8.0F;
const int kOffsetRange = 7;
const size_t kMaxResourcesPerExpansion = 12;
}  // namespace

namespace sc2::search {

size_t CalculateQueries(float radius, float step_size, const Point2D& center,
                        std::vector<QueryInterface::PlacementQuery>& queries) {
    Point2D current_grid;
    Point2D previous_grid(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    size_t valid_queries = 0;
    // Find a buildable location on the circumference of the sphere
    float loc = 0.0F;
    while (loc < 360.0F) {
        const Point2D point = Point2D((radius * std::cos((loc * PI) / 180.0F)) + center.x,
                                      (radius * std::sin((loc * PI) / 180.0F)) + center.y);

        const QueryInterface::PlacementQuery query(ABILITY_ID::BUILD_COMMANDCENTER, point);

        current_grid = Point2D(std::floor(point.x), std::floor(point.y));

        if (previous_grid != current_grid) {
            queries.push_back(query);
            ++valid_queries;
        }

        previous_grid = current_grid;
        loc += step_size;
    }

    return valid_queries;
}

std::vector<std::pair<Point3D, std::vector<Unit> > > Cluster(const Units& units, float distance_apart) {
    const float squared_distance_apart = distance_apart * distance_apart;
    std::vector<std::pair<Point3D, std::vector<Unit> > > clusters;
    for (const auto* unit : units) {
        const Unit& u = *unit;

        float distance = std::numeric_limits<float>::max();
        std::pair<Point3D, std::vector<Unit> >* target_cluster = nullptr;
        // Find the cluster this mineral patch is closest to.
        for (auto& cluster : clusters) {
            const float d = DistanceSquared3D(u.pos, cluster.first);
            if (d < distance) {
                distance = d;
                target_cluster = &cluster;
            }
        }

        // If the target cluster is some distance away don't use it.
        if (distance > squared_distance_apart) {
            clusters.push_back(std::pair<Point3D, std::vector<Unit> >(u.pos, std::vector<Unit>{u}));
            continue;
        }

        // Otherwise append to that cluster and update it's center of mass.
        target_cluster->second.push_back(u);
        auto size = static_cast<float>(target_cluster->second.size());
        target_cluster->first = ((target_cluster->first * (size - 1)) + u.pos) / size;
    }

    return clusters;
}

namespace {

Units GatherExpansionResources(const ObservationInterface* observation) {
    const IsMineralPatch is_mineral;
    const IsGeyser is_geyser;
    return observation->GetUnits([&](const Unit& unit) {
        if (unit.unit_type == UNIT_TYPEID::NEUTRAL_MINERALFIELD450) {
            return false;
        }
        return is_mineral(unit) || is_geyser(unit);
    });
}

Point2D ClusterCenter(const Units& group) {
    float total_x = 0.0F;
    float total_y = 0.0F;
    for (const auto* unit : group) {
        total_x += unit->pos.x;
        total_y += unit->pos.y;
    }
    const auto count = static_cast<float>(group.size());
    return {total_x / count, total_y / count};
}

bool SameTerrainHeight(const HeightMap& height, const Units& group_a, const Units& group_b) {
    for (const auto* a : group_a) {
        const float ha = height.TerrainHeight(Point2DI(a->pos));
        for (const auto* b : group_b) {
            const float hb = height.TerrainHeight(Point2DI(b->pos));
            if (std::abs(ha - hb) > kHeightMergeDelta) {
                return false;
            }
        }
    }
    return true;
}

std::vector<Units> MergeResourceGroups(const Units& resources, float distance_apart, const HeightMap& height) {
    std::vector<Units> groups;
    groups.reserve(resources.size());
    for (const auto* resource : resources) {
        groups.push_back(Units{resource});
    }

    bool merged = true;
    while (merged) {
        merged = false;
        for (size_t i = 0; i < groups.size() && !merged; ++i) {
            for (size_t j = i + 1; j < groups.size(); ++j) {
                const Point2D center_a = ClusterCenter(groups[i]);
                const Point2D center_b = ClusterCenter(groups[j]);
                if (Distance2D(center_a, center_b) > distance_apart) {
                    continue;
                }
                if (!SameTerrainHeight(height, groups[i], groups[j])) {
                    continue;
                }
                groups[i].insert(groups[i].end(), groups[j].begin(), groups[j].end());
                groups.erase(groups.begin() + static_cast<std::ptrdiff_t>(j));
                merged = true;
                break;
            }
        }
    }
    return groups;
}

std::vector<Point2D> ExpansionOffsets() {
    std::vector<Point2D> offsets;
    for (int x = -kOffsetRange; x <= kOffsetRange; ++x) {
        for (int y = -kOffsetRange; y <= kOffsetRange; ++y) {
            const float hypot = std::hypot(static_cast<float>(x), static_cast<float>(y));
            if (hypot > 4.0F && hypot <= 8.0F) {
                offsets.emplace_back(static_cast<float>(x), static_cast<float>(y));
            }
        }
    }
    return offsets;
}

bool IsGeyserUnit(const Unit& unit) {
    return IsGeyser{}(unit);
}

std::optional<Point3D> FindExpansionLocation(const Units& resources, const std::vector<Point2D>& offsets,
                                             const PlacementGrid& placement) {
    if (resources.empty()) {
        return std::nullopt;
    }

    float sum_x = 0.0F;
    float sum_y = 0.0F;
    for (const auto* resource : resources) {
        sum_x += resource->pos.x;
        sum_y += resource->pos.y;
    }
    const auto amount = static_cast<float>(resources.size());
    // Town halls are 5x5, so valid centers sit on .5/.5.
    const float center_x = std::floor(sum_x / amount) + 0.5F;
    const float center_y = std::floor(sum_y / amount) + 0.5F;

    std::optional<Point2D> best;
    float best_score = std::numeric_limits<float>::max();
    for (const auto& offset : offsets) {
        const Point2D point(center_x + offset.x, center_y + offset.y);
        if (!placement.IsPlacable(Point2DI(point))) {
            continue;
        }

        bool far_enough = true;
        float score = 0.0F;
        for (const auto* resource : resources) {
            const float distance = Distance2D(point, resource->pos);
            const float min_distance = IsGeyserUnit(*resource) ? 7.0F : 6.0F;
            if (distance < min_distance) {
                far_enough = false;
                break;
            }
            score += distance;
        }
        if (!far_enough) {
            continue;
        }
        if (score < best_score) {
            best_score = score;
            best = point;
        }
    }

    if (!best) {
        return std::nullopt;
    }
    return Point3D(best->x, best->y, resources.front()->pos.z);
}

bool HasOppositeSideGeyserLayout(const Units& minerals, const Units& geysers) {
    if (geysers.size() != 2 || minerals.size() < 6) {
        return false;
    }

    float max_distance = 0.0F;
    const Unit* mineral_1 = minerals[0];
    const Unit* mineral_2 = minerals[1];
    for (size_t i = 0; i < minerals.size(); ++i) {
        for (size_t j = i + 1; j < minerals.size(); ++j) {
            const float distance = Distance2D(minerals[i]->pos, minerals[j]->pos);
            if (distance > max_distance) {
                max_distance = distance;
                mineral_1 = minerals[i];
                mineral_2 = minerals[j];
            }
        }
    }
    if (max_distance < 4.0F) {
        return false;
    }

    const float x1 = mineral_1->pos.x;
    const float y1 = mineral_1->pos.y;
    const float x2 = mineral_2->pos.x;
    const float y2 = mineral_2->pos.y;
    const Unit* geyser_1 = geysers[0];
    const Unit* geyser_2 = geysers[1];

    if (std::abs(x2 - x1) < 0.1F) {
        const float line_x = (x1 + x2) / 2.0F;
        const float side_1 = geyser_1->pos.x - line_x;
        const float side_2 = geyser_2->pos.x - line_x;
        return side_1 * side_2 < 0.0F && std::abs(side_1) > 3.0F && std::abs(side_2) > 3.0F;
    }

    const float slope = (y2 - y1) / (x2 - x1);
    const float intercept = y1 - slope * x1;
    const float side_1 = geyser_1->pos.y - slope * geyser_1->pos.x - intercept;
    const float side_2 = geyser_2->pos.y - slope * geyser_2->pos.x - intercept;
    return side_1 * side_2 < 0.0F;
}

void SplitMineralsAndGeysers(const Units& resources, Units& minerals, Units& geysers) {
    minerals.clear();
    geysers.clear();
    for (const auto* resource : resources) {
        if (IsGeyserUnit(*resource)) {
            geysers.push_back(resource);
        } else {
            minerals.push_back(resource);
        }
    }
}

std::vector<Point3D> CalculateExpansionLocationsFromGrid(const Units& resources, const GameInfo& game_info,
                                                         ExpansionParameters parameters) {
    const PlacementGrid placement(game_info);
    const HeightMap height(game_info);
    const std::vector<Point2D> offsets = ExpansionOffsets();
    const std::vector<Units> groups = MergeResourceGroups(resources, parameters.cluster_distance_, height);

    std::vector<Point3D> expansion_locations;
    for (const auto& group : groups) {
        if (group.size() > kMaxResourcesPerExpansion) {
            continue;
        }

        Units minerals;
        Units geysers;
        SplitMineralsAndGeysers(group, minerals, geysers);

        auto append_location = [&](const Units& local_resources) {
            const auto location = FindExpansionLocation(local_resources, offsets, placement);
            if (!location) {
                return;
            }
            if (parameters.debug_) {
                parameters.debug_->DebugSphereOut(*location, 0.35F, Colors::Red);
            }
            expansion_locations.push_back(*location);
        };

        if (HasOppositeSideGeyserLayout(minerals, geysers)) {
            for (const auto* geyser : geysers) {
                Units local = minerals;
                local.push_back(geyser);
                append_location(local);
            }
            continue;
        }

        append_location(group);
    }
    return expansion_locations;
}

std::vector<Point3D> CalculateExpansionLocationsByQuery(const Units& resources, QueryInterface* query,
                                                        ExpansionParameters parameters) {
    std::vector<Point3D> expansion_locations;
    std::vector<std::pair<Point3D, std::vector<Unit> > > clusters = Cluster(resources, parameters.cluster_distance_);

    std::vector<size_t> query_size;
    std::vector<QueryInterface::PlacementQuery> queries;
    for (const auto& cluster : clusters) {
        if (parameters.debug_) {
            for (auto r : parameters.radiuses_) {
                parameters.debug_->DebugSphereOut(cluster.first, r, Colors::Green);
            }
        }

        size_t query_count = 0;
        for (auto r : parameters.radiuses_) {
            query_count += CalculateQueries(r, parameters.circle_step_size_, cluster.first, queries);
        }

        query_size.push_back(query_count);
    }

    if (queries.empty()) {
        return expansion_locations;
    }

    std::vector<bool> results = query->Placement(queries);
    size_t start_index = 0;
    for (size_t i = 0; i < clusters.size(); ++i) {
        auto& cluster = clusters[i];
        float distance = std::numeric_limits<float>::max();
        Point2D closest;
        bool found = false;

        for (size_t j = start_index, e = start_index + query_size[i]; j < e; ++j) {
            if (!results[j]) {
                continue;
            }

            const Point2D& p = queries[j].target_pos;
            const float d = Distance2D(p, cluster.first);
            if (d < distance) {
                distance = d;
                closest = p;
                found = true;
            }
        }

        start_index += query_size[i];
        if (!found) {
            continue;
        }

        const Point3D expansion(closest.x, closest.y, cluster.second.begin()->pos.z);
        if (parameters.debug_) {
            parameters.debug_->DebugSphereOut(expansion, 0.35F, Colors::Red);
        }
        expansion_locations.push_back(expansion);
    }

    return expansion_locations;
}

}  // namespace

std::vector<Point3D> CalculateExpansionLocations(const ObservationInterface* observation, QueryInterface* query,
                                                 ExpansionParameters parameters) {
    if (!observation) {
        return {};
    }

    const Units resources = GatherExpansionResources(observation);
    if (resources.empty()) {
        return {};
    }

    const GameInfo& game_info = observation->GetGameInfo();
    if (game_info.placement_grid.width > 0 && game_info.placement_grid.height > 0 &&
        !game_info.placement_grid.data.empty()) {
        return CalculateExpansionLocationsFromGrid(resources, game_info, parameters);
    }

    if (!query) {
        return {};
    }
    return CalculateExpansionLocationsByQuery(resources, query, parameters);
}

}  // namespace sc2::search
