#pragma once
#include "Token.h"

#include <Base/Types.h>
#include <Base/Util/DebugHandler.h>
#include <Base/Util/StringUtils.h>

#include <robinhood/robinhood.h>

#include <any>
#include <optional>

namespace TypeParser
{
    struct LexerInfo
    {
    public:
        const char* buffer = nullptr;
        size_t size = 0;
        u64 index = 0;
        u64 tokenIndex = 0;

        u32 line = 1;
        u32 column = 1;

        std::vector<Token> tokens;

    public:
        char PeekBuffer(i64 offset = 0)
        {
            if (index + offset >= size)
                return 0;

            return buffer[index + offset];
        }
        void SkipBuffer(i64 offset = 1) { index += offset; }
        Token& NewToken() { return tokens.emplace_back(); }

        Token* PeekToken(i64 offset = 0)
        {
            if (tokenIndex + offset >= tokens.size())
                return &tokens.back();

            return &tokens[tokenIndex + offset];
        }
        Token* ConsumeToken()
        {
            if (tokenIndex == tokens.size())
                return &tokens.back();

            return &tokens[tokenIndex++];
        }

        Token* ExpectToken(Token::Kind kind)
        {
            Token* token = ConsumeToken();
            if (token->kind != kind)
            {
                Error(token, "Expecting '%s' but got '%s'", Token::GetTokenKindName(kind), Token::GetTokenKindName(token->kind));
            }

            return token;
        }

        Token* SkipToken(Token::Kind kind)
        {
            Token* token = ConsumeToken();
            if (token->kind != kind)
            {
                Error(token, "Expecting '%s' but got '%s'", Token::GetTokenKindName(kind), Token::GetTokenKindName(token->kind));
            }

            return PeekToken();
        }

        static constexpr u32 ReportNumLineHistory = 1;

        template <typename... Args>
        void Error(Token* token, const char* message, Args... args)
        {
            const char* startPtr = buffer;
            const char* currentPtr = token->nameHash.name;

            u32 i = 0;
            while (i < ReportNumLineHistory)
            {
                i++;

                while (currentPtr != startPtr && *currentPtr != '\n')
                {
                    currentPtr--;
                }

                if (currentPtr == startPtr)
                {
                    if (*currentPtr == '\n')
                        currentPtr++;

                    break;
                }

                if (i == ReportNumLineHistory)
                {
                    currentPtr++;
                }
                else
                {
                    currentPtr--;
                }
            }

            char buffer[1024] = { 0 };
            i32 length = StringUtils::FormatString(buffer, 1024, message, args...);
            
            std::string_view messageView(buffer, length);
            NC_LOG_ERROR("Error: {0}", messageView);

            u32 lineNum = static_cast<u32>(token->line) - i;
            std::string errorMsg = "";
            errorMsg.reserve(512);

            for (u32 j = 0; j < i; j++)
            {
                errorMsg += " " + std::to_string(lineNum++) + " | ";

                while (char c = *currentPtr++)
                {
                    if (c == '\n' || c == '\r')
                        break;

                    errorMsg += c;
                }

                errorMsg += "\n";
            }

            NC_LOG_INFO("{0}", errorMsg);
            errorMsg.clear();
            errorMsg += "     ";

            for (i = 1; i < token->column; i++)
            {
                errorMsg += " ";
            }

            for (i = 0; i < token->nameHash.length; i++)
            {
                errorMsg += "^";
            }

            errorMsg += "\n       ";
            NC_LOG_INFO("{0}\n", errorMsg);

            throw std::exception();
        }
    };
    
    enum class ParsedTypeKind
    {
        None,
        ClientDB,
        Command,
        Enum,
        Packet,
        LuaStruct
    };

    enum class TypePropertyKind
    {
        None,
        Auto,
        Value,
        boolean,
        i8,
        i16,
        i32,
        i64,
        u8,
        u16,
        u32,
        u64,
        f32,
        f64,
        vec2,
        vec3,
        vec4,
        ivec2,
        ivec3,
        ivec4,
        uvec2,
        uvec3,
        uvec4,
        Identifier,
        string,
        StringRef,
        array,
        objectguid
    };

    struct TypeArrayInfo
    {
    public:
        TypePropertyKind kind = TypePropertyKind::None;

        u64 count = 0;
        bool isNegative = false;
    };

    struct TypeProperty
    {
    public:
        std::string name;
        TypePropertyKind kind = TypePropertyKind::None;

        std::optional<TypeArrayInfo> arrayInfo;
        bool isOptional = false;
    };

    struct ParsedTypeProperty
    {
    public:
        static TypePropertyKind GetTypePropertyKindFromNameHash(u32 nameHash);
        static const char* GetTypePropertyKindName(TypePropertyKind kind);

    public:
        std::string name;
        TypeProperty type;

        std::vector<TypeProperty> values;
    };

    struct ParsedType
    {
    public:
        ParsedTypeKind kind = ParsedTypeKind::None;
        Token* startToken = nullptr;
        Token* endToken = nullptr;

        Token* identifierToken = nullptr;
        Token* kindToken = nullptr;

        std::vector<ParsedTypeProperty> properties;
        robin_hood::unordered_map<u32, u32> propertyHashToIndex;
    };

    struct ParserInfo
    {
    public:
        std::vector<ParsedType> parsedTypes;
        robin_hood::unordered_map<u64, u32> parsedTypeKeyToIndex;
    };

    struct Module
    {
    public:
        LexerInfo lexerInfo;
        ParserInfo parserInfo;
    };
}