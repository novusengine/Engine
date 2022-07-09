#pragma once
#include <string>

namespace ShaderCooker
{
    class Token
    {
    public:
        enum class Type : uint16_t
        {
            COMMA = 44,
            SEMICOLON = 59,
            ASSIGN = 61,
            LBRACKET = 91,
            RBRACKET = 93,

            IDENTIFIER = 256,
            KEYWORD_PERMUTATION,
            KEYWORD_INCLUDE,

            // Custom Special/Operator Types
            STRING,

            // Alias
            PARAM_SEPERATOR = COMMA,
            END_OF_LINE = SEMICOLON,

            NONE = 999
        };

        Type type = Type::NONE;
        size_t lineNum = 0;
        size_t colNum = 0;

        std::string_view stringview;
        uint32_t hash = 0;
    };
}