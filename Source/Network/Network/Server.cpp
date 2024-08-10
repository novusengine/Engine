#include "Server.h"
#include "Client.h"
#include "Util/DefineUtil.h"

#include "Base/Memory/Bytebuffer.h"
#include "Base/Util/DebugHandler.h"

#include <memory>
#include <chrono>
using namespace std::chrono_literals;

namespace Network
{
    void ServerSession::Start()
    {
        _readBuffer = Bytebuffer::Borrow<DEFAULT_BUFFER_SIZE>();
        _pongBuffer = Bytebuffer::Borrow<sizeof(MessageHeader)>();

        MessageHeader pongHeader;
        pongHeader.flags.isPing = 1;

        _pongBuffer->Put(pongHeader);

        ReadMessageHeader();
    }

    void ServerSession::Close()
    {
        if (!_socket.is_open())
            return;

        std::error_code ec;
        _socket.shutdown(asio::socket_base::shutdown_both, ec);
        if (!ec)
        {
            _socket.close(ec);
        }
    }

    void ServerSession::Write(const std::shared_ptr<Bytebuffer>& buffer)
    {
        asio::async_write(_socket, asio::buffer(buffer->GetDataPointer(), buffer->writtenData), [this](std::error_code ec, std::size_t length)
        {
            if (ec || !this)
            {
                NC_LOG_ERROR("Network::ServerSession : Write Failed ({0})", ec.message());
                CloseInternal();
            }
        });
    }

    void ServerSession::WriteBuffer(const BufferID bufferID, const std::shared_ptr<Bytebuffer>& buffer)
    {
        asio::async_write(_socket, asio::buffer(buffer->GetDataPointer(), buffer->writtenData), [this, bufferID](std::error_code ec, std::size_t length)
        {
            if (ec || !this)
            {
                NC_LOG_ERROR("Network::ServerSession : WriteBuffer Failed ({0})", ec.message());
                CloseInternal();
            }

            ClearBuffer(bufferID);
        });
    }

    void ServerSession::ReadMessageHeader()
    {
        asio::async_read(_socket, asio::buffer(_readBuffer->GetDataPointer(), sizeof(MessageHeader)), [this](std::error_code ec, std::size_t length)
        {
            if (ec || !this)
            {
                NC_LOG_ERROR("Network::ServerSession : ReadHeader Failed ({0})", ec.message());
                CloseInternal();
                return;
            }

            auto* header = reinterpret_cast<MessageHeader*>(_readBuffer->GetDataPointer());
            _readBuffer->SkipWrite(length);

            if (header->size > DEFAULT_BUFFER_SIZE - sizeof(MessageHeader))
            {
                NC_LOG_ERROR("Network::ServerSession : ReadHeader Failed (Message Size Too Large)");
                CloseInternal();
                return;
            }

            if (header->flags.isPing)
            {
                Write(_pongBuffer);
            }

            if (header->size > 0)
            {
                ReadMessageBody();
            }
            else
            {
                std::shared_ptr<Bytebuffer> messageBuffer = Bytebuffer::Borrow<sizeof(MessageHeader)>();
                {
                    messageBuffer->Put(*header);
                }

                // Emit Message Event
                EnqueueMessage(_socketID, messageBuffer);

                // Reset Read Buffer
                _readBuffer->Reset();

                ReadMessageHeader();
            }
        });
    }

