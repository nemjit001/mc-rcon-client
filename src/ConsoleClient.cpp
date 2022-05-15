#include "ConsoleClient.h"

ConsoleClient::ConsoleClient(const char* serverName, const char* serverPort, const char* serverPassword) :
    m_bRunning(true), m_pRCONClient(0), m_inputThread(), m_outputThread()
{
    m_pRCONClient = new RCONClient(serverName, serverPort, AF_INET);

    if (!m_pRCONClient->isConnected())
    {
        Logger::Log(LogLevel::WARNING, "Could not connect to %s:%s\n", serverName, serverPort);
        m_bRunning = false;
        return;
    }

    m_pRCONClient->authenticate(serverPassword, strlen(serverPassword));

    if (!m_pRCONClient->isAuthenticated())
    {
        Logger::Log(LogLevel::WARNING, "Authentication with %s:%s failed\n", serverName, serverPort);
        m_bRunning = false;
        return;
    }

    Logger::Log(LogLevel::INFO, "Connected to %s:%s\n", serverName, serverPort);

    m_inputThread = std::thread(_inputThread, this);
    m_outputThread = std::thread(_outputThread, this);
}

ConsoleClient::~ConsoleClient()
{
    delete m_pRCONClient;
}

void ConsoleClient::_inputThread()
{
    Logger::Log(LogLevel::DEBUG, "Started _inputThread()\n");
}

void ConsoleClient::_outputThread()
{
    Logger::Log(LogLevel::DEBUG, "Started _outputThread()\n");
    char* pOutBuffer = nullptr;

    while(this->isRunning())
    {
        ssize_t response = m_pRCONClient->recvResponse(&pOutBuffer);

        if (response < 0 || !pOutBuffer)
        {
            if (pOutBuffer)
                RCONClient::FreeOutBuffer(pOutBuffer, response);

            this->m_bRunning = false;
            continue;
        }

        Logger::LogWithPrefix("[RCON]", "%s", pOutBuffer);
        RCONClient::FreeOutBuffer(pOutBuffer, response);
        pOutBuffer = nullptr;
    }
}


bool ConsoleClient::isRunning()
{
    return m_bRunning;
}
