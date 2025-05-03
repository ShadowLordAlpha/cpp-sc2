#pragma once

#include <optional>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>

/**
 * @file arg_parser.h
 * @brief A small argument parser for the cpp_sc2 project that can process both Command-line and simple ini files.
 */
namespace sc2
{
    /**
     * @struct Arg
     * @brief Represents a single argument or option that can be parsed.
     */
    struct Arg
    {
        std::string name; ///< Canonical name of the argument (used as identifier).
        std::vector<std::string> flags; ///< List of flags or aliases usable as this argument (e.g. {"-h", "--help"}).
        std::string description; ///< Description of the argument for help/usage output.
        bool required = false; ///< Whether this argument is required.
    };

    /**
     * @class ArgParser
     * @brief An argument parser supporting type-safe retrieval and help/usage display.
     */
    class ArgParser
    {
    public:
        /**
         * @brief Default constructor.
         */
        ArgParser() = default;

        /**
         * @brief Registers the set of arguments/options that can be parsed.
         * @param arguments Vector of argument descriptors.
         */
        void addArguments(const std::vector<Arg> &arguments);

        /**
         * @brief Parses command-line style arguments.
         *
         * Takes a traditional argc/argv input as provided to main(). This method will match flags and options based on
         * the registered arguments. If any unknown arguments are encountered, a warning is output and parsing
         * continues. Actual printing of help/usage or exiting on errors is left up to the calling program but methods
         * are provided for printing help/usage to standard output. The program name is considered in the mapping but is
         * ignored as long as it doesn't start with `-`. Each time this method is called, any previous results are
         * cleared before inserting new results. An unsuccessful parse will also clear the results before returning.
         *
         * @param argc Argument count.
         * @param argv Argument values.
         * @return True iff all required arguments where present and their values were successfully parsed.
         */
        [[nodiscard]] bool parse(int argc, char *argv[]);

        /**
         * @brief Loads and parses arguments from a basic ini file.
         *
         * Parses a basic ini file, matching key-value pairs in the form "key = value" based on the registered
         * arguments. If any unknown arguments are encountered, a warning is output and parsing continues. Blank lines,
         * comment lines (starting with '#' or ';'), and section headers ("[header]") are ignored. If a line is in the
         * form "key" a warning is output and parsing continues with the value ignored. If the file cannot be opened
         * an error is output and parsing fails. Each time this method is called, any previous results are cleared
         * before inserting new results. An unsuccessful parse will also clear the results before returning.
         *
         * @param filename Name of the file containing arguments.
         * @return True iff all required arguments where present and their values were successfully parsed.
         */
        [[nodiscard]] bool parse(const std::string &filename);

        /**
         * @brief Gets the value of the named argument/option, parsed as the specified type.
         *
         * Returns an empty optional if the argument was not provided or if conversion to the type `T` failed. Types
         * must be stream-extractable from `std::istringstream`. For string retrieval, use std::string as the template
         * parameter.
         *
         * @tparam T The type to parse the value as (int, double, std::string, etc.).
         * @param identifier The canonical name of the argument (not the flag).
         * @return `std::optional<T>` containing the parsed value, or `std::nullopt`.
         *
         * @code
         * auto count = parser.get<int>("count");
         * if (count) { ... }
         * auto filename = parser.get<std::string>("file");
         * @endcode
         */
        template<typename T>
        [[nodiscard]] std::optional<T> get(const std::string &identifier) const;

        /**
         * @brief Prints help information to standard output.
         */
        void printHelp() const;

        /**
         * @brief Prints usage information to standard output.
         */
        void printUsage() const;

    private:
        std::vector<Arg> arguments; ///< Argument metadata.
        std::unordered_map<std::string, std::string> flagToName; ///< Maps flags/aliases to their canonical names.
        std::unordered_map<std::string, std::string> nameToValue; ///< Parsed values for each argument.
    };

    /**
     * @copydoc ArgParser::get(const std::string&) const
     */
    template<typename T>
    std::optional<T> ArgParser::get(const std::string &identifier) const
    {
        auto iterator = nameToValue.find(identifier);
        if (iterator == nameToValue.end())
            return std::nullopt;

        if constexpr (std::is_same_v<T, std::string>)
        {
            return iterator->second;
        }
        else
        {
            std::istringstream iss(iterator->second);
            T value;
            iss >> value;

            // Ensure successful conversion and no extra input
            if (!iss.fail() && iss.eof())
                return value;
        }

        return std::nullopt;
    }
}
