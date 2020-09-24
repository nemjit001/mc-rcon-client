#ifndef RCON_CLIENT_H
#define RCON_CLIENT_H

#define RCON_MULTI_PACKET 0
#define RCON_COMMAND 2
#define RCON_LOGIN 3

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

typedef struct _rcon_data
{
    int length;
    int req_id;
    int type;
    uint8_t text[RCON_BUFFER_SIZE];
    uint16_t padding;
} rcon_data;

class RCONClient
{
    // Variables
    private:
    bool _stopped;
    CircularLineBuffer *_send_buffer;
    CircularLineBuffer *_recv_buffer;
    std::string _server_addr;
    std::string _server_port;
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
    int _close();
    int _send_command();
    int _recv_command();

    public:
    RCONClient(std::string serverAddress, std::string serverPort);
    ~RCONClient();
    bool is_stopped();
    int send_command_to_server(const char *command, int command_length);
    std::string read_server_response();
};

#endif
