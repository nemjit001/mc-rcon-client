#include "ArgParser.h"

#include "Logger.h"

#include <cstring>
#include <cstdlib>
#include <regex>

#define SERVER_NAME_PARAM_REGEX "^(-n|--name)$"
#define SERVER_PORT_PARAM_REGEX "^(-p|--port)$"
#define SERVER_PASS_PARAM_REGEX "^(-s|--password)$"
#define HELP_PARAM_REGEX        "^(-h|--help)$"

#define SERVER_PORT_VALIDATION_REGEX "^[0-9]{1,5}$"

bool ArgParser::_setParam(char*& param, int argc, char** argv, int paramIdx)
{
    if (paramIdx < argc && ArgParser::_isArgValidInput(argv[paramIdx]))
    {
        size_t paramSize = strlen(argv[paramIdx]);

        param = (char*)calloc(paramSize + 1, sizeof(char));
        memcpy(param, argv[paramIdx], paramSize);
        param[paramSize] = '\0';
    }
    else
        return false;

    return true;
}

bool ArgParser::_isArgValidInput(char* arg)
{
    std::regex serverNameParamRegex(SERVER_NAME_PARAM_REGEX, std::regex_constants::ECMAScript);
    std::regex serverPortParamRegex(SERVER_PORT_PARAM_REGEX, std::regex_constants::ECMAScript);
    std::regex serverPassParamRegex(SERVER_PASS_PARAM_REGEX, std::regex_constants::ECMAScript);
    std::regex helpParamRegex(HELP_PARAM_REGEX, std::regex_constants::ECMAScript);

    return !(
        std::regex_match(arg, serverNameParamRegex) ||
        std::regex_match(arg, serverPortParamRegex) ||
        std::regex_match(arg, serverPassParamRegex) ||
        std::regex_match(arg, helpParamRegex)
    );
}

void ArgParser::DisplayHelp()
{
    Logger::LogWithPrefix("[Help]", "Minecraft RCON Client Help:\n");
    Logger::LogWithPrefix("[Help]", "\n");
    Logger::LogWithPrefix("[Help]", "Available parameters:\n");
    Logger::LogWithPrefix("[Help]", "\t\t-n, --name\t\tThe server address to connect to\n");
    Logger::LogWithPrefix("[Help]", "\t\t-p, --port\t\tThe RCON port of the server (range: 1~65535)\n");
    Logger::LogWithPrefix("[Help]", "\t\t-s, --password\t\tThe RCON password set in the server configuration\n");
    Logger::LogWithPrefix("[Help]", "\t\t-h, --help\t\tDisplay this help screen\n");
    Logger::LogWithPrefix("[Help]", "\n");
    Logger::LogWithPrefix("[Help]", "Any missing parameters will need to be provided when the program has started\n");
    Logger::LogWithPrefix("[Help]", "\n");
    Logger::LogWithPrefix("[Help]", "Author: Tijmen Verhoef\n");
    Logger::LogWithPrefix("[Help]", "Contact: See the github repository: https://www.github.com/nemjit001/mc-rcon-client\n");
    Logger::LogWithPrefix("[Help]", "\n");
    Logger::LogWithPrefix("[Help]", "Thank you for using the MC RCON Client!\n");
}

ArgInfo ArgParser::ParseArgv(int argc, char** argv)
{
    ArgInfo argInfo;
    memset(&argInfo, 0, sizeof(argInfo));

    std::regex serverNameParamRegex(SERVER_NAME_PARAM_REGEX, std::regex_constants::ECMAScript);
    std::regex serverPortParamRegex(SERVER_PORT_PARAM_REGEX, std::regex_constants::ECMAScript);
    std::regex serverPassParamRegex(SERVER_PASS_PARAM_REGEX, std::regex_constants::ECMAScript);
    std::regex serverPortValidationRegex(SERVER_PORT_VALIDATION_REGEX, std::regex_constants::ECMAScript);
    std::regex helpParamRegex(HELP_PARAM_REGEX, std::regex_constants::ECMAScript);

    if (argc > 1)
    {
        Logger::Log(LogLevel::LEVEL_DEBUG, "Parsing %d cli args:\n", argc);
        for (int i = 0; i < argc; i++)
            Logger::Log(LogLevel::LEVEL_DEBUG, "\t%s\n", argv[i]);
    }

    for (int i = 1; i < argc; i++)
    {
        if (std::regex_match(argv[i], serverNameParamRegex))
        {
            Logger::Log(LogLevel::LEVEL_DEBUG, "Found server name param @ idx %d\n", i);
            
            if (ArgParser::_setParam(argInfo.m_pServerName, argc, argv, i + 1))
            {
                Logger::Log(LogLevel::LEVEL_DEBUG, "\tSet \"%s\" as server name\n", argInfo.m_pServerName);
                argInfo.m_bServerNameSet = true;
                i += 1;
            }
            else
            {
                argInfo.m_bDisplayHelp = true;
                break;
            }
        }
        else if (std::regex_match(argv[i], serverPortParamRegex))
        {
            Logger::Log(LogLevel::LEVEL_DEBUG, "Found server port param @ idx %d\n", i);

            if (ArgParser::_setParam(argInfo.m_pServerPort, argc, argv, i + 1))
            {
                Logger::Log(LogLevel::LEVEL_DEBUG, "\tSet \"%s\" as server port\n", argInfo.m_pServerPort);
                argInfo.m_bServerPortSet = true;
                i += 1;

                if (!std::regex_match(argInfo.m_pServerPort, serverPortValidationRegex))
                {
                    argInfo.m_bDisplayHelp = true;
                    break;
                }
            }
            else
            {
                argInfo.m_bDisplayHelp = true;
                break;
            }
        }
        else if (std::regex_match(argv[i], serverPassParamRegex))
        {
            Logger::Log(LogLevel::LEVEL_DEBUG, "Found server password param @ idx %d\n", i);

            if (ArgParser::_setParam(argInfo.m_pServerPass, argc, argv, i + 1))
            {
                Logger::Log(LogLevel::LEVEL_DEBUG, "\tSet \"%s\" as server password\n", argInfo.m_pServerPass);
                argInfo.m_bServerPassSet = true;
                i += 1;
            }
            else
            {
                argInfo.m_bDisplayHelp = true;
                break;
            }
        }
        else if (std::regex_match(argv[i], helpParamRegex))
        {
            argInfo.m_bDisplayHelp = true;
            break;
        }
        else
        {
            argInfo.m_bUnknownArgFound = true;
            break;
        }
    }

    return argInfo;
}

void ArgParser::FreeArgInfo(ArgInfo argInfo)
{
    if (argInfo.m_bServerNameSet)
        free(argInfo.m_pServerName);

    if (argInfo.m_bServerPortSet)
        free(argInfo.m_pServerPort);

    if (argInfo.m_bServerPassSet)
        free(argInfo.m_pServerPass);
}
