#pragma once
#include "Define.h"

#include <Base/Types.h>
#include <Base/FunctionTraits.h>
#include <Base/Memory/Bytebuffer.h>

#include <Network/Define.h>

#include <robinhood/robinhood.h>

#include <memory>

class Bytebuffer;

namespace Network
{
    using MessageHandlerFn = std::function<bool(SocketID, Message& message)>;
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

        void SetMessageHandler(OpcodeType opcode, GameMessageHandler&& handler);

        template <typename PacketHandler>
        void RegisterPacketHandler(ConnectionStatus connectionStatus, PacketHandler callback)
        {
            using PacketStruct = std::decay_t<std::tuple_element_t<1, function_args_t<PacketHandler>>>;
            static_assert(std::is_invocable_r_v<bool, PacketHandler, SocketID, PacketStruct&>, "GameMessageHandler - The Callback provided to 'RegisterPacketHandler' must return a bool and take 2 parameters (SocketID, T&)");
            static_assert(PacketConcept<PacketStruct>, "GameMessageHandler::RegisterPacketHandler PacketStruct Parameter in the provided callback does not satisfy PacketConcept requirements");

            GameMessageHandler& handler = _handlers[static_cast<OpcodeType>(PacketStruct::PACKET_ID)];
            
            handler.status = connectionStatus;
            handler.minSize = 0;
            handler.maxSize = -1;
            handler.handler = [callback](SocketID socketID, Message& message) -> bool
            {
                PacketStruct packet;
                if (!message.buffer->Deserialize(packet))
                    return false;

                return callback(socketID, packet);
            };
        }

        template <PacketConcept PacketStruct>
        void UnregisterPacketHandler(OpcodeType opcode)
        {
            auto opcode = static_cast<OpcodeType>(PacketStruct::PACKET_ID);

            GameMessageHandler& handler = _handlers[opcode];
            handler.status = ConnectionStatus::None;
            handler.minSize = 0;
            handler.maxSize = 0;
            handler.handler = nullptr;
        }

        bool GetMessageHeader(Message& message, Network::MessageHeader& header) const;
        bool HasValidHandlerForHeader(const Network::MessageHeader& header) const;
        bool CallHandler(SocketID socketID, Network::MessageHeader& header, Message& message);

    private:
        GameMessageHandler _handlers[1024u];
    };
}