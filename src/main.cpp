#include "ArgParser.h"
#include "ConsoleClient.h"

int main(int argc, char** argv)
{
    sock_init();

    ArgInfo argInfo = ArgParser::ParseArgv(argc, argv);
    ConsoleClient client("127.0.0.1", "25575", "test_rcon");
    ArgParser::FreeArgInfo(argInfo);

    while(client.isRunning())
    {
        // busy wait until client is stopped
    }

    sock_quit();
    return 0;
}
