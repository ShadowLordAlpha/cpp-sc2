#pragma once

#include "sc2api/sc2_game_settings.h"

namespace sc2 {
    // Custom exception classes for argument parsing
    class ArgumentParsingError : public std::runtime_error {
    public:
        explicit ArgumentParsingError(const std::string& message)
                : std::runtime_error("Argument parsing error: " + message) {}
    };

    class FileParsingError : public std::runtime_error {
    public:
        explicit FileParsingError(const std::string& filename, const std::string& reason)
                : std::runtime_error("Failed to parse file '" + filename + "': " + reason) {}
    };

    class ExecutableNotFoundError : public std::runtime_error {
    public:
        explicit ExecutableNotFoundError(const std::string& path)
                : std::runtime_error("StarCraft II executable not found at: " + path) {}
    };

    bool ParseFromFile(ProcessSettings& process_settings, GameSettings& game_settings, const std::string& file_name);
    bool ParseSettings(int argc, char * argv[], ProcessSettings& process_settings, GameSettings& game_settings);
}
