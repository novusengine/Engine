#pragma once
#include "Descriptors/BufferDesc.h"

#include <Base/Types.h>
#include <Base/Container/BitSet.h>

namespace Renderer
{
    enum class AccessType : u8
    {
        READ,
        WRITE
    };

    enum class PipelineType : u8
    {
        GRAPHICS,
        COMPUTE,
        BOTH
    };

    inline const std::string GetBufferPassUsageName(BufferPassUsage bufferPassUsage)
    {
        if (bufferPassUsage == BufferPassUsage::TRANSFER) return "TRANSFER";
        if (bufferPassUsage == BufferPassUsage::GRAPHICS) return "GRAPHICS";
        if (bufferPassUsage == BufferPassUsage::COMPUTE) return "COMPUTE";

        return "COMBINED";
    }

    class TrackedBufferBitSets
    {
    public:
        TrackedBufferBitSets(Memory::Allocator* allocator, u32 numBufferSets)
            : _numBufferSets(numBufferSets)
            , _readBitSet(allocator, numBufferSets)
            , _writeBitSet(allocator, numBufferSets)
            , _transferBitSet(allocator, numBufferSets)
            , _graphicsBitSet(allocator, numBufferSets)
            , _computeBitSet(allocator, numBufferSets)
        {

        }

        void Add(BufferID bufferID, AccessType accessType, BufferPassUsage bufferPassUsage)
        {
            BufferID::type index = static_cast<BufferID::type>(bufferID);

            u32 setIndex = index / 64;
            u32 bitIndex = index % 64;

            if (accessType == AccessType::READ)
            {
                _readBitSet.Set(setIndex, bitIndex);
            }

            if (accessType == AccessType::WRITE)
            {
                // Write should actually allow both read and write
                _readBitSet.Set(setIndex, bitIndex);
                _writeBitSet.Set(setIndex, bitIndex);
            }

            if ((bufferPassUsage & BufferPassUsage::TRANSFER) == BufferPassUsage::TRANSFER)
            {
                _transferBitSet.Set(setIndex, bitIndex);
            }

            if ((bufferPassUsage & BufferPassUsage::GRAPHICS) == BufferPassUsage::GRAPHICS)
            {
                _graphicsBitSet.Set(setIndex, bitIndex);
            }

            if ((bufferPassUsage & BufferPassUsage::COMPUTE) == BufferPassUsage::COMPUTE)
            {
                _computeBitSet.Set(setIndex, bitIndex);
            }
        }

        bool Has(BufferID bufferID, BufferPassUsage bufferPassUsage) const
        {
            BufferID::type index = static_cast<BufferID::type>(bufferID);

            u32 setIndex = index / 64;
            u32 bitIndex = index % 64;

            if ((bufferPassUsage & BufferPassUsage::TRANSFER) == BufferPassUsage::TRANSFER)
            {
                if (!_transferBitSet.Has(setIndex, bitIndex))
                    return false;
            }
            if ((bufferPassUsage & BufferPassUsage::GRAPHICS) == BufferPassUsage::GRAPHICS)
            {
                if (!_graphicsBitSet.Has(setIndex, bitIndex))
                    return false;
            }
            if ((bufferPassUsage & BufferPassUsage::COMPUTE) == BufferPassUsage::COMPUTE)
            {
                if (!_computeBitSet.Has(setIndex, bitIndex))
                    return false;
            }

            return true;
        }

        bool Has(BufferID bufferID, AccessType accessType, BufferPassUsage bufferPassUsage) const
        {
            BufferID::type index = static_cast<BufferID::type>(bufferID);

            u32 setIndex = index / 64;
            u32 bitIndex = index % 64;

            // If the bits for this BufferID does not match accessType and bufferPassUsage, we return false
            if (accessType == AccessType::READ)
            {
                if (!_readBitSet.Has(setIndex, bitIndex))
                    return false;
            }
            if (accessType == AccessType::WRITE)
            {
                if (!_writeBitSet.Has(setIndex, bitIndex))
                    return false;
            }

            if (!Has(bufferID, bufferPassUsage))
                return false;

            return true;
        }

        u32 GetNumBufferSets() const { return _numBufferSets; }

        const BitSet& GetReadBitSet() const { return _readBitSet; }
        const BitSet& GetWriteBitSet() const { return _writeBitSet; }

        const BitSet& GetTransferBitSet() const { return _transferBitSet; }
        const BitSet& GetGraphicsBitSet() const { return _graphicsBitSet; }
        const BitSet& GetComputeBitSet() const { return _computeBitSet; }
    private:
        u32 _numBufferSets;

        // Access
        BitSet _readBitSet;
        BitSet _writeBitSet;

        // Usage
        BitSet _transferBitSet;
        BitSet _graphicsBitSet;
        BitSet _computeBitSet;
    };
}