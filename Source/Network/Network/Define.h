#pragma once
#include <Base/Types.h>
#include <Base/Memory/PacketArena.h>

#include <algorithm>
#include <atomic>
#include <limits>
#include <memory>
#include <string>
#include <utility>

class Bytebuffer;

namespace Network
{
    typedef u32 BufferID;
    typedef u16 OpcodeType;
    template<typename T>
    concept PacketConcept = requires(T t, Bytebuffer* buffer)
    {
        { std::decay_t<T>::PACKET_ID } -> std::convertible_to<u16>;
        
        { t.Serialize(buffer) } -> std::same_as<bool>;
        { t.Deserialize(buffer) } -> std::same_as<bool>;
        { t.GetSerializedSize() } -> std::same_as<u32>;
    };

    struct ConnectionInfo
    {
    public:
        std::string ipAddr;
        u16 port = 0;
    };

    struct MessageHeader
    {
    public:
        OpcodeType opcode = 0;
        u16 size = 0;
    };

    enum class SocketID : u32 {}; // 8 Bits for versioning, 24 bits for value
    constexpr SocketID SOCKET_ID_INVALID = static_cast<SocketID>(-1);
    constexpr u32 SOCKET_ID_BITS = sizeof(std::underlying_type<SocketID>::type) * 8;
    constexpr u32 SOCKET_ID_VERSION_BITS = 8;
    constexpr u32 SOCKET_ID_VALUE_BITS = SOCKET_ID_BITS - SOCKET_ID_VERSION_BITS;
    constexpr u32 SOCKET_ID_VERSION_MASK = (1 << SOCKET_ID_VERSION_BITS) - 1;
    constexpr u32 SOCKET_ID_VALUE_MASK = (1 << SOCKET_ID_VALUE_BITS) - 1;

    constexpr u64 DEFAULT_LANE_ID = std::numeric_limits<u64>::max();
    constexpr u32 DEFAULT_BUFFER_SIZE = 2048;

    enum class PacketPriority : u8
    {
        Critical,
        Replication
    };

    struct PacketSendOptions
    {
    public:
        PacketPriority priority = PacketPriority::Critical;
        u64 coalesceKey = 0;

        [[nodiscard]] bool CanCoalesce() const
        {
            return priority == PacketPriority::Replication && coalesceKey != 0;
        }
    };

    class PacketQueueBudget
    {
    public:
        PacketQueueBudget(size_t maxQueueBytes, size_t maxQueueEvents, size_t criticalReserveBytes = 0, size_t criticalReserveEvents = 0)
            : _maxQueueBytes(maxQueueBytes), _maxQueueEvents(maxQueueEvents), _criticalReserveBytes(criticalReserveBytes), _criticalReserveEvents(criticalReserveEvents)
        {
        }

        [[nodiscard]] bool TryReserve(size_t packetSize, PacketPriority priority)
        {
            const bool isReplication = priority == PacketPriority::Replication;
            const size_t maxQueueBytes = isReplication ? _maxQueueBytes - std::min(_criticalReserveBytes, _maxQueueBytes) : _maxQueueBytes;
            const size_t maxQueueEvents = isReplication ? _maxQueueEvents - std::min(_criticalReserveEvents, _maxQueueEvents) : _maxQueueEvents;

            if (packetSize > maxQueueBytes)
                return false;

            size_t queuedEvents = _queuedEvents.load(std::memory_order_relaxed);
            while (true)
            {
                if (queuedEvents >= maxQueueEvents)
                    return false;

                if (_queuedEvents.compare_exchange_weak(queuedEvents, queuedEvents + 1, std::memory_order_acq_rel, std::memory_order_relaxed))
                    break;
            }

            size_t queuedBytes = _queuedBytes.load(std::memory_order_relaxed);
            while (true)
            {
                if (queuedBytes >= maxQueueBytes || packetSize > maxQueueBytes - queuedBytes)
                {
                    _queuedEvents.fetch_sub(1, std::memory_order_acq_rel);
                    return false;
                }

                if (_queuedBytes.compare_exchange_weak(queuedBytes, queuedBytes + packetSize, std::memory_order_acq_rel, std::memory_order_relaxed))
                {
                    UpdateHighWater(_queuedBytesHighWater, queuedBytes + packetSize);
                    UpdateHighWater(_queuedEventsHighWater, queuedEvents + 1);
                    return true;
                }
            }
        }

