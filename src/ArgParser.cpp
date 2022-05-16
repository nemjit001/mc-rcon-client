#include "ArgParser.h"

#include "Logger.h"

#include <cstring>
#include <cstdlib>

ArgInfo ArgParser::ParseArgv(int argc, char** argv)
{
    ArgInfo argInfo;
    memset(&argInfo, 0, sizeof(argInfo));

    if (argc > 1)
    {
        Logger::Log(LogLevel::LEVEL_DEBUG, "Detected %d cli args:\n", argc);
        for (int i = 0; i < argc; i++)
        {
            Logger::Log(LogLevel::LEVEL_DEBUG, "\t%s\n", argv[i]);
        }
    }

    // TODO: start parsing

    return argInfo;
}

void ArgParser::FreeArgInfo(ArgInfo argInfo)
{
    if (argInfo.m_bServerNameSet && argInfo.m_pServerName)
        free(argInfo.m_pServerName);

    if (argInfo.m_bServerPortSet && argInfo.m_pServerPort)
        free(argInfo.m_pServerPort);
    
    if (argInfo.m_bServerPassSet && argInfo.m_pServerPass)
        free(argInfo.m_pServerPass);
}
