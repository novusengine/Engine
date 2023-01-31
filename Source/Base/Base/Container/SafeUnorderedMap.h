#pragma once
#include <robinhood/robinhood.h>
#include <shared_mutex>

template <typename T, typename U>
class SafeUnorderedMapScopedReadLock;

template <typename T, typename U>
class SafeUnorderedMapScopedWriteLock;

template <typename T, typename U>
class SafeUnorderedMap
{
public:
    void ReadLock(const std::function<void(const robin_hood::unordered_map<T, U>&)> callback)
    {
        std::shared_lock<std::shared_mutex> lock(SafeUnorderedMap<T, U>::_mutex);
        callback(_unorderedMap);
    }
    void WriteLock(const std::function<void(robin_hood::unordered_map<T, U>&)> callback)
    {
        std::unique_lock<std::shared_mutex> lock(SafeUnorderedMap<T, U>::_mutex);
        callback(_unorderedMap);
    }

    void Clear()
    {
        std::unique_lock<std::shared_mutex> lock(SafeUnorderedMap<T, U>::_mutex);
        _unorderedMap.clear();
    }

    void Add(T key, U& value)
    {
        std::unique_lock<std::shared_mutex> lock(SafeUnorderedMap<T, U>::_mutex);
        _unorderedMap[key] = value;
    }

    bool TryGetUnsafe(T key, U& value)
    {
        std::shared_lock<std::shared_mutex> lock(SafeUnorderedMap<T, U>::_mutex);

        auto itr = _unorderedMap.find(key);
        if (itr == _unorderedMap.end())
            return false;

        value = itr->second;
        return true;
    }

private:
    friend class SafeUnorderedMapScopedReadLock<T, U>;
    friend class SafeUnorderedMapScopedWriteLock<T, U>;

    std::shared_mutex _mutex;
    robin_hood::unordered_map<T, U> _unorderedMap;
};

template <typename T, typename U>
class SafeUnorderedMapScopedReadLock
{
public:
    SafeUnorderedMapScopedReadLock(SafeUnorderedMap<T, U>& safeUnorderedMap) : _safeUnorderedMap(safeUnorderedMap)
    {
        _safeUnorderedMap._mutex.lock_shared();
    }
    ~SafeUnorderedMapScopedReadLock()
    {
        _safeUnorderedMap._mutex.unlock_shared();
    }

    const robin_hood::unordered_map<T, U>& Get() { return _safeUnorderedMap._unorderedMap; }

private:
    SafeUnorderedMap<T, U>& _safeUnorderedMap;
};

template <typename T, typename U>
class SafeUnorderedMapScopedWriteLock
{
public:
    SafeUnorderedMapScopedWriteLock(SafeUnorderedMap<T, U>& safeUnorderedMap) : _safeUnorderedMap(safeUnorderedMap)
    {
        _safeUnorderedMap._mutex.lock();
    }
    ~SafeUnorderedMapScopedWriteLock()
    {
        _safeUnorderedMap._mutex.unlock();
    }

    robin_hood::unordered_map<T, U>& Get() { return _safeUnorderedMap._unorderedMap; }

private:
    SafeUnorderedMap<T, U>& _safeUnorderedMap;
};