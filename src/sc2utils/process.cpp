#include "sc2utils/process.h"

namespace sc2
{


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
}
