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
    bool DoesFileExist(const std::filesystem::path &path)
    {
        return std::filesystem::exists(path);
    }

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
    Process::~Process()
    {
        terminate();
    }

    bool Process::start(const std::filesystem::path &process_path, const std::vector<std::string> &command_line)
    {
#if defined(_WIN32)
        if (hProcess) {
            CloseHandle(hProcess);
            hProcess = nullptr;
        }

        static const unsigned int buffer_size = (1 << 16) + 1;

        char current_directory[buffer_size];
        char support_directory[buffer_size];

        strcpy_s(support_directory, process_path.string().c_str());
        int slashcount = 0;
        for (size_t i = strlen(support_directory); i > 0 && slashcount < 3; --i) {
            if (support_directory[i] == '/' || support_directory[i] == '\\') {
                ++slashcount;
                if (slashcount > 1)
                    support_directory[i + 1] = 0;
            }
        }
        if (slashcount != 3)
            return false;

        SPDLOG_INFO("Starting process: {}", process_path.string());
        std::string exe_name = "C:\\Program Files (x86)\\StarCraft II\\Versions\\Base93333\\SC2_x64.exe";
        exe_name = process_path.string();
        SPDLOG_INFO("Starting process: {}", exe_name);
        bool is64bit = exe_name.find("_x64") != std::string::npos;
        if (is64bit)
            strcat_s(support_directory, "Support64");
        else
            strcat_s(support_directory, "Support");

        char buffer[buffer_size];
        std::memset(buffer, 0, buffer_size);
        for (int i = 0; i < command_line.size(); ++i) {
            strcat_s(buffer, " ");
            strcat_s(buffer, command_line[i].c_str());
        }

        STARTUPINFOA si = { sizeof(STARTUPINFOA) };
        PROCESS_INFORMATION pi;
        BOOL success = CreateProcessA(
            exe_name.c_str(),  // Module name
            buffer,                         // Command line
            nullptr,                        // Process handle not inheritable
            nullptr,                        // Thread handle not inheritable
            FALSE,                          // Set handle inheritance to FALSE
            0,                              // No creation flags
            nullptr,                        // Use parent's environment block
            support_directory,              // Use parent's starting directory
            &si,                            // Pointer to PROCESS_INFORMATION structure
            &pi                             // Pointer to PROCESS_INFORMATION structure
            );
        if (!success) {
            return false;
        }
        CloseHandle(pi.hThread);
        hProcess = pi.hProcess;
        return true;
#else
        pid_t pid = fork();
        if (pid == 0) {
            std::vector<char*> argv;
            std::string exec_path = process_path.string();
            argv.push_back(const_cast<char*>(exec_path.c_str()));
            for (const auto& arg : command_line) {
                argv.push_back(const_cast<char*>(arg.c_str()));
            }
            argv.push_back(nullptr);
            execvp(exec_path.c_str(), argv.data());
            _exit(1); // exec failed
        }
        if (pid > 0) {
            processId = static_cast<uint64_t>(pid);
            return true;
        }
        processId = 0;
        return false;
#endif
    }

    bool Process::isRunning()
    {
#if defined(_WIN32)
        if (!hProcess) return false;
        DWORD exitCode = 0;
        BOOL result = GetExitCodeProcess(hProcess, &exitCode);
        return result && (exitCode == STILL_ACTIVE);
#else
        if (processId == 0) return false;
        return (kill(static_cast<pid_t>(processId), 0) == 0);
#endif

    }

    uint64_t Process::getProcessId() const
    {
#if defined(_WIN32)
        if (!hProcess)
            return 0;
        return static_cast<uint64_t>(::GetProcessId(hProcess));
#else
        return processId;
#endif
    }


    bool Process::terminate()
    {
#if defined(_WIN32)
        if (!hProcess) return false;
        BOOL result = ::TerminateProcess(hProcess, 1);
        CloseHandle(hProcess);
        hProcess = nullptr;
        return result ? true : false;
#else
        if (processId == 0) return false;
        bool result = (kill(static_cast<pid_t>(processId), SIGTERM) == 0);
        if (result) {
            int status = 0;
            waitpid(static_cast<pid_t>(processId), &status, 0); // Wait for process to exit
            processId = 0;
        }

        return result;
#endif
    }




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
