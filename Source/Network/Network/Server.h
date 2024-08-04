#pragma once
#include "Define.h"
#include "Socket.h"

#include "Base/Types.h"
#include "Base/Container/ConcurrentQueue.h"

#include <robinhood/robinhood.h>

namespace Network
{
    class Client;
    class Server
    {
    public:
        struct Connection
        {
        public:
            SocketID id = SOCKET_ID_INVALID;
            std::shared_ptr<Client> client = nullptr;
        };

        Server();

        Socket::Result Init(Socket::Mode mode, std::string& hostname, u16 port);
        Socket::Result Stop();

        void CloseSocketID(SocketID socketID);
        void SendPacket(SocketID socketID, std::shared_ptr<Bytebuffer>& buffer);

    public:
        const std::string& GetHostname() { return _hostname; }
        u16 GetPort() { return _port; }
        std::shared_ptr<Socket> GetSocket() { return _socket; }
        moodycamel::ConcurrentQueue<SocketConnectedEvent>& GetConnectedEvents() { return _connectedEvents; };
        moodycamel::ConcurrentQueue<SocketDisconnectedEvent>& GetDisconnectedEvents() { return _disconnectedEvents; };
        moodycamel::ConcurrentQueue<SocketMessageEvent>& GetMessageEvents() { return _messageEvents; };

    private:
        Socket::Result Accept(std::shared_ptr<Client> netClient);
        Socket::Result Close(Connection& connection);

        void Process();
        SocketID GetNextSocketID();

    private:
        bool _isInitialized = false;
        std::atomic<bool> _isStopped = false;
        std::string _hostname = "";
        u16 _port = 0;

        u64 _timeSinceLastUpdateFinish = 0;

        std::shared_ptr<Socket> _socket = nullptr;

        std::vector<Connection> _connections;
        robin_hood::unordered_set<SocketID> _activeSockets;

        moodycamel::ConcurrentQueue<SocketConnectedEvent> _connectedEvents;
        moodycamel::ConcurrentQueue<SocketDisconnectedEvent> _disconnectedEvents;
        moodycamel::ConcurrentQueue<SocketDisconnectedEvent> _disconnectRequest;
        moodycamel::ConcurrentQueue<SocketMessageEvent> _messageEvents;
        moodycamel::ConcurrentQueue<SocketMessageEvent> _messageRequests;
    };
}