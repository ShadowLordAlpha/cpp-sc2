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

    template<typename T>
    concept StringLike = std::convertible_to<T, std::string_view>;

    template<typename Container>
    concept StringContainer = std::ranges::range<Container> &&
                              StringLike<std::ranges::range_value_t<Container>>;

    template<typename Duration>
    concept DurationLike = requires(Duration d) {
        std::chrono::duration_cast<std::chrono::milliseconds>(d);
    };

    /**
     * @enum ProcessError
     * @brief Comprehensive error types for process operations
     */
    enum class ProcessError : uint8_t {
        AlreadyRunning = 1,
        ExecutableNotFound,
        InsufficientPermissions,
        SystemResourcesUnavailable,
        InvalidArguments,
        TerminationFailed,
        TimeoutExpired,
        ProcessNotStarted,
        Unknown = 255
    };

    /**
     * @struct ProcessInfo
     * @brief Extended process information with C++23 features
     */
    struct ProcessInfo {
        std::filesystem::path executablePath;
        std::filesystem::path workingDirectory;
        std::vector<std::string> commandLine;
        std::optional<uint64_t> processId;
        std::chrono::system_clock::time_point startTime;
        std::optional<std::chrono::system_clock::time_point> endTime;

        [[nodiscard]] std::string toString() const {
            return std::format(
                    "ProcessInfo{{\n"
                    "  executable: '{}',\n"
                    "  workingDir: '{}',\n"
                    "  commandLine: [{}],\n"
                    "  processId: {},\n"
                    "  startTime: {},\n"
                    "  endTime: {}\n"
                    "}}",
                    executablePath.string(),
                    workingDirectory.string(),
                    commandLine | std::views::transform([](const auto& arg) {
                        return std::format("'{}'", arg);
                    }) | std::views::join_with(", ") | std::ranges::to<std::string>(),
                    processId.has_value() ? std::format("{}", *processId) : "none",
                    std::format("{:%Y-%m-%d %H:%M:%S}", startTime),
                    endTime.has_value() ? std::format("{:%Y-%m-%d %H:%M:%S}", *endTime) : "running"
            );
        }
    };


    class Process {
    public:
        Process() noexcept;

        template<StringContainer CommandArgs>
        explicit Process(const std::filesystem::path &execuitable,
                         CommandArgs &&args,
                         const std::filesystem::path &workingDir = std::filesystem::current_path())requires (!std::same_as<std::decay_t<CommandArgs>, Process>);

        ~Process() noexcept;

        // Non-copyable but moveable
        Process(const Process &) = delete;
        Process &operator=(const Process &) = delete;
        Process(Process &&) noexcept;
        Process &operator=(Process &&) noexcept;

        // Modern methods with concepts and std::expected
        template<StringContainer CommandArgs>
        [[nodiscard]] std::expected<void, ProcessError>
        start(const std::filesystem::path &executablePath,
              CommandArgs &&arguments,
              const std::filesystem::path &workingDirectory = std::filesystem::current_path())requires StringContainer<CommandArgs>;

        // Overload for spans and arrays
        [[nodiscard]] std::expected<void, ProcessError>
        start(const std::filesystem::path &executablePath,
              std::span<const std::string> arguments,
              const std::filesystem::path &workingDirectory = std::filesystem::current_path());

        // Status checking with modern attributes
        [[nodiscard]] bool isRunning() const noexcept;

        [[nodiscard]] bool isTerminated() const noexcept;

        // Enhanced termination with timeout support
        [[nodiscard]] std::expected<void, ProcessError>
        terminate(std::chrono::milliseconds timeout = std::chrono::milliseconds{5000}) noexcept;

        [[nodiscard]] std::expected<void, ProcessError>
        forceKill() noexcept;

        // Process information getters with modern return types
        [[nodiscard]] const std::filesystem::path &getExecutablePath() const noexcept;

        [[nodiscard]] const std::filesystem::path &getWorkingDirectory() const noexcept;

        [[nodiscard]] std::span<const std::string> getCommandLine() const noexcept;

        // Process ID getter (platform abstracted)
        [[nodiscard]] std::optional<uint64_t> getProcessId() const noexcept;

        // Modern string formatting
        [[nodiscard]] std::string toString() const;

    private:
        // Forward declaration of the platform-specific implementation
        class Impl;

        std::unique_ptr<Impl> processImpl;
    };

    // Modern stream operator
    std::ostream &operator<<(std::ostream &os, const Process &process);

    // Template implementations (must be in header)
    template<StringContainer CommandArgs>
    Process::Process(const std::filesystem::path &executablePath,
                     CommandArgs &&args,
                     const std::filesystem::path &workingDirectory)
                     requires (!std::same_as<std::decay_t<CommandArgs>, Process>)
            : Process() {

        auto result = start(executablePath, std::forward<CommandArgs>(args), workingDirectory);
        if (!result) {
            throw std::runtime_error(std::format("Failed to start process: {}",
                                                 static_cast<int>(result.error())));
        }
    }

    template<StringContainer CommandArgs>
    [[nodiscard]] std::expected<void, ProcessError>
    Process::start(const std::filesystem::path &executablePath,
                   CommandArgs &&arguments,
                   const std::filesystem::path &workingDirectory)
                   requires StringContainer<CommandArgs> {

        // Convert to vector for internal storage
        std::vector<std::string> args;
        if constexpr (std::ranges::sized_range<CommandArgs>) {
            args.reserve(std::ranges::size(arguments));
        }

        for (const auto &arg: arguments) {
            args.emplace_back(std::string{arg});
        }

        return start(executablePath, std::span{args}, workingDirectory);
    }
}
