#ifndef RCON_CLIENT_H
#define RCON_CLIENT_H

#define RCON_SERVERDATA_RESPONSE_VALUE 0
#define RCON_SERVERDATA_EXECCOMMAND 2
#define RCON_SERVERDATA_AUTH 3

#define RCON_CONNECT_OK 0
#define RCON_ADDR_INFO_INVALID 1
#define RCON_SOCK_INVALID 2
#define RCON_REFUSED_BY_HOST 3

#define RCON_BUFFER_SIZE 4096

#ifndef _WIN32
    #include <pthread.h>
#endif
#include <thread>
#include <signal.h>
#include <string>
#include <iostream>
#include <cstring>

#include "CircularLineBuffer.h"
#include "socket.h"

class RCONClient
{
    // Variables
    private:
    bool _stopped;
    CircularLineBuffer *_send_buffer;
    CircularLineBuffer *_recv_buffer;
    std::string _server_addr;
    std::string _server_port;
    std::string _server_key;
    std::thread _send_thread;
    std::thread _recv_thread;
    SOCKET _rcon_socket;

    // Functions
    private:
    inline void _start_threads()
    {
        this->_send_thread = std::thread(&RCONClient::__send_command, this);
        this->_recv_thread = std::thread(&RCONClient::__recv_command, this);
    }

    inline void _stop_threads()
    {
        this->_send_thread.join();
        this->_recv_thread.join();
    }

    inline void __send_command()
    {
        while (!this->_stopped)
        {
            if (this->_send_command() != 0)
            {
                this->_stopped = true;
            }
        }
    }

    inline void __recv_command()
    {
        while (!this->_stopped)
        {
            if (this->_recv_command() != 0)
            {
                this->_stopped = true;
            }
        }
    }

    int _connect();
    int _authenticate();
    int _close();
    int _send_command();
    int _recv_command();

    public:
    RCONClient(std::string server_address, std::string server_port, std::string key);
    ~RCONClient();
    bool is_stopped();
    int send_command_to_server(const char *command, int command_length);
    std::string read_server_response();
};

#endif
