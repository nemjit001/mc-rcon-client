#ifndef RCON_CLIENT_H
#define RCON_CLIENT_H

#define RCON_SERVERDATA_RESPONSE_VALUE 0
#define RCON_SERVERDATA_EXECCOMMAND 2
#define RCON_SERVERDATA_AUTH 3

#define RCON_CONNECT_OK 0
#define RCON_ADDR_INFO_INVALID 1
#define RCON_SOCK_INVALID 2
#define RCON_REFUSED_BY_HOST 3

#define RCON_MAX_PAYLOAD 4096

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

struct rcon
{
    int32_t len;
    int32_t req_id;
    int32_t req_type;
    uint8_t *payload;
    uint16_t padding;
};

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
    int32_t _start_req_id;

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

    inline struct rcon *_generate_rcon_packet(const char *msg, size_t msg_len, int32_t req_id, int32_t req_type)
    {
        struct rcon *packet = (struct rcon*)malloc(sizeof(struct rcon));
        packet->len = msg_len + 10;
        packet->req_id = req_id;
        packet->req_type = req_type;
        packet->payload = (uint8_t *)calloc(msg_len, sizeof(uint8_t));

        memcpy(packet->payload, msg, msg_len);

        packet->padding = 0x00;

        return packet;
    }

    inline char *_pack_rcon_packet(struct rcon *packet)
    {
        int packet_size = packet->len + 4;

        char *data = (char *)calloc(packet_size, sizeof(char));

        for (int i = 0; i < 4; i++)
        {
            data[0 + i] += (char)(packet->len >> (i * 8));
            data[4 + i] += (char)(packet->req_id >> (i * 8));
            data[8 + i] += (char)(packet->req_type) >> (i * 8);
        }

        for (size_t i = 0; i < strlen((char *)packet->payload); i++)
        {
            data[12 + i] = (char)packet->payload[i];
        }

        data[packet_size - 2] = '\0';
        data[packet_size - 1] = '\0';

        return data;
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
