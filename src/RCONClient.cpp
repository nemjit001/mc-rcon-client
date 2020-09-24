#include "RCONClient.h"

RCONClient::RCONClient(std::string serverAddress, std::string serverPort)
{
    this->_serverAddress = serverAddress;
    this->_serverPort = serverPort;
    this->_stopped = false;
}

bool RCONClient::IsStopped()
{
    return this->_stopped;
}