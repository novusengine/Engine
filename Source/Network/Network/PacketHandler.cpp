#include "PacketHandler.h"
#include "Client.h"
#include "Packet.h"

namespace Network
{
    PacketHandler::PacketHandler() { }

    void PacketHandler::SetMessageHandler(Opcode opcode, OpcodeHandler handler)
    {
        handlers[static_cast<u16>(opcode)] = handler;
    }

    bool PacketHandler::CallHandler(SocketID socketID, std::shared_ptr<Packet> packet)
    {
        if (packet->header.opcode <= Opcode::INVALID || packet->header.opcode > Opcode::MAX_COUNT)
            return false;

        const OpcodeHandler& opcodeHandler = handlers[static_cast<u16>(packet->header.opcode)];

        if (!opcodeHandler.handler || packet->header.size < opcodeHandler.minSize || (packet->header.size > opcodeHandler.maxSize && opcodeHandler.maxSize != -1))
            return false;

        return opcodeHandler.handler(socketID, packet);
    }
}