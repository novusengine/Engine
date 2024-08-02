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
    Server::Server()
    {
        _connections.resize(512);
    }

    Socket::Result Server::Init(Socket::Mode mode, std::string& hostname, u16 port)
    {
        Socket::Result createResult = Socket::Create(Socket::Type::SERVER, mode, _socket);
        if (createResult != Socket::Result::SUCCESS)
        {
            return createResult;
        }

        _socket->SetBlockingState(false);

        _hostname = hostname;
        _port = port;

        Socket::Result bindResult = _socket->Bind(_hostname, port);
        if (bindResult != Socket::Result::SUCCESS)
        {
            return bindResult;
        }

        // Setup Threads
        {
            std::thread processThread = std::thread(&Server::Process, this);
            processThread.detach();
        }

        _isInitialized = true;

        return Socket::Result::SUCCESS;
    }

    Socket::Result Server::Stop()
    {
        if (_isStopped)
        {
            NC_LOG_ERROR("[Networking] Attempted to call Stop on an already closed Server");
            return Socket::Result::ERROR_SERVER_ALREADY_STOPPED;
        }

        _isStopped = true;

        for (Connection& connection : _connections)
        {
            connection.id = SOCKET_ID_INVALID;

            if (connection.client)
            {
                connection.client->Close();
                connection.client = nullptr;
            }
        }

        _activeSockets.clear();

        // Empty connected queue
        {
            SocketConnectedEvent connectedEvent;
            while (_connectedEvents.try_dequeue(connectedEvent)) {}
        }

        // Empty message queue
        {
            SocketMessageEvent messageEvent;
            while (_messageEvents.try_dequeue(messageEvent)) {}
        }

        Socket::Result result = _socket->Close();
        return result;
    }

    void Server::CloseSocketID(SocketID socketID)
    {
        SocketDisconnectedEvent disconnectedEvent;
        disconnectedEvent.socketID = socketID;

        _disconnectRequest.enqueue(disconnectedEvent);
    }

    void Server::SendPacket(SocketID socketID, std::shared_ptr<Bytebuffer>& buffer)
    {
        SocketMessageEvent request =
        {
            .socketID = socketID,
            .buffer = buffer
        };

        _messageRequests.enqueue(request);
    }

    Socket::Result Server::Accept(std::shared_ptr<Client> netClient)
    {
        if (_isInitialized == false)
        {
            NC_LOG_ERROR("[Networking] Attempted to call Accept on uninitialized Server");
            return Socket::Result::ERROR_SERVER_UNINITIALIZED;
        }

        std::shared_ptr<Socket> connection = nullptr;
        Socket::Result acceptResult = _socket->Accept(connection);
        if (acceptResult != Socket::Result::SUCCESS)
        {
            return acceptResult;
        }

        Socket::Result initResult = netClient->Init(connection);
        if (initResult != Socket::Result::SUCCESS)
        {
            return initResult;
        }

        netClient->GetSocket()->SetBlockingState(false);
        return Socket::Result::SUCCESS;
    }

    Socket::Result Server::Close(Connection& connection)
    {
        if (!connection.client)
        {
            return Socket::Result::ERROR_CLIENT_UNCONNECTED;
        }

        Socket::Result closeResult = connection.client->Close();
        if (closeResult != Socket::Result::SUCCESS &&
            closeResult != Socket::Result::ERROR_CLIENT_UNCONNECTED)
        {
            return closeResult;
        }

        SocketDisconnectedEvent disconnectedEvent;
        disconnectedEvent.socketID = connection.id;
        _disconnectedEvents.enqueue(disconnectedEvent);

        _activeSockets.erase(connection.id);
        connection.client = nullptr;

        return Socket::Result::SUCCESS;
    }

    void Server::Process()
    {
        while (!_isStopped)
        {
            std::shared_ptr<Client> netClient = std::make_shared<Client>();

            Socket::Result result = Accept(netClient);
            if (result == Socket::Result::SUCCESS)
            {
                SocketID socketID = GetNextSocketID();

                // Close Connection if we have too many already
                if (socketID == SOCKET_ID_INVALID)
                {
                    netClient->Close();
                }
                else
                {
                    u32 index = Util::DefineUtil::GetSocketIDValue(socketID);

                    Connection& connection = _connections[index];
                    connection.client = std::move(netClient);

                    {
                        SocketConnectedEvent connectedEvent;
                        connectedEvent.socketID = socketID;
                        connectedEvent.connectionInfo = connection.client->GetSocket()->GetConnectionInfo();

                        _connectedEvents.enqueue(connectedEvent);
                    }

                    _activeSockets.emplace(socketID);
                }
            }

            // Handle Socket Message Requests
            {
                SocketMessageEvent messageEvent;
                while (_messageRequests.try_dequeue(messageEvent))
                {
                    u32 index = Util::DefineUtil::GetSocketIDValue(messageEvent.socketID);

                    Connection& connection = _connections[index];
                    if (!connection.client)
                        continue;

                    u32 eventVersion = Util::DefineUtil::GetSocketIDVersion(messageEvent.socketID);
                    u32 connectionVersion = Util::DefineUtil::GetSocketIDVersion(connection.id);

                    if (eventVersion != connectionVersion)
                        continue;

                    connection.client->Send(messageEvent.buffer);
                }
            }

            // Handle Socket Disconnect Requests
            {
                SocketDisconnectedEvent disconnectedEvent;
                while (_disconnectRequest.try_dequeue(disconnectedEvent))
                {
                    u32 index = Util::DefineUtil::GetSocketIDValue(disconnectedEvent.socketID);

                    Connection& connection = _connections[index];
                    if (!connection.client)
                        continue;

                    u32 eventVersion = Util::DefineUtil::GetSocketIDVersion(disconnectedEvent.socketID);
                    u32 connectionVersion = Util::DefineUtil::GetSocketIDVersion(connection.id);

                    if (eventVersion != connectionVersion)
                        continue;

                    Close(connection);
                }
            }

            // Read Sockets
            for (const SocketID socketID : _activeSockets)
            {
                u32 index = Util::DefineUtil::GetSocketIDValue(socketID);

                Connection& connection = _connections[index];

                Socket::Result readResult = connection.client->Read();
                if (readResult == Socket::Result::SUCCESS)
                {
#ifdef NC_DEBUG
                    const ConnectionInfo& connectionInfo = connection.client->GetSocket()->GetConnectionInfo();
#endif // NC_DEBUG

                    std::shared_ptr<Bytebuffer>& buffer = connection.client->GetReadBuffer();
                    while (size_t activeSize = buffer->GetActiveSize())
                    {
                        static constexpr u8 PacketHeaderSize = sizeof(PacketHeader);

                        // We have received a partial header and need to read more
                        if (activeSize < PacketHeaderSize)
                        {
                            buffer->Normalize();
                            break;
                        }

                        PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer->GetReadPointer());

                        if (header->size > DEFAULT_BUFFER_SIZE - PacketHeaderSize)
                        {
#ifdef NC_DEBUG
                            NC_LOG_ERROR("Network : Received Invalid Opcode Size ({0} : {1}) from (SocketId : {2}, \"{3}:{4}\")", header->opcode, header->size, static_cast<u32>(socketID), connectionInfo.ipAddrStr, connectionInfo.port);
#endif // NC_DEBUG
                            CloseSocketID(connection.id);
                            break;
                        }

                        size_t receivedPayloadSize = activeSize - sizeof(PacketHeader);
                        if (receivedPayloadSize < header->size)
                        {
                            buffer->Normalize();
                            break;
                        }

                        buffer->SkipRead(sizeof(PacketHeader));

                        std::shared_ptr<Bytebuffer> messageBuffer = Bytebuffer::Borrow<DEFAULT_BUFFER_SIZE>();
                        {
                            // Payload
                            {
                                messageBuffer->Put(*header);

                                if (header->size)
                                {
                                    std::memcpy(messageBuffer->GetWritePointer(), buffer->GetReadPointer(), header->size);
                                    messageBuffer->SkipWrite(header->size);

                                    // Skip Payload
                                    buffer->SkipRead(header->size);
                                }
                            }

                            SocketMessageEvent messageEvent;
                            messageEvent.socketID = socketID;
                            messageEvent.buffer = std::move(messageBuffer);

                            _messageEvents.enqueue(messageEvent);
                        }
                    }

                    if (buffer->GetActiveSize() == 0)
                    {
                        buffer->Reset();
                    }
                }
                else if (readResult != Socket::Result::ERROR_WOULD_BLOCK)
                {
                    Close(connection);
                }
            }

            std::this_thread::sleep_for(1ms);
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
}