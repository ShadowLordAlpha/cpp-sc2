#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <optional>

#if defined(_WIN32)
#define NOMINMAX
#endif


namespace sc2::fs
{
    // TODO: proper documentation

    /**
     * TODO: extension assumes the leading . is included
     * @param path
     * @param extension
     * @return
     */
    bool HasExtension(const std::filesystem::path &path, const std::string &extension);
    std::optional<std::vector<std::filesystem::path>> ScanDirectory(const std::filesystem::path &path, bool full_path = false, bool list_directories = false);

}

namespace sc2
{


    void SleepFor(unsigned int ms);
    bool PollKeyPress();
}
