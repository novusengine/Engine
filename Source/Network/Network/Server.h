#pragma once
#include "Define.h"

#include "Base/Types.h"
#include "Base/Container/ConcurrentQueue.h"

#include <robinhood/robinhood.h>
#include <asio/asio.hpp>

#include <queue>

using asio::ip::tcp;

class Bytebuffer;

namespace Network
{
    class Server;
    class ServerSession : public std::enable_shared_from_this<ServerSession>
    {
    public:
        ServerSession(Server* server, SocketID socketID, tcp::socket socket) : _server(server), _socketID(socketID), _socket(std::move(socket)), _strand(asio::make_strand(_socket.get_executor())), _timer(_strand) { }

        void Start();
        bool RequestClose();

        void Send(std::shared_ptr<Bytebuffer> buffer);

    private:
        void Write();
        void Close();

        void ReadMessageHeader();
        void ReadMessageBody();

        void EnqueueMessage(SocketID socketID, std::shared_ptr<Bytebuffer>& buffer);
        void CloseInternal();

    private:
        SocketID _socketID = SOCKET_ID_INVALID;
        tcp::socket _socket;
        asio::strand<asio::any_io_executor> _strand;
        asio::steady_timer _timer;
        std::atomic<bool> _requestClose = false;

        std::queue<std::shared_ptr<Bytebuffer>> _msgQueue;
        std::shared_ptr<Bytebuffer> _readBuffer;

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
        void Update();

        void CloseSocketID(SocketID socketID);
        void SendPacket(SocketID socketID, std::shared_ptr<Bytebuffer>& buffer);

    public:
        moodycamel::ConcurrentQueue<SocketConnectedEvent>& GetConnectedEvents() { return _connectedEvents; };
        moodycamel::ConcurrentQueue<SocketDisconnectedEvent>& GetDisconnectedEvents() { return _disconnectedEvents; };
        moodycamel::ConcurrentQueue<SocketMessageEvent>& GetMessageEvents() { return _inMessageEvents; };

    private:
        SocketID GetNextSocketID();
        void ListenForNewConnection();

        void ProcessDeferredRequests();
        void DeferCloseSocketID(SocketID socketID);

    private:
        friend class ServerSession;

        asio::io_context _asioContext;
        tcp::acceptor _asioAcceptor;
        tcp::socket _asioSocket;
        std::thread* _asioThread = nullptr;

        std::vector<Connection> _connections;

        moodycamel::ConcurrentQueue<SocketConnectedEvent> _connectedEvents;
        moodycamel::ConcurrentQueue<SocketDisconnectedEvent> _disconnectedEvents;
        moodycamel::ConcurrentQueue<SocketDisconnectedEvent> _disconnectRequests;
        moodycamel::ConcurrentQueue<SocketMessageEvent> _inMessageEvents;
        moodycamel::ConcurrentQueue<SocketMessageEvent> _outMessageEvents;
    };
}