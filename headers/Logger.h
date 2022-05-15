#pragma once

enum class LogLevel
{
    DEBUG,
    WARNING,
    INFO
};

class Logger
{
public:
    static int Log(LogLevel logLevel, const char* format, ...);
    static int LogWithPrefix(const char* prefix, const char* format, ...);
};
