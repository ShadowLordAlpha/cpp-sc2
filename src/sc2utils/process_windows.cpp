#include "sc2utils/process.h"

#include <vector>
#include <string>

namespace sc2
{
    Process::~Process() noexcept {
        terminate();
    }

    Process::Process(Process&& other) noexcept
        : processPath(std::move(other.processPath))
        , workingPath(std::move(other.workingPath))
        , commandLine(std::move(other.commandLine))
        , processHandle(std::move(other.processHandle))
        , threadHandle(std::move(other.threadHandle))
    {

    }

    Process& Process::operator=(Process&& other) noexcept
    {
        if (this != &other) {
            // Lock both objects to ensure thread safety if needed
            // std::unique_lock<std::shared_mutex> lock1(mutex, std::defer_lock);
            // std::unique_lock<std::shared_mutex> lock2(other.mutex, std::defer_lock);
            // std::lock(lock1, lock2);

            processPath = std::move(other.processPath);
            workingPath = std::move(other.workingPath);
            commandLine = std::move(other.commandLine);
            processHandle = std::move(other.processHandle);
            threadHandle = std::move(other.threadHandle);
            // mutex is not assignable or movable; leave default-initialized
        }
        return *this;
    }

    bool Process::start(const std::filesystem::path &path, const std::vector<std::string> &commandLine, const std::filesystem::path &working)
    {
        std::unique_lock lock(mutex);
        if (processHandle && isRunning()) {
            if (!terminate())
                return false;
        }

        // Build command string
        std::string cmd;// = '\"' + path.string() + '\"';
        for (const auto &arg: commandLine)
        {
            cmd += " " + arg + "";
        }

        static const unsigned int buffer_size = (1 << 16) + 1;
        char buffer[buffer_size];
        strncpy_s(buffer, cmd.c_str(), buffer_size - 1);


        STARTUPINFOA si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        printf("Process: %s\n", cmd.c_str());

        BOOL success = CreateProcessA(
            path.string().c_str(),
            buffer, // Command line
            nullptr,
            nullptr,
            FALSE,
            0,
            nullptr,
            working.string().c_str(),
            &si,
            &pi
        );

        if (!success)
        {
            return false;
        }

        // Only update the state if the start was successful
        processPath = path;
        workingPath = working;
        this->commandLine = commandLine;
        processHandle = pi.hProcess;
        threadHandle = pi.hThread;
        return true;
    }

    bool Process::isRunning() const
    {
        std::shared_lock lock(mutex);
        if (!processHandle) return false;
        DWORD exitCode = 0;
        if (GetExitCodeProcess(processHandle, &exitCode))
        {
            return exitCode == STILL_ACTIVE;
        }
        return false;
    }

    std::string Process::string() const {
        std::shared_lock lock(mutex);

        std::string result = "Process[";
        result += "Path: " + processPath.string();
        result += ", WorkingDir: " + workingPath.string();
        result += ", Args: ";
        for (const auto& arg : commandLine) {
            result += "[" + arg + "] ";
        }
        result += ", Running: ";
        result += isRunning() ? "Yes" : "No";
        result += "]";
        return result;
    }

    bool Process::terminate() noexcept
    {
        std::unique_lock lock(mutex);
        bool result = true;
        if (processHandle)
        {
            // Try to terminate the process
            if (isRunning())
            {
                if (!TerminateProcess(processHandle, 1))
                {
                    result = false;
                }
                // Optionally, wait for process to really exit
                WaitForSingleObject(processHandle, 5000);
            }
            CloseHandle(processHandle);
            processHandle = nullptr;
        }

        if (threadHandle)
        {
            CloseHandle(threadHandle);
            threadHandle = nullptr;
        }
        return result;
    }
}
