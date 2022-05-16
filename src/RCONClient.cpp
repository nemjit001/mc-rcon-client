#include "RCONClient.h"

#ifndef _WIN32
    #include <poll.h>
#endif

#include <cstring>
#include <assert.h>

#define RCON_PACKET_MIN_SIZE (sizeof(int32_t) * 2 + sizeof(int8_t) * 2)
#define RCON_PACKET_MAX_SEND_SIZE 1446
#define RCON_PACKET_MAX_RECV_SIZE 4096

RCONPacket::RCONPacket(int32_t packetID, RCONPacketType packetType, uint8_t* pPacketData, size_t dataSize) :
    m_packetSize(0), m_packetID(packetID), m_packetType(static_cast<int32_t>(packetType)), m_pPacketData(nullptr), m_zero(0)
{
    if (!pPacketData)
        return;

    m_packetSize = RCON_PACKET_MIN_SIZE + dataSize;
    m_pPacketData = (uint8_t*)calloc(dataSize, sizeof(uint8_t));

    memcpy(m_pPacketData, pPacketData, dataSize);
}

RCONPacket::~RCONPacket()
{
    if (m_pPacketData)
        free(m_pPacketData);
}

bool RCONPacket::isValid()
{
    return  (m_packetSize >= static_cast<int32_t>(RCON_PACKET_MIN_SIZE)) && \
            (
                m_packetType == static_cast<int32_t>(RCONPacketType::SERVERDATA_RESPONSE_VALUE) || \
                m_packetType == static_cast<int32_t>(RCONPacketType::SERVERDATA_EXECCOMMAND)    || \
                m_packetType == static_cast<int32_t>(RCONPacketType::SERVERDATA_AUTH_RESPONSE)  || \
                m_packetType == static_cast<int32_t>(RCONPacketType::SERVERDATA_AUTH)
            );
}

size_t RCONPacket::getDataSegmentSize()
{
    return m_packetSize - RCON_PACKET_MIN_SIZE;
}

ssize_t RCONPacket::SerializeRCONPacket(RCONPacket* pRCONPacket, uint8_t** ppOutBuffer)
{
    assert(pRCONPacket);
    assert(ppOutBuffer);

    if (!pRCONPacket || !ppOutBuffer)
        return -1;

    int32_t totalPacketSize = pRCONPacket->m_packetSize + sizeof(int32_t);
    int32_t dataSegmentSize = pRCONPacket->getDataSegmentSize();

    *ppOutBuffer = (uint8_t*)calloc(totalPacketSize, sizeof(uint8_t));

    size_t offset = 0;
    memcpy((*ppOutBuffer) + offset, &(pRCONPacket->m_packetSize), sizeof(int32_t));
    offset += sizeof(int32_t);
    memcpy((*ppOutBuffer) + offset, &(pRCONPacket->m_packetID), sizeof(int32_t));
    offset += sizeof(int32_t);
    memcpy((*ppOutBuffer) + offset, &(pRCONPacket->m_packetType), sizeof(int32_t));
    offset += sizeof(int32_t);
    memcpy((*ppOutBuffer) + offset, pRCONPacket->m_pPacketData, dataSegmentSize);
    offset += dataSegmentSize;

    assert((offset + 1) == (totalPacketSize - sizeof(int8_t)));

    return totalPacketSize;
}

RCONPacket* RCONPacket::DeserializeRCONPacket(uint8_t* pBuffer, size_t bufferSize)
{
    assert(bufferSize >= RCON_PACKET_MIN_SIZE + sizeof(int32_t));

    if (bufferSize < RCON_PACKET_MIN_SIZE + sizeof(int32_t))
        return nullptr;

    size_t offset = 0;
    int32_t* packetSize = reinterpret_cast<int32_t*>(pBuffer + offset);
    offset += sizeof(int32_t);
    int32_t* packetID = reinterpret_cast<int32_t*>(pBuffer + offset);
    offset += sizeof(int32_t);
    int32_t* packetType = reinterpret_cast<int32_t*>(pBuffer + offset);
    offset += sizeof(int32_t);
    uint8_t* pDataSegment = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(pBuffer) + offset);

    size_t dataSegmentSize = (*packetSize) - RCON_PACKET_MIN_SIZE;

    return new RCONPacket(*packetID, static_cast<RCONPacketType>(*packetType), pDataSegment, dataSegmentSize);
}

