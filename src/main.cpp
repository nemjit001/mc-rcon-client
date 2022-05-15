#include "ConsoleClient.h"

int main()
{
    sock_init();

    ConsoleClient client("127.0.0.1", "25575", "test_rcon");

    while(client.isRunning())
    {
        // busy wait until client is stopped
    }

    sock_quit();
    return 0;
}
