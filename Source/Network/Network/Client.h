#pragma once
#include "Define.h"
#include "Socket.h"

#include "Base/Types.h"
#include "Base/Memory/Bytebuffer.h"

#include <entt/fwd.hpp>

namespace Network
{
    class Client
    {
    public:
        Client();
        Socket::Result Init(Socket::Mode mode);
        Socket::Result Reinit();

        Socket::Result Connect(u32 host, u16 port);
        Socket::Result Connect(std::string& hostname, u16 port);
        Socket::Result Close();
        Socket::Result Send(void* data, size_t size);
        Socket::Result Send(std::shared_ptr<Bytebuffer> buffer);

        Socket::Result Read();
        Socket::Result Read(void* data, size_t size);

    public:
        bool IsConnected() { return _isConnected; }
        std::shared_ptr<Bytebuffer> GetReadBuffer() { return _socket->GetReadBuffer(); }
        std::shared_ptr<Socket> GetSocket() { return _socket; }

    private:
        Socket::Result Init(std::shared_ptr<Socket> socket);

    private:
        friend class Server;
        bool _isInitialized = false;
        bool _isConnected = false;

        std::shared_ptr<Socket> _socket = nullptr;
    };
}