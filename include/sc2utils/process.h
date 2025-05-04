#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <shared_mutex>
#include <memory>

#ifdef _WIN32
    #include <windows.h>
#endif

/**
 * @file process.h
 * @brief A process abstraction to remove system differences to the main code for easier maintenance.
 */
namespace sc2
{
    /**
     * @class Process
     * @brief Represents and manages the lifetime of an external process.
     *
     * This class provides a platform-independent abstraction for creating, managing, and terminating external
     * processes. The implementation hides OS-specific details, enabling easier maintenance and portability.
     *
     * @note All public methods are safe for concurrent use by multiple threads.
     */
    class Process
    {
    public:
        /**
         * @brief Constructs a new, uninitialized Process object.
         *
         * The process is not started upon construction. Use start() to launch a new process.
         */
        Process() = default;

        /**
         * @brief Destructor. Cleans up any associated process resources.
         *
         * If the process is still running, the destructor will attempt to terminate it. Termination is not guaranteed
         * if the process does not respond to termination requests.
         */
        ~Process() noexcept;

        /**
         * @brief Copy constructor (deleted).
         *
         * This class is non-copyable to prevent copying of process state or handles.
         */
        Process(const Process&) = delete;

        /**
         * @brief Copy assignment operator (deleted).
         *
         * This class is non-copyable to prevent copying of process state or handles.
         */
        Process& operator=(const Process&) = delete;

        /**
         * @brief Move constructor.
         *
         * Transfers ownership of the process from another Process instance.
         */
        Process(Process&&) noexcept;

        /**
         * @brief Move assignment operator.
         *
         * Transfers ownership of the process from another Process instance.
         */
        Process& operator=(Process&&) noexcept;

        /**
         * @brief Starts a new external process.
         *
         * Launches the specified executable with the provided command line arguments. If a process is already running,
         * this function will attempt to terminate it first. If the running process cannot be terminated, the new
         * process will not be started and this function will return false.
         *
         * @param path The path to the executable to launch.
         * @param commandLine The command line arguments to pass to the executable.
         * @param working The working directory to launch the process from, this will default to the current directory.
         * @return true if the process was successfully started, false otherwise.
         * @note The process state (such as executable path, working directory, and command line) will be updated if
         * successful.
         * @threadsafe
         */
        bool start(const std::filesystem::path &path, const std::vector<std::string> &commandLine, const std::filesystem::path &working = std::filesystem::current_path());

        /**
         * @brief Checks whether the process is currently running.
         *
         * @return true if the process is running, false otherwise.
         * @threadsafe
         */
        [[nodiscard]] bool isRunning() const;

        /**
         * @brief Attempts to terminate the process.
         *
         * Requests the operating system to terminate the process. This call does not guarantee immediate termination.
         * The function returns false if the termination request could not be completed successfully.
         *
         * @return true if the process was successfully terminated or was not running,
         *         false otherwise.
         * @note The call may block until the process has exited, depending on the implementation.
         * @threadsafe
         */
        bool terminate() noexcept;

        /**
         * @brief Returns a human-readable description of the process state.
         *
         * @return A string summarizing the executable path, working directory, command line, and running status.
         * @threadsafe
         */
        [[nodiscard]] std::string string() const;

    private:
        std::filesystem::path processPath; ///< Executable path set on process start, used for status/reporting.
        std::filesystem::path workingPath; ///< Working directory the process was launched from.
        std::vector<std::string> commandLine; ///< Arguments used to start the process.
        mutable std::shared_mutex mutex{}; ///< Ensures thread-safe access to internal state.
#ifdef _WIN32
        HANDLE processHandle{nullptr};
        HANDLE threadHandle{nullptr};
#elif
        pid_t processId;
#endif

    };

    /**
     * @brief Outputs a `Process` description to a stream.
     *
     * Uses the Process::string() method to generate the output.
     *
     * @param os The output stream.
     * @param process The process to describe.
     * @return Reference to the output stream.
     * @threadsafe
     */
    inline std::ostream& operator<<(std::ostream& os, const Process& process) {
        os << process.string();
        return os;
    }
}
