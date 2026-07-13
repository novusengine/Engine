#pragma once
#include "Define.h"

#include "Base/Types.h"
#include "Base/Container/ConcurrentQueue.h"

#include <robinhood/robinhood.h>
#include <asio/asio.hpp>

#include <array>
#include <chrono>
#include <deque>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>

using asio::ip::tcp;

class Bytebuffer;

namespace Network
{
    class Server;

    using SocketMessageQueue = moodycamel::ConcurrentQueue<SocketMessageEvent>;
    using SocketMessageQueuePtr = std::shared_ptr<SocketMessageQueue>;

    struct SocketLaneChangeRequest
    {
    public:
        SocketID socketID = SOCKET_ID_INVALID;
        SocketMessageQueuePtr laneQueue;
    };

    struct ServerConfig
    {
    public:
        u32 maxConnections = 20000;
        size_t maxOutboundQueueBytes = 64ull * 1024ull * 1024ull;
        size_t maxOutboundQueueEvents = 65536;
        size_t criticalOutboundQueueReserveBytes = 8ull * 1024ull * 1024ull;
        // Zero derives a reserve large enough for one critical event per configured session.
        size_t criticalOutboundQueueReserveEvents = 0;
        size_t maxSessionQueueBytes = 256ull * 1024ull;
        size_t maxSessionQueueEvents = 256;
        size_t criticalSessionQueueReserveBytes = 32ull * 1024ull;
        size_t criticalSessionQueueReserveEvents = 16;
        size_t maxInboundQueueBytes = 64ull * 1024ull * 1024ull;
        size_t maxInboundQueueEvents = 65536;
        size_t maxSessionInboundQueueBytes = 64ull * 1024ull;
        size_t maxSessionInboundQueueEvents = 64;
        size_t inboundPacketArenaMaxReservedBytes = PacketArena::DEFAULT_MAX_RESERVED_BYTES;
        size_t inboundPacketArenaBlockSize = PacketArena::DEFAULT_BLOCK_SIZE;
        size_t inboundPacketArenaWarmBlocksPerSizeClass = PacketArena::DEFAULT_WARM_BLOCKS_PER_SIZE_CLASS;
        u32 sessionReadTimeoutMilliseconds = 60000;
        u32 sessionCloseDrainMilliseconds = 100;
        u32 maxDeferredRequestsPerDispatch = 4096;
        u32 inboundPacketArenaTrimIntervalMilliseconds = 1000;
        bool replicationCoalescingEnabled = true;
        u32 minimumPingIntervalMilliseconds = 1000;
    };

    class ServerSession : public std::enable_shared_from_this<ServerSession>
    {
    public:
        ServerSession(Server* server, SocketID socketID, tcp::socket socket, SocketMessageQueuePtr laneQueue, std::shared_ptr<PacketQueueBudget> inboundQueueBudget, const ServerConfig& config);

        void Start();
        bool RequestClose();

        void SetLaneQueue(SocketMessageQueuePtr laneQueue);
        void QueuePacketFromServer(SocketPacketEvent&& packet);

    private:
        void Write();
        void BeginClose();
        void CloseNow();
        void StopForShutdown();

        void ReadMessageHeader();
        void ReadMessageBody();
        bool QueuePong();
        bool AdmitPing();
        void ArmReadTimeout();
        void CancelReadTimeout();

        void EnqueueMessage(SocketID socketID, PacketRef&& packet);
        void CloseInternal();
        bool QueuePacket(SocketPacketEvent&& packet);
        void FlushPendingPackets();
        bool HandleSessionQueuePressure(const SocketPacketEvent& packet);
        bool TryReserveSessionQueue(SocketPacketEvent& packet);

    private:
        friend class Server;

        SocketID _socketID = SOCKET_ID_INVALID;
        tcp::socket _socket;
        asio::strand<asio::any_io_executor> _strand;
        asio::steady_timer _readTimer;
        asio::steady_timer _closeTimer;
        std::atomic<bool> _requestClose = false;
        u64 _readTimeoutGeneration = 0;
        std::chrono::steady_clock::time_point _lastAcceptedPingTime;
        bool _closeStarted = false;
        bool _closed = false;
        bool _writeInProgress = false;

        SocketMessageQueuePtr _laneQueue;
        std::shared_ptr<PacketQueueBudget> _inboundQueueBudget;
        std::shared_ptr<PacketQueueBudget> _sessionInboundQueueBudget;
        std::shared_ptr<PacketQueueBudget> _sessionOutboundQueueBudget;
        std::deque<SocketPacketEvent> _msgQueue;
        std::vector<SocketPacketEvent> _pendingPacketBatch;
        bool _pendingPacketBatchScheduled = false;
        MessageHeader _readHeader;
        PacketWriter _readWriter;

