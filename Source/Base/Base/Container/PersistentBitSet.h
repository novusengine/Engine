#pragma once
#include <Base/Types.h>
#include <vector>
#include <functional>

class BitSet; // Forward declaration

class PersistentBitSet
{
public:
    PersistentBitSet() = default;

    // Resets all bits to 0 (keeps capacity)
    void Reset();

    // Set bit at linear index (grows if needed)
    void Set(u32 index);

    // Set bit at (setIndex, bitIndex) - grows if needed
    void Set(u32 setIndex, u32 bitIndex);

    // Unset bit at linear index
    void Unset(u32 index);

    // Unset bit at (setIndex, bitIndex)
    void Unset(u32 setIndex, u32 bitIndex);

    // Check bit at linear index
    [[nodiscard]] bool Has(u32 index) const;

    // Check bit at (setIndex, bitIndex)
    [[nodiscard]] bool Has(u32 setIndex, u32 bitIndex) const;

    [[nodiscard]] bool IsEmpty() const { return _bitSet.empty(); }
    [[nodiscard]] u32 NumBitSets() const { return static_cast<u32>(_bitSet.size()); }

    // Returns true if any bit is set
    [[nodiscard]] bool HasAnyBitSet() const;

    // Get raw u64 at index (returns 0 if out of bounds)
    [[nodiscard]] u64 GetBitSet(u32 index) const;

    // Copy bits from another PersistentBitSet
    void SetEquals(const PersistentBitSet& other);

    // Copy bits from a BitSet (defined in .cpp to avoid circular include)
    void SetEquals(const BitSet& other);

    // Check if this is a subset of a BitSet (defined in .cpp to avoid circular include)
    [[nodiscard]] bool IsSubsetOf(const BitSet& other) const;

    // Check if this is a subset of another PersistentBitSet
    [[nodiscard]] bool IsSubsetOf(const PersistentBitSet& other) const;

    // Bitwise operations (grows to match other's size if needed)
    void BitwiseUnset(const PersistentBitSet& other);

    // BitwiseUnset with BitSet (defined in .cpp to avoid circular include)
    void BitwiseUnset(const BitSet& other);

    void BitwiseAND(const PersistentBitSet& other);
    void BitwiseOR(const PersistentBitSet& other);
    void BitwiseXOR(const PersistentBitSet& other);

    // Efficient iteration using countr_zero (only visits set bits)
    void ForEachSetBit(std::function<void(u32 set, u32 bit)> callback) const;

private:
    void EnsureCapacity(u32 setIndex);

    std::vector<u64> _bitSet;
};
