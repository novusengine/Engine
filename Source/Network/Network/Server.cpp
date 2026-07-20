#include "Server.h"
#include "Client.h"
#include "Util/DefineUtil.h"

#include <Base/Memory/Bytebuffer.h>
#include <Base/Util/DebugHandler.h>

#include <MetaGen/Shared/Packet/Packet.h>

#include <algorithm>
#include <chrono>
#include <future>
#include <memory>
using namespace std::chrono_literals;

namespace Network
{
    bool TryReplaceCoalescedPacket(SocketPacketEvent& existingPacket, SocketPacketEvent&& replacementPacket)
    {
        if (!existingPacket.queueReservation.HasSessionReservation())
        {
            existingPacket = std::move(replacementPacket);
            return true;
        }

        if (!existingPacket.queueReservation.RetargetSessionReservation(replacementPacket.queueReservation))
            return false;

        existingPacket = std::move(replacementPacket);
        return true;
    }

    ServerSession::ServerSession(Server* server, SocketID socketID, tcp::socket socket, SocketMessageQueuePtr laneQueue, std::shared_ptr<PacketQueueBudget> inboundQueueBudget, const ServerConfig& config)
        : _socketID(socketID), _socket(std::move(socket)), _strand(asio::make_strand(_socket.get_executor())), _readTimer(_strand), _closeTimer(_strand), _laneQueue(std::move(laneQueue)), _inboundQueueBudget(std::move(inboundQueueBudget)), _sessionInboundQueueBudget(std::make_shared<PacketQueueBudget>(config.maxSessionInboundQueueBytes, config.maxSessionInboundQueueEvents)), _sessionOutboundQueueBudget(std::make_shared<PacketQueueBudget>(config.maxSessionQueueBytes, config.maxSessionQueueEvents, config.criticalSessionQueueReserveBytes, config.criticalSessionQueueReserveEvents)), _server(server)
    {
    }

    void ServerSession::Start()
    {
        ReadMessageHeader();
    }

