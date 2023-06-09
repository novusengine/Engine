#pragma once

struct Color
{
    Color(float inR, float inG, float inB, float inA = 1.0f)
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

    float r;
    float g;
    float b;
    float a;

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
        u8 a = static_cast<u8>(rhs.a * 255.f);
        u8 r = static_cast<u8>(rhs.r * 255.f);
        u8 g = static_cast<u8>(rhs.g * 255.f);
        u8 b = static_cast<u8>(rhs.b * 255.f);

        return (static_cast<u32>(a) << 24) | (static_cast<u32>(b) << 16) | (static_cast<u32>(g) << 8) | (static_cast<u32>(r));
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