#include "sc2utils/platform.h"


#include <algorithm>   // For std::find

#include <cstdio>
#include <chrono>

#include <string>
#include <vector>
#include <filesystem>
#include <iostream>


#if defined(_WIN32)

// Windows headers for process manipulation.
#include <windows.h>
#include <shlobj.h>
#include <conio.h>
#include <tchar.h>
#include <codecvt>
#include <locale>

#include <spdlog/spdlog.h>

#elif defined(__APPLE__)

// Mac headers for process manipulation.
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <errno.h>
#include <cstring>
#include <unistd.h>
#include <pwd.h>
#include <termios.h>
#include <mach-o/dyld.h>
#include <ctype.h>

#include <Carbon/Carbon.h>

#elif defined(__linux__)

// Linux headers for process manipulation.
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <errno.h>
#include <cstring>
#include <unistd.h>
#include <pwd.h>
#include <termios.h>
#include <linux/limits.h>

#else
#error "Unsupported platform"
#endif

namespace sc2::fs
{

    bool HasExtension(const std::filesystem::path &path, const std::string &extension)
    {
        return path.extension() == extension;
    }

    std::optional<std::vector<std::filesystem::path>> ScanDirectory(const std::filesystem::path &path, bool full_path, bool list_directories)
    {
        if (!std::filesystem::exists(path))
        {
            SPDLOG_ERROR("Path {} does not exist", path.string());
            return std::nullopt;
        }

        if (!std::filesystem::is_directory(path))
        {
            SPDLOG_ERROR("Path {} is not a directory", path.string());
            return std::nullopt;
        }

        std::vector<std::filesystem::path> result;
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (std::filesystem::is_regular_file(entry) && !list_directories) {
                result.push_back(entry);
            } else if (std::filesystem::is_directory(entry) && list_directories) {
                result.push_back(entry);
            }
        }

        return result;
    }

    std::string GetGameDataDirectory()
    {
#if defined(_WIN32)
        unsigned int csidl = CSIDL_PERSONAL;
        WCHAR windowsPath[MAX_PATH];

        HRESULT result = SHGetFolderPathW(nullptr, csidl, nullptr, SHGFP_TYPE_CURRENT, windowsPath);

        if (result == S_OK)
        {
            std::wstring_convert<std::codecvt_utf8_utf16<WCHAR>, WCHAR> convertor;
            return convertor.to_bytes(windowsPath);
        }

        return std::string();
#endif
    }

    std::string GetLibraryMapsDirectory()
    {
#if defined(_WIN32)
        std::string result = ""; // GetExePath(); // TODO: this doesn't really seem right?
        result = result.substr(0, result.find_last_of("\\"));
        result = result.substr(0, result.find_last_of("\\"));
        result = result.substr(0, result.find_last_of("\\"));
        result += "\\maps\\";
        return result;
#endif
    }

    std::filesystem::path GetGameMapsDirectory(const std::filesystem::path &path)
    {
#if defined(_WIN32)
        return path.parent_path().parent_path().parent_path() / "maps";
#endif
    }
}


namespace sc2
{
#ifdef _WIN32

    static std::string GetExePath()
    {
        WCHAR windowsPath[MAX_PATH];

        DWORD length = GetModuleFileNameW(0, windowsPath, MAX_PATH);
        if (length > 0)
        {
            std::wstring_convert<std::codecvt_utf8_utf16<WCHAR>, WCHAR> convertor;
            return convertor.to_bytes(windowsPath);
        }

        return std::string();
    }





#elif defined(__linux__) || defined(__APPLE__)





#if defined(__linux__)
std::string GetUserDirectory() {
    const char* home_directory = getenv("HOME");
    if (!home_directory)
        home_directory = getpwuid(getuid())->pw_dir;
    return std::string(home_directory);
}
#else

void GetDirectory(std::string& path, uint32_t folderType, short domain) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

    FSRef fsref;
    OSErr err = FSFindFolder(domain, folderType, false, &fsref);

    if (err == noErr) {
        char pathBuffer[PATH_MAX];
        FSRefMakePath(&fsref, reinterpret_cast<unsigned char*>(pathBuffer), PATH_MAX);
        path = pathBuffer;
    }

#pragma clang diagnostic pop
}

std::string GetUserDirectory() {
    std::string result;
    GetDirectory(result, kApplicationSupportFolderType, kUserDomain);
    result += "/Blizzard";
    return result;
}
#endif

static std::string GetExePath() {
#if defined(__linux__)
    char path[PATH_MAX + 1] = { 0 };
    if (readlink("/proc/self/exe", path, PATH_MAX) == -1)
        return std::string();

    return std::string(path);
#else
    char path[PATH_MAX];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) != 0)
        return std::string();

    return std::string(path);
#endif
}

std::string GetLibraryMapsDirectory() {
    std::string result = GetExePath();

    char* resolvedPath = realpath(result.c_str(), nullptr);
    if (resolvedPath != nullptr) {
        result = resolvedPath;
        free(resolvedPath);
    }

    result = result.substr(0, result.find_last_of("/"));
    result = result.substr(0, result.find_last_of("/"));
    result = result.substr(0, result.find_last_of("/"));
    result += "/maps/";
    return result;
}

std::string GetGameMapsDirectory(const std::string& process_path) {
    std::string result = process_path;
#if defined(__linux__)
    result = result.substr(0, result.find_last_of("/"));
    result = result.substr(0, result.find_last_of("/"));
    result = result.substr(0, result.find_last_of("/"));
#else
    result = result.substr(0, result.find_last_of("/"));
    result = result.substr(0, result.find_last_of("/"));
    result = result.substr(0, result.find_last_of("/"));
    result = result.substr(0, result.find_last_of("/"));
    result = result.substr(0, result.find_last_of("/"));
    result = result.substr(0, result.find_last_of("/"));
#endif
    result += "/maps/";
    return result;
}

#endif

    void SleepFor(unsigned int ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    bool PollKeyPress()
    {
#ifdef _WIN32
        return _kbhit();
#else
        static const int STDIN = 0;
        static bool initialized = false;

        if (!initialized) {
            termios term;
            tcgetattr(STDIN, &term);
            term.c_lflag &= ~ICANON;
            tcsetattr(STDIN, TCSANOW, &term);
            setbuf(stdin, NULL);
            initialized = true;
        }

        int bytesWaiting;
        ioctl(STDIN, FIONREAD, &bytesWaiting);
        return bytesWaiting > 0;
#endif
    }

    std::optional<std::filesystem::path> folderExistsWithName(const std::filesystem::path& parent, const std::string& folder_name)
    {
        auto checking = parent;
        while (checking.has_parent_path()) {
            checking = checking.parent_path();
            if (checking.filename() == folder_name) {
                return checking;
            }
    }
        return std::nullopt;
    }

    bool FindBaseExe(std::filesystem::path& path, uint32_t base_build) {
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


    bool FindLatestExe(std::filesystem::path &path)
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
}
