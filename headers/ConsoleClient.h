#pragma once

#include <thread>

#include "RCONClient.h"

class ConsoleClient
{
private:
    bool m_bRunning;
    RCONClient* m_pRCONClient;

    std::thread m_inputThread;
    std::thread m_outputThread;

    void _inputThread();
    void _outputThread();

public:
    ConsoleClient(const char* serverName, const char* serverPort, const char* serverPassword);
    ~ConsoleClient();

    bool isRunning();
};
