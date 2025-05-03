#include "sc2api/sc2_args.h"

#include "sc2utils/arg_parser.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_property_reader.h"

#include <cassert>
#include <cstdlib>
#include <iostream>

namespace sc2 {

const char* StarCraft2UserDirectory = "StarCraft II";
const char* StarCraft2ExecuteInfo = "ExecuteInfo.txt";

bool ParseFromFile(ProcessSettings& process_settings, GameSettings& game_settings, const std::string& file_name) {
    sc2::PropertyReader reader;

    if (!reader.LoadFile(file_name)) {
        return false;
    }

    reader.ReadString("executable", process_settings.process_path);
    int real_time = 0;
    reader.ReadInt("realtime", real_time);
    if (real_time) {
        process_settings.realtime = true;
    }
    else {
        process_settings.realtime = false;
    }
    reader.ReadInt("port", process_settings.port_start);
    reader.ReadString("map", game_settings.map_name);
    reader.ReadInt("timeout", process_settings.timeout_ms);
    return true;
}

#if defined(_WIN32)
    const char kDirectoryDivider = '\\';
#else
    const char kDirectoryDivider = '/';
#endif

std::string ParseExecuteInfo(ProcessSettings& process_settings, GameSettings& game_settings) {
    std::string execute_info_filepath = GetUserDirectory();
    if (execute_info_filepath.empty())
        return "Failed to determine path to the user's directory";

    execute_info_filepath += kDirectoryDivider;
    execute_info_filepath += StarCraft2UserDirectory;
    execute_info_filepath += kDirectoryDivider;
    execute_info_filepath += StarCraft2ExecuteInfo;

    if (!ParseFromFile(process_settings, game_settings, execute_info_filepath))
        return "Failed to parse " + execute_info_filepath;

    if (!FindLatestExe(process_settings.process_path))
        return "Failed to find latest StarCraft II executable in " + process_settings.process_path;

    return std::string();
}

bool ParseSettings(int argc, char* argv[], ProcessSettings& process_settings, GameSettings& game_settings) {
    assert(argc);
    ArgParser arg_parser{};

    // NB (alkurbatov): First attempt to parse from the SC2 user directory.
    // Note that ExecuteInfo.txt may be missing on Linux and command line
    // options should be used instead.
    std::string parse_error = ParseExecuteInfo(process_settings, game_settings);

    arg_parser.addArguments({
        { "executable", {"-e", "--executable"}, "The path to StarCraft II." },
        { "step_size", {"-s", "--step_size"}, "How many steps to take per call." },
        { "port", {"-p", "--port"}, "The port to make StarCraft II listen on." },
        { "realtime", {"-r", "--realtime"}, "Whether to run StarCraft II in real time or not." },
        { "map", {"-m", "--map"}, "Which map to run." },
        { "timeout", {"-t", "--timeout"}, "Timeout for how long the library will block for a response." },
        { "data_version", {"-d", "--data_version"}, "Data hash of the game version to run (see versions.json)" }
    });

    if (const char* sc2path = std::getenv("SC2PATH"))
        process_settings.process_path = sc2path;

    if (!arg_parser.parse(argc, argv))
        return false;

    process_settings.process_path = arg_parser.get<std::string>("executable").value_or(process_settings.process_path);
    if (process_settings.process_path.length() < 2) {
        std::cerr << "Path to StarCraft II executable is not specified.\n";

        if (!parse_error.empty())
            std::cerr << parse_error << '\n';

        std::cerr << "Please run StarCraft II before running this application or provide command line arguments.\n";
        std::cerr << "For more options: " << argv[0] << " --help\n\n";

        return false;
    }

    process_settings.step_size = arg_parser.get<int>("step_size").value_or(5);
    process_settings.realtime = arg_parser.get<bool>("realtime").value_or(false);
    process_settings.timeout_ms = arg_parser.get<int>("timeout").value_or(60000);
    process_settings.data_version = arg_parser.get<std::string>("data_version").value_or("");

    game_settings.map_name = arg_parser.get<std::string>("map").value_or("");

    return true;
}

}
