#include "Logger.h"

#include <iostream>
#include <stdarg.h>

int Logger::Log(LogLevel logLevel, const char* format, ...)
{
    va_list argv;
    int headerRes = 0;
    va_start(argv, format);

    switch (logLevel)
    {
    case LogLevel::LEVEL_DEBUG:
        #ifdef NDEBUG
            return 0;
        #else
            headerRes = printf("[DEBUG] ");
            break;
        #endif
    case LogLevel::LEVEL_INFO:
        headerRes = printf("[INFO] ");
        break;
    case LogLevel::LEVEL_WARNING:
        headerRes = printf("[WARNING] ");
        break;
    case LogLevel::LEVEL_ERROR:
        headerRes = printf("[ERROR] ");
        break;
    default:
        break;
    }

    if (headerRes < 0)
        return headerRes;

    int result = vprintf(format, argv);
    va_end(argv);

    return result;
}

int Logger::LogWithPrefix(const char* prefix, const char* format, ...)
{
    va_list argv;
    int prefixRes = 0;
    va_start(argv, format);

    prefixRes = printf("%s ", prefix);
    if (prefixRes < 0)
        return prefixRes;

    int result = vprintf(format, argv);
    va_end(argv);
    return result;
}
