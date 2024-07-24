#pragma once
#include <Base/Types.h>

class Bytebuffer;

namespace Network
{

    enum class Opcode : u16
    {
        INVALID,
        CMSG_LOGON_CHALLENGE,
        SMSG_LOGON_CHALLENGE,
        CMSG_LOGON_HANDSHAKE,
        SMSG_LOGON_HANDSHAKE,
        CMSG_CONNECTED,
        SMSG_CONNECTED,
        MSG_REQUEST_ADDRESS,
        SMSG_SEND_ADDRESS,
        MSG_REQUEST_INTERNAL_SERVER_INFO,
        SMSG_SEND_FULL_INTERNAL_SERVER_INFO,
        SMSG_SEND_ADD_INTERNAL_SERVER_INFO,
        SMSG_SEND_REMOVE_INTERNAL_SERVER_INFO,

        CMSG_REQUEST_REALMLIST,
        SMSG_SEND_REALMLIST,
        CMSG_SELECT_REALM,
        SMSG_SEND_REALM_CONNECTION_INFO,
        SMSG_REQUEST_REALM_CONNECTION_INFO,

        SMSG_PLAYER_CREATE,
        SMSG_ENTITY_CREATE,
        SMSG_ENTITY_DESTROY,
        SMSG_ENTITY_UPDATE,
        MSG_ENTITY_MOVE,
        MSG_ENTITY_MOVE_HEARTBEAT,
        MSG_ENTITY_MOVE_STOP,
        CMSG_STORELOC,
        SMSG_STORELOC,
        CMSG_GOTO,

        SMSG_ENTITY_RESOURCES_UPDATE,
        SMSG_ENTITY_ATTRIBUTES_UPDATE,
        SMSG_ENTITY_RATINGS_UPDATE,
        SMSG_ENTITY_DISPLAYINFO_UPDATE,

        MSG_ENTITY_TARGET_UPDATE,

        CMSG_REQUEST_SPELLCAST,
        SMSG_SEND_SPELLCAST_RESULT,
        SMSG_ENTITY_CAST_SPELL,

        SMSG_COMBAT_EVENT,

        CMSG_CHEAT_DAMAGE,
        CMSG_CHEAT_KILL,
        CMSG_CHEAT_RESURRECT,
        CMSG_CHEAT_MORPH,
        CMSG_CHEAT_CREATE_CHARACTER,
        SMSG_CHEAT_CREATE_CHARACTER_RESULT,
        CMSG_CHEAT_DELETE_CHARACTER,
        SMSG_CHEAT_DELETE_CHARACTER_RESULT,

        MAX_COUNT
    };

    enum class ConnectionStatus : u8
    {
        AUTH_NONE,
        AUTH_CHALLENGE,
        AUTH_HANDSHAKE,
        AUTH_FAILED,
        AUTH_SUCCESS,
        CONNECTED,
    };

    enum class AddressType : u8
    {
        AUTH,
        REALM,
        WORLD,
        INSTANCE,
        CHAT,
        LOADBALANCE,
        REGION,
        COUNT,
        INVALID
    };

    enum class RegionType : u8
    {
        NA,
        SA,
        EU,
        OCEANIC,
        COUNT
    };

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
        Opcode opcode = Opcode::INVALID;
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