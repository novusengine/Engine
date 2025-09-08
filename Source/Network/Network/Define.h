#pragma once
#include <Base/Types.h>

#include <Meta/Generated/Shared/NetworkEnum.h>

#include <memory>

class Bytebuffer;

namespace Network
{
    typedef u32 BufferID;
    typedef u16 OpcodeType;
    template<typename T>
    concept PacketConcept = requires(T t, Bytebuffer* buffer)
    {
        { std::decay_t<T>::PACKET_ID } -> std::convertible_to<u16>;
        
        { t.Serialize(buffer) } -> std::same_as<bool>;
        { t.Deserialize(buffer) } -> std::same_as<bool>;
        { t.GetSerializedSize() } -> std::same_as<u16>;
    };

    struct ConnectionInfo
    {
    public:
        std::string ipAddr;
        u16 port;
    };

    struct MessageHeader
    {
    public:
        OpcodeType opcode = 0;
        u16 size = 0;
    };

    struct Message
    {
    public:
        std::shared_ptr<Bytebuffer> buffer;
    };

    enum class SocketID : u32 {}; // 8 Bits for versioning, 24 bits for value
    constexpr SocketID SOCKET_ID_INVALID = static_cast<SocketID>(-1);
    constexpr u32 SOCKET_ID_BITS = sizeof(std::underlying_type<SocketID>::type) * 8;
    constexpr u32 SOCKET_ID_VERSION_BITS = 8;
    constexpr u32 SOCKET_ID_VALUE_BITS = SOCKET_ID_BITS - SOCKET_ID_VERSION_BITS;
    constexpr u32 SOCKET_ID_VERSION_MASK = (1 << SOCKET_ID_VERSION_BITS) - 1;
    constexpr u32 SOCKET_ID_VALUE_MASK = (1 << SOCKET_ID_VALUE_BITS) - 1;

    constexpr u32 DEFAULT_BUFFER_SIZE = 2048;

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
