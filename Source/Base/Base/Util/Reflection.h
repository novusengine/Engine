#pragma once
#include <Base/Types.h>

#include <refl-cpp/refl.hpp>

namespace Reflection
{
    struct ReadOnly : refl::attr::usage::field {};
    struct Hidden : refl::attr::usage::field, refl::attr::usage::function {};
    struct DragSpeed : refl::attr::usage::field
    {
        constexpr DragSpeed(f32 speed) : speed(speed) {}
        f32 speed;
    };
}