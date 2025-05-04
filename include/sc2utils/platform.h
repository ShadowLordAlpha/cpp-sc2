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
     * @brief Convienence method to make sure a file exists
     * @param path The Filesystem to check
     * @return True if the file exists
     */
    bool DoesFileExist(const std::filesystem::path &path);

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
    class Process
    {
    public:
        ~Process();

        bool start(const std::filesystem::path &process_path, const std::vector<std::string> &command_line);
        bool isRunning();
        [[nodiscard]] uint64_t getProcessId() const;
        bool terminate();
    private:
#if defined(_WIN32)
        HANDLE hProcess = nullptr;
#else
        uint64_t processId = 0;
#endif

    };
}

namespace sc2
{
    bool FindLatestExe(std::filesystem::path &path);
    bool FindBaseExe(std::filesystem::path &path, uint32_t base_build);

    void SleepFor(unsigned int ms);
    bool PollKeyPress();
}
