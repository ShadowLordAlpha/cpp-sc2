#include "sc2utils/process.h"

#include <vector>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <cstring>
#include <spawn.h>
#include <mutex>
#include <atomic>
#include <filesystem>
#include <iostream>

extern char **environ;

namespace sc2
{
    Process::~Process() noexcept {
        terminate();
    }

    Process::Process(Process&& other) noexcept
        : processPath(std::move(other.processPath))
        , workingPath(std::move(other.workingPath))
        , commandLine(std::move(other.commandLine))
        , processId(other.processId)
    {
        other.processId = 0;
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
            processId = other.processId;
            // mutex is not assignable or movable; leave default-initialized
            other.processId = 0;
        }
        return *this;
    }

    bool Process::start(const std::filesystem::path &path, const std::vector<std::string> &commandLine, const std::filesystem::path &working)
    {
        std::unique_lock lock(mutex);
        if (processId != 0 && isRunning()) {
            if (!terminate())
                return false;
        }

        // Convert arguments to char*[]
        std::vector<char*> argv;
        argv.push_back(const_cast<char*>(path.c_str()));
        for (const auto& arg : commandLine)
            argv.push_back(const_cast<char*>(arg.c_str()));
        argv.push_back(nullptr);

        posix_spawnattr_t attr;
        posix_spawnattr_init(&attr);

        posix_spawn_file_actions_t fileActions;
        posix_spawn_file_actions_init(&fileActions);

        // Change working directory before exec
        pid_t childPID = 0;
        int result;

        if (!working.empty())
        {
            // Set working dir by setting file action for posix_spawn
            // Not supported directly, so use chdir in child process
            // Instead, fork to set cwd, then exec
            childPID = fork();
            if (childPID == 0) { // Child
                if (chdir(working.c_str()) != 0) {
                    std::cerr << "Failed to chdir to " << working << '\n';
                    _exit(127);
                }
                execvp(path.c_str(), argv.data());
                // If execvp returns, exec failed
                _exit(127);
            } else if (childPID < 0) {
                // Fork failed
                return false;
            }
        }
        else
        {
            result = posix_spawn(&childPID, path.c_str(), &fileActions, &attr, argv.data(), environ);
            if (result != 0) {
                return false;
            }
        }

        // Only update state if the start was successful
        processPath = path;
        workingPath = working;
        this->commandLine = commandLine;
        processId = childPID;
        return true;
    }

    bool Process::isRunning() const
    {
        std::shared_lock lock(mutex);
        if (processId == 0) return false;
        int status;
        pid_t result = waitpid(processId, &status, WNOHANG);
        if (result == 0)
            return true; // Still running
        return false; // Exited, update state if needed on next start

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
        if (processId != 0) {
            if (isRunning()) {
                if (kill(processId, SIGTERM) != 0) {
                    result = false;
                } else {
                    // Wait for up to 5 seconds
                    int status = 0;
                    for (int i = 0; i < 50; ++i) {
                        if (waitpid(processId, &status, WNOHANG) != 0)
                            break;
                        usleep(100 * 1000); // 100ms
                    }
                    // Force kill if still running
                    if (isRunning()) {
                        kill(processId, SIGKILL);
                        waitpid(processId, &status, 0);
                    }
                }
            }
            processId = 0;
        }
        return result;
    }
}
