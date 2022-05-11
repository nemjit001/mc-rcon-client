#include <iostream>

#include "RCONClient.h"

int main()
{
    sock_init();

    RCONClient client("127.0.0.1", "25575", AF_INET);

    if (!client.isConnected())
        return 0;

    client.authenticate("42", 2);

    printf("%d %d\n", client.isAuthenticated(), client.isConnected());

    if (!client.isAuthenticated())
        return 0;
    
    char* responseBuffer = nullptr;
    client.sendCommand("/help", 5);
    client.recvResponse(&responseBuffer);

    printf("%s\n", responseBuffer);

    free(responseBuffer);
    sock_quit();
    return 0;
}