    bool ServerSession::RequestClose()
    {
        bool expected = false;
        if (!_requestClose.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
            return false; // Already requested close

        asio::post(_strand,
            [self = shared_from_this()]()
            {
                self->BeginClose();
            });

        return true;
    }

    void ServerSession::SetLaneQueue(SocketMessageQueuePtr laneQueue)
    {
        _laneQueue = std::move(laneQueue);
    }

    void ServerSession::QueuePacketFromServer(SocketPacketEvent&& packet)
    {
        // Server::ProcessDeferredRequests and all session strands run on the server's single ASIO worker thread.
        if (_server == nullptr || _closed || _requestClose.load(std::memory_order_acquire))
            return;

        if (_server->_config.replicationCoalescingEnabled && packet.options.CanCoalesce())
        {
            for (auto it = _pendingPacketBatch.rbegin(); it != _pendingPacketBatch.rend(); ++it)
            {
                if (it->options.coalesceKey != packet.options.coalesceKey)
                    continue;

                if (TryReplaceCoalescedPacket(*it, std::move(packet)))
                {
                    _server->_replicationPacketCoalesceCount.fetch_add(1, std::memory_order_relaxed);
                    return;
                }

                HandleSessionQueuePressure(packet);
                return;
            }
        }

        if (!TryReserveSessionQueue(packet))
        {
            HandleSessionQueuePressure(packet);
            return;
        }

        _pendingPacketBatch.push_back(std::move(packet));
        size_t pendingPacketBatchHighWater = _server->_pendingPacketBatchHighWater.load(std::memory_order_relaxed);
        while (_pendingPacketBatch.size() > pendingPacketBatchHighWater && !_server->_pendingPacketBatchHighWater.compare_exchange_weak(pendingPacketBatchHighWater, _pendingPacketBatch.size(), std::memory_order_relaxed, std::memory_order_relaxed))
        {
        }
        if (_pendingPacketBatchScheduled)
            return;

        _pendingPacketBatchScheduled = true;
        asio::post(_strand,
            [self = shared_from_this()]()
            {
                self->FlushPendingPackets();
            });
    }

    void ServerSession::Write()
    {
        if (_closed || _writeInProgress || _msgQueue.empty())
            return;

        _writeInProgress = true;
        _server->_writeCount.fetch_add(1, std::memory_order_relaxed);
        SocketPacketEvent& packet = _msgQueue.front();
        asio::async_write(_socket, asio::buffer(packet.GetDataPointer(), packet.GetWrittenData()), asio::bind_executor(_strand, [self = shared_from_this()](std::error_code ec, std::size_t)
        {
            self->_writeInProgress = false;
            if (ec)
            {
                if (!self->_closed && !self->_requestClose.load(std::memory_order_acquire) && ec != asio::error::operation_aborted)
                    NC_LOG_ERROR("Network::ServerSession : Write Failed ({0})", ec.message());

                if (self->_closed)
                {
                    self->_msgQueue.clear();
                }
                else if (self->_requestClose.load(std::memory_order_acquire))
                {
                    self->CloseNow();
                }
                else
                {
                    self->CloseInternal();
                }
                return;
            }

            self->_msgQueue.pop_front();
            if (!self->_msgQueue.empty())
            {
                self->Write();
            }
            else if (self->_requestClose.load(std::memory_order_acquire))
            {
                self->BeginClose();
            }
        }));
    }

    void ServerSession::BeginClose()
    {
        if (_closed)
            return;

        if (_closeStarted)
        {
            if (!_writeInProgress && _msgQueue.empty())
                CloseNow();
            return;
        }

        _closeStarted = true;
        CancelReadTimeout();
        _pendingPacketBatch.clear();
        _pendingPacketBatchScheduled = false;

        if (!_msgQueue.empty() && !_writeInProgress)
            Write();

        if (!_writeInProgress)
        {
            CloseNow();
            return;
        }

        const u32 closeDrainMilliseconds = _server != nullptr ? _server->_config.sessionCloseDrainMilliseconds : 0;
        if (closeDrainMilliseconds == 0)
        {
            CloseNow();
            return;
        }

        _closeTimer.expires_after(std::chrono::milliseconds(closeDrainMilliseconds));
        _closeTimer.async_wait(asio::bind_executor(_strand, [self = shared_from_this()](std::error_code ec)
        {
            if (!ec)
                self->CloseNow();
        }));
    }

    void ServerSession::CloseNow()
    {
        if (_closed)
            return;

        _closed = true;
        _requestClose.store(true, std::memory_order_release);

        std::error_code ignoredError;
        _readTimer.cancel(ignoredError);
        _closeTimer.cancel(ignoredError);
        _socket.shutdown(asio::socket_base::shutdown_both, ignoredError);
        _socket.close(ignoredError);

        _pendingPacketBatch.clear();
        _pendingPacketBatchScheduled = false;
        if (!_writeInProgress)
            _msgQueue.clear();

        Server* server = _server;
        _server = nullptr;
        if (server != nullptr)
            server->FinalizeClosedSocketID(_socketID, this);
    }

    void ServerSession::StopForShutdown()
    {
        if (_closed)
            return;

        _closed = true;
        _requestClose.store(true, std::memory_order_release);

        std::error_code ignoredError;
        _readTimer.cancel(ignoredError);
        _closeTimer.cancel(ignoredError);
        _socket.shutdown(asio::socket_base::shutdown_both, ignoredError);
        _socket.close(ignoredError);

        _pendingPacketBatch.clear();
        if (!_writeInProgress)
            _msgQueue.clear();
        _server = nullptr;
    }

    void ServerSession::ReadMessageHeader()
    {
        if (_closed || _server == nullptr)
            return;

        _server->_readHeaderCount.fetch_add(1, std::memory_order_relaxed);
        ArmReadTimeout();
        asio::async_read(_socket, asio::buffer(&_readHeader, sizeof(MessageHeader)), asio::bind_executor(_strand, [self = shared_from_this()](std::error_code ec, std::size_t)
        {
            self->CancelReadTimeout();
            if (ec)
            {
                if (!self->_closed && !self->_requestClose.load(std::memory_order_acquire) && ec != asio::error::operation_aborted)
                    NC_LOG_ERROR("Network::ServerSession : ReadHeader Failed ({0})", ec.message());

                if (!self->_closed)
                    self->CloseInternal();
                return;
            }
        
            const MessageHeader header = self->_readHeader;
        
            if (header.size > DEFAULT_BUFFER_SIZE - sizeof(MessageHeader))
            {
                NC_LOG_ERROR("Network::ServerSession : ReadHeader Failed (Message Size Too Large)");
                self->CloseInternal();
                return;
            }
        
            if (header.opcode == MetaGen::Shared::Packet::ClientPingPacket::PACKET_ID && header.size != sizeof(u16))
            {
                NC_LOG_WARNING("Network::ServerSession : ReadHeader Failed (Invalid Ping Size)");
                self->CloseInternal();
                return;
            }
        
            PacketWriter readWriter = self->_server->_packetArena.Acquire(sizeof(MessageHeader) + header.size);
            if (!readWriter.IsValid() || !readWriter.GetBuffer().Put(header))
            {
                self->CloseInternal();
                return;
            }

            self->_readWriter = std::move(readWriter);
            if (header.size > 0)
            {
                self->ReadMessageBody();
            }
            else
            {
                // Emit Message Event
                self->EnqueueMessage(self->_socketID, self->_readWriter.Seal());
                if (!self->_requestClose.load(std::memory_order_acquire))
                    self->ReadMessageHeader();
            }
        }));
    }

    void ServerSession::ReadMessageBody()
    {
        NC_ASSERT(_readWriter.IsValid() && _readWriter.GetBuffer().writtenData == sizeof(MessageHeader), "Network::ServerSession : ReadMessageBody Failed (Invalid Buffer)");

        _server->_readBodyCount.fetch_add(1, std::memory_order_relaxed);
        const u32 sizeToRead = _readHeader.size;
        ArmReadTimeout();
        asio::async_read(_socket, asio::buffer(_readWriter.GetBuffer().GetWritePointer(), sizeToRead), asio::bind_executor(_strand, [self = shared_from_this()](std::error_code ec, std::size_t length)
        {
            self->CancelReadTimeout();
            if (ec)
            {
                if (!self->_closed && !self->_requestClose.load(std::memory_order_acquire) && ec != asio::error::operation_aborted)
                    NC_LOG_ERROR("Network::ServerSession : ReadBody Failed ({0})", ec.message());

                if (!self->_closed)
                    self->CloseInternal();
                return;
            }
        
            if (!self->_readWriter.GetBuffer().SkipWrite(length))
            {
                self->CloseInternal();
                return;
            }

            if (self->_readHeader.opcode == MetaGen::Shared::Packet::ClientPingPacket::PACKET_ID && (!self->AdmitPing() || !self->QueuePong()))
            {
                self->CloseInternal();
                return;
            }

            // Emit Message Event
            self->EnqueueMessage(self->_socketID, self->_readWriter.Seal());
            if (!self->_requestClose.load(std::memory_order_acquire))
                self->ReadMessageHeader();
        }));
    }

    bool ServerSession::QueuePong()
    {
        if (_server == nullptr || _closed)
            return false;

        PacketWriter pongWriter = _server->_packetArena.Acquire(sizeof(MessageHeader));
        if (!pongWriter.IsValid())
        {
            NC_LOG_ERROR("Network::ServerSession : Failed to allocate pong packet");
            return false;
        }

        MessageHeader pongHeader =
        {
            .opcode = MetaGen::Shared::Packet::ServerPongPacket::PACKET_ID,
            .size = 0
        };
        if (!pongWriter.GetBuffer().Put(pongHeader))
            return false;

        SocketPacketEvent pongPacket;
        pongPacket.packet = pongWriter.Seal();
        pongPacket.queueReservation = PacketQueueReservation::ReserveGlobal(_server->_outboundQueueBudget, pongPacket.GetWrittenData(), PacketPriority::Critical);
        if (!pongPacket.queueReservation.HasGlobalReservation())
        {
            _server->_outboundQueueOverflowCount.fetch_add(1, std::memory_order_relaxed);
            return false;
        }

        const bool writeInProgress = _writeInProgress;
        if (!QueuePacket(std::move(pongPacket)))
            return false;

        if (!writeInProgress)
            Write();
        return true;
    }

    bool ServerSession::AdmitPing()
    {
        if (_server == nullptr)
            return false;

        const u32 minimumPingIntervalMilliseconds = _server->_config.minimumPingIntervalMilliseconds;
        if (minimumPingIntervalMilliseconds == 0)
            return true;

        const auto now = std::chrono::steady_clock::now();
        if (_lastAcceptedPingTime.time_since_epoch().count() != 0 && now - _lastAcceptedPingTime < std::chrono::milliseconds(minimumPingIntervalMilliseconds))
        {
            _server->_pingRateLimitCount.fetch_add(1, std::memory_order_relaxed);
            return false;
        }

        _lastAcceptedPingTime = now;
        return true;
    }

    void ServerSession::ArmReadTimeout()
    {
        if (_closed || _server == nullptr || _server->_config.sessionReadTimeoutMilliseconds == 0)
            return;

        const u64 generation = ++_readTimeoutGeneration;
        _server->_readTimeoutArmCount.fetch_add(1, std::memory_order_relaxed);
        _readTimer.expires_after(std::chrono::milliseconds(_server->_config.sessionReadTimeoutMilliseconds));
        _readTimer.async_wait(asio::bind_executor(_strand, [self = shared_from_this(), generation](std::error_code ec)
        {
            if (!ec && !self->_closed && self->_readTimeoutGeneration == generation)
            {
                self->_server->_readTimeoutCount.fetch_add(1, std::memory_order_relaxed);
                NC_LOG_WARNING("Network::ServerSession : Read timed out for SocketID {0}", static_cast<u32>(self->_socketID));
                self->CloseInternal();
            }
        }));
    }

    void ServerSession::CancelReadTimeout()
    {
        _readTimeoutGeneration++;
        if (_server != nullptr)
            _server->_readTimeoutCancelCount.fetch_add(1, std::memory_order_relaxed);
        std::error_code ignoredError;
        _readTimer.cancel(ignoredError);
    }

    void ServerSession::EnqueueMessage(SocketID socketID, PacketRef&& packet)
    {
        if (_server == nullptr || _laneQueue == nullptr || !packet.IsValid())
        {
            CloseInternal();
            return;
        }

        const size_t packetSize = packet.GetWrittenData();
        PacketQueueReservation queueReservation = PacketQueueReservation::ReserveGlobal(_inboundQueueBudget, packetSize, PacketPriority::Critical);
        if (!queueReservation.HasGlobalReservation() || !queueReservation.TryReserveSession(_sessionInboundQueueBudget, packetSize, PacketPriority::Critical))
        {
            _server->_inboundQueueOverflowCount.fetch_add(1, std::memory_order_relaxed);
            NC_LOG_WARNING("Network::ServerSession : Closing SocketID {0} after inbound queue limit was reached", static_cast<u32>(_socketID));
            CloseInternal();
            return;
        }

        SocketMessageEvent messageEvent;
        messageEvent.socketID = socketID;
        messageEvent.message.SetPacket(std::move(packet), std::move(queueReservation));

        if (!_laneQueue->enqueue(std::move(messageEvent)))
        {
            _server->_inboundQueueOverflowCount.fetch_add(1, std::memory_order_relaxed);
            CloseInternal();
        }
    }

    void ServerSession::CloseInternal()
    {
        RequestClose();
    }

    bool ServerSession::QueuePacket(SocketPacketEvent&& packet)
    {
        if (_closed || _requestClose.load(std::memory_order_acquire))
            return false;

        if (_server->_config.replicationCoalescingEnabled && packet.options.CanCoalesce() && _msgQueue.size() > 1)
        {
            for (auto it = _msgQueue.end() - 1; it != _msgQueue.begin(); --it)
            {
                if (it->options.coalesceKey != packet.options.coalesceKey)
                    continue;

                if (TryReplaceCoalescedPacket(*it, std::move(packet)))
                {
                    _server->_replicationPacketCoalesceCount.fetch_add(1, std::memory_order_relaxed);
                    return true;
                }

                return HandleSessionQueuePressure(packet);
            }
        }

        if (!TryReserveSessionQueue(packet))
            return HandleSessionQueuePressure(packet);

        _msgQueue.push_back(std::move(packet));
        return true;
    }

    void ServerSession::FlushPendingPackets()
    {
        const bool writeInProgress = _writeInProgress;
        for (SocketPacketEvent& packet : _pendingPacketBatch)
        {
            if (!QueuePacket(std::move(packet)) && _requestClose.load(std::memory_order_acquire))
                break;
        }

        _pendingPacketBatch.clear();
        _pendingPacketBatchScheduled = false;

        if (!writeInProgress && !_msgQueue.empty())
            Write();
    }

    bool ServerSession::HandleSessionQueuePressure(const SocketPacketEvent& packet)
    {
        if (packet.options.priority == PacketPriority::Replication)
        {
            _server->_replicationPacketDropCount.fetch_add(1, std::memory_order_relaxed);
            return false;
        }

        _server->_sessionQueueOverflowCount.fetch_add(1, std::memory_order_relaxed);
        NC_LOG_WARNING("Network::ServerSession : Closing SocketID {0} after outbound queue limit was reached", static_cast<u32>(_socketID));
        CloseInternal();
        return false;
    }

    bool ServerSession::TryReserveSessionQueue(SocketPacketEvent& packet)
    {
        if (packet.queueReservation.HasSessionReservation())
            return true;

        return packet.queueReservation.TryReserveSession(_sessionOutboundQueueBudget, packet.GetWrittenData(), packet.options.priority);
    }

    Server::Server(u16 port, ServerConfig config, std::shared_ptr<PacketArenaBudget> packetArenaBudget) : _asioAcceptor(_asioContext, tcp::endpoint(tcp::v4(), port)), _asioSocket(_asioContext), _acceptRetryTimer(_asioContext), _connectedEvents(1024), _disconnectedEvents(1024), _disconnectRequests(1024), _changeLaneRequests(1024), _outMessageEvents(1024), _packetArena(std::move(packetArenaBudget), config.inboundPacketArenaMaxReservedBytes, config.inboundPacketArenaBlockSize), _config(std::move(config))
    {
        _config.maxConnections = std::clamp(_config.maxConnections, 1u, SOCKET_ID_VALUE_MASK + 1u);
        if (_config.criticalOutboundQueueReserveEvents == 0)
            _config.criticalOutboundQueueReserveEvents = std::min(static_cast<size_t>(_config.maxConnections), _config.maxOutboundQueueEvents);
        _outboundQueueBudget = std::make_shared<PacketQueueBudget>(_config.maxOutboundQueueBytes, _config.maxOutboundQueueEvents, _config.criticalOutboundQueueReserveBytes, _config.criticalOutboundQueueReserveEvents);
        _inboundQueueBudget = std::make_shared<PacketQueueBudget>(_config.maxInboundQueueBytes, _config.maxInboundQueueEvents);
        const auto now = std::chrono::steady_clock::now();
        _nextMaintenanceRequest = now;
        _nextInboundPacketArenaTrim = now + std::chrono::milliseconds(std::max(1u, _config.inboundPacketArenaTrimIntervalMilliseconds));

        _connections.resize(_config.maxConnections);
        _availableConnectionIndices.reserve(_config.maxConnections);
        for (u32 index = _config.maxConnections; index > 0; index--)
            _availableConnectionIndices.push_back(index - 1);

        _laneToInMessageQueue.reserve(8192);
        AddLane(DEFAULT_LANE_ID);
    }

    Server::~Server()
    {
        Stop();
    }

    bool Server::Start()
    {
        if (_asioThread != nullptr || _isStopping.load(std::memory_order_acquire))
            return false;

        _asioContext.restart();
        _asioWorkGuard = std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(_asioContext.get_executor());
        ListenForNewConnection();
        _asioThread = std::make_unique<std::thread>([this]() { _asioContext.run(); });

        return true;
    }

    void Server::Stop()
    {
        if (_asioThread == nullptr)
            return;

        if (_asioThread->get_id() == std::this_thread::get_id())
        {
            NC_LOG_ERROR("Network::Server : Stop must be called from outside the ASIO worker thread");
            return;
        }

        if (_isStopping.exchange(true, std::memory_order_acq_rel))
            return;

        auto completion = std::make_shared<std::promise<void>>();
        std::future<void> completionFuture = completion->get_future();
        asio::post(_asioContext, [this, completion]()
        {
            std::error_code ignoredError;
            _asioAcceptor.close(ignoredError);
            _asioSocket.close(ignoredError);
            _acceptRetryTimer.cancel(ignoredError);

            for (Connection& connection : _connections)
            {
                if (connection.client != nullptr)
                {
                    connection.client->StopForShutdown();
                    connection.client.reset();
                }
            }

            completion->set_value();
        });

        completionFuture.wait();
        _asioWorkGuard.reset();
        if (_asioThread->joinable())
            _asioThread->join();
        _asioThread.reset();
    }

    void Server::Update()
    {
        if (_isStopping.load(std::memory_order_acquire))
            return;

        const auto now = std::chrono::steady_clock::now();
        if (now < _nextMaintenanceRequest)
            return;

        _nextMaintenanceRequest = now + std::chrono::milliseconds(std::max(1u, _config.inboundPacketArenaTrimIntervalMilliseconds));
        RequestDeferredProcessing();
    }

    ServerTelemetryDetail Server::GetTelemetryDetail() const
    {
        return
        {
            .inboundQueueBytesHighWater = _inboundQueueBudget->GetQueuedBytesHighWater(),
            .inboundQueueEventsHighWater = _inboundQueueBudget->GetQueuedEventsHighWater(),
            .outboundQueueBytesHighWater = _outboundQueueBudget->GetQueuedBytesHighWater(),
            .outboundQueueEventsHighWater = _outboundQueueBudget->GetQueuedEventsHighWater(),
            .readHeaderCount = _readHeaderCount.load(std::memory_order_relaxed),
            .readBodyCount = _readBodyCount.load(std::memory_order_relaxed),
            .writeCount = _writeCount.load(std::memory_order_relaxed),
            .readTimeoutArmCount = _readTimeoutArmCount.load(std::memory_order_relaxed),
            .readTimeoutCancelCount = _readTimeoutCancelCount.load(std::memory_order_relaxed),
            .readTimeoutCount = _readTimeoutCount.load(std::memory_order_relaxed),
            .pingRateLimitCount = _pingRateLimitCount.load(std::memory_order_relaxed),
            .deferredPostCount = _deferredPostCount.load(std::memory_order_relaxed),
            .deferredDispatchCount = _deferredDispatchCount.load(std::memory_order_relaxed),
            .pendingPacketBatchHighWater = _pendingPacketBatchHighWater.load(std::memory_order_relaxed)
        };
    }

    void Server::AddLane(u64 laneID)
    {
        std::unique_lock lock(_laneMutex);
        if (_laneToInMessageQueue.contains(laneID))
            return;

        _laneToInMessageQueue.emplace(laneID, std::make_shared<SocketMessageQueue>(1024));
    }

    void Server::SetSocketIDLane(SocketID socketID, u64 laneID)
    {
        SocketMessageQueuePtr laneQueue = GetLaneQueue(laneID);
        if (laneQueue == nullptr)
            return;

        SocketLaneChangeRequest changeLaneRequest;
        changeLaneRequest.socketID = socketID;
        changeLaneRequest.laneQueue = std::move(laneQueue);

        if (_changeLaneRequests.enqueue(std::move(changeLaneRequest)))
            RequestDeferredProcessing();
    }

    void Server::SetConnectionInfoCapture(bool enabled)
    {
        _captureConnectionInfo.store(enabled, std::memory_order_relaxed);
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

    bool Server::SendPacket(SocketID socketID, PacketRef packet, PacketSendOptions options)
    {
        if (_isStopping.load(std::memory_order_acquire) || socketID == SOCKET_ID_INVALID)
        {
            NC_LOG_ERROR("Network::Server : SendPacket Failed (Invalid SocketID or stopped server)");
            return false;
        }

        if (!packet.IsValid())
        {
            NC_LOG_ERROR("Network::Server : SendPacket Failed (Invalid Packet)");
            return false;
        }

        SocketPacketEvent messageEvent;
        messageEvent.socketID = socketID;
        messageEvent.packet = std::move(packet);
        messageEvent.options = options;
        messageEvent.queueReservation = PacketQueueReservation::ReserveGlobal(_outboundQueueBudget, messageEvent.GetWrittenData(), options.priority);
        if (!messageEvent.queueReservation.HasGlobalReservation())
        {
            _outboundQueueOverflowCount.fetch_add(1, std::memory_order_relaxed);
            if (options.priority == PacketPriority::Replication)
                _replicationPacketDropCount.fetch_add(1, std::memory_order_relaxed);
            else
                NC_LOG_WARNING("Network::Server : SendPacket Dropped (Outbound Queue Limit Reached)");
            return false;
        }

        if (!_outMessageEvents.enqueue(std::move(messageEvent)))
        {
            NC_LOG_ERROR("Network::Server : SendPacket Failed (Outbound Queue Allocation Failed)");
            return false;
        }

        RequestDeferredProcessing();
        return true;
    }

    SocketMessageQueue& Server::GetMessageEvents(u64 lane)
    {
        SocketMessageQueuePtr laneQueue = GetLaneQueue(lane);
        NC_ASSERT(laneQueue != nullptr, "Network::Server : GetMessageEvents Failed (Unknown Lane)");
        return *laneQueue;
    }

    SocketID Server::GetNextSocketID()
    {
        if (_availableConnectionIndices.empty())
            return SOCKET_ID_INVALID;

        const u32 index = _availableConnectionIndices.back();
        _availableConnectionIndices.pop_back();
        Connection& connection = _connections[index];

        // Set SocketID or Increment Version
        if (connection.id == SOCKET_ID_INVALID)
        {
            connection.id = static_cast<SocketID>(index);
        }
        else
        {
            const u8 version = static_cast<u8>(Util::DefineUtil::GetSocketIDVersion(connection.id) + 1);
            connection.id = Util::DefineUtil::CreateSocketID(version, index);
        }

        return connection.id;
    }

    void Server::ListenForNewConnection()
    {
        if (_isStopping.load(std::memory_order_acquire))
            return;

        _asioAcceptor.async_accept(_asioSocket, [this](std::error_code ec)
        {
            if (_isStopping.load(std::memory_order_acquire))
                return;

            if (ec)
            {
                if (ec != asio::error::operation_aborted)
                    NC_LOG_ERROR("Network::Server : New Connection Failed ({0})", ec.message());
                std::error_code ignoredError;
                _asioSocket.close(ignoredError);
                ScheduleAcceptRetry();
                return;
            }

            SocketConnectedEvent connectedEvent;
            std::error_code endpointError;
            if (_captureConnectionInfo.load(std::memory_order_relaxed))
            {
                const tcp::endpoint remoteEndpoint = _asioSocket.remote_endpoint(endpointError);
                if (!endpointError)
                {
                    connectedEvent.connectionInfo.ipAddr = remoteEndpoint.address().to_string();
                    connectedEvent.connectionInfo.port = remoteEndpoint.port();
                }
            }

            std::error_code optionError;
            _asioSocket.set_option(asio::socket_base::receive_buffer_size(DEFAULT_BUFFER_SIZE), optionError);
            if (!optionError)
                _asioSocket.set_option(asio::socket_base::send_buffer_size(DEFAULT_BUFFER_SIZE), optionError);
            if (!optionError)
                _asioSocket.set_option(asio::ip::tcp::no_delay(true), optionError);
            if (optionError)
                NC_LOG_WARNING("Network::Server : Failed to configure accepted socket ({0})", optionError.message());

            const SocketID socketID = GetNextSocketID();
            if (socketID == SOCKET_ID_INVALID)
            {
                NC_LOG_WARNING("Network::Server : Connection capacity reached");
                std::error_code ignoredError;
                _asioSocket.close(ignoredError);
                ScheduleAcceptRetry();
                return;
            }

            const u32 index = Util::DefineUtil::GetSocketIDValue(socketID);
            SocketMessageQueuePtr defaultLaneQueue = GetLaneQueue(DEFAULT_LANE_ID);
            if (defaultLaneQueue == nullptr)
            {
                NC_LOG_ERROR("Network::Server : Missing default inbound lane");
                std::error_code ignoredError;
                _asioSocket.close(ignoredError);
                _availableConnectionIndices.push_back(index);
                ScheduleAcceptRetry();
                return;
            }

            Connection& connection = _connections[index];
            connection.client = std::make_shared<ServerSession>(this, socketID, std::move(_asioSocket), std::move(defaultLaneQueue), _inboundQueueBudget, _config);
            connection.client->Start();

            connectedEvent.socketID = socketID;
            if (!_connectedEvents.enqueue(std::move(connectedEvent)))
            {
                NC_LOG_ERROR("Network::Server : Failed to enqueue connected event");
                connection.client->RequestClose();
            }
            ListenForNewConnection();
        });
    }

    void Server::ScheduleAcceptRetry()
    {
        if (_isStopping.load(std::memory_order_acquire))
            return;

        _acceptRetryTimer.expires_after(100ms);
        _acceptRetryTimer.async_wait([this](std::error_code ec)
        {
            if (!ec && !_isStopping.load(std::memory_order_acquire))
                ListenForNewConnection();
        });
    }

    void Server::ProcessDeferredRequests()
    {
        _deferredDispatchCount.fetch_add(1, std::memory_order_relaxed);
        _deferredRequestsScheduled.store(false, std::memory_order_release);
        if (_isStopping.load(std::memory_order_acquire))
            return;

        const u32 maxRequestsPerQueue = std::max(1u, _config.maxDeferredRequestsPerDispatch);
        bool reachedDispatchLimit = false;

        SocketDisconnectedEvent disconnectedEvent;
        u32 processedDisconnectRequests = 0;
        while (processedDisconnectRequests < maxRequestsPerQueue && _disconnectRequests.try_dequeue(disconnectedEvent))
        {
            processedDisconnectRequests++;
            if (!IsSocketIDCurrent(disconnectedEvent.socketID))
                continue;

            const u32 index = Util::DefineUtil::GetSocketIDValue(disconnectedEvent.socketID);
            _connections[index].client->RequestClose();
        }
        reachedDispatchLimit |= processedDisconnectRequests == maxRequestsPerQueue;

        SocketLaneChangeRequest changeLaneRequest;
        u32 processedLaneChangeRequests = 0;
        while (processedLaneChangeRequests < maxRequestsPerQueue && _changeLaneRequests.try_dequeue(changeLaneRequest))
        {
            processedLaneChangeRequests++;
            if (!IsSocketIDCurrent(changeLaneRequest.socketID) || changeLaneRequest.laneQueue == nullptr)
                continue;

            const u32 index = Util::DefineUtil::GetSocketIDValue(changeLaneRequest.socketID);
            _connections[index].client->SetLaneQueue(std::move(changeLaneRequest.laneQueue));
        }
        reachedDispatchLimit |= processedLaneChangeRequests == maxRequestsPerQueue;

        SocketPacketEvent messageEvent;
        u32 processedOutboundRequests = 0;
        while (processedOutboundRequests < maxRequestsPerQueue && _outMessageEvents.try_dequeue(messageEvent))
        {
            processedOutboundRequests++;
            if (!IsSocketIDCurrent(messageEvent.socketID))
                continue;

            const u32 index = Util::DefineUtil::GetSocketIDValue(messageEvent.socketID);
            _connections[index].client->QueuePacketFromServer(std::move(messageEvent));
        }
        reachedDispatchLimit |= processedOutboundRequests == maxRequestsPerQueue;

        const auto now = std::chrono::steady_clock::now();
        if (now >= _nextInboundPacketArenaTrim)
        {
            _packetArena.Trim(_config.inboundPacketArenaWarmBlocksPerSizeClass);
            _nextInboundPacketArenaTrim = now + std::chrono::milliseconds(std::max(1u, _config.inboundPacketArenaTrimIntervalMilliseconds));
        }

        if (reachedDispatchLimit)
            RequestDeferredProcessing();
    }

    void Server::DeferCloseSocketID(SocketID socketID)
    {
        if (_isStopping.load(std::memory_order_acquire))
            return;

        if (_disconnectRequests.enqueue({ socketID }))
            RequestDeferredProcessing();
        else
            NC_LOG_ERROR("Network::Server : CloseSocketID Failed (Deferred queue allocation failed)");
    }

    void Server::RequestDeferredProcessing()
    {
        if (_isStopping.load(std::memory_order_acquire))
            return;

        bool expected = false;
        if (!_deferredRequestsScheduled.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
            return;

        asio::post(_asioContext, [this]()
        {
            ProcessDeferredRequests();
        });
        _deferredPostCount.fetch_add(1, std::memory_order_relaxed);
    }

    void Server::FinalizeClosedSocketID(SocketID socketID, ServerSession* session)
    {
        if (_isStopping.load(std::memory_order_acquire) || !IsSocketIDCurrent(socketID))
            return;

        const u32 index = Util::DefineUtil::GetSocketIDValue(socketID);
        Connection& connection = _connections[index];
        if (connection.client.get() != session)
            return;

        connection.client.reset();
        _availableConnectionIndices.push_back(index);
        if (!_disconnectedEvents.enqueue({ socketID }))
            NC_LOG_ERROR("Network::Server : Failed to enqueue disconnected event for SocketID {0}", static_cast<u32>(socketID));
    }

    SocketMessageQueuePtr Server::GetLaneQueue(u64 laneID) const
    {
        std::shared_lock lock(_laneMutex);
        auto it = _laneToInMessageQueue.find(laneID);
        return it != _laneToInMessageQueue.end() ? it->second : nullptr;
    }

    bool Server::IsSocketIDCurrent(SocketID socketID) const
    {
        if (socketID == SOCKET_ID_INVALID)
            return false;

        const u32 index = Util::DefineUtil::GetSocketIDValue(socketID);
        return index < _connections.size() && _connections[index].id == socketID && _connections[index].client != nullptr;
    }

}
