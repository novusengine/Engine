#pragma once
#include <Base/Types.h>

class Bytebuffer;

namespace Network
{
    typedef u16 OpcodeType;

    struct ConnectionInfo
    {
    public:
        u32 ipAddr;
        std::string ipAddrStr;

        u16 port;
    };

    struct PacketHeader
    {
    public:
        OpcodeType opcode = 0;
        u16 size = 0;
    };

    enum class SocketID : u32 {}; // 8 Bits for versioning, 24 bits for value
    constexpr SocketID SOCKET_ID_INVALID = static_cast<SocketID>(-1);
    constexpr u32 SOCKET_ID_BITS = sizeof(std::underlying_type<SocketID>::type) * 8;
    constexpr u32 SOCKET_ID_VERSION_BITS = 8;
    constexpr u32 SOCKET_ID_VALUE_BITS = SOCKET_ID_BITS - SOCKET_ID_VERSION_BITS;
    constexpr u32 SOCKET_ID_VERSION_MASK = (1 << SOCKET_ID_VERSION_BITS) - 1;
    constexpr u32 SOCKET_ID_VALUE_MASK = (1 << SOCKET_ID_VALUE_BITS) - 1;

    constexpr u32 DEFAULT_BUFFER_SIZE = 8192;

    struct SocketConnectedEvent
    {
    public:
        SocketID socketID = SOCKET_ID_INVALID;
        ConnectionInfo connectionInfo;
    };
    struct SocketDisconnectedEvent
    {
    public:
        SocketID socketID = SOCKET_ID_INVALID;
    };
    struct SocketMessageEvent
    {
    public:
        SocketID socketID = SOCKET_ID_INVALID;
        std::shared_ptr<Bytebuffer> buffer = nullptr;
    };
}