#include "BitSet.h"
#include <bit>

BitSet::BitSet(Memory::Allocator* allocator, u32 numSets)
    : _allocator(allocator)
    , _bitSet(allocator, numSets, numSets)
{
    Reset();
}

void BitSet::SetEquals(const BitSet& other)
{
    // Guard self assignment
    if (this == &other)
        return;

    memcpy(&_bitSet[0], &other._bitSet[0], sizeof(u64) * _bitSet.Count());
}

void BitSet::Reset()
{
    memset(&_bitSet[0], 0, sizeof(u64) * _bitSet.Count());
}

void BitSet::Set(u32 index)
{
    u32 setIndex = index / 64;
    u32 bitIndex = index % 64;

    Set(setIndex, bitIndex);
}

void BitSet::Set(u32 setIndex, u32 bitIndex)
{
    _bitSet[setIndex] |= 1ull << bitIndex;
}

void BitSet::Unset(u32 index)
{
    u32 setIndex = index / 64;
    u32 bitIndex = index % 64;

    Unset(setIndex, bitIndex);
}

void BitSet::Unset(u32 setIndex, u32 bitIndex)
{
    u64 inverseMask = ~(1ull << bitIndex);
    _bitSet[setIndex] &= inverseMask;
}

bool BitSet::Has(u32 index) const
{
    u32 setIndex = index / 64;
    u32 bitIndex = index % 64;
    return Has(setIndex, bitIndex);
}

bool BitSet::Has(u32 setIndex, u32 bitIndex) const
{
    return (_bitSet[setIndex] & (1ull << bitIndex)) != 0;
}

u64 BitSet::GetBitSet(u32 index) const
{
    return _bitSet[index];
}

bool BitSet::IsSubsetOf(const BitSet& other) const
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

void BitSet::BitwiseUnset(const BitSet& other)
{
    u32 numBitSets = static_cast<u32>(_bitSet.Count());

    for (u32 i = 0; i < numBitSets; i++)
    {
        _bitSet[i] &= (~other._bitSet[i]);
    }
}

void BitSet::BitwiseAND(const BitSet& other)
{
    u32 numBitSets = static_cast<u32>(_bitSet.Count());

    for (u32 i = 0; i < numBitSets; i++)
    {
        _bitSet[i] &= other._bitSet[i];
    }
}

void BitSet::BitwiseOR(const BitSet& other)
{
    u32 numBitSets = static_cast<u32>(_bitSet.Count());

    for (u32 i = 0; i < numBitSets; i++)
    {
        _bitSet[i] |= other._bitSet[i];
    }
}

void BitSet::BitwiseXOR(const BitSet& other)
{
    u32 numBitSets = static_cast<u32>(_bitSet.Count());

    for (u32 i = 0; i < numBitSets; i++)
    {
        _bitSet[i] ^= other._bitSet[i];
    }
}

BitSet* BitSet::NewBitwiseUnset(const BitSet& other) const
{
    u32 numBitSets = static_cast<u32>(_bitSet.Count());

    BitSet* result = Memory::Allocator::New<BitSet>(_allocator, _allocator, numBitSets);
    *result = *this;

    result->BitwiseUnset(other);

    return result;
}

BitSet* BitSet::NewBitwiseAND(const BitSet& other) const
{
    u32 numBitSets = static_cast<u32>(_bitSet.Count());

    BitSet* result = Memory::Allocator::New<BitSet>(_allocator, _allocator, numBitSets);
    *result = *this;

    result->BitwiseAND(other);

    return result;
}

BitSet* BitSet::NewBitwiseOR(const BitSet& other) const
{
    u32 numBitSets = static_cast<u32>(_bitSet.Count());

    BitSet* result = Memory::Allocator::New<BitSet>(_allocator, _allocator, numBitSets);
    *result = *this;

    result->BitwiseOR(other);

    return result;
}

BitSet* BitSet::NewBitwiseXOR(const BitSet& other) const
{
    u32 numBitSets = static_cast<u32>(_bitSet.Count());

    BitSet* result = Memory::Allocator::New<BitSet>(_allocator, _allocator, numBitSets);
    *result = *this;

    result->BitwiseXOR(other);

    return result;
}

u32 BitSet::NumBitSets() const
{
    return static_cast<u32>(_bitSet.Count());
}

void BitSet::ForEachSetBit(std::function<void(u32 set, u32 bit)> callback) const
{
    u32 numBitSets = NumBitSets();
    for (u32 i = 0; i < numBitSets; i++)
    {
        u64 bits = _bitSet[i];
        while (bits != 0)
        {
            u32 bit = static_cast<u32>(std::countr_zero(bits));
            callback(i, bit);
            bits &= bits - 1; // Clear lowest set bit
        }
    }
}
