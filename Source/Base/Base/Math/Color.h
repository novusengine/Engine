#pragma once

#include <Base/Types.h>

struct Color
{
    Color(f32 inR, f32 inG, f32 inB, f32 inA = 1.0f)
        : r(inR)
        , g(inG)
        , b(inB)
        , a(inA)
    {
        
    }

    Color()
        : r(0.0f)
        , g(0.0f)
        , b(0.0f)
        , a(1.0f)
    {

    }

    f32 r;
    f32 g;
    f32 b;
    f32 a;

    Color operator+(const Color& rhs)
    {
        Color result = Color(r, g, b, a);

        result.r += rhs.r;
        result.g += rhs.g;
        result.b += rhs.b;
        result.a += rhs.a;

        return result;
    }
    Color operator-(const Color& rhs)
    {
        Color result = Color(r, g, b, a);

        result.r -= rhs.r;
        result.g -= rhs.g;
        result.b -= rhs.b;
        result.a -= rhs.a;

        return result;
    }
    Color operator*(const Color& rhs)
    {
        Color result = Color(r, g, b, a);

        result.r *= rhs.r;
        result.g *= rhs.g;
        result.b *= rhs.b;
        result.a *= rhs.a;

        return result;
    }
    Color operator/(const Color& rhs)
    {
        Color result = Color(r, g, b, a);

        result.r /= rhs.r;
        result.g /= rhs.g;
        result.b /= rhs.b;
        result.a /= rhs.a;

        return result;
    }

    static u32 ToU32(const Color& rhs)
    {
        u32 a = static_cast<u32>(rhs.a * 255.f);
        u32 r = static_cast<u32>(rhs.r * 255.f);
        u32 g = static_cast<u32>(rhs.g * 255.f);
        u32 b = static_cast<u32>(rhs.b * 255.f);

        u32 result = (a << 24) | (b << 16) | (g << 8) | (r);
        return result;
    }

    static Color Black;
    static Color Blue;
    static Color Clear;
    static Color Cyan;
    static Color Gray;
    static Color Green;
    static Color Grey;
    static Color Magenta;
    static Color Red;
    static Color White;
    static Color Yellow;
    static Color PastelPurple;
    static Color PastelBlue;
    static Color PastelGreen;
    static Color PastelYellow;
    static Color PastelOrange;
    static Color PastelRed;
};