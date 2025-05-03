#include "sc2utils/arg_parser.h"

#include <spdlog/spdlog.h>

#include <iostream>
#include <fstream>
#include <string>

namespace sc2
{
    void ArgParser::addArguments(const std::vector<Arg> &args)
    {
        for (const auto &arg: args)
        {
            arguments.push_back(arg);
            for (const auto &flag: arg.flags)
            {
                if (flagToName.contains(flag))
                {
                    SPDLOG_WARN("Duplicate Argument: {}, ignoring.", flag);
                }
                flagToName.try_emplace(flag, arg.name);
            }
        }
    }

    bool ArgParser::parse(int argc, char *argv[])
    {
        nameToValue.clear();
        for (int i = 0; i < argc; ++i)
        {
            // Skip values that are not flags
            if (argv[i][0] != '-') continue;

            const auto flag = argv[i];
            if (!flagToName.contains(flag))
            {
                SPDLOG_WARN("Unknown argument: {}.", flag);
                continue;
            }

            std::string value = "1";
            if (i < argc - 1 && argv[i + 1][0] != '-')
            {
                value = argv[i + 1];
                ++i;
            }

            nameToValue[flagToName[flag]] = value;
        }

        for (const auto &arg: arguments)
        {
            if (arg.required && !nameToValue.contains(arg.name))
            {
                SPDLOG_ERROR("Missing required argument: {}.", arg.name);
                nameToValue.clear();
                return false;
            }
        }

        return true;
    }

    void trim(std::string &s) {
        s.erase(s.find_last_not_of(" \n\r\t")+1);
        s.erase(0, s.find_first_not_of(" \n\r\t"));
    }

    bool ArgParser::parse(const std::string &filename)
    {
        nameToValue.clear();
        std::ifstream file(filename);
        if (!file)
        {
            SPDLOG_ERROR("Could not open argument file: {}.", filename);
            return false;
        }

        std::string line;
        while (std::getline(file, line))
        {
            // Trim leading/trailing spaces
            trim(line);

            // Skip blank lines or comments
            if (line.empty() || line[0] == '#' || line[0] == ';')
                continue;

            auto pos = line.find('=');
            if (pos == std::string::npos)
            {
                SPDLOG_WARN("Invalid line in argument file (missing '='): [{}]", line);
                continue;
            }
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            trim(key);
            trim(value);

            // Only process known arguments using their canonical names
            auto arg_it = std::find_if(arguments.begin(), arguments.end(),
                                       [&](const Arg &arg) { return arg.name == key; });

            if (arg_it == arguments.end())
            {
                SPDLOG_WARN("Unknown argument in file: '{}'", key);
                continue;
            }

            nameToValue[key] = value.empty() ? "1" : value; // treat missing value as a boolean true
        }

        // Check for missing required arguments
        for (const auto &arg : arguments)
        {
            if (arg.required && !nameToValue.contains(arg.name))
            {
                SPDLOG_ERROR("Missing required argument from file: {}.", arg.name);
                nameToValue.clear();
                return false;
            }
        }

        return true;
    }

    void ArgParser::printHelp() const
    {
        std::cout << "A small argument parser for the cpp_sc2 project." << std::endl;
        printUsage();
        std::cout << "Arguments:" << std::endl;
        for (const auto &arg: arguments)
        {
            std::cout << "  ";
            auto separator = "";
            for (const auto &flag: arg.flags)
            {
                std::cout << separator << flag;
                separator = ", ";
            }
            std::cout << std::endl << "    " << arg.description;

            if (arg.required)
                std::cout << " [required]";
            std::cout << std::endl;
        }
    }

    void ArgParser::printUsage() const
    {
        std::cout << "Usage:" << std::endl;
        for (const auto &arg: arguments)
        {
            std::cout << "  ";
            if (!arg.required)
                std::cout << "[";

            auto separator = "";
            for (const auto &flag: arg.flags)
            {
                std::cout << separator << flag;
                separator = ", ";
            }
            std::cout << " <value>";

            if (!arg.required)
                std::cout << "]";
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}
