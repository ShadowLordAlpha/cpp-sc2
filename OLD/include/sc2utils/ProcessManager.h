#pragma once

#include <filesystem>
#include <vector>
#include <string>
#include <memory>
#include <concepts>
#include <ranges>
#include <span>
#include <expected>
#include <chrono>
#include <format>
#include <ostream>
#include <optional>
#include <source_location>
#include <stacktrace>

/**
 * @file Process.h
 * @brief Process abstraction with enhanced debugging and error handling
 * @version 3.0.0
 */
namespace sc2 {

    /**
     *
     */
    enum class ProcessError {
        FileNotFound,
        InvalidWorkingDirectory,
        ProcessNotRunning,
        InvalidPid,
        StartFailed,
        TerminateFailed,
        WaitFailed,
        NotExitedNormally
    };

    /**
     * @class ProcessManager
     * @brief
     */
    class ProcessManager
    {
    public:
        explicit ProcessManager(const std::filesystem::path& processPath, std::string_view args = "", const std::filesystem::path& workingDirectory = std::filesystem::current_path());
        ~ProcessManager();

        // Non-copyable, movable
        ProcessManager(const ProcessManager&) = delete;
        ProcessManager& operator=(const ProcessManager&) = delete;
        ProcessManager(ProcessManager&&) noexcept;
        ProcessManager& operator=(ProcessManager&&) noexcept;

        // Start the process, returns success or error message
        std::expected<void, std::pair<ProcessError, std::string>> start();

        // Check if process is running
        [[nodiscard]] bool isRunning() const;

        // Get process ID (PID)
        [[nodiscard]] std::optional<int> getPid() const;

        // Terminate the process, returns success or error message
        std::expected<void, std::pair<ProcessError, std::string>> terminate();

        // Wait for process to finish, returns exit code or error message
        std::expected<int, std::pair<ProcessError, std::string>> wait();

        // Get string representation of the process state
        [[nodiscard]] std::string string() const noexcept;

    private:
        class Impl; // Forward declaration of implementation
        std::unique_ptr<Impl> processManagerImpl;
    };
}
