#ifndef RCON_CLIENT_H
#define RCON_CLIENT_H

#ifndef _WIN32
    #include <pthread.h>
#endif
#include <thread>
#include <string>

class RCONClient
{
    // Variables
    private:
    bool _stopped;
    std::string _serverAddress;
    std::string _serverPort;

    public:

    // Functions
    private:
    
    public:
    RCONClient(std::string serverAddress, std::string serverPort);
    bool IsStopped();
};

#endif