RCONClient::RCONClient(const char* serverIP, const char* port, int inFamily) :
    m_lastSentPacketID(0), m_lastReceivedPacketID(0), m_bAuthenticated(false), m_bSocketConnected(false)
{    
    addrinfo hints;
    addrinfo* results = nullptr;

    memset(&hints, 0, sizeof(hints));
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = inFamily;

    if (getaddrinfo(serverIP, port, &hints, &results) != 0)
        return;

    m_RCONServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (!sock_valid(m_RCONServerSocket))
    {
        freeaddrinfo(results);
        return;
    }
    
    if (connect(m_RCONServerSocket, results->ai_addr, results->ai_addrlen) < 0)
    {
        freeaddrinfo(results);
        return;
    }

    m_bSocketConnected = true;
    freeaddrinfo(results);
}

RCONClient::~RCONClient()
{
    if (sock_valid(m_RCONServerSocket))
        sock_close(m_RCONServerSocket);
}

ssize_t RCONClient::_sendPacket(RCONPacket* pPacket)
{
    assert(pPacket);
    if (!pPacket)
        return static_cast<size_t>(RCONErrorCode::ERROR_PACKET_INVALID);

    if (pPacket->m_packetSize - RCON_PACKET_MIN_SIZE > RCON_PACKET_MAX_SEND_SIZE) 
        return static_cast<size_t>(RCONErrorCode::ERROR_PACKET_TOO_LONG);

    if (!pPacket->isValid())
        return static_cast<size_t>(RCONErrorCode::ERROR_PACKET_INVALID);

    uint8_t *pOutBuffer = nullptr;
    ssize_t bufferSize = RCONPacket::SerializeRCONPacket(pPacket, &pOutBuffer);

    ssize_t bytesSent = send(m_RCONServerSocket, (char*)pOutBuffer, bufferSize, 0);
    free(pOutBuffer);

    if (bytesSent < 0)
        return static_cast<size_t>(RCONErrorCode::ERROR_SEND_FAILED);

    return bytesSent;
}

RCONPacket* RCONClient::_recvPacket()
{
    int32_t packetSize = 0;
    int res = recv(m_RCONServerSocket, (char*)&packetSize, sizeof(int32_t), 0);

    if (res <= 0)
        return nullptr;

    Logger::Log(LogLevel::LEVEL_DEBUG, "server bytes recvd: %d\n", res);
    uint8_t* pDataBuffer = (uint8_t*)calloc(packetSize, sizeof(uint8_t));
    res = recv(m_RCONServerSocket, (char*)pDataBuffer, packetSize, 0);
    if (res < 0)
    {
        free(pDataBuffer);
        return nullptr;
    }

    uint8_t* pPacketBuffer = (uint8_t*)calloc(packetSize + sizeof(int32_t), sizeof(uint8_t));
    memcpy(pPacketBuffer, &packetSize, sizeof(int32_t));
    memcpy(pPacketBuffer + sizeof(int32_t), pDataBuffer, packetSize);
    
    RCONPacket* pPacket = RCONPacket::DeserializeRCONPacket(pPacketBuffer, packetSize + sizeof(int32_t));

    free(pDataBuffer);
    free(pPacketBuffer);
    return pPacket;
}

bool RCONClient::isConnected()
{
    return m_bSocketConnected;
}

bool RCONClient::isAuthenticated()
{
    return m_bAuthenticated;
}

