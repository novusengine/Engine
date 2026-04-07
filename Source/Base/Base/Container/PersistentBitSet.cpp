#include "PersistentBitSet.h"
#include "BitSet.h"
#include <algorithm>
#include <bit>

void PersistentBitSet::Reset()
{
    std::fill(_bitSet.begin(), _bitSet.end(), 0ull);
}

void PersistentBitSet::Set(u32 index)
{
    u32 setIndex = index / 64;
    u32 bitIndex = index % 64;
    EnsureCapacity(setIndex);
    _bitSet[setIndex] |= 1ull << bitIndex;
}

void PersistentBitSet::Set(u32 setIndex, u32 bitIndex)
{
    EnsureCapacity(setIndex);
    _bitSet[setIndex] |= 1ull << bitIndex;
}

void PersistentBitSet::Unset(u32 index)
{
    u32 setIndex = index / 64;
    u32 bitIndex = index % 64;
    if (setIndex < _bitSet.size())
        _bitSet[setIndex] &= ~(1ull << bitIndex);
}

void PersistentBitSet::Unset(u32 setIndex, u32 bitIndex)
{
    if (setIndex < _bitSet.size())
        _bitSet[setIndex] &= ~(1ull << bitIndex);
}

bool PersistentBitSet::Has(u32 index) const
{
    u32 setIndex = index / 64;
    u32 bitIndex = index % 64;
    if (setIndex >= _bitSet.size()) [[unlikely]]
        return false;
    return (_bitSet[setIndex] & (1ull << bitIndex)) != 0;
}

bool PersistentBitSet::Has(u32 setIndex, u32 bitIndex) const
{
    if (setIndex >= _bitSet.size()) [[unlikely]]
        return false;
    return (_bitSet[setIndex] & (1ull << bitIndex)) != 0;
}

bool PersistentBitSet::HasAnyBitSet() const
{
    u64 combined = 0;
    for (u64 bits : _bitSet)
        combined |= bits;
    return combined != 0;
}

u64 PersistentBitSet::GetBitSet(u32 index) const
{
    return (index < _bitSet.size()) ? _bitSet[index] : 0ull;
}

void PersistentBitSet::SetEquals(const PersistentBitSet& other)
{
    _bitSet = other._bitSet;
}

void PersistentBitSet::SetEquals(const BitSet& other)
{
    u32 numBitSets = other.NumBitSets();
    _bitSet.resize(numBitSets);

    for (u32 i = 0; i < numBitSets; i++)
    {
        _bitSet[i] = other.GetBitSet(i);
    }
}

bool PersistentBitSet::IsSubsetOf(const BitSet& other) const
{
    u32 otherNumSets = other.NumBitSets();

    for (u32 i = 0; i < _bitSet.size(); i++)
    {
        u64 ourBits = _bitSet[i];
        // If we have bits beyond other's capacity, they can't be subset
        u64 otherBits = (i < otherNumSets) ? other.GetBitSet(i) : 0ull;

        if ((ourBits & otherBits) != ourBits)
            return false;
    }
    return true;
}

bool PersistentBitSet::IsSubsetOf(const PersistentBitSet& other) const
{
    for (u32 i = 0; i < _bitSet.size(); i++)
    {
        u64 ourBits = _bitSet[i];
        u64 otherBits = other.GetBitSet(i);
        if ((ourBits & otherBits) != ourBits)
            return false;
    }
    return true;
}

void PersistentBitSet::BitwiseUnset(const PersistentBitSet& other)
{
    u32 count = std::min(static_cast<u32>(_bitSet.size()), other.NumBitSets());
    for (u32 i = 0; i < count; i++)
        _bitSet[i] &= ~other._bitSet[i];
}

void PersistentBitSet::BitwiseUnset(const BitSet& other)
{
    u32 otherNumSets = other.NumBitSets();
    u32 count = std::min(static_cast<u32>(_bitSet.size()), otherNumSets);

    for (u32 i = 0; i < count; i++)
    {
        _bitSet[i] &= ~other.GetBitSet(i);
    }
}

void PersistentBitSet::BitwiseAND(const PersistentBitSet& other)
{
    u32 count = std::min(static_cast<u32>(_bitSet.size()), other.NumBitSets());
    for (u32 i = 0; i < count; i++)
        _bitSet[i] &= other._bitSet[i];
    // Bits beyond other's size become 0 (AND with implicit 0)
    for (u32 i = count; i < _bitSet.size(); i++)
        _bitSet[i] = 0;
}

void PersistentBitSet::BitwiseOR(const PersistentBitSet& other)
{
    if (other.NumBitSets() > _bitSet.size())
        _bitSet.resize(other.NumBitSets(), 0ull);
    for (u32 i = 0; i < other.NumBitSets(); i++)
        _bitSet[i] |= other._bitSet[i];
}

void PersistentBitSet::BitwiseXOR(const PersistentBitSet& other)
{
    if (other.NumBitSets() > _bitSet.size())
        _bitSet.resize(other.NumBitSets(), 0ull);
    for (u32 i = 0; i < other.NumBitSets(); i++)
        _bitSet[i] ^= other._bitSet[i];
}

void PersistentBitSet::ForEachSetBit(std::function<void(u32 set, u32 bit)> callback) const
{
    for (u32 i = 0; i < _bitSet.size(); i++)
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

void PersistentBitSet::EnsureCapacity(u32 setIndex)
{
    if (setIndex >= _bitSet.size()) [[unlikely]]
        _bitSet.resize(setIndex + 1, 0ull);
}