        void Release(size_t packetSize)
        {
            _queuedBytes.fetch_sub(packetSize, std::memory_order_acq_rel);
            _queuedEvents.fetch_sub(1, std::memory_order_acq_rel);
        }

        [[nodiscard]] size_t GetQueuedBytes() const { return _queuedBytes.load(std::memory_order_relaxed); }
        [[nodiscard]] size_t GetQueuedEvents() const { return _queuedEvents.load(std::memory_order_relaxed); }
        [[nodiscard]] size_t GetQueuedBytesHighWater() const { return _queuedBytesHighWater.load(std::memory_order_relaxed); }
        [[nodiscard]] size_t GetQueuedEventsHighWater() const { return _queuedEventsHighWater.load(std::memory_order_relaxed); }

    private:
        static void UpdateHighWater(std::atomic<size_t>& highWater, size_t value)
        {
            size_t currentHighWater = highWater.load(std::memory_order_relaxed);
            while (value > currentHighWater && !highWater.compare_exchange_weak(currentHighWater, value, std::memory_order_relaxed, std::memory_order_relaxed))
            {
            }
        }

    private:
        size_t _maxQueueBytes = 0;
        size_t _maxQueueEvents = 0;
        size_t _criticalReserveBytes = 0;
        size_t _criticalReserveEvents = 0;
        std::atomic<size_t> _queuedBytes = 0;
        std::atomic<size_t> _queuedEvents = 0;
        std::atomic<size_t> _queuedBytesHighWater = 0;
        std::atomic<size_t> _queuedEventsHighWater = 0;
    };

    class PacketQueueReservation
    {
    public:
        PacketQueueReservation() = default;
        PacketQueueReservation(const PacketQueueReservation&) = delete;
        PacketQueueReservation& operator=(const PacketQueueReservation&) = delete;

        PacketQueueReservation(PacketQueueReservation&& other) noexcept
            : _globalBudget(std::move(other._globalBudget)), _sessionBudget(std::move(other._sessionBudget)), _packetSize(other._packetSize), _priority(other._priority)
        {
            other._packetSize = 0;
        }

        PacketQueueReservation& operator=(PacketQueueReservation&& other) noexcept
        {
            if (this == &other)
                return *this;

            Reset();
            _globalBudget = std::move(other._globalBudget);
            _sessionBudget = std::move(other._sessionBudget);
            _packetSize = other._packetSize;
            _priority = other._priority;
            other._packetSize = 0;
            return *this;
        }

        ~PacketQueueReservation()
        {
            Reset();
        }

        [[nodiscard]] static PacketQueueReservation ReserveGlobal(const std::shared_ptr<PacketQueueBudget>& budget, size_t packetSize, PacketPriority priority)
        {
            if (budget == nullptr || !budget->TryReserve(packetSize, priority))
                return { };

            return PacketQueueReservation(budget, packetSize, priority);
        }

        [[nodiscard]] bool TryReserveSession(const std::shared_ptr<PacketQueueBudget>& budget, size_t packetSize, PacketPriority priority)
        {
            if (_sessionBudget != nullptr)
                return true;

            if (_packetSize != 0 && (_packetSize != packetSize || _priority != priority))
                return false;

            if (budget == nullptr || !budget->TryReserve(packetSize, priority))
                return false;

            _sessionBudget = budget;
            _packetSize = packetSize;
            _priority = priority;
            return true;
        }

        void ReleaseSession()
        {
            if (_sessionBudget == nullptr)
                return;

            _sessionBudget->Release(_packetSize);
            _sessionBudget.reset();
        }

        [[nodiscard]] bool TransferSessionReservation(PacketQueueReservation& target)
        {
            if (_sessionBudget == nullptr || target._sessionBudget != nullptr || _packetSize != target._packetSize || _priority != target._priority)
                return false;

            target._sessionBudget = std::move(_sessionBudget);
            return true;
        }

        // Session queue ownership is confined to the ASIO worker. This permits a
        // replacement packet with a different size to take over the one existing
        // session reservation without transiently charging two queue events.
        [[nodiscard]] bool RetargetSessionReservation(PacketQueueReservation& target)
        {
            if (_sessionBudget == nullptr || target._sessionBudget != nullptr)
                return false;

            if (_packetSize == target._packetSize && _priority == target._priority)
                return TransferSessionReservation(target);

            std::shared_ptr<PacketQueueBudget> sessionBudget = _sessionBudget;
            const size_t previousPacketSize = _packetSize;
            const PacketPriority previousPriority = _priority;
            sessionBudget->Release(previousPacketSize);
            if (!sessionBudget->TryReserve(target._packetSize, target._priority))
            {
                const bool restored = sessionBudget->TryReserve(previousPacketSize, previousPriority);
                if (!restored)
                    return false;

                return false;
            }

            _sessionBudget.reset();
            target._sessionBudget = std::move(sessionBudget);
            return true;
        }

