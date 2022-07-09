#pragma once
#include <vector>
#include <atomic>
#include <shared_mutex>

template <typename T>
class SafeVectorScopedReadLock;

template <typename T>
class SafeVectorScopedWriteLock;

template <typename T>
class SafeVector
{
public:
    SafeVector() { }
    SafeVector(SafeVector& other)
    {
        std::unique_lock lock(_mutex);
        std::unique_lock otherLock(other._mutex);

        size_t newSize = other._vector.size();
        _vector.resize(newSize);

        memcpy(_vector.data(), other._vector.data(), newSize * sizeof(T));
    }

    void ReadLock(const std::function<void(const std::vector<T>&)> callback) const
    {
        std::shared_lock lock(_mutex);
        callback(_vector);
    }

    void WriteLock(const std::function<void(std::vector<T>&)> callback)
    {
        std::unique_lock lock(_mutex);
        callback(_vector);
    }

    const T ReadGet(size_t index) const
    {
        std::shared_lock lock(_mutex);
        return _vector[index];
    }

    // ReadGetUnsafe returns a ref compared to ReadGet which creates a copy
    // This function should only be used when you are absolutely sure no data races
    // can occur while you're using the ref.
    const T& ReadGetUnsafe(size_t index) const
    {
        std::shared_lock lock(_mutex);
        return _vector[index];
    }

    size_t Size() const
    {
        std::shared_lock lock(_mutex);
        return _vector.size();
    }

    void Resize(size_t newSize)
    {
        std::unique_lock lock(_mutex);
        _vector.resize(newSize);
    }

    void Reserve(size_t reserveSize)
    {
        std::unique_lock lock(_mutex);
        _vector.reserve(reserveSize);
    }

    void Clear()
    {
        std::unique_lock lock(_mutex);
        _vector.clear();
    }

    void PushBack(T& obj)
    {
        std::unique_lock lock(_mutex);
        _vector.push_back(obj);
    }

    void PushBack(const T& obj)
    {
        std::unique_lock lock(_mutex);
        _vector.push_back(obj);
    }

protected:
    friend class SafeVectorScopedReadLock<T>;
    friend class SafeVectorScopedWriteLock<T>;

    mutable std::shared_mutex _mutex;
    std::vector<T> _vector;
};

template <typename T>
class SafeVectorScopedReadLock
{
public:
    SafeVectorScopedReadLock(SafeVector<T>& safeVector) : _safeVector(safeVector)
    {
        _safeVector._mutex.lock_shared();
    }
    ~SafeVectorScopedReadLock()
    {
        _safeVector._mutex.unlock_shared();
    }

    const std::vector<T>& Get() { return _safeVector._vector; }

private:
    SafeVector<T>& _safeVector;
};

template <typename T>
class SafeVectorScopedWriteLock
{
public:
    SafeVectorScopedWriteLock(SafeVector<T>& safeVector) : _safeVector(safeVector)
    {
        _safeVector._mutex.lock();
    }
    ~SafeVectorScopedWriteLock()
    {
        _safeVector._mutex.unlock();
    }

    std::vector<T>& Get() { return _safeVector._vector; }

private:
    SafeVector<T>& _safeVector;
};