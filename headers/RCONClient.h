#ifndef RCON_CLIENT_H
#define RCON_CLIENT_H

#ifndef _WIN32
    #include <pthread.h>
#endif
#include <thread>
#include <signal.h>
#include <string>
#include <iostream>

class RCONClient
{
    // Variables
    private:
    bool _stopped;
    std::string _server_addr;
    std::string _server_port;
    std::thread _send_thread;
    std::thread _recv_thread;

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
    int _send_command();
    int _recv_command();

    public:
    RCONClient(std::string serverAddress, std::string serverPort);
    ~RCONClient();
    bool is_stopped();
};

#endif