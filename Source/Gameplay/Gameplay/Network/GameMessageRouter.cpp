#include "GameMessageRouter.h"

#include <Base/Memory/Bytebuffer.h>

#include <Meta/Generated/Shared/NetworkPacket.h>

namespace Network
{
    GameMessageRouter::GameMessageRouter() { }

    void GameMessageRouter::SetMessageHandler(OpcodeType opcode, GameMessageHandler&& handler)
    {
        _handlers[opcode] = std::move(handler);
    }

    bool GameMessageRouter::GetMessageHeader(Message& message, Network::MessageHeader& header) const
    {
        if (!message.buffer->Get(header))
            return false;

        if (header.opcode == Generated::InvalidPacket::PACKET_ID || header.opcode >= 1024)
            return false;

        return true;
    }

    bool GameMessageRouter::HasValidHandlerForHeader(const Network::MessageHeader& header) const
    {
        const GameMessageHandler& opcodeHandler = _handlers[header.opcode];

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
        const GameMessageHandler& opcodeHandler = _handlers[header.opcode];
        return opcodeHandler.handler(socketID, message);
    }
}