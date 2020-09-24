#include "RCONClient.h"

RCONClient::RCONClient(std::string server_address, std::string server_port)
{
    this->_server_addr = server_address;
    this->_server_port = server_port;
    this->_stopped = false;
    this->_connect();
    this->_start_threads();
}

RCONClient::~RCONClient()
{
    this->_stop_threads();
}

int RCONClient::_connect()
{
    return 0;
}

int RCONClient::_send_command()
{
    std::string command;
    std::cin >> command;

    std::cout << command << std::endl;

    return 0;
}

int RCONClient::_recv_command()
{
    return 0;
}


bool RCONClient::is_stopped()
{
    return this->_stopped;
}