#pragma once
#include "Define.h"

#include "Base/Types.h"
#include "Base/Container/ConcurrentQueue.h"
#include "Base/Memory/Bytebuffer.h"
#include "Base/Util/DebugHandler.h"

#include <asio/asio.hpp>
#include <entt/fwd.hpp>
#include <robinhood/robinhood.h>

namespace Network
{
    class Client
    {
    public:
        Client(asio::io_context& asioContext, std::shared_ptr<asio::ip::tcp::resolver>& resolver);

        void Stop();

        bool Connect(const char* address, u16 port);
        void Send(std::shared_ptr<Bytebuffer>& buffer);

        bool IsConnected() { return _socket != nullptr; }

        moodycamel::ConcurrentQueue<SocketMessageEvent>& GetMessageEvents() { return _messageEvents; };

    private:
        void ReadMessageHeader();
        void ReadMessageBody();

        void EnqueueMessage(std::shared_ptr<Bytebuffer>& buffer);
        void ClearBuffer(BufferID bufferID);

    private:
        asio::io_context& _asioContext;
        std::shared_ptr<asio::ip::tcp::socket> _socket = nullptr;
        std::shared_ptr<asio::ip::tcp::resolver> _resolver = nullptr;

        BufferID _nextBufferID = 0;
        robin_hood::unordered_map<BufferID, std::shared_ptr<Bytebuffer>> _activeBuffers;

        std::shared_ptr<Bytebuffer> _readBuffer;
        moodycamel::ConcurrentQueue<SocketMessageEvent> _messageEvents;
    };
}