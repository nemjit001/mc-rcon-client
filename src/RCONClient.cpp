#include "RCONClient.h"

RCONClient::RCONClient(std::string server_address, std::string server_port)
{
    this->_server_addr = server_address;
    this->_server_port = server_port;
    this->_stopped = false;
    this->_send_buffer = new CircularLineBuffer();
    this->_recv_buffer = new CircularLineBuffer();
    this->_connect();
    this->_start_threads();
}

RCONClient::~RCONClient()
{
    this->_stop_threads();
    delete this->_send_buffer;
    delete this->_recv_buffer;
}

int RCONClient::_connect()
{
    return 0;
}

int RCONClient::_send_command()
{
    std::string command = this->_send_buffer->read();
    std::cout << command;
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

int RCONClient::send_command_to_server(const char *command, int command_length)
{
    // writes to the send buffer so the send thread can consume it when it is available again
    return this->_send_buffer->write(command, command_length);
}

std::string RCONClient::read_server_response()
{
    // reads responses in the order they come in
    return this->_recv_buffer->read();
}
