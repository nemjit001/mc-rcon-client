#include "socket.h"

int sock_init()
{
    #ifdef _WIN32
    WSADATA wsa_data;
    return WSAStartup(MAKEWORD(2,2), &wsa_data);
    #else
    return 0;
    #endif
}

bool sock_valid(SOCKET socket)
{
    #ifdef _WIN32
    return socket != INVALID_SOCKET;
    #else
    return socket >= 0;
    #endif
}

int sock_error_code()
{
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

int sock_close(SOCKET sock)
{
    int status = 0;

#ifdef _WIN32
    status = shutdown(sock, SD_BOTH);
    if (status == 0) { status = closesocket(sock); }
#else
    status = shutdown(sock, SHUT_RDWR);
    if (status == 0) { status = close(sock); }
#endif

    return status;
}

int sock_quit()
{
#ifdef _WIN32
    return WSACleanup();
#else
    return 0;
#endif
}
