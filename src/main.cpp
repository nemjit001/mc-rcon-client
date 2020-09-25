#include <iostream>

#include "RCONClient.h"

int main(int argc, char **argv)
{
    std::string host, port, password;
    std::cout << "Starting RCON Client..." << std::endl;

    std::cout << "Enter Host (Default 127.0.0.1): ";
    getline(std::cin, host);

    std::cout << "Enter Port (Default 25575): ";
    getline(std::cin, port);

    std::cout << "Enter Password: ";
    std::getline(std::cin, password);

    if (host == "") host = "127.0.0.1";
    if (port == "") port = "25575";

    printf("Setting up connection to %s:%s\n", host.c_str(), port.c_str());

    RCONClient *client = new RCONClient(host, port, password);

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
