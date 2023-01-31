#pragma once
#include "Define.h"

#include "Base/Types.h"

#include <robinhood/robinhood.h>

#include <memory>

namespace Network
{
    struct Packet;
    typedef bool (*MessageHandlerFn)(SocketID, std::shared_ptr<Packet>);

    struct OpcodeHandler
    {
        OpcodeHandler() { }
        OpcodeHandler(ConnectionStatus inStatus, u16 inSize, MessageHandlerFn inHandler) : status(inStatus), minSize(inSize), maxSize(inSize), handler(inHandler) { }
        OpcodeHandler(ConnectionStatus inStatus, u16 inMinSize, i16 inMaxSize, MessageHandlerFn inHandler) : status(inStatus), minSize(inMinSize), maxSize(inMaxSize), handler(inHandler) { }

        ConnectionStatus status = ConnectionStatus::AUTH_NONE;
        u16 minSize = 0;
        i16 maxSize = 0;
        MessageHandlerFn handler = nullptr;
    };

    class PacketHandler
    {
    public:
        PacketHandler();

        void SetMessageHandler(Opcode opcode, OpcodeHandler handler);
        bool CallHandler(SocketID socketID, std::shared_ptr<Packet> packet);

    private:
        OpcodeHandler handlers[static_cast<u16>(Opcode::MAX_COUNT)];
    };
}