#include <iostream>
#include <cstring>

#include "Config.h"
#include "ArgParser.h"
#include "Logger.h"
#include "ConsoleClient.h"

char* getUserInput()
{
    std::string input;
    std::getline(std::cin, input);

    char* pInput = (char*)calloc(input.length() + 1, sizeof(char));
    memcpy(pInput, input.c_str(), input.length());
    pInput[input.length()] = '\0';

    return pInput;
}

void askForMissingArgs(ArgInfo& argInfo)
{   
    if (argInfo.m_bDisplayHelp)
        return;

    Logger::Log(LogLevel::LEVEL_INFO, "Follow the prompts to connect to your Minecraft server\n");
    
    if (!argInfo.m_bServerNameSet)
    {
        Logger::Log(LogLevel::LEVEL_INFO, "What is your server's hostname? : ");
        argInfo.m_pServerName = getUserInput();
        argInfo.m_bServerNameSet = true;
    }

    if (!argInfo.m_bServerPortSet)
    {
        Logger::Log(LogLevel::LEVEL_INFO, "On what port is RCON running? : ");
        argInfo.m_pServerPort = getUserInput();
        argInfo.m_bServerPortSet = true;
    }

    if (!argInfo.m_bServerPassSet)
    {
        Logger::Log(LogLevel::LEVEL_INFO, "What is your RCON password? : ");
        argInfo.m_pServerPass = getUserInput();
        argInfo.m_bServerPassSet = true;
    }
}

int main(int argc, char** argv)
{
    sock_init();

    Logger::Log(LogLevel::LEVEL_INFO, "Running %s version %s\n", PROJECT_NAME, PROJECT_VERSION);

    ArgInfo argInfo = ArgParser::ParseArgv(argc, argv);

    if (argInfo.m_bUnknownArgFound)
    {
        Logger::Log(LogLevel::LEVEL_ERROR, "Unknown argument passed to client. See the Help page for valid CLI arguments:\n");
        ArgParser::DisplayHelp();
        sock_quit();
        return -1;
    }

    if (argInfo.m_bDisplayHelp)
    {
        ArgParser::DisplayHelp();
        sock_quit();
        return 0;
    }

    if (!argInfo.m_bDisplayHelp && !(argInfo.m_bServerNameSet && argInfo.m_bServerPortSet && argInfo.m_bServerPassSet))
        askForMissingArgs(argInfo);

    ConsoleClient client(argInfo.m_pServerName, argInfo.m_pServerPort, argInfo.m_pServerPass);
    ArgParser::FreeArgInfo(argInfo);

    while(client.isRunning())
    {
        // busy wait until client is stopped
    }

    sock_quit();
    return 0;
}
