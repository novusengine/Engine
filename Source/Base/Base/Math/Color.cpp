#include "Color.h"

const Color Color::Black = Color(0, 0, 0, 1);
const Color Color::Blue = Color(0, 0, 1, 1);
const Color Color::Clear = Color(0, 0, 0, 0);
const Color Color::Cyan = Color(0, 1, 1, 0);
const Color Color::Gray = Color(0.5f, 0.5f, 0.5f, 1.0f);
const Color Color::Green = Color(0, 1, 0, 1);
const Color Color::Grey = Color(0.5f, 0.5f, 0.5f, 1.0f);
const Color Color::Magenta = Color(1, 0, 1, 1);
const Color Color::Red = Color(1, 0, 0, 1);
const Color Color::White = Color(1, 1, 1, 1);
const Color Color::Yellow = Color(1.0f, 0.92f, 0.016f, 1.0f);

const Color Color::PastelPurple = Color(0.8f, 0.6f, 0.788f, 1.0f);
const Color Color::PastelBlue = Color(0.619f, 0.756f, 0.811f, 1.0f);
const Color Color::PastelGreen = Color(0.619f, 0.878f, 0.619f, 1.0f);
const Color Color::PastelYellow = Color(0.992f, 0.992f, 0.592f, 1.0f);
const Color Color::PastelOrange = Color(0.996f, 0.694f, 0.266f, 1.0f);
const Color Color::PastelRed = Color(1.0f, 0.4f, 0.388f, 1.0f);

u32 Color::ToRGBA32(const Color& rhs)
{
    u32 a = static_cast<u32>(rhs.a * 255.f);
    u32 r = static_cast<u32>(rhs.r * 255.f);
    u32 g = static_cast<u32>(rhs.g * 255.f);
    u32 b = static_cast<u32>(rhs.b * 255.f);

    u32 result = (a << 24) | (b << 16) | (g << 8) | (r);
    return result;
}

u32 Color::ToABGR32(const Color& rhs)
{
    u32 a = static_cast<u32>(rhs.a * 255.f);
    u32 r = static_cast<u32>(rhs.r * 255.f);
    u32 g = static_cast<u32>(rhs.g * 255.f);
    u32 b = static_cast<u32>(rhs.b * 255.f);

    u32 result = (r << 24) | (g << 16) | (b << 8) | (a);
    return result;
}

u32 Color::ToRGBA32() const
{
    return ToRGBA32(*this);
}

u32 Color::ToABGR32() const
{
    return ToABGR32(*this);
}

Color Color::FromRGB32(u32 value, f32 a)
{
    float b = static_cast<float>((value >> 16) & 0xFF) / 255.f;
    float g = static_cast<float>((value >> 8) & 0xFF) / 255.f;
    float r = static_cast<float>(value & 0xFF) / 255.f;

    return Color(r, g, b, a);
}

Color Color::FromRGBA32(u32 value)
{
    float a = static_cast<float>((value >> 24) & 0xFF) / 255.f;
    float b = static_cast<float>((value >> 16) & 0xFF) / 255.f;
    float g = static_cast<float>((value >> 8) & 0xFF) / 255.f;
    float r = static_cast<float>(value & 0xFF) / 255.f;

    return Color(r, g, b, a);
}

Color Color::FromBGR32(u32 value, f32 a)
{
    float r = static_cast<float>((value >> 16) & 0xFF) / 255.f;
    float g = static_cast<float>((value >> 8) & 0xFF) / 255.f;
    float b = static_cast<float>(value & 0xFF) / 255.f;

    return Color(r, g, b, a);
}

Color Color::FromABGR32(u32 value)
{
    float r = static_cast<float>((value >> 24) & 0xFF) / 255.f;
    float g = static_cast<float>((value >> 16) & 0xFF) / 255.f;
    float b = static_cast<float>((value >> 8) & 0xFF) / 255.f;
    float a = static_cast<float>(value & 0xFF) / 255.f;

    return Color(r, g, b, a);
}