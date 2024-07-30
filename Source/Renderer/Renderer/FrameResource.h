#pragma once
#include <array>
#include <cstddef>

template <typename T, size_t NumFrames>
struct FrameResource
{
    T& Get(size_t frame)
    {
        return items[frame % NumFrames];
    }

    std::array<T, NumFrames> items;

    static constexpr size_t Num = NumFrames;
};