#pragma once
#include "Define.h"
#include "Socket.h"

#include "Base/Types.h"
#include "Base/Memory/Bytebuffer.h"
#include "Base/Memory/SharedPool.h"

namespace Network
{
    struct Packet
    {
    public:
        struct Header
        {
        public:
            Opcode opcode = Opcode::INVALID;
            u16 size = 0;
        };

        Header header;
        std::shared_ptr<Bytebuffer> payload = nullptr;

        static std::shared_ptr<Packet> Borrow()
        {
            std::shared_ptr<Packet> packet = _packets.acquireOrCreate();

            packet->header.opcode = Opcode::INVALID;
            packet->header.size = 0;

            if (packet->payload)
            {
                packet->payload = nullptr;
            }

            return packet;
        }

        static SharedPool<Packet> _packets;
    };
}