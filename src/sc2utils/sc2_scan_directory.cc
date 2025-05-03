#include "sc2utils/sc2_scan_directory.h"

#include <string>
#include <vector>
#include <filesystem>


namespace sc2
{
    int scan_directory(const char *directory_path, std::vector<std::string> &files, bool full_path, bool list_directories)
    {
        if (!directory_path || !*directory_path)
            return 0;

        namespace fs = std::filesystem;
        fs::path dir_path(directory_path);

        if (!fs::exists(dir_path) || !fs::is_directory(dir_path))
            return 0;

        for (const auto &entry: fs::directory_iterator(dir_path))
        {
            if (entry.is_regular_file())
            {
                if (list_directories)
                {
                    continue;
                }

                if (full_path)
                {
                    files.push_back(entry.path().string());
                } else
                {
                    files.push_back(entry.path().filename().string());
                }
            } else if (entry.is_directory())
            {
                if (!list_directories)
                {
                    continue;
                }

                auto name = entry.path().filename().string();
                // skip "." and ".."
                if (name == "." || name == "..")
                {
                    continue;
                }

                if (full_path)
                {
                    files.push_back(entry.path().string());
                } else
                {
                    files.push_back(name);
                }
            }
        }

        return static_cast<int>(files.size());
    }
}
