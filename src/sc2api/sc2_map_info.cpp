#include "sc2api/sc2_map_info.h"

#include <fstream>

namespace sc2 {

GameInfo::GameInfo () :
    width(0),
    height(0)
{
}

SampleImage::SampleImage(const SC2APIProtocol::ImageData& data):
    data_(data.data()), area_({0, 0}, {data.size().x(), data.size().y()}),
    bits_per_pixel_(data.bits_per_pixel()) {
}

SampleImage::SampleImage(const ImageData& data):
    data_(data.data), area_({0, 0}, {data.width, data.height}),
    bits_per_pixel_(data.bits_per_pixel) {
}

bool SampleImage::GetBit(const Point2i& point, bool* dst) const {
    assert(bits_per_pixel_ == 1);

    if (!area_.contains(point))
        return false;

    auto areaSize = area_.size(); // width, height, depth
    div_t idx = div(point.x() + point.y() * areaSize[0], 8);
    *dst = (data_[idx.quot] >> (7 - idx.rem)) & 1;
    return true;
}

bool SampleImage::GetBit(const Point2i& point, unsigned char* dst) const {
    assert(bits_per_pixel_ > 1);

    if (!area_.contains(point))
        return false;

    // Image data is stored with an upper left origin.
    auto areaSize = area_.size(); // width, height, depth
    assert(data_.size() == areaSize[0] * areaSize[1]);
    *dst = data_[point.x() + point.y() * areaSize[0]];
    return true;
}

int SampleImage::BPP() const {
    return bits_per_pixel_;
}

Rect2i SampleImage::Area() const {
    return area_;
}

PathingGrid::PathingGrid(const GameInfo& info):
    pathing_grid_(info.pathing_grid) {
}

bool PathingGrid::IsPathable(const Point2i& point) const {
    if (pathing_grid_.BPP() == 1) {
        bool value;
        if (!pathing_grid_.GetBit(point, &value))
            return false;

        return value;
    }

    unsigned char value;
    if (!pathing_grid_.GetBit(point, &value))
        return false;

    return value != 255;
}

void PathingGrid::Dump(const std::string& file_path) const {
    std::ofstream dst(file_path);

    auto areaSize = pathing_grid_.Area().size(); // width, height, depth
    for (int y = areaSize[1] - 1; y >= 0; --y) {
        for (int x = 0; x < areaSize[0]; ++x) {
            dst << (IsPathable({x, y}) ? ' ' : '#');
        }

        dst << std::endl;
    }
}

PlacementGrid::PlacementGrid(const GameInfo& info):
    placement_grid_(info.placement_grid) {
}

bool PlacementGrid::IsPlacable(const Point2i& point) const {
    if (placement_grid_.BPP() == 1) {
        bool value;
        if (!placement_grid_.GetBit(point, &value))
            return false;

        return value;
    }

    unsigned char value;
    if (!placement_grid_.GetBit(point, &value))
        return false;

    return value == 255;
}

void PlacementGrid::Dump(const std::string& file_path) const {
    std::ofstream dst(file_path);

    auto areaSize = placement_grid_.Area().size(); // width, height, depth
    for (int y = areaSize[1] - 1; y >= 0; --y) {
        for (int x = 0; x < areaSize[0]; ++x) {
            dst << (IsPlacable({x, y}) ? ' ' : '#');
        }

        dst << std::endl;
    }
}

HeightMap::HeightMap(const GameInfo& info):
    height_map_(info.terrain_height) {
}

float HeightMap::TerrainHeight(const Point2i& point) const {
    unsigned char value;
    if (!height_map_.GetBit(point, &value))
        return 0.0f;

    return (static_cast<float>(value) - 127) / 8.f;
}

void HeightMap::Dump(const std::string& file_path) const {
    std::ofstream dst(file_path);

    auto areaSize = height_map_.Area().size(); // width, height, depth
    for (int x = 0; x < areaSize[0]; ++x) {
        for (int y = 0; y < areaSize[1]; ++y)
            dst << TerrainHeight({x, y}) << "|";

        dst << std::endl;
    }
}

}
