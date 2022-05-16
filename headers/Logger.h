#pragma once

enum class LogLevel
{
    LEVEL_DEBUG,
    LEVEL_INFO,
    LEVEL_WARNING,
    LEVEL_ERROR
};

class Logger
{
public:
    static int Log(LogLevel logLevel, const char* format, ...);
    static int LogWithPrefix(const char* prefix, const char* format, ...);
};
