#pragma once

#include <cstdlib>
#include <cstdint>

#include "socket.h"
#include "Logger.h"

enum class RCONPacketType
{
    SERVERDATA_RESPONSE_VALUE   = 0,
    SERVERDATA_EXECCOMMAND      = 2,
    SERVERDATA_AUTH_RESPONSE    = 2,
    SERVERDATA_AUTH             = 3
};

enum class RCONErrorCode
{
    ERROR_PACKET_TOO_LONG       = -1,
    ERROR_PACKET_INVALID        = -2,
    ERROR_SEND_FAILED           = -4,
    ERROR_RECV_FAILED           = -8
};

struct RCONPacket
{
    int32_t m_packetSize;
    int32_t m_packetID;
    int32_t m_packetType;
    uint8_t* m_pPacketData;
    uint8_t m_zero;

    RCONPacket() = delete;
    RCONPacket(int32_t packetID, RCONPacketType packetType, uint8_t* pPacketData, size_t dataSize);
    ~RCONPacket();

    bool isValid();
    size_t getDataSegmentSize();

    static ssize_t SerializeRCONPacket(RCONPacket* pRCONPacket, uint8_t** ppOutBuffer);
    static RCONPacket* DeserializeRCONPacket(uint8_t* pBuffer, size_t bufferSize);
};

class RCONClient
{
private:
    SOCKET m_RCONServerSocket;
    int32_t m_lastSentPacketID;
    int32_t m_lastReceivedPacketID;
    bool m_bAuthenticated;
    bool m_bSocketConnected;

    ssize_t _sendPacket(RCONPacket* pPacket);
    RCONPacket* _recvPacket();

public:
    RCONClient() = delete;
    RCONClient(const char* serverIP, const char* port, int inFamily);
    ~RCONClient();

    bool isConnected();
    bool isAuthenticated();
    bool authenticate(const char* serverPassword, size_t passwordLength);
    bool serverHasData();
    int sendCommand(const char* commandBuff, size_t commandBuffLength);
    ssize_t recvResponse(char** ppOutBuffer);

    static void FreeOutBuffer(char* pOutBuffer, ssize_t outBufferSize);
};
