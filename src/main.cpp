#include <iostream>
#include <cstring>

#include "RCONClient.h"

int main()
{
    sock_init();

    RCONClient client("127.0.0.1", "25575", AF_INET);
    
    if (!client.isConnected())
    {
        printf("Not connected to server\n");
        sock_quit();
        return -1;
    }

    client.authenticate("test_rcon", 9);

    if (!client.isAuthenticated())
    {
        printf("Not authenticated with server\n");
        sock_quit();
        return -1;
    }

    printf("Auth OK\n");

    sock_quit();
    return 0;
}
