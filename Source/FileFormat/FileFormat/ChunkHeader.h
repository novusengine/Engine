#pragma once
#include <Base/Types.h>

struct ChunkHeader
{
public:
    struct Token
    {
    public:
        constexpr Token(const char* fourCC) noexcept : _value(0)
        {
            _value = static_cast<u32>(fourCC[0]) | (static_cast<u32>(fourCC[1]) << 8) | (static_cast<u32>(fourCC[2]) << 16) | (static_cast<u32>(fourCC[3]) << 24);
        }
        Token(const Token& other) = default;

        constexpr operator u32() noexcept { return _value; }

    private:
        u32 _value;
    };

public:
    ChunkHeader(const char* fourCC) : token(fourCC) { }

public:
    Token token;
    u32 size = 0;
};