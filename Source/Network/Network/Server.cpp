#include "Server.h"
#include "Client.h"
#include "Util/DefineUtil.h"

#include <Base/Memory/Bytebuffer.h>
#include <Base/Util/DebugHandler.h>

#include <MetaGen/Shared/Packet/Packet.h>

#include <memory>
#include <chrono>
using namespace std::chrono_literals;

namespace Network
{
    void ServerSession::Start()
    {
        _readBuffer = Bytebuffer::Borrow<DEFAULT_BUFFER_SIZE>();

        ReadMessageHeader();
    }

    bool ServerSession::RequestClose()
    {
        if (_requestClose)
            return false; // Already requested close

        asio::post(_strand,
            [self = shared_from_this()]()
            {
                self->_requestClose = true;

                if (self->_msgQueue.empty())
                    self->Close();
            });

        return true;
    }

    void ServerSession::SetLaneID(u64 laneID)
    {
        asio::post(_strand, [self = shared_from_this(), laneID]() mutable
        {
            self->_laneID = laneID;
        });
    }

    void ServerSession::Send(std::shared_ptr<Bytebuffer> buffer)
    {
        asio::post(_strand,
            [self = shared_from_this(), msg = buffer]() mutable
            {
                if (self->_requestClose)
                    return; // Ignore if closing

                bool writeInProgress = !self->_msgQueue.empty();
                self->_msgQueue.push(msg);

                if (!writeInProgress)
                    self->Write();
            });
    }

    void ServerSession::Write()
    {
        auto& buffer = _msgQueue.front();
        asio::async_write(_socket, asio::buffer(buffer->GetDataPointer(), buffer->writtenData), asio::bind_executor(_strand, [self = shared_from_this()](std::error_code ec, std::size_t length)
        {
            if (ec || !self)
            {
                NC_LOG_ERROR("Network::ServerSession : Write Failed ({0})", ec.message());
                self->CloseInternal();
            }
            else
            {
                self->_msgQueue.pop();
        
                if (!self->_msgQueue.empty())
                {
                    self->Write();
                }
                else if (self->_requestClose)
                {
                    self->Close();
                }
            }
        }));
    }

    void ServerSession::Close()
    {
        if (!_socket.is_open())
            return;

        _timer.expires_after(100ms);
        _timer.async_wait(asio::bind_executor(_strand, [self = shared_from_this()](std::error_code ec)
        {
            std::error_code ignored_ec;
            self->_socket.shutdown(asio::socket_base::shutdown_send, ignored_ec);

            auto buf = std::make_shared<std::array<char, 1>>();
            self->_socket.async_read_some(asio::buffer(*buf),
                [self, buf](std::error_code ec, std::size_t size)
                {
                    std::error_code ignored_ec;
                    self->_socket.close(ignored_ec);
                });
        }));
    }

    void ServerSession::ReadMessageHeader()
    {
        asio::async_read(_socket, asio::buffer(_readBuffer->GetDataPointer(), sizeof(MessageHeader)), asio::bind_executor(_strand, [self = shared_from_this()](std::error_code ec, std::size_t length)
        {
            if (ec || !self)
            {
                NC_LOG_ERROR("Network::ServerSession : ReadHeader Failed ({0})", ec.message());
                self->CloseInternal();
                return;
            }
        
            auto* header = reinterpret_cast<MessageHeader*>(self->_readBuffer->GetDataPointer());
            self->_readBuffer->SkipWrite(length);
        
            if (header->size > DEFAULT_BUFFER_SIZE - sizeof(MessageHeader))
            {
                NC_LOG_ERROR("Network::ServerSession : ReadHeader Failed (Message Size Too Large)");
                self->CloseInternal();
                return;
            }
        
            if (header->opcode == MetaGen::Shared::Packet::ClientPingPacket::PACKET_ID)
            {
                auto pongBuffer = Bytebuffer::Borrow<sizeof(MessageHeader)>();
        
                MessageHeader pongHeader =
                {
                    .opcode = MetaGen::Shared::Packet::ServerPongPacket::PACKET_ID,
                    .size = 0
                };
                pongBuffer->Put(pongHeader);

                self->Send(pongBuffer);
            }
        
            if (header->size > 0)
            {
                self->ReadMessageBody();
            }
            else
            {
                std::shared_ptr<Bytebuffer> messageBuffer = Bytebuffer::Borrow<sizeof(MessageHeader)>();
                {
                    messageBuffer->Put(*header);
                }
        
                // Emit Message Event
                self->EnqueueMessage(self->_socketID, messageBuffer, self->_laneID);
        
                // Reset Read Buffer
                self->_readBuffer->Reset();
        
                self->ReadMessageHeader();
            }
        }));
    }

