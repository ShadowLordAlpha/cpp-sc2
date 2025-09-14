//
// Created by Shadow on 8/16/2025.
//

#include "astra/process.h"
#include <spdlog/spdlog.h>
#include <iostream>

namespace astra::process
{

    Process::Process() = default;

    Process::~Process()
    {
        stop();
    }

    void Process::setWorkingDirectory(const std::filesystem::path& path)
    {
        workingDirectory = path;
    }

    void Process::setArguments(const std::vector<std::string>& args)
    {
        arguments = args;
    }

    void Process::setExecutable(const std::filesystem::path& path)
    {
        executablePath = path;
    }

    bool Process::start()
    {
        SPDLOG_INFO(std::format("Starting process: {} {} ", executablePath.string(), workingDirectory.string()));

        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        //char args[] = "C:\\Program Files (x86)\\StarCraft II\\Versions\\Base93333\\SC2_x64.exe -listen 127.0.0.1 -port 5679 -displayMode 0";
        std::string cmd = executablePath.string();
        for(auto& arg : arguments)
        {
            cmd += " " + arg;
        }

        auto success = CreateProcess(
                nullptr,         // No module name (use command line)
                cmd.data(),            // Command line
                nullptr,            // Process handle not inheritable
                nullptr,            // Thread handle not inheritable
                FALSE,               // Set handle inheritance to FALSE
                0,                  // No creation flags
                nullptr,            // Use parent's environment block
                workingDirectory.string().c_str(),   // The working directory to use, if null use the parents
                &si,                // Pointer to STARTUPINFO structure
                &pi                 // Pointer to PROCESS_INFORMATION structure
        );

        if(success)
        {
            // Wait until child process exits.
            WaitForInputIdle(pi.hProcess, INFINITE);
            SPDLOG_INFO("Process has been started");
        }

        // Close process and thread handles.
        processHandle = pi.hProcess;
        CloseHandle(pi.hThread); // Don't need the thread handle

        return success;
    }

    bool Process::isRunning()
    {
        DWORD exitCode;
        return GetExitCodeProcess(processHandle, &exitCode) && exitCode == STILL_ACTIVE;
    }

    void Process::stop()
    {
        TerminateProcess(processHandle, 0);
    }
}
