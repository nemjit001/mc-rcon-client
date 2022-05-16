#pragma once

struct ArgInfo
{
    bool m_bDisplayHelp     : 1;
    bool m_bServerNameSet   : 1;
    bool m_bServerPortSet   : 1;
    bool m_bServerPassSet   : 1;

    char* m_pServerName;
    char* m_pServerPort;
    char* m_pServerPass;
};

class ArgParser
{
public:
    static void PrintHelp();
    static ArgInfo ParseArgv(int argc, char** argv);
    static void FreeArgInfo(ArgInfo argInfo);
};
