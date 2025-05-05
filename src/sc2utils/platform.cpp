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
}


namespace sc2
{
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
