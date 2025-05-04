#include "sc2utils/platform.h"


#include <algorithm>   // For std::find

#include <cstdio>
#include <chrono>

#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <spdlog/spdlog.h>

#if defined(_WIN32)

// Windows headers for process manipulation.
#include <windows.h>
#include <shlobj.h>
#include <conio.h>
#include <tchar.h>
#include <codecvt>
#include <locale>



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

//     std::string GetGameDataDirectory()
//     {
// #if defined(_WIN32)
//         unsigned int csidl = CSIDL_PERSONAL;
//         WCHAR windowsPath[MAX_PATH];
//
//         HRESULT result = SHGetFolderPathW(nullptr, csidl, nullptr, SHGFP_TYPE_CURRENT, windowsPath);
//
//         if (result == S_OK)
//         {
//             std::wstring_convert<std::codecvt_utf8_utf16<WCHAR>, WCHAR> convertor;
//             return convertor.to_bytes(windowsPath);
//         }
//
//         return std::string();
// #endif
//     }
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







}
