#include "RCONClient.h"

RCONClient::RCONClient()
{
    this->_server_addr = "";
    this->_server_port = "";
    this->_server_key = "";
    this->_stopped = false;
    this->_send_buffer = new CircularLineBuffer();
    this->_rcon_socket = INVALID_SOCKET;
    this->_start_req_id = 1;
}

RCONClient::~RCONClient()
{
    this->_close();
    this->_stop_threads();
    
    sock_quit();

    delete this->_send_buffer;
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
        return RCON_ADDR_INFO_INVALID;

    this->_rcon_socket = socket(results->ai_family, results->ai_socktype, results->ai_protocol);

    if (!sock_valid(this->_rcon_socket))
        return RCON_SOCK_INVALID;

    if (connect(this->_rcon_socket, results->ai_addr, results->ai_addrlen) != 0)
        return RCON_REFUSED_BY_HOST;

    freeaddrinfo(results);

    return RCON_CONNECT_OK;
}

int RCONClient::_authenticate()
{
    /*
     * Auth flow:
     * send with password in body and RCON_LOGIN as type
     * recv SERVER_DATA_RESPONSE value with empty body, id of -1 is failed, same id is completed
     */

    char *buffer = (char *)calloc(RCON_MAX_PACKET_SIZE, sizeof(char));
    char *packed_packet;

    struct rcon *packet = this->_generate_rcon_packet(this->_server_key.c_str(), this->_server_key.length(), this->_start_req_id++, RCON_SERVERDATA_AUTH);
    packed_packet = this->_pack_rcon_packet(packet);
    
    if (send(this->_rcon_socket, packed_packet, packet->len + 4, 0) < 0)
        return 1;

    if (recv(this->_rcon_socket, buffer, RCON_MAX_PACKET_SIZE, 0) <= 0)
        return 2;

    struct rcon *return_packet = this->_unpack_rcon_packet(buffer);

    if (return_packet->req_id != packet->req_id)
        return 3;

    free(buffer);
    free(packed_packet);
    this->_free_packet(packet);
    this->_free_packet(return_packet);

    return 0;
}

int RCONClient::_close()
{
    return sock_close(this->_rcon_socket);
}

int RCONClient::_send_command()
{
    std::string command;
    
    getline(std::cin, command);

    if (command == "exit" || command == "quit")
        return 1;
    
    command += '\n';
    this->_send_buffer->write(command.c_str(), command.length());

    return 0;
}

int RCONClient::_recv_command()
{
    char buffer[RCON_MAX_PACKET_SIZE];

    memset(&buffer, 0, sizeof(buffer));

    if (recv(this->_rcon_socket, buffer, RCON_MAX_PACKET_SIZE, 0) <= 0)
        return -1;

    struct rcon *packet = this->_unpack_rcon_packet(buffer);
    
    UI::print_msg(packet->payload);

    this->_free_packet(packet);

    return 0;
}

int RCONClient::init(std::string server_address, std::string server_port, std::string key)
{
    this->_server_addr = server_address;
    this->_server_port = server_port;
    this->_server_key = key;
    this->_stopped = false;

    if (sock_init() != 0)
    {
        this->_stopped = true;
        UI::print_error(ERROR_SOCK_INIT_FAILED);
        return 1;
    }

    int res = this->_connect();
    if (res == RCON_CONNECT_OK)
    {
        UI::print_info(INFO_CONNECT_OK);
    }
    else
    {
        this->_stopped = true;
        switch(res)
        {
        case RCON_ADDR_INFO_INVALID:
            UI::print_error(ERROR_INVALID_SERVER_ADDRESS);
            break;
        case RCON_SOCK_INVALID:
            UI::print_error(ERROR_INVALID_SOCKET_RES);
            break;
        case RCON_REFUSED_BY_HOST:
            UI::print_error(ERROR_CONNECT_FAILED);
            break;
        default:
            break;
        }

        return 1;
    }

    res = this->_authenticate();
    if (res == 0)
    {
        UI::print_info(INFO_LOGIN_OK);
    }
    else
    {
        this->_stopped = true;
        switch(res)
        {
        case 1:
            UI::print_error(ERROR_AUTH_SEND_FAILED);
            break;
        case 2:
            UI::print_error(ERROR_AUTH_RECV_FAILED);
            break;
        case 3:
            UI::print_error(ERROR_AUTH_FAILED);
            break;
        default:
            break;
        }

        return 1;
    }

    return 0;
}

bool RCONClient::is_stopped()
{
    return this->_stopped;
}

int RCONClient::step()
{
    std::string command = this->_send_buffer->read();

    if (command != "")
    {
        struct rcon *packet = this->_generate_rcon_packet(command.c_str(), command.length(), this->_start_req_id++, RCON_SERVERDATA_EXECCOMMAND);

        if (send(this->_rcon_socket, this->_pack_rcon_packet(packet), packet->len + 4, 0) < 0)
            return -1;

        this->_free_packet(packet);
    }

    return 0;
}