    void ServerSession::ReadMessageBody()
    {
        NC_ASSERT(_readBuffer && _readBuffer->writtenData == sizeof(MessageHeader), "Network::ServerSession : ReadMessageBody Failed (Invalid Buffer)");

        u32 sizeToRead = reinterpret_cast<MessageHeader*>(_readBuffer->GetDataPointer())->size;
        asio::async_read(_socket, asio::buffer(_readBuffer->GetWritePointer(), sizeToRead), [this](std::error_code ec, std::size_t length)
        {
            if (ec || !this)
            {
                NC_LOG_ERROR("Network::ServerSession : ReadBody Failed ({0})", ec.message());
                CloseInternal();
                return;
            }

            auto* header = reinterpret_cast<MessageHeader*>(_readBuffer->GetDataPointer());
            _readBuffer->SkipRead(sizeof(MessageHeader));

            std::shared_ptr<Bytebuffer> messageBuffer = Bytebuffer::Borrow<DEFAULT_BUFFER_SIZE>();
            {
                messageBuffer->Put(*header);

                // Payload
                {
                    std::memcpy(messageBuffer->GetWritePointer(), _readBuffer->GetReadPointer(), header->size);
                    messageBuffer->SkipWrite(header->size);
                }
            }

            // Emit Message Event
            EnqueueMessage(_socketID, messageBuffer);

            // Reset Read Buffer
            _readBuffer->Reset();

            ReadMessageHeader();
        });
    }

    void ServerSession::ClearBuffer(BufferID bufferID)
    {
        _server->_activeBuffers.erase(bufferID);
    }

    void ServerSession::EnqueueMessage(SocketID socketID, std::shared_ptr<Bytebuffer>& buffer)
    {
        if (!_server)
            return;

        SocketMessageEvent messageEvent;
        messageEvent.socketID = _socketID;
        messageEvent.message.buffer = std::move(buffer);
        _server->_inMessageEvents.enqueue(messageEvent);
    }

    void ServerSession::CloseInternal()
    {
        u32 isClosing = _isClosing.fetch_add(1);
        if (isClosing || !_socket.is_open())
            return;

        Close();

        _server->DeferCloseSocketID(_socketID);
        u32 isHandleClosedRequestsMessageScheduled = _server->_handleClosedRequestsScheduled.fetch_add(1);
        if (isHandleClosedRequestsMessageScheduled == 0)
        {
            asio::post(_server->_asioContext, std::bind(&Server::ProcessDeferredCloseRequests, _server));
        }
    }

    Server::Server(u16 port) : _asioAcceptor(_asioContext, tcp::endpoint(tcp::v4(), port)), _asioSocket(_asioContext), _connectedEvents(1024), _disconnectedEvents(1024), _disconnectRequests(1024), _inMessageEvents(1024), _outMessageEvents(1024)
    {
        _connections.resize(512);
        _activeBuffers.reserve(1024);
    }

    bool Server::Start()
    {
        if (_asioThread != nullptr)
            return false;

        ListenForNewConnection();
        _asioThread = new std::thread([this]() { _asioContext.run(); });

        return true;
    }

    void Server::CloseSocketID(SocketID socketID)
    {
        if (socketID == SOCKET_ID_INVALID)
        {
            NC_LOG_ERROR("Network::Server : CloseSocketID Failed (Invalid SocketID)");
            return;
        }

        DeferCloseSocketID(socketID);
        u32 isHandleClosedRequestsMessageScheduled = _handleClosedRequestsScheduled.fetch_add(1);
        if (isHandleClosedRequestsMessageScheduled == 0)
        {
            asio::post(_asioContext, std::bind(&Server::ProcessDeferredCloseRequests, this));
        }
    }

    void Server::SendPacket(SocketID socketID, std::shared_ptr<Bytebuffer>& buffer)
    {
        if (socketID == SOCKET_ID_INVALID)
        {
            NC_LOG_ERROR("Network::Server : SendPacket Failed (Invalid SocketID)");
            return;
        }

        SocketMessageEvent messageEvent;
        messageEvent.socketID = socketID;
        messageEvent.message.buffer = buffer;
        _outMessageEvents.enqueue(messageEvent);

        u32 flushOutMessageEventsScheduled = _flushOutMessageEventsScheduled.fetch_add(1);
        if (flushOutMessageEventsScheduled == 0)
        {
            asio::post(_asioContext, std::bind(&Server::ProcessDeferredOutMessageRequests, this));
        }
    }

