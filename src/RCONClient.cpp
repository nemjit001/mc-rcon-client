#include "RCONClient.h"

RCONClient::RCONClient(std::string server_address, std::string server_port)
{
    this->_server_addr = server_address;
    this->_server_port = server_port;
    this->_stopped = false;
    this->_send_buffer = new CircularLineBuffer();
    this->_recv_buffer = new CircularLineBuffer();
    this->_rcon_socket = INVALID_SOCKET;

    if (sock_init() != 0 || this->_connect() != 0)
        this->_stopped = true;
    this->_start_threads();
}

RCONClient::~RCONClient()
{
    this->_close();
    this->_stop_threads();

    sock_quit();

    delete this->_send_buffer;
    delete this->_recv_buffer;
}

int RCONClient::_connect()
{
    addrinfo hints;
    addrinfo *results;

    memset(&hints, 0, sizeof(hints));
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;

    if (getaddrinfo(this->_server_addr.c_str(), this->_server_port.c_str(), &hints, &results) != 0)
        return 1;

    printf("Got address info\t%d\t%d\t%d\n", results->ai_socktype, results->ai_family, results->ai_protocol);

    this->_rcon_socket = socket(results->ai_family, results->ai_socktype, results->ai_protocol);

    if (!sock_valid(this->_rcon_socket))
        return 1;

    printf("Socket is valid...\n");

    if (connect(this->_rcon_socket, results->ai_addr, results->ai_addrlen) != 0)
        return 1;

    printf("Connected to RCON server...\n");

    return 0;
}

int RCONClient::_close()
{
    return sock_close(this->_rcon_socket);
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
