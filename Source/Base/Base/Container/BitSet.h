#pragma once
#include <Base/Types.h>
#include <Base/Container/DynamicArray.h>

class BitSet
{
public:
    BitSet(Memory::Allocator* allocator, u32 numSets)
        : _allocator(allocator)
        , _bitSet(allocator, numSets, numSets)
    {
        Reset();
    }

    void SetEquals(const BitSet& other)
    {
        // Guard self assignment
        if (this == &other)
            return;

        memcpy(&_bitSet[0], &other._bitSet[0], sizeof(u64) * _bitSet.Count());
    }

    // Resets all bits back to 0
    void Reset()
    {
        memset(&_bitSet[0], 0, sizeof(u64) * _bitSet.Count());
    }

    void Set(u32 index)
    {
        u32 setIndex = index / 64;
        u32 bitIndex = index % 64;

        Set(setIndex, bitIndex);
    }

    void Set(u32 setIndex, u32 bitIndex)
    {
        _bitSet[setIndex] |= 1ull << bitIndex;
    }

    void Unset(u32 index)
    {
        u32 setIndex = index / 64;
        u32 bitIndex = index % 64;

        Unset(setIndex, bitIndex);
    }

    void Unset(u32 setIndex, u32 bitIndex)
    {
        u64 inverseMask = ~(1ull << bitIndex);
        _bitSet[setIndex] &= inverseMask;
    }

    bool Has(u32 index) const
    {
        u32 setIndex = index / 64;
        u32 bitIndex = index % 64;
        return Has(setIndex, bitIndex);
    }

    bool Has(u32 setIndex, u32 bitIndex) const
    {
        return (_bitSet[setIndex] & (1ull << bitIndex)) != 0;
    }

    u64 GetBitSet(u32 index)
    {
        return _bitSet[index];
    }

    // Returns true if this bitset is a subset of the other bitset
    bool IsSubsetOf(const BitSet& other) const
    {
        u32 numBitSets = static_cast<u32>(_bitSet.Count());

        // If they are different size, it is not a subset
        if (numBitSets != static_cast<u32>(other._bitSet.Count()))
            return false;

        for (u32 i = 0; i < numBitSets; i++)
        {
            u64 a = _bitSet[i];
            u64 b = other._bitSet[i];

            if ((a & b) != a)
                return false;
        }

        return true;
    }

    void BitwiseUnset(const BitSet& other)
    {
        u32 numBitSets = static_cast<u32>(_bitSet.Count());

        for (u32 i = 0; i < numBitSets; i++)
        {
            _bitSet[i] &= (~other._bitSet[i]);
        }
    }

    void BitwiseAND(const BitSet& other)
    {
        u32 numBitSets = static_cast<u32>(_bitSet.Count());

        for (u32 i = 0; i < numBitSets; i++)
        {
            _bitSet[i] &= other._bitSet[i];
        }
    }

    void BitwiseOR(const BitSet& other)
    {
        u32 numBitSets = static_cast<u32>(_bitSet.Count());

        for (u32 i = 0; i < numBitSets; i++)
        {
            _bitSet[i] |= other._bitSet[i];
        }
    }

    void BitwiseXOR(const BitSet& other)
    {
        u32 numBitSets = static_cast<u32>(_bitSet.Count());

        for (u32 i = 0; i < numBitSets; i++)
        {
            _bitSet[i] ^= other._bitSet[i];
        }
    }

    BitSet* NewBitwiseUnset(const BitSet& other) const
    {
        u32 numBitSets = static_cast<u32>(_bitSet.Count());

        BitSet* result = Memory::Allocator::New<BitSet>(_allocator, _allocator, numBitSets);
        *result = *this;

        result->BitwiseUnset(other);

        return result;
    }

    BitSet* NewBitwiseAND(const BitSet& other) const
    {
        u32 numBitSets = static_cast<u32>(_bitSet.Count());

        BitSet* result = Memory::Allocator::New<BitSet>(_allocator, _allocator, numBitSets);
        *result = *this;

        result->BitwiseAND(other);

        return result;
    }

    BitSet* NewBitwiseOR(const BitSet& other) const
    {
        u32 numBitSets = static_cast<u32>(_bitSet.Count());

        BitSet* result = Memory::Allocator::New<BitSet>(_allocator, _allocator, numBitSets);
        *result = *this;

        result->BitwiseOR(other);

        return result;
    }

    BitSet* NewBitwiseXOR(const BitSet& other) const
    {
        u32 numBitSets = static_cast<u32>(_bitSet.Count());

        BitSet* result = Memory::Allocator::New<BitSet>(_allocator, _allocator, numBitSets);
        *result = *this;

        result->BitwiseXOR(other);

        return result;
    }

    u32 NumBitSets() const { return static_cast<u32>(_bitSet.Count()); }

    void ForEachSetBit(std::function<void(u32 set, u32 bit)> callback) const
    {
        u32 numBitSets = NumBitSets();
        for (u32 i = 0; i < numBitSets; i++)
        {
            u64 set = _bitSet[i];

            for (u32 bit = 0; bit < 64; bit++)
            {
                bool bitIsSet = (set >> bit) & 0x1;

                if (bitIsSet)
                {
                    callback(i, bit);
                }
            }
        }
    }

private:
    Memory::Allocator* _allocator;
    DynamicArray<u64> _bitSet;
};