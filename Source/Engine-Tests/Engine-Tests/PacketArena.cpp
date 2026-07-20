#include <catch2/catch2.hpp>

#include <Base/Memory/PacketArena.h>

#include <thread>
#include <vector>

TEST_CASE("PacketArena keeps fan-out packets alive across a remote release", "[Base][PacketArena]")
{
    PacketArena arena(4096, 512);

    PacketWriter writer = arena.Acquire(sizeof(u32));
    REQUIRE(writer.IsValid());
    REQUIRE(writer.GetBuffer().PutU32(0x12345678));

    PacketRef packet = writer.Seal();
    REQUIRE(packet.IsValid());
    REQUIRE(packet.GetWrittenData() == sizeof(u32));

    PacketRef remotePacket = packet;
    std::thread remoteRelease([packet = std::move(remotePacket)]() mutable
    {
        packet = { };
    });
    remoteRelease.join();

    CHECK(arena.GetInUseAllocationCount() == 1);
    CHECK(*reinterpret_cast<const u32*>(packet.GetDataPointer()) == 0x12345678);

    packet = { };
    arena.Drain();

    CHECK(arena.GetInUseBytes() == 0);
    CHECK(arena.GetInUseAllocationCount() == 0);
}

TEST_CASE("PacketArena accounts for an exhausted reservation budget", "[Base][PacketArena]")
{
    PacketArena arena(512, 512);
    std::vector<PacketWriter> writers;

    while (true)
    {
        PacketWriter writer = arena.Acquire(128);
        if (!writer.IsValid())
            break;

        writers.push_back(std::move(writer));
    }

    REQUIRE_FALSE(writers.empty());
    CHECK(arena.GetAllocationFailureCount() == 1);
}

TEST_CASE("PacketArena trims fully free blocks while retaining no warm blocks", "[Base][PacketArena]")
{
    PacketArena arena(4096, 512);
    std::vector<PacketRef> packets;

    for (size_t i = 0; i < 8; i++)
    {
        PacketWriter writer = arena.Acquire(128);
        REQUIRE(writer.IsValid());
        packets.push_back(writer.Seal());
    }

    const size_t reservedBytes = arena.GetReservedBytes();
    REQUIRE(reservedBytes > 0);

    packets.clear();
    CHECK(arena.Trim(0) == reservedBytes);
    CHECK(arena.GetReservedBytes() == 0);
}

TEST_CASE("PacketRef keeps an arena alive after its owner is destroyed", "[Base][PacketArena]")
{
    PacketRef packet;
    {
        PacketArena arena(4096, 512);
        PacketWriter writer = arena.Acquire(sizeof(u32));
        REQUIRE(writer.IsValid());
        REQUIRE(writer.GetBuffer().PutU32(0xCAFEBABE));
        packet = writer.Seal();
    }

    REQUIRE(packet.IsValid());
    REQUIRE(packet.GetWrittenData() == sizeof(u32));
    CHECK(*reinterpret_cast<const u32*>(packet.GetDataPointer()) == 0xCAFEBABE);
}
