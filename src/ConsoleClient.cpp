#include "ConsoleClient.h"

#include <iostream>
#include <cstring>
#include <regex>

#define CLIENT_EXIT_COMMAND_REGEX "(\\/?exit|\\/?quit)"
#define RCON_SHUTDOWN_TEXT "Stopping the server"

ConsoleClient::ConsoleClient(const char* serverName, const char* serverPort, const char* serverPassword) :
    m_bRunning(true), m_pRCONClient(0), m_inputThread(), m_outputThread()
{
    m_pRCONClient = new RCONClient(serverName, serverPort, AF_INET);

    if (!m_pRCONClient->isConnected())
    {
        Logger::Log(LogLevel::LEVEL_WARNING, "Could not connect to %s:%s\n", serverName, serverPort);
        m_bRunning = false;
        return;
    }

    m_pRCONClient->authenticate(serverPassword, strlen(serverPassword));

    if (!m_pRCONClient->isAuthenticated())
    {
        Logger::Log(LogLevel::LEVEL_WARNING, "Authentication with %s:%s failed\n", serverName, serverPort);
        m_bRunning = false;
        return;
    }

    Logger::Log(LogLevel::LEVEL_INFO, "Connected to %s:%s\n", serverName, serverPort);
    Logger::Log(LogLevel::LEVEL_INFO, "Client startup OK. To exit the program type: \"quit\" or \"exit\"\n");

    m_inputThread = std::thread(&ConsoleClient::_inputThread, this);
    m_outputThread = std::thread(&ConsoleClient::_outputThread, this);
}

ConsoleClient::~ConsoleClient()
{
    if (m_inputThread.joinable())
        m_inputThread.join();

    if (m_outputThread.joinable())
        m_outputThread.join();

    delete m_pRCONClient;
}

void ConsoleClient::_inputThread()
{
    Logger::Log(LogLevel::LEVEL_DEBUG, "Started _inputThread()\n");

    std::regex exitRegex(CLIENT_EXIT_COMMAND_REGEX, std::regex_constants::ECMAScript);
    char* pCommandBuffer = nullptr;
    while (this->isRunning())
    {
        // read input, send to server
        std::string command;
        size_t bufferSize = 0;

        getline(std::cin, command);

        bufferSize = command.length() + 1;
        pCommandBuffer = (char*)calloc(bufferSize, sizeof(char));
        memcpy(pCommandBuffer, command.c_str(), bufferSize - 1);
        pCommandBuffer[bufferSize - 1] = '\0';

        if (std::regex_match(command, exitRegex))
        {
            Logger::Log(LogLevel::LEVEL_INFO, "Shutting down MC RCON Client...\n");
            this->m_bRunning = false;
            free(pCommandBuffer);
            pCommandBuffer = nullptr;
            continue;
        }
        
        if (m_pRCONClient->sendCommand(pCommandBuffer, bufferSize) < 0)
        {
            Logger::Log(LogLevel::LEVEL_ERROR, "Failed to send command to server!\n");
            this->m_bRunning = false;
            free(pCommandBuffer);
            pCommandBuffer = nullptr;
            continue;
        }

        free(pCommandBuffer);
        pCommandBuffer = nullptr;
    }

    Logger::Log(LogLevel::LEVEL_DEBUG, "End of thread reached (_inputThread())\n");
}

void ConsoleClient::_outputThread()
{
    Logger::Log(LogLevel::LEVEL_DEBUG, "Started _outputThread()\n");
    char* pOutBuffer = nullptr;

    while(this->isRunning())
    {
        if (!m_pRCONClient->serverHasData())
            continue;

        Logger::Log(LogLevel::LEVEL_DEBUG, "server has data available\n");

        ssize_t response = m_pRCONClient->recvResponse(&pOutBuffer);
        Logger::Log(LogLevel::LEVEL_DEBUG, "response size: %d\n", response);

        if (response < 0 || !pOutBuffer)
        {
            if (pOutBuffer)
                RCONClient::FreeOutBuffer(pOutBuffer, response);

            Logger::Log(LogLevel::LEVEL_ERROR, "Failed to receive response from server!\n");
            this->m_bRunning = false;
            continue;
        }

        if (strcmp(pOutBuffer, RCON_SHUTDOWN_TEXT) == 0)
        {
            RCONClient::FreeOutBuffer(pOutBuffer, response);
            this->m_bRunning = false;

            Logger::Log(LogLevel::LEVEL_INFO, "Server has disconnected, please exit the program\n");
            continue;
        }

        if (response > 0)
            Logger::LogWithPrefix("[RCON]", "%s\n", pOutBuffer);

        RCONClient::FreeOutBuffer(pOutBuffer, response);
        pOutBuffer = nullptr;
    }

    Logger::Log(LogLevel::LEVEL_DEBUG, "End of thread reached (_outputThread())\n");
}


bool ConsoleClient::isRunning()
{
    return m_bRunning;
}
