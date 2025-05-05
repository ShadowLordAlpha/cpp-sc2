/**
 * @file logger.h
 * @brief Logging utilities for the CPPSC2 project.
 *
 * Supports two backends:
 *   - SPDLog us USE_SPDLOG (if spdlog_FOUND is set in CMake, this happens automatically)
 *   - Minimal fallback logger using std::format and streams
 *
 * Log levels can be configured at compile time with SC2_ACTIVE_LOG_LEVEL.
 *
 * Example usage:
 *   SC2_LOG_INFO("Hello {}", name);
 *
 * To change log level globally (for both the macros and SPDLog):
 *   #define SC2_ACTIVE_LOG_LEVEL SC2_LOG_LEVEL_DEBUG
 *   // before including logger.h
 *
 * To use SPDLog as the backend, define USE_SPDLOG and add SPDLog to your dependencies.
 *
 * Thread-safe and reentrant (for SPDLog backend).
 */
#pragma once

#ifdef USE_SPDLOG
#define SC2_LOG_LEVEL_TRACE    SPDLOG_LEVEL_TRACE
#define SC2_LOG_LEVEL_DEBUG    SPDLOG_LEVEL_DEBUG
#define SC2_LOG_LEVEL_INFO     SPDLOG_LEVEL_INFO
#define SC2_LOG_LEVEL_WARN     SPDLOG_LEVEL_WARN
#define SC2_LOG_LEVEL_ERROR    SPDLOG_LEVEL_ERROR
#define SC2_LOG_LEVEL_CRITICAL SPDLOG_LEVEL_CRITICAL
#define SC2_LOG_LEVEL_OFF      SPDLOG_LEVEL_OFF

// Default to INFO level logs for output
#ifndef SPDLOG_ACTIVE_LEVEL
#define SC2_ACTIVE_LOG_LEVEL SC2_LOG_LEVEL_INFO
#define SPDLOG_ACTIVE_LEVEL SC2_ACTIVE_LOG_LEVEL
#endif

#include <spdlog/spdlog.h>
#include <memory>

namespace sc2::logger
{
    inline std::shared_ptr<spdlog::logger> getLogger() {
        auto logger = spdlog::get("sc2");
        if (!logger) {
            logger = spdlog::stdout_color_mt("sc2");
        }
        return logger;
    }
}

#define SC2_LOG_TRACE(...)    SPDLOG_LOGGER_TRACE(::sc2::logger::getLogger(), __VA_ARGS__)
#define SC2_LOG_DEBUG(...)    SPDLOG_LOGGER_DEBUG(::sc2::logger::getLogger(), __VA_ARGS__)
#define SC2_LOG_INFO(...)     SPDLOG_LOGGER_INFO(::sc2::logger::getLogger(), __VA_ARGS__)
#define SC2_LOG_WARN(...)     SPDLOG_LOGGER_WARN(::sc2::logger::getLogger(), __VA_ARGS__)
#define SC2_LOG_ERROR(...)    SPDLOG_LOGGER_ERROR(::sc2::logger::getLogger(), __VA_ARGS__)
#define SC2_LOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(::sc2::logger::getLogger(), __VA_ARGS__)

#else // Fallback: Write to stdout/stderr using std::format
#define SC2_LOG_LEVEL_TRACE    0
#define SC2_LOG_LEVEL_DEBUG    1
#define SC2_LOG_LEVEL_INFO     2
#define SC2_LOG_LEVEL_WARN     3
#define SC2_LOG_LEVEL_ERROR    4
#define SC2_LOG_LEVEL_CRITICAL 5
#define SC2_LOG_LEVEL_OFF      6

// Default to INFO level logs for output
#ifndef SC2_ACTIVE_LOG_LEVEL
#define SC2_ACTIVE_LOG_LEVEL SC2_LOG_LEVEL_INFO
#endif

#include <format>
#include <ostream>
#include <string>

namespace sc2::logger
{
    inline std::string currentTimeString() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm tm_buf;
#if defined(_MSC_VER)
        localtime_s(&tm_buf, &now_c);
#else
        localtime_r(&now_c, &tm_buf);
#endif
        char buf[20];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_buf);
        return std::string(buf);
    }

    // ANSI color codes
    inline const char* teal() { return "\033[36m"; }
    inline const char* resetColor() { return "\033[0m"; }
    inline const char* levelColor(const std::string &level) {
        if (level == "TRACE")    return "\033[90m"; // Light gray
        if (level == "DEBUG")    return "\033[97m"; // White
        if (level == "INFO")     return "\033[32m"; // Green
        if (level == "WARN")     return "\033[33m"; // Yellow
        if (level == "ERROR")    return "\033[31m"; // Red
        if (level == "CRITICAL") return "\033[35m"; // Magenta
        return "";
    }

    template<typename... Args>
    void fallbackLog(const std::string &level, std::ostream& stream, const std::string& fmt, Args&&... args)
    {
        try {
            auto message = std::vformat(fmt, std::make_format_args(args...));
            stream << "[ " << teal() << currentTimeString() << resetColor() << " ] [ " << levelColor(level) << level << resetColor() << " ] " << message << '\n';
        } catch (const std::format_error& e) {
            stream << "[LOGGING ERROR] format error: " << e.what() << " [level: " << level << ", format: " << fmt << "]\n";
        }

    }
}

#if SC2_ACTIVE_LOG_LEVEL <= SC2_LOG_LEVEL_TRACE
#define SC2_LOG_TRACE(fmt, ...)    ::sc2::logger::fallbackLog("TRACE", std::cout, fmt, ##__VA_ARGS__)
#else
#define SC2_LOG_TRACE(fmt, ...)    (void)0
#endif

#if SC2_ACTIVE_LOG_LEVEL <= SC2_LOG_LEVEL_DEBUG
#define SC2_LOG_DEBUG(fmt, ...)    ::sc2::logger::fallbackLog("DEBUG", std::cout, fmt, ##__VA_ARGS__)
#else
#define SC2_LOG_DEBUG(fmt, ...)    (void)0
#endif

#if SC2_ACTIVE_LOG_LEVEL <= SC2_LOG_LEVEL_INFO
#define SC2_LOG_INFO(fmt, ...)     ::sc2::logger::fallbackLog("INFO",  std::cout, fmt, ##__VA_ARGS__)
#else
#define SC2_LOG_INFO(fmt, ...)     (void)0
#endif

#if SC2_ACTIVE_LOG_LEVEL <= SC2_LOG_LEVEL_WARN
#define SC2_LOG_WARN(fmt, ...)     ::sc2::logger::fallbackLog("WARN",  std::cout, fmt, ##__VA_ARGS__)
#else
#define SC2_LOG_WARN(fmt, ...)     (void)0
#endif

#if SC2_ACTIVE_LOG_LEVEL <= SC2_LOG_LEVEL_ERROR
#define SC2_LOG_ERROR(fmt, ...)    ::sc2::logger::fallbackLog("ERROR", std::cerr, fmt, ##__VA_ARGS__)
#else
#define SC2_LOG_ERROR(fmt, ...)    (void)0
#endif

#if SC2_ACTIVE_LOG_LEVEL <= SC2_LOG_LEVEL_CRITICAL
#define SC2_LOG_CRITICAL(fmt, ...) ::sc2::logger::fallbackLog("CRITICAL", std::cerr, fmt, ##__VA_ARGS__)
#else
#define SC2_LOG_CRITICAL(fmt, ...) (void)0
#endif

#endif
