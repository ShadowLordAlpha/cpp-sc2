#include "ProcessManager.h"

#include <format>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <cstring>
#endif

namespace sc2
{
    class ProcessManager::Impl
    {
    public:
        Impl(const std::filesystem::path &processPath, std::string_view args, const std::filesystem::path &workingDirectory) : process(processPath), args(args), workingDirectory(workingDirectory) {}

        ~Impl()
        {
            if(isRunning())
            {
                (void) terminate();
            }
        }

        std::expected<void, std::pair<ProcessError, std::string>> start()
        {
            if(!std::filesystem::exists(process))
            {
                return std::unexpected(std::make_pair(ProcessError::FileNotFound, std::format("Unable to find executable file: {}", process.string())));
            }

            if(!std::filesystem::is_directory(workingDirectory))
            {
                return std::unexpected(std::make_pair(ProcessError::FileNotFound, std::format("Unable to find working directory: {}", workingDirectory.string())));
            }
#ifdef _WIN32
                STARTUPINFOW si = { sizeof(si) };
                PROCESS_INFORMATION pi = {};
                std::wstring cmd = L"\"" + process.wstring() + L"\" " + std::wstring(args.begin(), args.end());
                std::wstring wd = workingDirectory.wstring();

                if (!CreateProcessW(nullptr, cmd.data(), nullptr, nullptr, FALSE, 0, nullptr, wd.c_str(), &si, &pi)) {
                    return std::unexpected(std::make_pair(ProcessError::StartFailed, std::format("CreateProcess failed: {}", GetLastError())));
                }

                hProcess = pi.hProcess;
                pid = pi.dwProcessId;
                CloseHandle(pi.hThread);
#else
            pImpl->pid = fork();
            if (pImpl->pid == -1) {
                return std::unexpected(std::make_pair(ProcessError::StartFailed, std::format("fork failed: {}", strerror(errno))));
            }
            if (pImpl->pid == 0) { // Child process
                if (chdir(pImpl->workingDirectory.c_str()) != 0) {
                    _exit(EXIT_FAILURE);
                }
                std::vector<std::string> argVec;
                argVec.push_back(pImpl->processPath.string());
                std::istringstream iss(pImpl->args);
                std::string arg;
                while (iss >> arg) {
                    argVec.push_back(arg);
                }
                std::vector<char*> argv;
                for (auto& a : argVec) {
                    argv.push_back(a.data());
                }
                argv.push_back(nullptr);
                execvp(pImpl->processPath.c_str(), argv.data());
                _exit(EXIT_FAILURE); // execvp failed
            }
#endif
        }

        std::expected<void, std::pair<ProcessError, std::string>> terminate()
        {
            if(isRunning())
            {
                return std::unexpected(std::make_pair(ProcessError::ProcessNotRunning, "Process is not running"));
            }
#ifdef _WIN32
            if(!TerminateProcess(hProcess, 1))
            {
                return std::unexpected(std::make_pair(ProcessError::TerminateFailed, std::format("TerminateProcess failed: {}", GetLastError())));
            }
#else
            if(kill(pid, SIGTERM) == -1)
            {
                return std::unexpected(std::make_pair(ProcessError::TerminateFailed, std::format("kill failed: {}", strerror(errno))));
            }
#endif
        }

        std::expected<int, std::pair<ProcessError, std::string>> wait() const
        {
            if(isRunning())
            {
                return std::unexpected(std::make_pair(ProcessError::ProcessNotRunning, "Process is not running"));
            }
#ifdef _WIN32
            if (WaitForSingleObject(hProcess, INFINITE) != WAIT_OBJECT_0) {
                return std::unexpected(std::make_pair(ProcessError::WaitFailed, std::format("WaitForSingleObject failed: {}", GetLastError())));
            }
            DWORD exitCode;
            if (!GetExitCodeProcess(hProcess, &exitCode)) {
                return std::unexpected(std::make_pair(ProcessError::WaitFailed, std::format("GetExitCodeProcess failed: {}", GetLastError())));
            }
            return static_cast<int>(exitCode);
#else
            int status;
            if (waitpid(pid, &status, 0) == -1) {
                return std::unexpected(std::make_pair(ProcessError::WaitFailed, std::format("waitpid failed: {}", strerror(errno))));
            }
            if (WIFEXITED(status)) {
                return WEXITSTATUS(status);
            }
            return std::unexpected(std::make_pair(ProcessError::NotExitedNormally, "Process did not exit normally"));
#endif
        }

        [[nodiscard]] bool isRunning() const
        {
#ifdef _WIN32
            if (!hProcess) return false;
            DWORD exitCode;
            if(GetExitCodeProcess(hProcess, &exitCode))
            {
                return exitCode == STILL_ACTIVE;
            }
            return false;
#else
            if (pid == 0) return false;
            int status;
            pid_t result = waitpid(pid, &status, WNOHANG);
            return result == 0;
#endif
        }

        [[nodiscard]] std::optional<int> getPid() const
        {
            return pid > 0 ? std::optional<int>(pid) : std::nullopt;
        }

        [[nodiscard]] std::string string() const noexcept
        {
            return std::format(R"(ProcessManager(process="{}", working_dir="{}", pid={}))", process.string(), workingDirectory.string(), std::to_string(pid));
        }

    protected:
        std::filesystem::path process;
        std::filesystem::path workingDirectory;
        std::string args;
#ifdef _WIN32
        HANDLE hProcess = nullptr;
        DWORD pid = 0;
#else
        pid_t pid = -1;
#endif
    };

    ProcessManager::ProcessManager(const std::filesystem::path &processPath, std::string_view args, const std::filesystem::path &workingDirectory) : processManagerImpl(std::make_unique<Impl>(processPath, args, workingDirectory)) {}
    ProcessManager::~ProcessManager() = default;

    ProcessManager::ProcessManager(ProcessManager&&) noexcept = default;
    ProcessManager& ProcessManager::operator=(ProcessManager&&) noexcept = default;

    std::expected<void, std::pair<ProcessError, std::string>> ProcessManager::start()
    {
        return processManagerImpl->start();
    }

    bool ProcessManager::isRunning() const
    {
        return processManagerImpl->isRunning();
    }

    std::optional<int> ProcessManager::getPid() const
    {
        return processManagerImpl->getPid();
    }

    std::expected<void, std::pair<ProcessError, std::string>> ProcessManager::terminate()
    {
        return processManagerImpl->terminate();
    }

    std::expected<int, std::pair<ProcessError, std::string>> ProcessManager::wait()
    {
        return processManagerImpl->wait();
    }

    std::string ProcessManager::string() const noexcept
    {
        return processManagerImpl->string();
    }
}