    void ServerSession::ReadMessageBody()
    {
        NC_ASSERT(_readBuffer && _readBuffer->writtenData == sizeof(MessageHeader), "Network::ServerSession : ReadMessageBody Failed (Invalid Buffer)");

        u32 sizeToRead = reinterpret_cast<MessageHeader*>(_readBuffer->GetDataPointer())->size;
        asio::async_read(_socket, asio::buffer(_readBuffer->GetWritePointer(), sizeToRead), asio::bind_executor(_strand, [self = shared_from_this()](std::error_code ec, std::size_t length)
        {
            if (ec || !self)
            {
                NC_LOG_ERROR("Network::ServerSession : ReadBody Failed ({0})", ec.message());
                self->CloseInternal();
                return;
            }
        
            auto* header = reinterpret_cast<MessageHeader*>(self->_readBuffer->GetDataPointer());
            self->_readBuffer->SkipRead(sizeof(MessageHeader));
        
            std::shared_ptr<Bytebuffer> messageBuffer = Bytebuffer::Borrow<DEFAULT_BUFFER_SIZE>();
            {
                messageBuffer->Put(*header);
        
                // Payload
                {
                    std::memcpy(messageBuffer->GetWritePointer(), self->_readBuffer->GetReadPointer(), header->size);
                    messageBuffer->SkipWrite(header->size);
                }
            }
        
            // Emit Message Event
            self->EnqueueMessage(self->_socketID, messageBuffer, self->_laneID);
        
            // Reset Read Buffer
            self->_readBuffer->Reset();
        
            self->ReadMessageHeader();
        }));
    }

    void ServerSession::EnqueueMessage(SocketID socketID, std::shared_ptr<Bytebuffer>& buffer, u64 laneID)
    {
        if (!_server)
            return;

        SocketMessageEvent messageEvent;
        messageEvent.socketID = _socketID;
        messageEvent.message.buffer = std::move(buffer);

        auto& laneInMessageQueue = _server->_laneToInMessageQueue.at(laneID);
        laneInMessageQueue.enqueue(messageEvent);
    }

    void ServerSession::CloseInternal()
    {
        if (!RequestClose())
            return; // Already requested close

        _server->DeferCloseSocketID(_socketID);
    }

    Server::Server(u16 port) : _asioAcceptor(_asioContext, tcp::endpoint(tcp::v4(), port)), _asioSocket(_asioContext), _connectedEvents(1024), _disconnectedEvents(1024), _disconnectRequests(1024), _changeLaneRequests(1024), _outMessageEvents(1024)
    {
        _connections.resize(512);

        _laneToInMessageQueue.reserve(8192);
        AddLane(DEFAULT_LANE_ID);
    }

    bool Server::Start()
    {
        if (_asioThread != nullptr)
            return false;

        ListenForNewConnection();
        _asioThread = new std::thread([this]() { _asioContext.run(); });

        return true;
    }

    void Server::Update()
    {
        asio::post(_asioContext, std::bind(&Server::ProcessDeferredRequests, this));
    }

    void Server::AddLane(u64 laneID)
    {
        if (_laneToInMessageQueue.contains(laneID))
            return;

        _laneToInMessageQueue[laneID] = moodycamel::ConcurrentQueue<SocketMessageEvent>(1024);
    }

    void Server::SetSocketIDLane(SocketID socketID, u64 laneID)
    {
        if (!_laneToInMessageQueue.contains(laneID))
            return;

        SocketChangeLaneEvent changeLaneEvent;
        changeLaneEvent.socketID = socketID;
        changeLaneEvent.laneID = laneID;

        _changeLaneRequests.enqueue(changeLaneEvent);
    }

    void Server::CloseSocketID(SocketID socketID)
    {
        if (socketID == SOCKET_ID_INVALID)
        {
            NC_LOG_ERROR("Network::Server : CloseSocketID Failed (Invalid SocketID)");
            return;
        }

        DeferCloseSocketID(socketID);
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

    void Server::ProcessDeferredRequests()
    {
        SocketMessageEvent messageEvent;
        while (_outMessageEvents.try_dequeue(messageEvent))
        {
            SocketID socketID = messageEvent.socketID;

            u32 index = Util::DefineUtil::GetSocketIDValue(socketID);
            Connection& connection = _connections[index];

            if (!connection.client || connection.id != socketID)
            {
                NC_LOG_ERROR("Network::Server : ProcessDeferred OutMessageEvent Failed (Client Not Found)");
                continue;
            }

            connection.client->Send(messageEvent.message.buffer);
        }

        SocketDisconnectedEvent disconnectedEvent;
        while (_disconnectRequests.try_dequeue(disconnectedEvent))
        {
            SocketID socketID = disconnectedEvent.socketID;
            u32 index = Util::DefineUtil::GetSocketIDValue(socketID);
            Connection& connection = _connections[index];

            if (!connection.client && connection.id == socketID)
            {
                NC_LOG_ERROR("Network::Server : ProcessDeferred DisconnectedEvent Failed (Client Not Found)");
                continue;
            }

            connection.client->RequestClose();
            connection.client = nullptr;

            // Emit disconnected event
            SocketDisconnectedEvent disconnectedEvent;
            disconnectedEvent.socketID = socketID;
            _disconnectedEvents.enqueue(disconnectedEvent);
        }

        SocketChangeLaneEvent changeLaneEvent;
        while (_changeLaneRequests.try_dequeue(changeLaneEvent))
        {
            SocketID socketID = messageEvent.socketID;

            u32 index = Util::DefineUtil::GetSocketIDValue(socketID);
            Connection& connection = _connections[index];

            if (!connection.client || connection.id != socketID)
            {
                NC_LOG_ERROR("Network::Server : ProcessDeferred ChangeLaneEvent Failed (Client Not Found)");
                continue;
            }

            connection.client->SetLaneID(changeLaneEvent.laneID);
        }
    }

    void Server::DeferCloseSocketID(SocketID socketID)
    {
        _disconnectRequests.enqueue({ socketID });
    }
}