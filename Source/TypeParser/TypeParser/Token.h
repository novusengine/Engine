#pragma once
#include <Base/Types.h>
#include <Base/Util/StringUtils.h>

#include <string>
#include <cstdint>

namespace TypeParser
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
        enum class Kind : u16
        {
            None,
            End_of_File,

            Parenthesis_Open,
            Parenthesis_Close,
            Bracket_Open,
            Bracket_Close,
            Curley_Bracket_Open,
            Curley_Bracket_Close,
            Semicolon,
            Colon,
            DoubleColon,
            Comma,
            Dot,
            Arrow,
            Hashtag,

            Identifier,
            String,
            Number,
            Comment,

            Op_Add,
            Op_Subtract,
            Op_Multiply,
            Op_Divide,
            Op_Modulo,
            Op_Assign,
            Op_LessThan,
            Op_LessEqual,
            Op_GreaterThan,
            Op_GreaterEqual,
            Op_Equal,
            Op_NotEqual,
            Op_Bitwise_And,
            Op_And,
            Op_Bitwise_Or,
            Op_Or,
            Op_At,

            Keyword_Type,
            Keyword_ClientDB,
            Keyword_Command,
            Keyword_Packet,
            Keyword_Enum,
            Keyword_Schema,
            Keyword_Table
        };

    public:
        static const char* GetTokenKindName(Kind kind);

    public:
        Kind kind = Kind::None;
        size_t line = 0;
        size_t column = 0;

        u64 number;
        NameHashView nameHash;
    };
}