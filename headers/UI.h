#ifndef UI_H
#define UI_H

#include <string.h>
#include <iostream>

typedef enum
{
    ERROR_SOCK_INIT_FAILED,
    ERROR_INVALID_SERVER_ADDRESS,
    ERROR_INVALID_SOCKET_RES,
    ERROR_CONNECT_FAILED,
    ERROR_AUTH_FAILED,
    ERROR_AUTH_SEND_FAILED,
    ERROR_AUTH_RECV_FAILED,
    ERROR_INIT_FAILED
} ui_error_type;

typedef enum
{
    INFO_START,
    INFO_CONNECT_OK,
    INFO_LOGIN_OK,
    INFO_EXIT_OK,
    INFO_CONNECTION_SETUP
} ui_info_type;

typedef enum
{
    SETUP_HOSTNAME,
    SETUP_PORT,
    SETUP_PASSWORD
} ui_setup_type;

class UI
{
private:
public:
    static void print_msg(const char *msg);
    static void print_info(ui_info_type info_type);
    static void print_error(ui_error_type error_type);
    static void print_setup(ui_setup_type setup_type);
};

#endif // UI_H