        [[nodiscard]] bool HasGlobalReservation() const { return _globalBudget != nullptr; }
        [[nodiscard]] bool HasSessionReservation() const { return _sessionBudget != nullptr; }

    private:
        PacketQueueReservation(std::shared_ptr<PacketQueueBudget> budget, size_t packetSize, PacketPriority priority)
            : _globalBudget(std::move(budget)), _packetSize(packetSize), _priority(priority)
        {
        }

        void Reset()
        {
            ReleaseSession();
            if (_globalBudget != nullptr)
            {
                _globalBudget->Release(_packetSize);
                _globalBudget.reset();
            }

            _packetSize = 0;
        }

    private:
        std::shared_ptr<PacketQueueBudget> _globalBudget;
        std::shared_ptr<PacketQueueBudget> _sessionBudget;
        size_t _packetSize = 0;
        PacketPriority _priority = PacketPriority::Critical;
    };

    // Client messages retain their existing owned Bytebuffer; server inbound messages own a
    // PacketRef and expose a read-only Bytebuffer view for existing deserializers.
    class Message
    {
    public:
        Message() = default;
        Message(const Message&) = delete;
        Message& operator=(const Message&) = delete;

        Message(Message&& other) noexcept
            : _ownedBuffer(std::move(other._ownedBuffer)), _packet(std::move(other._packet)), _readBuffer(std::move(other._readBuffer)), _queueReservation(std::move(other._queueReservation))
        {
            UpdateBufferPointer();
        }

        Message& operator=(Message&& other) noexcept
        {
            if (this == &other)
                return *this;

            _ownedBuffer = std::move(other._ownedBuffer);
            _packet = std::move(other._packet);
            _readBuffer = std::move(other._readBuffer);
            _queueReservation = std::move(other._queueReservation);
            UpdateBufferPointer();
            return *this;
        }

        void SetOwnedBuffer(std::shared_ptr<Bytebuffer> inBuffer)
        {
            _packet = { };
            _readBuffer = Bytebuffer();
            _queueReservation = { };
            _ownedBuffer = std::move(inBuffer);
            UpdateBufferPointer();
        }

        void SetPacket(PacketRef&& inPacket, PacketQueueReservation&& queueReservation = { })
        {
            _ownedBuffer.reset();
            _packet = std::move(inPacket);
            _readBuffer = _packet.CreateReadView();
            _queueReservation = std::move(queueReservation);
            UpdateBufferPointer();
        }

        Bytebuffer* buffer = nullptr;

    private:
        void UpdateBufferPointer()
        {
            buffer = _packet.IsValid() ? &_readBuffer : _ownedBuffer.get();
        }

    private:
        std::shared_ptr<Bytebuffer> _ownedBuffer;
        PacketRef _packet;
        Bytebuffer _readBuffer;
        PacketQueueReservation _queueReservation;
    };

    struct SocketConnectedEvent
    {
    public:
        SocketID socketID = SOCKET_ID_INVALID;
        ConnectionInfo connectionInfo;
    };
    struct SocketDisconnectedEvent
    {
    public:
        SocketID socketID = SOCKET_ID_INVALID;
    };
    struct SocketChangeLaneEvent
    {
    public:
        SocketID socketID = SOCKET_ID_INVALID;
        u64 laneID;
    };
    struct SocketMessageEvent
    {
    public:
        SocketID socketID = SOCKET_ID_INVALID;
        Message message;
    };
    struct SocketPacketEvent
    {
    public:
        SocketID socketID = SOCKET_ID_INVALID;
        PacketRef packet;
        PacketSendOptions options;
        PacketQueueReservation queueReservation;

        [[nodiscard]] bool IsValid() const
        {
            return packet.IsValid();
        }
        [[nodiscard]] const u8* GetDataPointer() const
        {
            return packet.GetDataPointer();
        }
        [[nodiscard]] size_t GetWrittenData() const
        {
            return packet.GetWrittenData();
        }
    };
}
