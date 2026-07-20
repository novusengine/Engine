#include <catch2/catch2.hpp>

#include <Network/Server.h>

namespace
{
    Network::SocketPacketEvent CreateReplicationPacket(PacketArena& arena, const std::shared_ptr<Network::PacketQueueBudget>& budget, size_t size)
    {
        PacketWriter writer = arena.Acquire(size);
        if (!writer.IsValid() || !writer.GetBuffer().SkipWrite(size))
            return { };

        Network::SocketPacketEvent packet;
        packet.packet = writer.Seal();
        packet.options =
        {
            .priority = Network::PacketPriority::Replication,
            .coalesceKey = 1
        };
        packet.queueReservation = Network::PacketQueueReservation::ReserveGlobal(budget, size, packet.options.priority);
        if (!packet.queueReservation.HasGlobalReservation())
            return { };

        return packet;
    }
}

TEST_CASE("Network queued coalescing retargets a session reservation for a new packet size", "[Network]")
{
    PacketArena arena(4096, 512);
    auto globalBudget = std::make_shared<Network::PacketQueueBudget>(1024, 8);
    auto sessionBudget = std::make_shared<Network::PacketQueueBudget>(1024, 8);

    Network::SocketPacketEvent existingPacket = CreateReplicationPacket(arena, globalBudget, 64);
    Network::SocketPacketEvent replacementPacket = CreateReplicationPacket(arena, globalBudget, 128);
    REQUIRE(existingPacket.IsValid());
    REQUIRE(replacementPacket.IsValid());
    REQUIRE(existingPacket.queueReservation.TryReserveSession(sessionBudget, existingPacket.GetWrittenData(), existingPacket.options.priority));

    REQUIRE(Network::TryReplaceCoalescedPacket(existingPacket, std::move(replacementPacket)));

    CHECK(existingPacket.queueReservation.HasSessionReservation());
    CHECK(sessionBudget->GetQueuedEvents() == 1);
    CHECK(sessionBudget->GetQueuedBytes() == 128);
    CHECK(globalBudget->GetQueuedEvents() == 1);
    CHECK(globalBudget->GetQueuedBytes() == 128);
}
