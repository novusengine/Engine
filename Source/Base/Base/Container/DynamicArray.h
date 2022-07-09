#pragma once
#include "Base/Types.h"
#include "Base/Memory/Allocator.h"
#include "Base/Math/Math.h"

constexpr float GrowFactor = 2.0f;

template <typename T>
class DynamicArray
{
public:
    DynamicArray(Memory::Allocator* allocator, size_t capacity = 32)
        : _allocator(allocator)
        , _data(nullptr)
        , _count(0)
        , _capacity(capacity)
    {
        _data = Memory::Allocator::NewArray<T>(allocator, capacity);
    }

    // Insert an object into the array
    void Insert(const T& item)
    {
        // Grow if we have to
        if (_capacity <= _count)
        {
            Grow();
        }

        // Insert the item at the end
        _data[_count] = item;

        // Increase our size
        _count++;
    }

    // Remove an object at a certain index from the array, compressing the array while we're at it
    void RemoveAt(size_t index)
    {
        assert(_count > index);

        for (int i = index; index < _count; i++)
        {
            if (i + 1 < _count)
            {
                _data[i] = _data[i + 1];
            }
        }

        _count--;
    }

    size_t Count()
    {
        return _count;
    }

    size_t Capacity()
    {
        return _capacity;
    }

    T& operator[](size_t index)
    {
        return _data[index];
    }

    // Range-based For loop support
public:
    class iterator
    {
    public:
        iterator(T* ptr) : ptr(ptr) {}

        iterator operator++() { ++ptr; return *this; }
        bool operator!=(const iterator& other) const { return ptr != other.ptr; }
        T& operator*() const { return *ptr; }
    private:
        T* ptr;
    };

    iterator begin() const { return iterator(_data); }
    iterator end() const { return iterator(_data + _count); }

private:
    void Grow()
    {
        // Create a new, double as large array
        size_t newCapacity = Math::FloorToInt(_capacity * GrowFactor);
        T* newArray = Memory::Allocator::NewArray<T>(_allocator, newCapacity);

        // Memcopy the old one into the new one
        memcpy(newArray, _data, _capacity * sizeof(T));

        // Delete the previous
        _allocator->Free(_data);

        // Make our variables correct again
        _data = newArray;
        _capacity = newCapacity;
    }

private:
    Memory::Allocator* _allocator;
    T* _data;
    size_t _count;
    size_t _capacity;
};