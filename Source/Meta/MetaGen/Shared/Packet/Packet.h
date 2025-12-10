#pragma once

#include <Base/Types.h>
#include <Base/Memory/Bytebuffer.h>
#include <FileFormat/Novus/ClientDB/ClientDB.h>

namespace MetaGen::Shared::Packet
{
    struct ClientConnectPacket
    {
    public:
        std::string accountName;

    public:
        static inline std::string NAME = "ClientConnectPacket";
        static constexpr u32 NAME_HASH = 1352108380;
        static constexpr u16 PACKET_ID = 1;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutString(accountName);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetString(accountName);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += static_cast<u32>(accountName.size()) + 1; // accountName

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += std::string("accountName : ") + accountName;

            return tmp;
        }
    };

    struct ServerConnectResultPacket
    {
    public:
        u8 result;

    public:
        static inline std::string NAME = "ServerConnectResultPacket";
        static constexpr u32 NAME_HASH = 1397553103;
        static constexpr u16 PACKET_ID = 2;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU8(result);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU8(result);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 1; // result

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "result : " + std::to_string(result);

            return tmp;
        }
    };

    struct ServerAuthChallengePacket
    {
    public:
        std::array<u8, 36> challenge;

    public:
        static inline std::string NAME = "ServerAuthChallengePacket";
        static constexpr u32 NAME_HASH = 3505828647;
        static constexpr u16 PACKET_ID = 3;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutBytes(challenge.data(), challenge.size());

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetBytes(challenge.data(), 36 * sizeof(u8));

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 36 * sizeof(u8); // challenge

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += std::string("challenge : Arrays are unsupported");

            return tmp;
        }
    };

    struct ClientAuthChallengePacket
    {
    public:
        std::array<u8, 32> challenge;

    public:
        static inline std::string NAME = "ClientAuthChallengePacket";
        static constexpr u32 NAME_HASH = 920051139;
        static constexpr u16 PACKET_ID = 4;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutBytes(challenge.data(), challenge.size());

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetBytes(challenge.data(), 32 * sizeof(u8));

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 32 * sizeof(u8); // challenge

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += std::string("challenge : Arrays are unsupported");

            return tmp;
        }
    };

    struct ServerAuthProofPacket
    {
    public:
        std::array<u8, 64> proof;

    public:
        static inline std::string NAME = "ServerAuthProofPacket";
        static constexpr u32 NAME_HASH = 556882186;
        static constexpr u16 PACKET_ID = 5;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutBytes(proof.data(), proof.size());

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetBytes(proof.data(), 64 * sizeof(u8));

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 64 * sizeof(u8); // proof

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += std::string("proof : Arrays are unsupported");

            return tmp;
        }
    };

    struct ClientAuthProofPacket
    {
    public:
        std::array<u8, 32> proof;

    public:
        static inline std::string NAME = "ClientAuthProofPacket";
        static constexpr u32 NAME_HASH = 3162601342;
        static constexpr u16 PACKET_ID = 6;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutBytes(proof.data(), proof.size());

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetBytes(proof.data(), 32 * sizeof(u8));

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 32 * sizeof(u8); // proof

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += std::string("proof : Arrays are unsupported");

            return tmp;
        }
    };

    struct ClientPingPacket
    {
    public:
        u16 ping;

    public:
        static inline std::string NAME = "ClientPingPacket";
        static constexpr u32 NAME_HASH = 2078010;
        static constexpr u16 PACKET_ID = 7;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU16(ping);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU16(ping);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 2; // ping

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "ping : " + std::to_string(ping);

            return tmp;
        }
    };

    struct ServerPongPacket
    {
    public:
        static inline std::string NAME = "ServerPongPacket";
        static constexpr u32 NAME_HASH = 2167316240;
        static constexpr u16 PACKET_ID = 8;

        bool Serialize(Bytebuffer* buffer) const
        {
            return true;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            return true;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;
            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            return tmp;
        }
    };

    struct ServerUpdateStatsPacket
    {
    public:
        u8 serverTickTime;

    public:
        static inline std::string NAME = "ServerUpdateStatsPacket";
        static constexpr u32 NAME_HASH = 1175251898;
        static constexpr u16 PACKET_ID = 9;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU8(serverTickTime);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU8(serverTickTime);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 1; // serverTickTime

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "serverTickTime : " + std::to_string(serverTickTime);

            return tmp;
        }
    };

    struct ServerCharacterListPacket
    {
    public:
        static inline std::string NAME = "ServerCharacterListPacket";
        static constexpr u32 NAME_HASH = 2600245863;
        static constexpr u16 PACKET_ID = 10;

        bool Serialize(Bytebuffer* buffer) const
        {
            return true;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            return true;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;
            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            return tmp;
        }
    };

    struct ClientCharacterSelectPacket
    {
    public:
        u8 characterIndex;

    public:
        static inline std::string NAME = "ClientCharacterSelectPacket";
        static constexpr u32 NAME_HASH = 1759469911;
        static constexpr u16 PACKET_ID = 11;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU8(characterIndex);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU8(characterIndex);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 1; // characterIndex

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "characterIndex : " + std::to_string(characterIndex);

            return tmp;
        }
    };

    struct ClientCharacterLogoutPacket
    {
    public:
        static inline std::string NAME = "ClientCharacterLogoutPacket";
        static constexpr u32 NAME_HASH = 2333713361;
        static constexpr u16 PACKET_ID = 12;

        bool Serialize(Bytebuffer* buffer) const
        {
            return true;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            return true;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;
            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            return tmp;
        }
    };

    struct ServerCharacterLogoutPacket
    {
    public:
        static inline std::string NAME = "ServerCharacterLogoutPacket";
        static constexpr u32 NAME_HASH = 2954752261;
        static constexpr u16 PACKET_ID = 13;

        bool Serialize(Bytebuffer* buffer) const
        {
            return true;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            return true;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;
            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            return tmp;
        }
    };

    struct ServerWorldTransferPacket
    {
    public:
        static inline std::string NAME = "ServerWorldTransferPacket";
        static constexpr u32 NAME_HASH = 2541000419;
        static constexpr u16 PACKET_ID = 14;

        bool Serialize(Bytebuffer* buffer) const
        {
            return true;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            return true;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;
            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            return tmp;
        }
    };

    struct ServerLoadMapPacket
    {
    public:
        u32 mapID;

    public:
        static inline std::string NAME = "ServerLoadMapPacket";
        static constexpr u32 NAME_HASH = 2560009060;
        static constexpr u16 PACKET_ID = 15;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(mapID);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(mapID);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // mapID

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "mapID : " + std::to_string(mapID);

            return tmp;
        }
    };

    struct ServerUnitAddPacket
    {
    public:
        ObjectGUID guid;
        std::string name;
        u8 unitClass;
        vec3 position;
        vec3 scale;
        vec2 pitchYaw;

    public:
        static inline std::string NAME = "ServerUnitAddPacket";
        static constexpr u32 NAME_HASH = 3302639921;
        static constexpr u16 PACKET_ID = 16;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->Serialize(guid);
            failed |= !buffer->PutString(name);
            failed |= !buffer->PutU8(unitClass);
            failed |= !buffer->Put(position);
            failed |= !buffer->Put(scale);
            failed |= !buffer->Put(pitchYaw);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->Deserialize(guid);
            failed |= !buffer->GetString(name);
            failed |= !buffer->GetU8(unitClass);
            failed |= !buffer->Get(position);
            failed |= !buffer->Get(scale);
            failed |= !buffer->Get(pitchYaw);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += guid.GetCounterBytesUsed() + 1; // guid
            result += static_cast<u32>(name.size()) + 1; // name
            result += 1; // unitClass
            result += 12; // position
            result += 12; // scale
            result += 8; // pitchYaw

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "guid : " + guid.ToString() + ", ";
            tmp += std::string("name : ") + name + ", ";
            tmp += "unitClass : " + std::to_string(unitClass) + ", ";
            tmp += std::string("position : ") + "(X : " + std::to_string(position.x) + ", Y : " + std::to_string(position.y) + ", Z : " + std::to_string(position.z) + ")" + ", ";
            tmp += std::string("scale : ") + "(X : " + std::to_string(scale.x) + ", Y : " + std::to_string(scale.y) + ", Z : " + std::to_string(scale.z) + ")" + ", ";
            tmp += std::string("pitchYaw : ") + "(X : " + std::to_string(pitchYaw.x) + ", Y : " + std::to_string(pitchYaw.y) + ")";

            return tmp;
        }
    };

    struct ServerUnitRemovePacket
    {
    public:
        ObjectGUID guid;

    public:
        static inline std::string NAME = "ServerUnitRemovePacket";
        static constexpr u32 NAME_HASH = 103679156;
        static constexpr u16 PACKET_ID = 17;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->Serialize(guid);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->Deserialize(guid);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += guid.GetCounterBytesUsed() + 1; // guid

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "guid : " + guid.ToString();

            return tmp;
        }
    };

    struct ServerObjectNetFieldUpdatePacket
    {
    public:
        static inline std::string NAME = "ServerObjectNetFieldUpdatePacket";
        static constexpr u32 NAME_HASH = 1260013287;
        static constexpr u16 PACKET_ID = 18;

        bool Serialize(Bytebuffer* buffer) const
        {
            return true;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            return true;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;
            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            return tmp;
        }
    };

    struct ServerUnitNetFieldUpdatePacket
    {
    public:
        static inline std::string NAME = "ServerUnitNetFieldUpdatePacket";
        static constexpr u32 NAME_HASH = 2986400584;
        static constexpr u16 PACKET_ID = 19;

        bool Serialize(Bytebuffer* buffer) const
        {
            return true;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            return true;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;
            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            return tmp;
        }
    };

    struct ServerUnitStatUpdatePacket
    {
    public:
        u8 kind;
        f64 base;
        f64 current;

    public:
        static inline std::string NAME = "ServerUnitStatUpdatePacket";
        static constexpr u32 NAME_HASH = 2263664577;
        static constexpr u16 PACKET_ID = 20;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU8(kind);
            failed |= !buffer->PutF64(base);
            failed |= !buffer->PutF64(current);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU8(kind);
            failed |= !buffer->GetF64(base);
            failed |= !buffer->GetF64(current);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 1; // kind
            result += 8; // base
            result += 8; // current

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "kind : " + std::to_string(kind) + ", ";
            tmp += "base : " + std::to_string(base) + ", ";
            tmp += "current : " + std::to_string(current);

            return tmp;
        }
    };

    struct ServerUnitResistanceUpdatePacket
    {
    public:
        u8 kind;
        f64 base;
        f64 current;
        f64 max;

    public:
        static inline std::string NAME = "ServerUnitResistanceUpdatePacket";
        static constexpr u32 NAME_HASH = 1451477616;
        static constexpr u16 PACKET_ID = 21;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU8(kind);
            failed |= !buffer->PutF64(base);
            failed |= !buffer->PutF64(current);
            failed |= !buffer->PutF64(max);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU8(kind);
            failed |= !buffer->GetF64(base);
            failed |= !buffer->GetF64(current);
            failed |= !buffer->GetF64(max);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 1; // kind
            result += 8; // base
            result += 8; // current
            result += 8; // max

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "kind : " + std::to_string(kind) + ", ";
            tmp += "base : " + std::to_string(base) + ", ";
            tmp += "current : " + std::to_string(current) + ", ";
            tmp += "max : " + std::to_string(max);

            return tmp;
        }
    };

    struct ServerUnitPowerUpdatePacket
    {
    public:
        ObjectGUID guid;
        u8 kind;
        f64 base;
        f64 current;
        f64 max;

    public:
        static inline std::string NAME = "ServerUnitPowerUpdatePacket";
        static constexpr u32 NAME_HASH = 2181718716;
        static constexpr u16 PACKET_ID = 22;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->Serialize(guid);
            failed |= !buffer->PutU8(kind);
            failed |= !buffer->PutF64(base);
            failed |= !buffer->PutF64(current);
            failed |= !buffer->PutF64(max);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->Deserialize(guid);
            failed |= !buffer->GetU8(kind);
            failed |= !buffer->GetF64(base);
            failed |= !buffer->GetF64(current);
            failed |= !buffer->GetF64(max);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += guid.GetCounterBytesUsed() + 1; // guid
            result += 1; // kind
            result += 8; // base
            result += 8; // current
            result += 8; // max

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "guid : " + guid.ToString() + ", ";
            tmp += "kind : " + std::to_string(kind) + ", ";
            tmp += "base : " + std::to_string(base) + ", ";
            tmp += "current : " + std::to_string(current) + ", ";
            tmp += "max : " + std::to_string(max);

            return tmp;
        }
    };

    struct ServerUnitEquippedItemUpdatePacket
    {
    public:
        ObjectGUID guid;
        u8 slot;
        u32 itemID;

    public:
        static inline std::string NAME = "ServerUnitEquippedItemUpdatePacket";
        static constexpr u32 NAME_HASH = 1422700029;
        static constexpr u16 PACKET_ID = 23;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->Serialize(guid);
            failed |= !buffer->PutU8(slot);
            failed |= !buffer->PutU32(itemID);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->Deserialize(guid);
            failed |= !buffer->GetU8(slot);
            failed |= !buffer->GetU32(itemID);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += guid.GetCounterBytesUsed() + 1; // guid
            result += 1; // slot
            result += 4; // itemID

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "guid : " + guid.ToString() + ", ";
            tmp += "slot : " + std::to_string(slot) + ", ";
            tmp += "itemID : " + std::to_string(itemID);

            return tmp;
        }
    };

    struct ServerUnitVisualItemUpdatePacket
    {
    public:
        ObjectGUID guid;
        u8 slot;
        u32 itemID;

    public:
        static inline std::string NAME = "ServerUnitVisualItemUpdatePacket";
        static constexpr u32 NAME_HASH = 3138043066;
        static constexpr u16 PACKET_ID = 24;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->Serialize(guid);
            failed |= !buffer->PutU8(slot);
            failed |= !buffer->PutU32(itemID);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->Deserialize(guid);
            failed |= !buffer->GetU8(slot);
            failed |= !buffer->GetU32(itemID);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += guid.GetCounterBytesUsed() + 1; // guid
            result += 1; // slot
            result += 4; // itemID

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "guid : " + guid.ToString() + ", ";
            tmp += "slot : " + std::to_string(slot) + ", ";
            tmp += "itemID : " + std::to_string(itemID);

            return tmp;
        }
    };

    struct ServerContainerAddPacket
    {
    public:
        ObjectGUID guid;
        u16 index;
        u32 itemID;
        u16 numSlots;
        u16 numFreeSlots;

    public:
        static inline std::string NAME = "ServerContainerAddPacket";
        static constexpr u32 NAME_HASH = 875986192;
        static constexpr u16 PACKET_ID = 25;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->Serialize(guid);
            failed |= !buffer->PutU16(index);
            failed |= !buffer->PutU32(itemID);
            failed |= !buffer->PutU16(numSlots);
            failed |= !buffer->PutU16(numFreeSlots);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->Deserialize(guid);
            failed |= !buffer->GetU16(index);
            failed |= !buffer->GetU32(itemID);
            failed |= !buffer->GetU16(numSlots);
            failed |= !buffer->GetU16(numFreeSlots);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += guid.GetCounterBytesUsed() + 1; // guid
            result += 2; // index
            result += 4; // itemID
            result += 2; // numSlots
            result += 2; // numFreeSlots

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "guid : " + guid.ToString() + ", ";
            tmp += "index : " + std::to_string(index) + ", ";
            tmp += "itemID : " + std::to_string(itemID) + ", ";
            tmp += "numSlots : " + std::to_string(numSlots) + ", ";
            tmp += "numFreeSlots : " + std::to_string(numFreeSlots);

            return tmp;
        }
    };

    struct ServerContainerAddToSlotPacket
    {
    public:
        ObjectGUID guid;
        u16 index;
        u16 slot;

    public:
        static inline std::string NAME = "ServerContainerAddToSlotPacket";
        static constexpr u32 NAME_HASH = 2756839587;
        static constexpr u16 PACKET_ID = 26;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->Serialize(guid);
            failed |= !buffer->PutU16(index);
            failed |= !buffer->PutU16(slot);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->Deserialize(guid);
            failed |= !buffer->GetU16(index);
            failed |= !buffer->GetU16(slot);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += guid.GetCounterBytesUsed() + 1; // guid
            result += 2; // index
            result += 2; // slot

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "guid : " + guid.ToString() + ", ";
            tmp += "index : " + std::to_string(index) + ", ";
            tmp += "slot : " + std::to_string(slot);

            return tmp;
        }
    };

    struct ServerContainerRemoveFromSlotPacket
    {
    public:
        u16 index;
        u16 slot;

    public:
        static inline std::string NAME = "ServerContainerRemoveFromSlotPacket";
        static constexpr u32 NAME_HASH = 1459192507;
        static constexpr u16 PACKET_ID = 27;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU16(index);
            failed |= !buffer->PutU16(slot);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU16(index);
            failed |= !buffer->GetU16(slot);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 2; // index
            result += 2; // slot

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "index : " + std::to_string(index) + ", ";
            tmp += "slot : " + std::to_string(slot);

            return tmp;
        }
    };

    struct SharedContainerSwapSlotsPacket
    {
    public:
        u16 srcContainer;
        u16 dstContainer;
        u16 srcSlot;
        u16 dstSlot;

    public:
        static inline std::string NAME = "SharedContainerSwapSlotsPacket";
        static constexpr u32 NAME_HASH = 2553264207;
        static constexpr u16 PACKET_ID = 28;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU16(srcContainer);
            failed |= !buffer->PutU16(dstContainer);
            failed |= !buffer->PutU16(srcSlot);
            failed |= !buffer->PutU16(dstSlot);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU16(srcContainer);
            failed |= !buffer->GetU16(dstContainer);
            failed |= !buffer->GetU16(srcSlot);
            failed |= !buffer->GetU16(dstSlot);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 2; // srcContainer
            result += 2; // dstContainer
            result += 2; // srcSlot
            result += 2; // dstSlot

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "srcContainer : " + std::to_string(srcContainer) + ", ";
            tmp += "dstContainer : " + std::to_string(dstContainer) + ", ";
            tmp += "srcSlot : " + std::to_string(srcSlot) + ", ";
            tmp += "dstSlot : " + std::to_string(dstSlot);

            return tmp;
        }
    };

    struct ServerItemAddPacket
    {
    public:
        ObjectGUID guid;
        u32 itemID;
        u16 count;
        u16 durability;

    public:
        static inline std::string NAME = "ServerItemAddPacket";
        static constexpr u32 NAME_HASH = 159061624;
        static constexpr u16 PACKET_ID = 29;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->Serialize(guid);
            failed |= !buffer->PutU32(itemID);
            failed |= !buffer->PutU16(count);
            failed |= !buffer->PutU16(durability);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->Deserialize(guid);
            failed |= !buffer->GetU32(itemID);
            failed |= !buffer->GetU16(count);
            failed |= !buffer->GetU16(durability);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += guid.GetCounterBytesUsed() + 1; // guid
            result += 4; // itemID
            result += 2; // count
            result += 2; // durability

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "guid : " + guid.ToString() + ", ";
            tmp += "itemID : " + std::to_string(itemID) + ", ";
            tmp += "count : " + std::to_string(count) + ", ";
            tmp += "durability : " + std::to_string(durability);

            return tmp;
        }
    };

    struct ServerSendCombatEventPacket
    {
    public:
        u16 eventID;

    public:
        static inline std::string NAME = "ServerSendCombatEventPacket";
        static constexpr u32 NAME_HASH = 3464580704;
        static constexpr u16 PACKET_ID = 30;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU16(eventID);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU16(eventID);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 2; // eventID

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "eventID : " + std::to_string(eventID);

            return tmp;
        }
    };

    struct ClientUnitTargetUpdatePacket
    {
    public:
        ObjectGUID targetGUID;

    public:
        static inline std::string NAME = "ClientUnitTargetUpdatePacket";
        static constexpr u32 NAME_HASH = 2802193624;
        static constexpr u16 PACKET_ID = 31;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->Serialize(targetGUID);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->Deserialize(targetGUID);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += targetGUID.GetCounterBytesUsed() + 1; // targetGUID

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "targetGUID : " + targetGUID.ToString();

            return tmp;
        }
    };

    struct ServerUnitTargetUpdatePacket
    {
    public:
        ObjectGUID guid;
        ObjectGUID targetGUID;

    public:
        static inline std::string NAME = "ServerUnitTargetUpdatePacket";
        static constexpr u32 NAME_HASH = 3540115548;
        static constexpr u16 PACKET_ID = 32;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->Serialize(guid);
            failed |= !buffer->Serialize(targetGUID);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->Deserialize(guid);
            failed |= !buffer->Deserialize(targetGUID);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += guid.GetCounterBytesUsed() + 1; // guid
            result += targetGUID.GetCounterBytesUsed() + 1; // targetGUID

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "guid : " + guid.ToString() + ", ";
            tmp += "targetGUID : " + targetGUID.ToString();

            return tmp;
        }
    };

    struct ClientSpellCastPacket
    {
    public:
        u32 spellID;

    public:
        static inline std::string NAME = "ClientSpellCastPacket";
        static constexpr u32 NAME_HASH = 1190923529;
        static constexpr u16 PACKET_ID = 33;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(spellID);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(spellID);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // spellID

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "spellID : " + std::to_string(spellID);

            return tmp;
        }
    };

    struct ServerSpellCastResultPacket
    {
    public:
        u8 result;

    public:
        static inline std::string NAME = "ServerSpellCastResultPacket";
        static constexpr u32 NAME_HASH = 2491143958;
        static constexpr u16 PACKET_ID = 34;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU8(result);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU8(result);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 1; // result

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "result : " + std::to_string(result);

            return tmp;
        }
    };

    struct ServerUnitCastSpellPacket
    {
    public:
        ObjectGUID guid;
        u32 spellID;
        f32 castTime;
        f32 timeToCast;

    public:
        static inline std::string NAME = "ServerUnitCastSpellPacket";
        static constexpr u32 NAME_HASH = 3728156089;
        static constexpr u16 PACKET_ID = 35;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->Serialize(guid);
            failed |= !buffer->PutU32(spellID);
            failed |= !buffer->PutF32(castTime);
            failed |= !buffer->PutF32(timeToCast);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->Deserialize(guid);
            failed |= !buffer->GetU32(spellID);
            failed |= !buffer->GetF32(castTime);
            failed |= !buffer->GetF32(timeToCast);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += guid.GetCounterBytesUsed() + 1; // guid
            result += 4; // spellID
            result += 4; // castTime
            result += 4; // timeToCast

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "guid : " + guid.ToString() + ", ";
            tmp += "spellID : " + std::to_string(spellID) + ", ";
            tmp += "castTime : " + std::to_string(castTime) + ", ";
            tmp += "timeToCast : " + std::to_string(timeToCast);

            return tmp;
        }
    };

    struct ServerUnitAddAuraPacket
    {
    public:
        ObjectGUID guid;
        u32 auraInstanceID;
        u32 spellID;
        f32 duration;
        u16 stacks;

    public:
        static inline std::string NAME = "ServerUnitAddAuraPacket";
        static constexpr u32 NAME_HASH = 1163012756;
        static constexpr u16 PACKET_ID = 36;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->Serialize(guid);
            failed |= !buffer->PutU32(auraInstanceID);
            failed |= !buffer->PutU32(spellID);
            failed |= !buffer->PutF32(duration);
            failed |= !buffer->PutU16(stacks);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->Deserialize(guid);
            failed |= !buffer->GetU32(auraInstanceID);
            failed |= !buffer->GetU32(spellID);
            failed |= !buffer->GetF32(duration);
            failed |= !buffer->GetU16(stacks);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += guid.GetCounterBytesUsed() + 1; // guid
            result += 4; // auraInstanceID
            result += 4; // spellID
            result += 4; // duration
            result += 2; // stacks

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "guid : " + guid.ToString() + ", ";
            tmp += "auraInstanceID : " + std::to_string(auraInstanceID) + ", ";
            tmp += "spellID : " + std::to_string(spellID) + ", ";
            tmp += "duration : " + std::to_string(duration) + ", ";
            tmp += "stacks : " + std::to_string(stacks);

            return tmp;
        }
    };

    struct ServerUnitUpdateAuraPacket
    {
    public:
        ObjectGUID guid;
        u32 auraInstanceID;
        f32 duration;
        u16 stacks;

    public:
        static inline std::string NAME = "ServerUnitUpdateAuraPacket";
        static constexpr u32 NAME_HASH = 2001797906;
        static constexpr u16 PACKET_ID = 37;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->Serialize(guid);
            failed |= !buffer->PutU32(auraInstanceID);
            failed |= !buffer->PutF32(duration);
            failed |= !buffer->PutU16(stacks);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->Deserialize(guid);
            failed |= !buffer->GetU32(auraInstanceID);
            failed |= !buffer->GetF32(duration);
            failed |= !buffer->GetU16(stacks);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += guid.GetCounterBytesUsed() + 1; // guid
            result += 4; // auraInstanceID
            result += 4; // duration
            result += 2; // stacks

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "guid : " + guid.ToString() + ", ";
            tmp += "auraInstanceID : " + std::to_string(auraInstanceID) + ", ";
            tmp += "duration : " + std::to_string(duration) + ", ";
            tmp += "stacks : " + std::to_string(stacks);

            return tmp;
        }
    };

    struct ServerUnitRemoveAuraPacket
    {
    public:
        ObjectGUID guid;
        u32 auraInstanceID;

    public:
        static inline std::string NAME = "ServerUnitRemoveAuraPacket";
        static constexpr u32 NAME_HASH = 1869256673;
        static constexpr u16 PACKET_ID = 38;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->Serialize(guid);
            failed |= !buffer->PutU32(auraInstanceID);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->Deserialize(guid);
            failed |= !buffer->GetU32(auraInstanceID);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += guid.GetCounterBytesUsed() + 1; // guid
            result += 4; // auraInstanceID

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "guid : " + guid.ToString() + ", ";
            tmp += "auraInstanceID : " + std::to_string(auraInstanceID);

            return tmp;
        }
    };

    struct ServerUnitSetMoverPacket
    {
    public:
        ObjectGUID guid;

    public:
        static inline std::string NAME = "ServerUnitSetMoverPacket";
        static constexpr u32 NAME_HASH = 2445654247;
        static constexpr u16 PACKET_ID = 39;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->Serialize(guid);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->Deserialize(guid);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += guid.GetCounterBytesUsed() + 1; // guid

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "guid : " + guid.ToString();

            return tmp;
        }
    };

    struct ServerUnitMovePacket
    {
    public:
        ObjectGUID guid;
        u32 movementFlags;
        vec3 position;
        vec2 pitchYaw;
        f32 verticalVelocity;

    public:
        static inline std::string NAME = "ServerUnitMovePacket";
        static constexpr u32 NAME_HASH = 4112331687;
        static constexpr u16 PACKET_ID = 40;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->Serialize(guid);
            failed |= !buffer->PutU32(movementFlags);
            failed |= !buffer->Put(position);
            failed |= !buffer->Put(pitchYaw);
            failed |= !buffer->PutF32(verticalVelocity);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->Deserialize(guid);
            failed |= !buffer->GetU32(movementFlags);
            failed |= !buffer->Get(position);
            failed |= !buffer->Get(pitchYaw);
            failed |= !buffer->GetF32(verticalVelocity);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += guid.GetCounterBytesUsed() + 1; // guid
            result += 4; // movementFlags
            result += 12; // position
            result += 8; // pitchYaw
            result += 4; // verticalVelocity

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "guid : " + guid.ToString() + ", ";
            tmp += "movementFlags : " + std::to_string(movementFlags) + ", ";
            tmp += std::string("position : ") + "(X : " + std::to_string(position.x) + ", Y : " + std::to_string(position.y) + ", Z : " + std::to_string(position.z) + ")" + ", ";
            tmp += std::string("pitchYaw : ") + "(X : " + std::to_string(pitchYaw.x) + ", Y : " + std::to_string(pitchYaw.y) + ")" + ", ";
            tmp += "verticalVelocity : " + std::to_string(verticalVelocity);

            return tmp;
        }
    };

    struct SharedUnitMoveStopPacket
    {
    public:
        ObjectGUID guid;

    public:
        static inline std::string NAME = "SharedUnitMoveStopPacket";
        static constexpr u32 NAME_HASH = 2993028497;
        static constexpr u16 PACKET_ID = 41;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->Serialize(guid);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->Deserialize(guid);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += guid.GetCounterBytesUsed() + 1; // guid

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "guid : " + guid.ToString();

            return tmp;
        }
    };

    struct ServerUnitTeleportPacket
    {
    public:
        ObjectGUID guid;
        vec3 position;
        f32 orientation;

    public:
        static inline std::string NAME = "ServerUnitTeleportPacket";
        static constexpr u32 NAME_HASH = 3086205777;
        static constexpr u16 PACKET_ID = 42;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->Serialize(guid);
            failed |= !buffer->Put(position);
            failed |= !buffer->PutF32(orientation);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->Deserialize(guid);
            failed |= !buffer->Get(position);
            failed |= !buffer->GetF32(orientation);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += guid.GetCounterBytesUsed() + 1; // guid
            result += 12; // position
            result += 4; // orientation

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "guid : " + guid.ToString() + ", ";
            tmp += std::string("position : ") + "(X : " + std::to_string(position.x) + ", Y : " + std::to_string(position.y) + ", Z : " + std::to_string(position.z) + ")" + ", ";
            tmp += "orientation : " + std::to_string(orientation);

            return tmp;
        }
    };

    struct ClientUnitMovePacket
    {
    public:
        u32 movementFlags;
        vec3 position;
        vec2 pitchYaw;
        f32 verticalVelocity;

    public:
        static inline std::string NAME = "ClientUnitMovePacket";
        static constexpr u32 NAME_HASH = 106575091;
        static constexpr u16 PACKET_ID = 43;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(movementFlags);
            failed |= !buffer->Put(position);
            failed |= !buffer->Put(pitchYaw);
            failed |= !buffer->PutF32(verticalVelocity);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(movementFlags);
            failed |= !buffer->Get(position);
            failed |= !buffer->Get(pitchYaw);
            failed |= !buffer->GetF32(verticalVelocity);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // movementFlags
            result += 12; // position
            result += 8; // pitchYaw
            result += 4; // verticalVelocity

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "movementFlags : " + std::to_string(movementFlags) + ", ";
            tmp += std::string("position : ") + "(X : " + std::to_string(position.x) + ", Y : " + std::to_string(position.y) + ", Z : " + std::to_string(position.z) + ")" + ", ";
            tmp += std::string("pitchYaw : ") + "(X : " + std::to_string(pitchYaw.x) + ", Y : " + std::to_string(pitchYaw.y) + ")" + ", ";
            tmp += "verticalVelocity : " + std::to_string(verticalVelocity);

            return tmp;
        }
    };

    struct ClientSendChatMessagePacket
    {
    public:
        std::string message;

    public:
        static inline std::string NAME = "ClientSendChatMessagePacket";
        static constexpr u32 NAME_HASH = 3124781967;
        static constexpr u16 PACKET_ID = 44;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutString(message);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetString(message);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += static_cast<u32>(message.size()) + 1; // message

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += std::string("message : ") + message;

            return tmp;
        }
    };

    struct ServerSendChatMessagePacket
    {
    public:
        ObjectGUID guid;
        std::string message;

    public:
        static inline std::string NAME = "ServerSendChatMessagePacket";
        static constexpr u32 NAME_HASH = 3991799539;
        static constexpr u16 PACKET_ID = 45;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->Serialize(guid);
            failed |= !buffer->PutString(message);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->Deserialize(guid);
            failed |= !buffer->GetString(message);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += guid.GetCounterBytesUsed() + 1; // guid
            result += static_cast<u32>(message.size()) + 1; // message

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "guid : " + guid.ToString() + ", ";
            tmp += std::string("message : ") + message;

            return tmp;
        }
    };

    struct ServerTriggerAddPacket
    {
    public:
        u32 triggerID;
        std::string name;
        u8 flags;
        u16 mapID;
        vec3 position;
        vec3 extents;

    public:
        static inline std::string NAME = "ServerTriggerAddPacket";
        static constexpr u32 NAME_HASH = 279437167;
        static constexpr u16 PACKET_ID = 46;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(triggerID);
            failed |= !buffer->PutString(name);
            failed |= !buffer->PutU8(flags);
            failed |= !buffer->PutU16(mapID);
            failed |= !buffer->Put(position);
            failed |= !buffer->Put(extents);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(triggerID);
            failed |= !buffer->GetString(name);
            failed |= !buffer->GetU8(flags);
            failed |= !buffer->GetU16(mapID);
            failed |= !buffer->Get(position);
            failed |= !buffer->Get(extents);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // triggerID
            result += static_cast<u32>(name.size()) + 1; // name
            result += 1; // flags
            result += 2; // mapID
            result += 12; // position
            result += 12; // extents

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "triggerID : " + std::to_string(triggerID) + ", ";
            tmp += std::string("name : ") + name + ", ";
            tmp += "flags : " + std::to_string(flags) + ", ";
            tmp += "mapID : " + std::to_string(mapID) + ", ";
            tmp += std::string("position : ") + "(X : " + std::to_string(position.x) + ", Y : " + std::to_string(position.y) + ", Z : " + std::to_string(position.z) + ")" + ", ";
            tmp += std::string("extents : ") + "(X : " + std::to_string(extents.x) + ", Y : " + std::to_string(extents.y) + ", Z : " + std::to_string(extents.z) + ")";

            return tmp;
        }
    };

    struct ServerTriggerRemovePacket
    {
    public:
        u32 triggerID;

    public:
        static inline std::string NAME = "ServerTriggerRemovePacket";
        static constexpr u32 NAME_HASH = 590074234;
        static constexpr u16 PACKET_ID = 47;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(triggerID);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(triggerID);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // triggerID

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "triggerID : " + std::to_string(triggerID);

            return tmp;
        }
    };

    struct ClientTriggerEnterPacket
    {
    public:
        u32 triggerID;

    public:
        static inline std::string NAME = "ClientTriggerEnterPacket";
        static constexpr u32 NAME_HASH = 2335390744;
        static constexpr u16 PACKET_ID = 48;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(triggerID);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(triggerID);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // triggerID

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "triggerID : " + std::to_string(triggerID);

            return tmp;
        }
    };

    struct ClientPathGeneratePacket
    {
    public:
        vec3 start;
        vec3 end;

    public:
        static inline std::string NAME = "ClientPathGeneratePacket";
        static constexpr u32 NAME_HASH = 386624494;
        static constexpr u16 PACKET_ID = 49;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->Put(start);
            failed |= !buffer->Put(end);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->Get(start);
            failed |= !buffer->Get(end);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 12; // start
            result += 12; // end

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += std::string("start : ") + "(X : " + std::to_string(start.x) + ", Y : " + std::to_string(start.y) + ", Z : " + std::to_string(start.z) + ")" + ", ";
            tmp += std::string("end : ") + "(X : " + std::to_string(end.x) + ", Y : " + std::to_string(end.y) + ", Z : " + std::to_string(end.z) + ")";

            return tmp;
        }
    };

    struct ServerPathVisualizationPacket
    {
    public:
        static inline std::string NAME = "ServerPathVisualizationPacket";
        static constexpr u32 NAME_HASH = 2438157837;
        static constexpr u16 PACKET_ID = 50;

        bool Serialize(Bytebuffer* buffer) const
        {
            return true;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            return true;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;
            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            return tmp;
        }
    };

    struct ClientSendCheatCommandPacket
    {
    public:
        u8 command;

    public:
        static inline std::string NAME = "ClientSendCheatCommandPacket";
        static constexpr u32 NAME_HASH = 3366849384;
        static constexpr u16 PACKET_ID = 51;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU8(command);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU8(command);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 1; // command

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "command : " + std::to_string(command);

            return tmp;
        }
    };

    struct ServerCheatCommandResultPacket
    {
    public:
        u8 command;
        u8 result;
        std::string response;

    public:
        static inline std::string NAME = "ServerCheatCommandResultPacket";
        static constexpr u32 NAME_HASH = 3427292017;
        static constexpr u16 PACKET_ID = 52;

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU8(command);
            failed |= !buffer->PutU8(result);
            failed |= !buffer->PutString(response);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU8(command);
            failed |= !buffer->GetU8(result);
            failed |= !buffer->GetString(response);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 1; // command
            result += 1; // result
            result += static_cast<u32>(response.size()) + 1; // response

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "command : " + std::to_string(command) + ", ";
            tmp += "result : " + std::to_string(result) + ", ";
            tmp += std::string("response : ") + response;

            return tmp;
        }
    };
}