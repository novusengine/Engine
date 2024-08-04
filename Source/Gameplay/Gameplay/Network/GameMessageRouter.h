#pragma once
#include "Define.h"
#include "Opcode.h"

#include "Base/Types.h"
#include "Network/Define.h"

#include <robinhood/robinhood.h>

#include <memory>

class Bytebuffer;

namespace Network
{
    typedef bool (*MessageHandlerFn)(SocketID, Message& message);

    struct GameMessageHandler
    {
        GameMessageHandler() { }
        GameMessageHandler(ConnectionStatus inStatus, u16 inSize, MessageHandlerFn inHandler) : status(inStatus), minSize(inSize), maxSize(inSize), handler(inHandler) { }
        GameMessageHandler(ConnectionStatus inStatus, u16 inMinSize, i16 inMaxSize, MessageHandlerFn inHandler) : status(inStatus), minSize(inMinSize), maxSize(inMaxSize), handler(inHandler) { }

        ConnectionStatus status = ConnectionStatus::None;
        u16 minSize = 0;
        i16 maxSize = 0;
        MessageHandlerFn handler = nullptr;
    };

    class GameMessageRouter
    {
    public:
        GameMessageRouter();

        void SetMessageHandler(GameOpcode opcode, GameMessageHandler&& handler);
        bool CallHandler(SocketID socketID, Message& message);

    private:
        GameMessageHandler _handlers[static_cast<OpcodeType>(GameOpcode::Count)];
    };
}