        Server* _server = nullptr;
    };

    class Client;

    struct ServerTelemetryDetail
    {
    public:
        size_t inboundQueueBytesHighWater = 0;
        size_t inboundQueueEventsHighWater = 0;
        size_t outboundQueueBytesHighWater = 0;
        size_t outboundQueueEventsHighWater = 0;
        size_t readHeaderCount = 0;
        size_t readBodyCount = 0;
        size_t writeCount = 0;
        size_t readTimeoutArmCount = 0;
        size_t readTimeoutCancelCount = 0;
        size_t readTimeoutCount = 0;
        size_t pingRateLimitCount = 0;
        size_t deferredPostCount = 0;
        size_t deferredDispatchCount = 0;
        size_t pendingPacketBatchHighWater = 0;
    };

    // Replaces an older coalescible packet while retaining the existing per-session
    // reservation when one has already been made. Staging batches have no such
    // reservation yet and can always replace their older packet.
    bool TryReplaceCoalescedPacket(SocketPacketEvent& existingPacket, SocketPacketEvent&& replacementPacket);

    class Server
    {
    public:
        static constexpr u32 DEFAULT_MAX_CONNECTIONS = 20000;

        struct Connection
        {
        public:
            SocketID id = SOCKET_ID_INVALID;
            std::shared_ptr<ServerSession> client = nullptr;
        };

    public:
        Server(u16 port, ServerConfig config = { }, std::shared_ptr<PacketArenaBudget> packetArenaBudget = nullptr);
        ~Server();

        bool Start();
        void Stop();
        void Update();

        void AddLane(u64 laneID);
        void SetSocketIDLane(SocketID socketID, u64 laneID);
        void SetConnectionInfoCapture(bool enabled);
        void CloseSocketID(SocketID socketID);
        bool SendPacket(SocketID socketID, PacketRef packet, PacketSendOptions options = { });
        [[nodiscard]] ServerTelemetryDetail GetTelemetryDetail() const;

    public:
        moodycamel::ConcurrentQueue<SocketConnectedEvent>& GetConnectedEvents() { return _connectedEvents; };
        moodycamel::ConcurrentQueue<SocketDisconnectedEvent>& GetDisconnectedEvents() { return _disconnectedEvents; };
        SocketMessageQueue& GetMessageEvents(u64 lane);
        [[nodiscard]] size_t GetOutboundQueueBytes() const { return _outboundQueueBudget->GetQueuedBytes(); }
        [[nodiscard]] size_t GetOutboundQueueEvents() const { return _outboundQueueBudget->GetQueuedEvents(); }
        [[nodiscard]] size_t GetOutboundQueueBytesHighWater() const { return _outboundQueueBudget->GetQueuedBytesHighWater(); }
        [[nodiscard]] size_t GetOutboundQueueEventsHighWater() const { return _outboundQueueBudget->GetQueuedEventsHighWater(); }
        [[nodiscard]] size_t GetOutboundQueueOverflowCount() const { return _outboundQueueOverflowCount.load(std::memory_order_relaxed); }
        [[nodiscard]] size_t GetSessionQueueOverflowCount() const { return _sessionQueueOverflowCount.load(std::memory_order_relaxed); }
        [[nodiscard]] size_t GetReplicationPacketDropCount() const { return _replicationPacketDropCount.load(std::memory_order_relaxed); }
        [[nodiscard]] size_t GetReplicationPacketCoalesceCount() const { return _replicationPacketCoalesceCount.load(std::memory_order_relaxed); }
        [[nodiscard]] size_t GetInboundQueueBytes() const { return _inboundQueueBudget->GetQueuedBytes(); }
        [[nodiscard]] size_t GetInboundQueueEvents() const { return _inboundQueueBudget->GetQueuedEvents(); }
        [[nodiscard]] size_t GetInboundQueueBytesHighWater() const { return _inboundQueueBudget->GetQueuedBytesHighWater(); }
        [[nodiscard]] size_t GetInboundQueueEventsHighWater() const { return _inboundQueueBudget->GetQueuedEventsHighWater(); }
        [[nodiscard]] size_t GetInboundQueueOverflowCount() const { return _inboundQueueOverflowCount.load(std::memory_order_relaxed); }
        [[nodiscard]] size_t GetInboundPacketArenaReservedBytes() const { return _packetArena.GetReservedBytes(); }
        [[nodiscard]] size_t GetInboundPacketArenaInUseBytes() const { return _packetArena.GetInUseBytes(); }
        [[nodiscard]] size_t GetInboundPacketArenaInUseAllocationCount() const { return _packetArena.GetInUseAllocationCount(); }
        [[nodiscard]] size_t GetInboundPacketArenaAllocationFailureCount() const { return _packetArena.GetAllocationFailureCount(); }
        [[nodiscard]] size_t GetReadHeaderCount() const { return _readHeaderCount.load(std::memory_order_relaxed); }
        [[nodiscard]] size_t GetReadBodyCount() const { return _readBodyCount.load(std::memory_order_relaxed); }
        [[nodiscard]] size_t GetWriteCount() const { return _writeCount.load(std::memory_order_relaxed); }
        [[nodiscard]] size_t GetReadTimeoutArmCount() const { return _readTimeoutArmCount.load(std::memory_order_relaxed); }
        [[nodiscard]] size_t GetReadTimeoutCancelCount() const { return _readTimeoutCancelCount.load(std::memory_order_relaxed); }
        [[nodiscard]] size_t GetReadTimeoutCount() const { return _readTimeoutCount.load(std::memory_order_relaxed); }
        [[nodiscard]] size_t GetPingRateLimitCount() const { return _pingRateLimitCount.load(std::memory_order_relaxed); }
        [[nodiscard]] size_t GetDeferredPostCount() const { return _deferredPostCount.load(std::memory_order_relaxed); }
        [[nodiscard]] size_t GetDeferredDispatchCount() const { return _deferredDispatchCount.load(std::memory_order_relaxed); }
        [[nodiscard]] size_t GetPendingPacketBatchHighWater() const { return _pendingPacketBatchHighWater.load(std::memory_order_relaxed); }

