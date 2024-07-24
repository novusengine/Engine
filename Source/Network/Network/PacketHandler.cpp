#include "PacketHandler.h"
#include "Client.h"

namespace Network
{
    PacketHandler::PacketHandler() { }

    void PacketHandler::SetMessageHandler(Opcode opcode, OpcodeHandler handler)
    {
        handlers[static_cast<u16>(opcode)] = handler;
    }

    bool PacketHandler::CallHandler(SocketID socketID, std::shared_ptr<Bytebuffer>& packet)
    {
        Network::PacketHeader header;
        if (!packet->Get(header))
            return false;

        if (header.opcode <= Opcode::INVALID || header.opcode > Opcode::MAX_COUNT)
            return false;

        const OpcodeHandler& opcodeHandler = handlers[static_cast<u16>(header.opcode)];

        if (!opcodeHandler.handler || header.size < opcodeHandler.minSize || (header.size > opcodeHandler.maxSize && opcodeHandler.maxSize != -1))
            return false;

        return opcodeHandler.handler(socketID, packet);
    }
}