    SocketID Server::GetNextSocketID()
    {
        SocketID result = SOCKET_ID_INVALID;

        for (u32 i = 0; i < _connections.size(); i++)
        {
            Connection& connection = _connections[i];

            if (connection.client != nullptr)
                continue;

            // Set SocketID or Increment Version
            if (connection.id == SOCKET_ID_INVALID)
            {
                connection.id = static_cast<SocketID>(i);
            }
            else
            {
                u32 version = Util::DefineUtil::GetSocketIDVersion(connection.id) + 1;
                connection.id = Util::DefineUtil::CreateSocketID(version, i);
            }

            result = connection.id;
            break;
        }

        return result;
    }

    void Server::ListenForNewConnection()
    {
        _asioAcceptor.async_accept(_asioSocket, [this](std::error_code ec)
        {
            if (!ec)
            {
                SocketID socketID = GetNextSocketID();

                if (socketID != SOCKET_ID_INVALID)
                {
                    auto remoteEndpoint = _asioSocket.remote_endpoint();
                    auto address = remoteEndpoint.address();

                    // Add the new connection
                    u32 index = Util::DefineUtil::GetSocketIDValue(socketID);

                    // Set Receive/Send Buffer Size
                    _asioSocket.set_option(asio::socket_base::receive_buffer_size(DEFAULT_BUFFER_SIZE));
                    _asioSocket.set_option(asio::socket_base::send_buffer_size(DEFAULT_BUFFER_SIZE));

                    // Disable nagle's algorithm
                    _asioSocket.set_option(asio::ip::tcp::no_delay(true));

                    Connection& connection = _connections[index];
                    connection.client = std::make_shared<ServerSession>(this, socketID, std::move(_asioSocket));
                    connection.client->Start();

                    // Emit connected event
                    SocketConnectedEvent connectedEvent;
                    connectedEvent.socketID = socketID;
                    connectedEvent.connectionInfo.ipAddr = address.to_string();
                    connectedEvent.connectionInfo.port = remoteEndpoint.port();
                    _connectedEvents.enqueue(connectedEvent);
                }
                else
                {
                    NC_LOG_ERROR("Network::Server : New Connection Failed (No Available SocketID)");
                }

                // Schedule the next accept
                ListenForNewConnection();
            }
            else
            {
                NC_LOG_ERROR("Network::Server : New Connection Failed ({0})", ec.message());
            }
        });
    }

    void Server::ProcessDeferredOutMessageRequests()
    {
        SocketMessageEvent messageEvent;
        while (_outMessageEvents.try_dequeue(messageEvent))
        {
            SocketID socketID = messageEvent.socketID;
            std::shared_ptr<Bytebuffer>& buffer = messageEvent.message.buffer;

            u32 index = Util::DefineUtil::GetSocketIDValue(socketID);
            Connection& connection = _connections[index];

            if (!connection.client || connection.id != socketID)
            {
                NC_LOG_ERROR("Network::Server : ProcessDeferredOutMessageRequests Failed (Client Not Found)");
                continue;
            }

            const BufferID bufferID = _nextBufferID++;
            _activeBuffers[bufferID] = buffer;

            connection.client->WriteBuffer(bufferID, buffer);
        }

        _flushOutMessageEventsScheduled.store(0);
    }

    void Server::ProcessDeferredCloseRequests()
    {
        SocketDisconnectedEvent disconnectedEvent;
        while (_disconnectRequests.try_dequeue(disconnectedEvent))
        {
            SocketID socketID = disconnectedEvent.socketID;
            u32 index = Util::DefineUtil::GetSocketIDValue(socketID);
            Connection& connection = _connections[index];

            if (!connection.client && connection.id == socketID)
            {
                NC_LOG_ERROR("Network::Server : CloseSocketID Failed (Client Not Found)");
                continue;
            }

            connection.client->Close();
            connection.client = nullptr;

            // Emit disconnected event
            SocketDisconnectedEvent disconnectedEvent;
            disconnectedEvent.socketID = socketID;
            _disconnectedEvents.enqueue(disconnectedEvent);
        }

        _handleClosedRequestsScheduled.store(0);
    }

    void Server::DeferCloseSocketID(SocketID socketID)
    {
        _disconnectRequests.enqueue({ socketID });
    }
}