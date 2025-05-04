#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <filesystem>

#if defined(_WIN32)
#define NOMINMAX
    #include <Windows.h>
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
    std::string GetGameDataDirectory();
    std::string GetLibraryMapsDirectory();
    std::filesystem::path GetGameMapsDirectory(const std::filesystem::path &path);
}

namespace sc2
{
    bool FindLatestExe(std::filesystem::path &path);
    bool FindBaseExe(std::filesystem::path &path, uint32_t base_build);

    void SleepFor(unsigned int ms);
    bool PollKeyPress();
}
