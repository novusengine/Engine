#pragma once
#include "Base/Types.h"

namespace Math
{
    constexpr f32 PI = 3.1415926535f;
    constexpr f32 HALF_PI = PI / 2.0f;
    constexpr f32 TAU = 6.2831853071f; // Clearly superior
    constexpr f32 INV_TAU = 1.0f / TAU;

    f32 Sqrt(f32 in);

    constexpr f32 DegToRad(f32 deg)
    {
        return (deg * PI) / 180.0f;
    }

    constexpr f32 RadToDeg(f32 rad)
    {
        return (rad * 180.0f) / PI;
    }

    inline u32 FloorToInt(f32 x)
    {
        i32 xi = (i32)x;
        return x < xi ? xi - 1 : xi;
    }

    inline f32 Floor(f32 x)
    {
        return static_cast<f32>(FloorToInt(x));
    }

    inline u32 RoofToInt(f32 x)
    {
        i32 xi = (i32)x;
        return x < xi ? xi + 1 : xi;
    }

    inline u32 RoofToInt(f64 x)
    {
        i32 xi = (i32)x;
        return x < xi ? xi + 1 : xi;
    }

    inline f32 Roof(f32 x)
    {
        return static_cast<f32>(RoofToInt(x));
    }

    inline f64 Roof(f64 x)
    {
        return static_cast<f64>(RoofToInt(x));
    }

    inline f32 Modulus(f32 a, f32 b)
    {
        return (a - b * Floor(a / b));
    }

    inline i32 Abs(i32 x)
    {
        return x < 0 ? -x : x;
    }

    inline f32 Abs(f32 x)
    {
        return x < 0 ? -x : x;
    }

    inline f32 Hill(f32 x)
    {
        const f32 a0 = 1.0f;
        const f32 a2 = 2.0f / PI - 12.0f / (PI * PI);
        const f32 a3 = 16.0f / (PI * PI * PI) - 4.0f / (PI * PI);
        const f32 xx = x * x;
        const f32 xxx = xx * x;
        return a0 + a2 * xx + a3 * xxx;
    }

    inline f32 Sin(f32 x)
    {
        const f32 a = x * INV_TAU;
        x -= static_cast<i32>(a) * TAU;
        if (x < 0.0f)
            x += TAU;

        // 4 pieces of hills
        if (x < HALF_PI)
            return Hill(HALF_PI - x);
        else if (x < PI)
            return Hill(x - HALF_PI);
        else if (x < 3.0f * HALF_PI)
            return -Hill(3.0f * HALF_PI - x);
        else
            return -Hill(x - 3.0f * HALF_PI);
    }

    inline f32 Cos(f32 x)
    {
        return Sin(x + HALF_PI);
    }

    f32 Tan(f32 x);

    inline f32 Min(f32 a, f32 b)
    {
        return (a <= b) ? a : b;
    }

    inline f32 Max(f32 a, f32 b)
    {
        return (a >= b) ? a : b;
    }

    inline i32 Min(i32 a, i32 b)
    {
        return (a <= b) ? a : b;
    }

    inline i32 Max(i32 a, i32 b)
    {
        return (a >= b) ? a : b;
    }

    inline size_t Min(size_t a, size_t b)
    {
        return (a <= b) ? a : b;
    }

    inline size_t Max(size_t a, size_t b)
    {
        return (a >= b) ? a : b;
    }

    inline f32 Clamp(f32 x, f32 min, f32 max)
    {
        return Max(Min(x, max), min);
    }

    inline i32 Clamp(i32 x, i32 min, i32 max)
    {
        return Max(Min(x, max), min);
    }

    inline size_t Clamp(size_t x, size_t min, size_t max)
    {
        return Max(Min(x, max), min);
    }

    inline f32 Lerp(f32 start, f32 end, f32 t)
    {
        return (start + t * (end - start));
    }

    inline f32 Map(f32 value, f32 originalMin, f32 originalMax, f32 newMin, f32 newMax)
    {
        return (value - originalMin) / (originalMax - originalMin) * (newMax - newMin) + newMin;
    }

    inline i32 NextMultipleOf(i32 number, i32 multiple)
    {
        if (multiple == 0)
            return number;

        i32 remainder = Abs(number) % multiple;
        if (remainder == 0)
            return number;

        if (number < 0)
            return -(Abs(number) - remainder);
        else
            return number + multiple - remainder;
    }

    inline bool IntersectsCircleSquare(vec2 cricle, f32 circleRadius, vec2 square, f32 squareExtent)
    {
        vec2 delta = glm::abs(cricle - square) - squareExtent;

        bool componentA = (delta.x > 0  && delta.y > 0)  * ((delta.x * delta.x + delta.y * delta.y) < (circleRadius * circleRadius));
        bool componentB = (delta.x > 0  && delta.y <= 0) * (delta.x < circleRadius);
        bool componentC = (delta.x <= 0 && delta.y <= 0) * (delta.y < circleRadius);

        bool result = componentA || componentB || componentC;
        return result;
    }
};