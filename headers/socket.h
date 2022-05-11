#pragma once

#ifdef _WIN32
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0A00
    #endif
    // WINDOWS INCLUDES
    #pragma comment(lib,"Ws2_32.lib")
    #include <WinSock2.h> 
    #include <WS2tcpip.h>
#else
    // POSIX INCLUDES
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <errno.h>

    typedef int SOCKET;

    #define INVALID_SOCKET (SOCKET (~0))
    #define SOCKET_ERROR (-1)
#endif

int sock_init();
bool sock_valid(SOCKET socket);
int sock_error_code();
int sock_close(SOCKET sock);
int sock_quit();
