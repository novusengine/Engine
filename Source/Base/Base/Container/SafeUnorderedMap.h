#pragma once
#include <robin_hood.h>
#include <shared_mutex>

template <typename T, typename U>
class SafeUnorderedMap
{
public:
    void ReadLock(const std::function<void(const robin_hood::unordered_map<T, U>&)> callback)
    {
        std::shared_lock lock(_mutex);
        callback(_unorderedMap);
    }
    void WriteLock(const std::function<void(robin_hood::unordered_map<T, U>&)> callback)
    {
        std::unique_lock lock(_mutex);
        callback(_unorderedMap);
    }

    void Clear()
    {
        std::unique_lock lock(_mutex);
        _unorderedMap.clear();
    }

    void Add(T key, U& value)
    {
        std::unique_lock lock(_mutex);
        _unorderedMap[key] = value;
    }

    bool TryGetUnsafe(T key, U& value)
    {
        std::shared_lock lock(_mutex);

        auto itr = _unorderedMap.find(key);
        if (itr == _unorderedMap.end())
            return false;

        value = itr->second;
        return true;
    }

private:
    std::shared_mutex _mutex;
    robin_hood::unordered_map<T, U> _unorderedMap;
};