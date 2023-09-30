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

    static u32 ToRGBA32(const Color& rhs);
    static u32 ToABGR32(const Color& rhs);

    u32 ToRGBA32() const;
    u32 ToABGR32() const;

    static Color FromRGB32(u32 value, f32 a = 1.0f);
    static Color FromRGBA32(u32 value);
    static Color FromBGR32(u32 value, f32 a = 1.0f);
    static Color FromABGR32(u32 value);

    static const Color Black;
    static const Color Blue;
    static const Color Clear;
    static const Color Cyan;
    static const Color Gray;
    static const Color Green;
    static const Color Grey;
    static const Color Magenta;
    static const Color Red;
    static const Color White;
    static const Color Yellow;
    static const Color PastelPurple;
    static const Color PastelBlue;
    static const Color PastelGreen;
    static const Color PastelYellow;
    static const Color PastelOrange;
    static const Color PastelRed;
};