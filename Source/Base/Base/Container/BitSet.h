#pragma once
#include <Base/Types.h>
#include <Base/Container/DynamicArray.h>

class BitSet
{
public:
    BitSet(Memory::Allocator* allocator, u32 numSets);

    void SetEquals(const BitSet& other);

    // Resets all bits back to 0
    void Reset();

    void Set(u32 index);
    void Set(u32 setIndex, u32 bitIndex);

    void Unset(u32 index);
    void Unset(u32 setIndex, u32 bitIndex);

    bool Has(u32 index) const;
    bool Has(u32 setIndex, u32 bitIndex) const;

    u64 GetBitSet(u32 index) const;

    // Returns true if this bitset is a subset of the other bitset
    bool IsSubsetOf(const BitSet& other) const;

    void BitwiseUnset(const BitSet& other);
    void BitwiseAND(const BitSet& other);
    void BitwiseOR(const BitSet& other);
    void BitwiseXOR(const BitSet& other);

    BitSet* NewBitwiseUnset(const BitSet& other) const;
    BitSet* NewBitwiseAND(const BitSet& other) const;
    BitSet* NewBitwiseOR(const BitSet& other) const;
    BitSet* NewBitwiseXOR(const BitSet& other) const;

    u32 NumBitSets() const;

    void ForEachSetBit(std::function<void(u32 set, u32 bit)> callback) const;

private:
    Memory::Allocator* _allocator;
    DynamicArray<u64> _bitSet;
};
