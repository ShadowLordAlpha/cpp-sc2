/**
 *
 */

#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <memory>

#ifdef _WIN32
#include <windows.h>
#endif

namespace astra::process
{
    /**
     * @class Process
     */
    class Process
    {
    public:
        Process();
        ~Process();

        void setWorkingDirectory(const std::filesystem::path& path);
        void setArguments(const std::vector<std::string>& args);
        void setExecutable(const std::filesystem::path& path);

        bool start();
        bool isRunning();
        void stop();

    private:
        std::filesystem::path executablePath;
        std::vector<std::string> arguments;
        std::filesystem::path workingDirectory;
#ifdef _WIN32
        HANDLE processHandle = nullptr;
#endif
    };
}
