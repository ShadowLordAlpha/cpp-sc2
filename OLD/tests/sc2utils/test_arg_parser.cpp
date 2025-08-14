#include "sc2utils/arg_parser.h"

#include <gtest/gtest.h>
#include <fstream>

namespace sc2
{
    // Helper to create temporary ini file
    std::string createTemporaryIniFile(const std::string& content)
    {
        static int counter = 0;
        std::string fname = "./test_property_" + std::to_string(counter++) + ".ini";
        std::ofstream ofs(fname);
        ofs << content;
        ofs.close();
        return fname;

    }

    // Helper to delete temporary ini file
    void deleteTemporaryIniFile(const std::string& fname)
    {
        std::remove(fname.c_str());
    }

    TEST(ArgParser, HandlesNoArguments) {
        ArgParser parser;
        int argc = 1;
        const char* argv[] = {"program"};
        EXPECT_TRUE(parser.parse(argc, const_cast<char**>(argv)));
    }

    // Single CLI Arguments tests

    TEST(ArgParser, ParsesSingleArgument) {
        ArgParser parser;

        parser.addArguments({
                {"option", {"--option", "-o"}, "A Testing Option" },
        });

        int argc = 3;
        const char* argv[] = {"program", "--option", "value"};
        ASSERT_TRUE(parser.parse(argc, const_cast<char**>(argv)));

        EXPECT_TRUE(parser.get<std::string>("option"));
        EXPECT_EQ(parser.get<std::string>("option").value(), "value");
    }

    TEST(ArgParser, ParsesSingleShortArgument) {
        ArgParser parser;

        parser.addArguments({
                {"option", {"--option", "-o"}, "A Testing Option" },
        });

        int argc = 3;
        const char* argv[] = {"program", "-o", "value"};
        ASSERT_TRUE(parser.parse(argc, const_cast<char**>(argv)));

        EXPECT_TRUE(parser.get<std::string>("option"));
        EXPECT_EQ(parser.get<std::string>("option").value(), "value");
    }

    TEST(ArgParser, ParsesSingleArgumentDoubleEntry) {
        ArgParser parser;

        parser.addArguments({
                {"option", {"--option", "-o"}, "A Testing Option" },
        });

        int argc = 5;
        const char* argv[] = {"program", "-o", "value", "--option", "other"};
        ASSERT_TRUE(parser.parse(argc, const_cast<char**>(argv)));

        EXPECT_TRUE(parser.get<std::string>("option"));
        EXPECT_EQ(parser.get<std::string>("option").value(), "other");
    }

    TEST(ArgParser, ParsesSingleDoubleParseArgument) {
        ArgParser parser;

        parser.addArguments({
            {"option", {"--option", "-o"}, "A Testing Option" },
        });

        int argc = 3;
        const char* argv[] = {"program", "--option", "value"};
        ASSERT_TRUE(parser.parse(argc, const_cast<char**>(argv)));

        EXPECT_TRUE(parser.get<std::string>("option"));
        EXPECT_EQ(parser.get<std::string>("option").value(), "value");

        const char* argv2[] = {"program", "-o", " value2 "};
        ASSERT_TRUE(parser.parse(argc, const_cast<char**>(argv2)));

        EXPECT_TRUE(parser.get<std::string>("option"));
        EXPECT_NE(parser.get<std::string>("option").value(), "value");
        EXPECT_EQ(parser.get<std::string>("option").value(), " value2 ");
    }

    TEST(ArgParser, HandlesSingleMissingArgument) {
        ArgParser parser;

        parser.addArguments({
                {"option", {"--option", "-o"}, "A Testing Option" },
        });

        int argc = 1;
        const char* argv[] = {"program"};

        EXPECT_TRUE(parser.parse(argc, const_cast<char**>(argv)));
    }

    TEST(ArgParser, HandlesSingleMissingRequiredArgument) {
        ArgParser parser;

        parser.addArguments({
            {"option", {"--option", "-o"}, "A Testing Option", true },
        });

        int argc = 1;
        const char* argv[] = {"program"};
        EXPECT_FALSE(parser.parse(argc, const_cast<char**>(argv)));
    }

    TEST(ArgParser, HandlesSingleUnknownArgument) {
        ArgParser parser;

        int argc = 3;
        const char* argv[] = {"program", "--unknown", "7555"};
        EXPECT_TRUE(parser.parse(argc, const_cast<char**>(argv)));

        EXPECT_FALSE(parser.get<std::string>("unknown"));
    }

    TEST(ArgParser, HandlesSingleConversionFailureArgument) {
        ArgParser parser;

        parser.addArguments({
            {"option", {"--option", "-o"}, "A Testing Option" },
        });

        int argc = 3;
        const char* argv[] = {"program", "--option", "notanumber"};
        EXPECT_TRUE(parser.parse(argc, const_cast<char**>(argv)));

        EXPECT_FALSE(parser.get<int>("option"));
    }

    TEST(ArgParser, HandlesSingleConversionSuccessArgument) {
        ArgParser parser;

        parser.addArguments({
            {"option", {"--option", "-o"}, "A Testing Option" },
        });

        int argc = 3;
        const char* argv[] = {"program", "--option", "1778"};
        EXPECT_TRUE(parser.parse(argc, const_cast<char**>(argv)));

        EXPECT_EQ(parser.get<int>("option"), 1778);
    }

    TEST(ArgParser, HandlesSingleBooleanArgument) {
        ArgParser parser;

        parser.addArguments({
            {"option", {"--option", "-o"}, "A Testing Option" },
        });

        int argc = 2;
        const char* argv[] = {"program", "--option"};
        EXPECT_TRUE(parser.parse(argc, const_cast<char**>(argv)));

        EXPECT_TRUE(parser.get<bool>("option").value_or(false));
    }

    TEST(ArgParser, HandlesSingleEmptyStringArgument) {
        ArgParser parser;

        parser.addArguments({
            {"option", {"--option", "-o"}, "A Testing Option" },
        });

        int argc = 3;
        const char* argv[] = {"program", "--option", ""};
        EXPECT_TRUE(parser.parse(argc, const_cast<char**>(argv)));

        EXPECT_EQ(parser.get<std::string>("option").value(), "");
    }

    TEST(ArgParser, HandlesCaseSensitivityArgument) {
        ArgParser parser;

        parser.addArguments({
            {"option", {"--option", "-o"}, "A Testing Option" },
        });

        int argc = 11;
        const char* argv[] = {"program", "--option", "thisone", "--o", "notthisone", "-O", "orthisone", "--OptIon", "oreventhisone", "-option", "thisonefails as well"};
        EXPECT_TRUE(parser.parse(argc, const_cast<char**>(argv)));

        EXPECT_EQ(parser.get<std::string>("option").value(), "thisone");
    }

    // TODO: file testing and above cleanup
}

