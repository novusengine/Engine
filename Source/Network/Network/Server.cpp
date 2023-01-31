#include "Server.h"
#include "Client.h"
#include "Packet.h"
#include "Util/DefineUtil.h"

#include "Base/Memory/Bytebuffer.h"
#include "Base/Util/DebugHandler.h"

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
            DebugHandler::PrintError("[Networking] Attempted to call Stop on an already closed Server");
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

    void Server::SendPacket(SocketMessageEvent& messageRequest)
    {
        _messageRequests.enqueue(messageRequest);
    }

    Socket::Result Server::Accept(std::shared_ptr<Client> netClient)
    {
        if (_isInitialized == false)
        {
            DebugHandler::PrintError("[Networking] Attempted to call Accept on uninitialized Server");
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

            // Handle Socket Disconnect Requests
            {
                SocketDisconnectedEvent disconnectedEvent;
                while (_disconnectRequest.try_dequeue(disconnectedEvent))
                {
                    u32 index = Util::DefineUtil::GetSocketIDValue(disconnectedEvent.socketID);

                    Connection& connection = _connections[index];

                    if (connection.client)
                    {
                        Close(connection);
                    }
                }
            }

            // Handle Socket Message Requests
            {
                SocketMessageEvent messageEvent;
                while (_messageRequests.try_dequeue(messageEvent))
                {
                    u32 index = Util::DefineUtil::GetSocketIDValue(messageEvent.socketID);

                    Connection& connection = _connections[index];

                    if (connection.client)
                    {
                        connection.client->Send(messageEvent.packet->payload);
                    }
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
#ifdef NC_Debug
                    const ConnectionInfo& connectionInfo = connection.client->GetSocket()->GetConnectionInfo();
#endif // NC_Debug

                    std::shared_ptr<Bytebuffer>& buffer = connection.client->GetReadBuffer();
                    while (size_t activeSize = buffer->GetActiveSize())
                    {
                        // We have received a partial header and need to read more
                        if (activeSize < sizeof(Packet::Header))
                        {
                            buffer->Normalize();
                            break;
                        }

                        Packet::Header* header = reinterpret_cast<Packet::Header*>(buffer->GetReadPointer());

                        if (header->opcode == Opcode::INVALID || header->opcode > Opcode::MAX_COUNT)
                        {
#ifdef NC_Debug
                            DebugHandler::PrintError("Network : Received Invalid Opcode ({0}) from (SocketId : {1}, \"{2}:{3}\")", static_cast<std::underlying_type<Opcode>::type>(header->opcode), static_cast<u32>(socketID), connectionInfo.ipAddrStr, connectionInfo.port);
#endif // NC_Debug
                            CloseSocketID(connection.id);
                            break;
                        }

                        if (header->size > DEFAULT_BUFFER_SIZE)
                        {
#ifdef NC_Debug
                            DebugHandler::PrintError("Network : Received Invalid Opcode Size ({0} : {1}) from (SocketId : {2}, \"{3}:{4}\")", static_cast<std::underlying_type<Opcode>::type>(header->opcode), header->size, static_cast<u32>(socketID), connectionInfo.ipAddrStr, connectionInfo.port);
#endif // NC_Debug
                            CloseSocketID(connection.id);
                            break;
                        }

                        size_t receivedPayloadSize = activeSize - sizeof(Packet::Header);
                        if (receivedPayloadSize < header->size)
                        {
                            buffer->Normalize();
                            break;
                        }

                        buffer->SkipRead(sizeof(Packet::Header));

                        std::shared_ptr<Packet> packet = Packet::Borrow();
                        {
                            // Header
                            {
                                packet->header = *header;
                            }

                            // Payload
                            {
                                if (packet->header.size)
                                {
                                    packet->payload = Bytebuffer::Borrow<DEFAULT_BUFFER_SIZE>();
                                    packet->payload->size = packet->header.size;
                                    packet->payload->writtenData = packet->header.size;
                                    std::memcpy(packet->payload->GetDataPointer(), buffer->GetReadPointer(), packet->header.size);

                                    // Skip Payload
                                    buffer->SkipRead(header->size);
                                }
                            }

                            SocketMessageEvent messageEvent;
                            messageEvent.socketID = socketID;
                            messageEvent.packet = std::move(packet);

                            _messageEvents.enqueue(messageEvent);
                        }
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