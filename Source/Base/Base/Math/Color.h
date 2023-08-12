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

    u32 ToU32() const
    {
        u32 color = 0;
        color = (u32)(r * 255.0f) << 24;
        color |= (u32)(g * 255.0f) << 16;
        color |= (u32)(b * 255.0f) << 8;
        color |= (u32)(a * 255.0f) << 0;
        return color;
    }

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