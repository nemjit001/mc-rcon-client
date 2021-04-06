#include <iostream>

#include "RCONClient.h"

#define SETUP_RETRIES 3

struct connection_params
{
    std::string hostname;
    std::string port;
    std::string password;
};

void connection_setup(struct connection_params &params)
{
    std::string host, port, password;

    UI::print_setup(SETUP_HOSTNAME);
    getline(std::cin, host);

    UI::print_setup(SETUP_PORT);
    getline(std::cin, port);

    UI::print_setup(SETUP_PASSWORD);
    std::getline(std::cin, password);

    if (host == "")
        host = "127.0.0.1";
        
    if (port == "")
        port = "25575";

    params.hostname = host;
    params.port = port;
    params.password = password;

    UI::print_info(INFO_CONNECTION_SETUP);
}

int main(int argc, char **argv)
{
    bool successful_setup = false;
    RCONClient *client = new RCONClient();
    UI::print_info(INFO_START);
    
    for (int i = 0; i < SETUP_RETRIES; i++)
    {
        struct connection_params params;
        connection_setup(params);

        if (client->init(params.hostname, params.port, params.password) == 0)
        {
            successful_setup = true;

            break;
        }
    }

    if (!successful_setup)
    {
        delete client;
        
        UI::print_error(ERROR_INIT_FAILED);

        return 1;
    }

    // after successful init we can start the client
    client->start();

    while(!client->is_stopped() && client->step() == 0)
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(10)
        );
    }

    delete client;

    UI::print_info(INFO_EXIT_OK);

    return 0;
}