    private:
        SocketID GetNextSocketID();
        void ListenForNewConnection();
        void ScheduleAcceptRetry();

        void ProcessDeferredRequests();
        void RequestDeferredProcessing();
        void DeferCloseSocketID(SocketID socketID);
        void FinalizeClosedSocketID(SocketID socketID, ServerSession* session);
        SocketMessageQueuePtr GetLaneQueue(u64 laneID) const;
        bool IsSocketIDCurrent(SocketID socketID) const;

    private:
        friend class ServerSession;

        asio::io_context _asioContext;
        tcp::acceptor _asioAcceptor;
        tcp::socket _asioSocket;
        asio::steady_timer _acceptRetryTimer;
        std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>> _asioWorkGuard;
        std::unique_ptr<std::thread> _asioThread;
        std::atomic<bool> _isStopping = false;
        std::atomic<bool> _deferredRequestsScheduled = false;

        std::vector<Connection> _connections;
        std::vector<u32> _availableConnectionIndices;

        moodycamel::ConcurrentQueue<SocketConnectedEvent> _connectedEvents;
        moodycamel::ConcurrentQueue<SocketDisconnectedEvent> _disconnectedEvents;
        moodycamel::ConcurrentQueue<SocketDisconnectedEvent> _disconnectRequests;
        moodycamel::ConcurrentQueue<SocketLaneChangeRequest> _changeLaneRequests;
        moodycamel::ConcurrentQueue<SocketPacketEvent> _outMessageEvents;
        std::shared_ptr<PacketQueueBudget> _outboundQueueBudget;
        std::shared_ptr<PacketQueueBudget> _inboundQueueBudget;
        std::atomic<size_t> _outboundQueueOverflowCount = 0;
        std::atomic<size_t> _sessionQueueOverflowCount = 0;
        std::atomic<size_t> _replicationPacketDropCount = 0;
        std::atomic<size_t> _replicationPacketCoalesceCount = 0;
        std::atomic<size_t> _inboundQueueOverflowCount = 0;
        std::atomic<size_t> _readHeaderCount = 0;
        std::atomic<size_t> _readBodyCount = 0;
        std::atomic<size_t> _writeCount = 0;
        std::atomic<size_t> _readTimeoutArmCount = 0;
        std::atomic<size_t> _readTimeoutCancelCount = 0;
        std::atomic<size_t> _readTimeoutCount = 0;
        std::atomic<size_t> _pingRateLimitCount = 0;
        std::atomic<size_t> _deferredPostCount = 0;
        std::atomic<size_t> _deferredDispatchCount = 0;
        std::atomic<size_t> _pendingPacketBatchHighWater = 0;
        std::atomic<bool> _captureConnectionInfo = false;
        // Accessed only by Server::Update on the Server-Game thread.
        std::chrono::steady_clock::time_point _nextMaintenanceRequest;
        std::chrono::steady_clock::time_point _nextInboundPacketArenaTrim;

        PacketArena _packetArena;
        ServerConfig _config;

        mutable std::shared_mutex _laneMutex;
        robin_hood::unordered_map<u64, SocketMessageQueuePtr> _laneToInMessageQueue;
    };
}
