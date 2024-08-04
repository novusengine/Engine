#pragma once
#include <Base/Types.h>
#include <memory>

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

    struct MessageHeader
    {
    public:
        struct Flags
        {
            u8 isPing : 1;
        };

    public:
        OpcodeType opcode = 0;
        u16 size = 0;
        Flags flags = { 0 };
    };

    struct Message
    {
    public:
        u64 timestampProcessed = 0; // Specifies the timestamp when the message was processed
        u16 networkSleepDiff = 0; // Specifies the time wasted sleeping in network thread
        u16 networkUpdateDiff = 0; // Specifies the time wasted in network update
        u16 timeToProcess = 0; // Specifies the time taken to process the message since the socket this message was received on started updating
        std::shared_ptr<Bytebuffer> buffer;
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
        Message message;
    };
}
