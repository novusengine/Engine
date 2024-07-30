#include "Client.h"

#include "Base/Util/DebugHandler.h"

#include <entt/entt.hpp>

namespace Network
{
    Client::Client() { }

    Socket::Result Client::Init(Socket::Mode mode)
    {
        if (_isInitialized == true)
        {
            if (mode == _socket->GetMode())
                return Reinit();

            return Socket::Result::ERROR_CLIENT_ALREADY_INITIALIZED;
        }

        Socket::Result result = Socket::Create(Socket::Type::CLIENT, mode, _socket);

        if (_socket)
        {
            _isInitialized = true;
        }

        return result;
    }

    Socket::Result Client::Reinit()
    {
        if (_isInitialized == false)
        {
            return Socket::Result::ERROR_CLIENT_UNINITIALIZED;
        }

        _socket->Close();

        _isConnected = false;
        _isInitialized = false;

        return Init(_socket->GetMode());
    }

    Socket::Result Client::Init(std::shared_ptr<Socket>& socket)
    {
        if (_isInitialized == true)
        {
            return Socket::Result::ERROR_CLIENT_ALREADY_INITIALIZED;
        }

        _isInitialized = true;
        _isConnected = true;
        _socket = socket;

        return Socket::Result::SUCCESS;
    }

    Socket::Result Client::Connect(u32 host, u16 port)
    {
        if (_isInitialized == false)
        {
            NC_LOG_ERROR("[Networking] Attempted to call Connect on uninitialized Client");
            return Socket::Result::ERROR_CLIENT_UNINITIALIZED;
        }

        if (_isConnected == true)
        {
            NC_LOG_ERROR("[Networking] Attempted to call Connect on connected Client");
            return Socket::Result::ERROR_CLIENT_ALREADY_CONNECTED;
        }

        Socket::Result result = _socket->Connect(host, port);
        if (result == Socket::Result::SUCCESS ||
            result == Socket::Result::ERROR_WOULD_BLOCK)
        {
            _isConnected = true;
        }

        return result;
    }

    Socket::Result Client::Connect(const char* hostname, u16 port)
    {
        if (_isInitialized == false)
        {
            return Socket::Result::ERROR_CLIENT_UNINITIALIZED;
        }

        if (_isConnected == true)
        {
            return Socket::Result::ERROR_CLIENT_ALREADY_CONNECTED;
        }

        Socket::Result result = _socket->Connect(hostname, port);
        if (result == Socket::Result::SUCCESS ||
            result == Socket::Result::ERROR_WOULD_BLOCK)
        {
            _isConnected = true;
        }

        return result;
    }

    Socket::Result Client::Connect(std::string& hostname, u16 port)
    {
        return Connect(hostname.c_str(), port);
    }

    Socket::Result Client::Close()
    {
        if (_isInitialized == false)
            return Socket::Result::ERROR_CLIENT_UNINITIALIZED;

        if (_isConnected == false)
            return Socket::Result::ERROR_CLIENT_UNCONNECTED;

        _isConnected = false;

        Socket::Result result = _socket->Close();
        return result;
    }

    Socket::Result Client::Send(void* data, size_t size)
    {
        if (_isInitialized == false)
        {
            return Socket::Result::ERROR_CLIENT_UNINITIALIZED;
        }

        if (_isConnected == false)
        {
            return Socket::Result::ERROR_CLIENT_UNCONNECTED;
        }

        Socket::Result result = _socket->Send(data, size);
        if (result != Socket::Result::SUCCESS &&
            result != Socket::Result::ERROR_WOULD_BLOCK)
        {
            Close();
        }

        return result;
    }

    Socket::Result Client::Send(std::shared_ptr<Bytebuffer>& buffer)
    {
        return Send(buffer->GetDataPointer(), buffer->writtenData);
    }

    Socket::Result Client::Read()
    {
        if (!_isConnected)
            return Socket::Result::ERROR_CLIENT_UNCONNECTED;

        Socket::Result result = _socket->Read();
        if (result != Socket::Result::SUCCESS &&
            result != Socket::Result::ERROR_WOULD_BLOCK)
        {
            Close();
        }

        return result;
    }

    Socket::Result Client::Read(void* data, size_t size)
    {
        if (!_isConnected)
            return Socket::Result::ERROR_CLIENT_UNCONNECTED;

        Socket::Result result = _socket->Read(data, size);
        if (result != Socket::Result::SUCCESS &&
            result != Socket::Result::ERROR_WOULD_BLOCK)
        {
            Close();
        }

        return result;
    }
}