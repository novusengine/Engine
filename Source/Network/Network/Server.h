#pragma once
#include "Define.h"

#include "Base/Types.h"
#include "Base/Container/ConcurrentQueue.h"

#include <robinhood/robinhood.h>
#include <asio/asio.hpp>

using asio::ip::tcp;

class Bytebuffer;

namespace Network
{
    class Server;
    class ServerSession : public std::enable_shared_from_this<ServerSession>
    {
    public:
        ServerSession(Server* server, SocketID socketID, tcp::socket socket) : _server(server), _socketID(socketID), _socket(std::move(socket)) { }

        void Start();
        void Close();

        void Write(const std::shared_ptr<Bytebuffer>& buffer);
        void WriteBuffer(const BufferID bufferID, const std::shared_ptr<Bytebuffer>& buffer);

    private:
        void ReadMessageHeader();
        void ReadMessageBody();

        void ClearBuffer(BufferID bufferID);
        void EnqueueMessage(SocketID socketID, std::shared_ptr<Bytebuffer>& buffer);
        void CloseInternal();

    private:
        SocketID _socketID = SOCKET_ID_INVALID;
        tcp::socket _socket;
        std::atomic<u32> _isClosing = 0;

        std::shared_ptr<Bytebuffer> _readBuffer;
        std::shared_ptr<Bytebuffer> _pongBuffer;

        Server* _server = nullptr;
    };

    class Client;
    class Server
    {
    public:
        struct Connection
        {
        public:
            SocketID id = SOCKET_ID_INVALID;
            std::shared_ptr<ServerSession> client = nullptr;
        };

    public:
        Server(u16 port);

        bool Start();

        void CloseSocketID(SocketID socketID);
        void SendPacket(SocketID socketID, std::shared_ptr<Bytebuffer>& buffer);

    public:
        moodycamel::ConcurrentQueue<SocketConnectedEvent>& GetConnectedEvents() { return _connectedEvents; };
        moodycamel::ConcurrentQueue<SocketDisconnectedEvent>& GetDisconnectedEvents() { return _disconnectedEvents; };
        moodycamel::ConcurrentQueue<SocketMessageEvent>& GetMessageEvents() { return _inMessageEvents; };

    private:
        SocketID GetNextSocketID();
        void ListenForNewConnection();

        void ProcessDeferredOutMessageRequests();
        void ProcessDeferredCloseRequests();
        void DeferCloseSocketID(SocketID socketID);

    private:
        friend class ServerSession;

        asio::io_context _asioContext;
        tcp::acceptor _asioAcceptor;
        tcp::socket _asioSocket;
        std::thread* _asioThread = nullptr;
        std::atomic<u32> _flushOutMessageEventsScheduled = false;
        std::atomic<u32> _handleClosedRequestsScheduled = false;

        std::vector<Connection> _connections;

        BufferID _nextBufferID = 0;
        robin_hood::unordered_map<BufferID, std::shared_ptr<Bytebuffer>> _activeBuffers;

        moodycamel::ConcurrentQueue<SocketConnectedEvent> _connectedEvents;
        moodycamel::ConcurrentQueue<SocketDisconnectedEvent> _disconnectedEvents;
        moodycamel::ConcurrentQueue<SocketDisconnectedEvent> _disconnectRequests;
        moodycamel::ConcurrentQueue<SocketMessageEvent> _inMessageEvents;
        moodycamel::ConcurrentQueue<SocketMessageEvent> _outMessageEvents;
    };
}