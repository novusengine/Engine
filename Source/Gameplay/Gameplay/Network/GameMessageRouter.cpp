#include "GameMessageRouter.h"

#include "Base/Memory/Bytebuffer.h"

namespace Network
{
    GameMessageRouter::GameMessageRouter() { }

    void GameMessageRouter::SetMessageHandler(GameOpcode opcode, GameMessageHandler&& handler)
    {
        _handlers[static_cast<u16>(opcode)] = std::move(handler);
    }

    bool GameMessageRouter::CallHandler(SocketID socketID, std::shared_ptr<Bytebuffer>& packet)
    {
        Network::PacketHeader header;
        if (!packet->Get(header))
            return false;

        GameOpcode opcode = static_cast<GameOpcode>(header.opcode);
        if (opcode == GameOpcode::Invalid || opcode >= GameOpcode::Count)
            return false;

        const GameMessageHandler& opcodeHandler = _handlers[static_cast<u16>(header.opcode)];
        bool isSmallerThanMinSize = header.size < opcodeHandler.minSize;
        bool isLargerThanMaxSize = header.size > opcodeHandler.maxSize && opcodeHandler.maxSize != -1;

        if (!opcodeHandler.handler || isSmallerThanMinSize || isLargerThanMaxSize)
            return false;

        return opcodeHandler.handler(socketID, packet);
    }
}