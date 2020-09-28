#include <iostream>

#include "RCONClient.h"

int main(int argc, char **argv)
{
    std::string host, port, password;
    std::cout << (USE_ANSI ? "\u001b[32m[INFO]\u001b[37m Starting RCON Client..."  : "[INFO] Starting RCON Client...") << std::endl;

    std::cout << (USE_ANSI ? "\u001b[36m[SETUP]\u001b[37m Enter Host (Default 127.0.0.1): " : "[SETUP] Enter Host (Default 127.0.0.1): ");
    getline(std::cin, host);

    std::cout << (USE_ANSI ? "\u001b[36m[SETUP]\u001b[37m Enter Port (Default 25575): " : "[SETUP] Enter Port (Default 25575): ");
    getline(std::cin, port);

    std::cout << (USE_ANSI ? "\u001b[36m[SETUP]\u001b[37m Enter Password: " : "[SETUP] Enter Password: ");
    std::getline(std::cin, password);

    if (host == "") host = "127.0.0.1";
    if (port == "") port = "25575";

    printf((USE_ANSI ? "\u001b[32m[INFO]\u001b[37m Setting up connection to %s:%s\n" : "[INFO] Setting up connection to %s:%s\n"), host.c_str(), port.c_str());

    RCONClient *client = new RCONClient(host, port, password);
    std::string command;

    while(!client->is_stopped() && client->step() == 0)
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(10)
        );
    }

    std::cout << (USE_ANSI ? "\u001b[32m[INFO]\u001b[37m Exit OK" : "[INFO] Exit OK") << std::endl;
    delete client;
    return 0;
}
