#pragma once
#include <Base/Types.h>
#include <Base/Util/StringUtils.h>

#include <string>
#include <cstdint>

namespace ShaderCooker
{
    struct NameHash
    {
        std::string name;
        u32 hash;

        void SetNameHash(std::string inName)
        {
            name = inName;
            hash = StringUtils::hash_djb2(name.c_str(), static_cast<i32>(name.length()));
        }

        void SetNameHash(char* ptr, size_t size)
        {
            name = std::string(ptr, size);
            hash = StringUtils::hash_djb2(name.c_str(), static_cast<i32>(name.length()));
        }
    };

    struct NameHashView
    {
        const char* name;
        u32 length;
        u32 hash;

        void SetNameHash(const char* ptr, size_t size)
        {
            name = ptr;
            length = static_cast<i32>(size);
            hash = StringUtils::hash_djb2(name, static_cast<i32>(size));
        }
    };

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

        NameHashView nameHash;
    };
}