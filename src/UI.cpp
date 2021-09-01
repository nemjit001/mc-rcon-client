#include "UI.h"

void UI::print_msg(const char *msg)
{
    if (strlen(msg) == 0)
        return;
        
    printf("[RCON] %s\n", msg);
}   

void UI::print_info(ui_info_type info_type)
{
    switch(info_type)
    {
    case INFO_START:
        printf("[INFO] Starting RCON Client...\n");
        break;
    case INFO_CONNECT_OK:
        printf("[INFO] Connected to server!\n");
        break;
    case INFO_LOGIN_OK:
        printf("[INFO] Logged in! Type 'quit' or 'exit' to end session\n");
        break;
    case INFO_EXIT_OK:
        printf("[INFO] Exit OK\n");
        break;
    case INFO_CONNECTION_SETUP:
        printf("[INFO] Trying to set up a connection to the remote server...\n");
        break;
    default:
        break;
    }
}

void UI::print_error(ui_error_type error_type)
{
    switch(error_type)
    {
    case ERROR_SOCK_INIT_FAILED:
        printf("[ERROR] Socket Initialization Failed\n");
        break;
    case ERROR_INVALID_SERVER_ADDRESS:
        printf("[ERROR] The server could not be found\n");
        break;
    case ERROR_INVALID_SOCKET_RES:
        printf("[ERROR] Socket is invalid\n");
        break;
    case ERROR_CONNECT_FAILED:
        printf("[ERROR] Failed to connect to Server\n");
        break;
    case ERROR_AUTH_SEND_FAILED:
        printf("[ERROR] Sending Auth request failed\n");
        break;
    case ERROR_AUTH_RECV_FAILED:
        printf("[ERROR] Receiving failed, no response from server\n");
        break;
    case ERROR_AUTH_FAILED:
        printf("[ERROR] Authentication failed\n");
        break;
    case ERROR_INIT_FAILED:
        printf("[ERROR] Failed to complete setup successfully\n");
        break;
    default:
        break;
    }
}

void UI::print_setup(ui_setup_type setup_type)
{
    switch(setup_type)
    {
    case SETUP_HOSTNAME:
        printf("[SETUP] Enter Hostname (Default 127.0.0.1): ");
        break;
    case SETUP_PORT:
        printf("[SETUP] Enter Port (Default 25575): ");
        break;
    case SETUP_PASSWORD:
        printf("[SETUP] Enter Password: ");
        break;
    default:
        break;
    }
}
