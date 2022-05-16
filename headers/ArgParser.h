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
private:
    static bool _setParam(char*& param, int argc, char** argv, int paramIdx);
    static bool _isArgValidInput(char* arg);

public:
    static void DisplayHelp();
    static ArgInfo ParseArgv(int argc, char** argv);
};