bool RCONClient::authenticate(const char* serverPassword, size_t passwordLength)
{
    assert(serverPassword);

    if (!serverPassword || (strlen(serverPassword) + 1) < passwordLength)
        return false;

    char* pPasswordNullTerm = NULL;
    size_t passwordNullTermLength = passwordLength;

    if (strlen(serverPassword) == passwordLength)
        passwordNullTermLength += 1;

    pPasswordNullTerm = (char*)calloc(passwordNullTermLength, sizeof(char));
    memcpy(pPasswordNullTerm, serverPassword, strlen(serverPassword));

    RCONPacket* pAuthPacket = new RCONPacket(m_lastSentPacketID, RCONPacketType::SERVERDATA_AUTH, (uint8_t*)pPasswordNullTerm, passwordNullTermLength);

    free(pPasswordNullTerm);

    if (_sendPacket(pAuthPacket) < 0)
    {
        delete(pAuthPacket);
        return false;
    }

    delete pAuthPacket;
    RCONPacket* pAuthResponse = _recvPacket();

    if (pAuthResponse->m_packetType != static_cast<int32_t>(RCONPacketType::SERVERDATA_AUTH_RESPONSE))
    {
        delete pAuthResponse;
        return false;
    }

    if (pAuthResponse->m_packetID == -1 || pAuthResponse->m_packetID != m_lastSentPacketID)
    {
        delete pAuthResponse;
        return false;
    }
    
    delete pAuthResponse;

    m_bAuthenticated = true;
    return true;
}

bool RCONClient::serverHasData()
{
#ifdef _WIN32
    fd_set readFds;
    FD_ZERO(&readFds);
    FD_SET(m_RCONServerSocket, &readFds);

    if (select(m_RCONServerSocket + 1, &readFds, nullptr, nullptr, 0) != 0)
    {
        Logger::Log(LogLevel::LEVEL_DEBUG, "Select on server sock failed\n");
        return false;
    }
    
    return (FD_ISSET(m_RCONServerSocket, &readFds)) ? true : false;
#else
    pollfd pollingfd;
    pollingfd.events = POLLIN;
    pollingfd.fd = m_RCONServerSocket;
    pollingfd.revents = 0;

    if (poll(&pollingfd, 1, 0) < 0) 
        return false;
    
    return pollingfd.revents & POLLIN;
#endif
}

int RCONClient::sendCommand(const char* commandBuff, size_t commandBuffLength)
{
    if (!m_bSocketConnected || !m_bAuthenticated)
        return -1;

    if (!commandBuff)
        return -1;

    m_lastSentPacketID++;
    RCONPacket* pPacket = new RCONPacket(m_lastSentPacketID, RCONPacketType::SERVERDATA_EXECCOMMAND, (uint8_t*)commandBuff, commandBuffLength);
    ssize_t sendResult = _sendPacket(pPacket);

    delete pPacket;

    if (sendResult < 0)
        return sendResult;

    return commandBuffLength;
}

ssize_t RCONClient::recvResponse(char** ppOutBuffer)
{
    if (!m_bSocketConnected || !m_bAuthenticated)
        return -1;
    
    if (!ppOutBuffer)
        return -1;
    
    size_t outBufferSize = 0;
    *ppOutBuffer = nullptr;
    RCONPacket* pPacket = _recvPacket();

    if (!pPacket)
        return static_cast<ssize_t>(RCONErrorCode::ERROR_RECV_FAILED);

    if (pPacket->m_packetType != static_cast<int32_t>(RCONPacketType::SERVERDATA_RESPONSE_VALUE))
    {
        delete pPacket;
        return static_cast<ssize_t>(RCONErrorCode::ERROR_PACKET_INVALID);
    }

    outBufferSize = pPacket->getDataSegmentSize();
    *ppOutBuffer = (char*)calloc(outBufferSize + 1, sizeof(char));

    if (!*ppOutBuffer)
    {
        delete pPacket;
        return -1;
    }


    memcpy((*ppOutBuffer), pPacket->m_pPacketData, outBufferSize);
    (*ppOutBuffer)[outBufferSize] = '\0';
    m_lastReceivedPacketID = pPacket->m_packetID;
    delete pPacket;
    return outBufferSize + 1;
}

void RCONClient::FreeOutBuffer(char* pOutBuffer, ssize_t outBufferSize)
{
    if (outBufferSize < 0)
        return;

    free(pOutBuffer);
}
