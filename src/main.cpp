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
        goto error;
    }

    client.authenticate("42", 2);

    if (!client.isAuthenticated())
    {
        printf("Not authenticated with server\n");
        goto error;
    }

    sock_quit();
    return 0;

error:
    sock_quit();
    return -1;
}
