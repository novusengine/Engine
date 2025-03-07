#include "GameMessageRouter.h"

#include "Base/Memory/Bytebuffer.h"

namespace Network
{
    GameMessageRouter::GameMessageRouter() { }

    void GameMessageRouter::SetMessageHandler(GameOpcode opcode, GameMessageHandler&& handler)
    {
        _handlers[static_cast<u16>(opcode)] = std::move(handler);
    }

    bool GameMessageRouter::GetMessageHeader(Message& message, Network::MessageHeader& header) const
    {
        if (!message.buffer->Get(header))
            return false;

        GameOpcode opcode = header.flags.isPing ? GameOpcode::Shared_Ping : static_cast<GameOpcode>(header.opcode);
        if (opcode == GameOpcode::Invalid || opcode >= GameOpcode::Count)
            return false;

        header.opcode = static_cast<u16>(opcode);
        return true;
    }

    bool GameMessageRouter::HasValidHandlerForHeader(const Network::MessageHeader& header) const
    {
        const GameMessageHandler& opcodeHandler = _handlers[static_cast<u16>(header.opcode)];

        if (!opcodeHandler.handler)
            return false;

        bool isSmallerThanMinSize = header.size < opcodeHandler.minSize;
        bool isLargerThanMaxSize = header.size > opcodeHandler.maxSize && opcodeHandler.maxSize != -1;
        if (isSmallerThanMinSize || isLargerThanMaxSize)
            return false;

        return true;
    }

    bool GameMessageRouter::CallHandler(SocketID socketID, Network::MessageHeader& header, Message& message)
    {
        const GameMessageHandler& opcodeHandler = _handlers[static_cast<u16>(header.opcode)];
        return opcodeHandler.handler(socketID, message);
    }
}