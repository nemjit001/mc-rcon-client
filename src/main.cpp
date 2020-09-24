#include <iostream>

#include "RCONClient.h"

int main(int argc, char **argv)
{
    RCONClient *client = new RCONClient("127.0.0.1", "25575");

    std::cout << "Starting RCON Client..." << std::endl;

    while(!client->is_stopped())
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(100)
        );
    }

    std::cout << "Shutdown of RCON Client Successfull..." << std::endl;
    delete client;
    return 0;
}
