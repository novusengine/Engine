#include "GameMessageRouter.h"

#include "Base/Memory/Bytebuffer.h"

namespace Network
{
    GameMessageRouter::GameMessageRouter() { }

    void GameMessageRouter::SetMessageHandler(GameOpcode opcode, GameMessageHandler&& handler)
    {
        _handlers[static_cast<u16>(opcode)] = std::move(handler);
    }

    bool GameMessageRouter::CallHandler(SocketID socketID, Message& message)
    {
        Network::MessageHeader header;
        if (!message.buffer->Get(header))
            return false;

        GameOpcode opcode = static_cast<GameOpcode>(header.opcode);
        if (header.flags.isPing)
            opcode = GameOpcode::Shared_Ping;

        if (opcode == GameOpcode::Invalid || opcode >= GameOpcode::Count)
            return false;

        const GameMessageHandler& opcodeHandler = _handlers[static_cast<u16>(opcode)];
        bool isSmallerThanMinSize = header.size < opcodeHandler.minSize;
        bool isLargerThanMaxSize = header.size > opcodeHandler.maxSize && opcodeHandler.maxSize != -1;

        if (!opcodeHandler.handler || isSmallerThanMinSize || isLargerThanMaxSize)
            return false;

        return opcodeHandler.handler(socketID, message);
    }
}