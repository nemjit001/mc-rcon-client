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
#define RCON_MAX_PACKET_SIZE RCON_MAX_PAYLOAD + 14

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
    char *payload;
    uint16_t padding;
};

class RCONClient
{
    // Variables
    private:
    bool _stopped;
    CircularLineBuffer *_send_buffer;
    std::string _server_addr;
    std::string _server_port;
    std::string _server_key;
    std::thread _send_thread;
    std::thread _recv_thread;
    std::mutex _output_mtx;
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
        packet->payload = (char *)calloc(msg_len, sizeof(char));

        strcpy(packet->payload, msg);

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
            data[12 + i] = packet->payload[i];
        }

        data[packet_size - 2] = '\0';
        data[packet_size - 1] = '\0';

        return data;
    }

    inline struct rcon *_unpack_rcon_packet(const char *buffer)
    {
        struct rcon *packet = (struct rcon *)malloc(sizeof(struct rcon));

        packet->len = 0;
        packet->req_id = 0;
        packet->req_type = 0;

        for (int i = 3; i >= 0; i--)
        {
            packet->len += (uint8_t)buffer[0 + i] >> (8 * i);
            packet->req_id += (uint8_t)buffer[4 + i] >> (8 * i);
            packet->req_type += (uint8_t)buffer[8 + i] >> (8 * i);
        }

        int msg_len = packet->len - 10;
        packet->payload = (char *)calloc(msg_len, sizeof(char));

        for (int i = 0; i < msg_len; i++)
        {
            packet->payload[i] = buffer[12 + i];
        }

        packet->padding = 0x00;

        return packet;
    }

    inline void _free_packet(struct rcon *packet)
    {
        free(packet->payload);
        free(packet);
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
    int step();
};

#endif
