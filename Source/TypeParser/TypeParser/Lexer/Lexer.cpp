#include "Lexer.h"
#include "TypeParser/Types.h"

#include <Base/Util/StringUtils.h>

#include <iostream>
#include <execution>

namespace TypeParser
{
    robin_hood::unordered_map<u32, Token::Kind> Lexer::keywords =
    {
        { "type"_djb2, Token::Kind::Keyword_Type },
        { "ClientDB"_djb2, Token::Kind::Keyword_ClientDB },
        { "Command"_djb2, Token::Kind::Keyword_Command },
        { "Packet"_djb2, Token::Kind::Keyword_Packet },
        { "Enum"_djb2, Token::Kind::Keyword_Enum },
        { "LuaStruct"_djb2, Token::Kind::Keyword_LuaStruct },
        { "Schema"_djb2, Token::Kind::Keyword_Schema },
        { "Table"_djb2, Token::Kind::Keyword_Table }
    };

    void Lexer::Process(Module& module)
    {
        NC_LOG_INFO("Lexer : Processing (Bytes: {0})", module.lexerInfo.size);
        NC_LOG_INFO("Lexer : Token Capacity ({0})", module.lexerInfo.tokens.capacity());

        while (true)
        {
            Token& token = module.lexerInfo.NewToken();
            GetNextToken(module.lexerInfo, token);

            if (token.kind == Token::Kind::End_of_File)
                break;
        }

        NC_LOG_INFO("Lexer : Processed (Bytes: {0}, Tokens: {1}, Lines: {2})", module.lexerInfo.size, module.lexerInfo.tokens.size(), module.lexerInfo.line);
    }

    void Lexer::GetNextToken(LexerInfo& lexerInfo, Token& token)
    {
        token.kind = Token::Kind::None;

        SkipWhitespaces(lexerInfo);

        token.nameHash.name = &lexerInfo.buffer[lexerInfo.index];
        token.line = lexerInfo.line;
        token.column = lexerInfo.column;

        char c = lexerInfo.PeekBuffer();
        if (c == 0)
        {
            token.kind = Token::Kind::End_of_File;
            return;
        }

        if (IsCharacter(c))
        {
            ParseIdentifier(lexerInfo, token);
        }
        else if (IsNumber(c))
        {
            ParseNumber(lexerInfo, token);
        }
        else if (IsSingleQuote(c))
        {
            ParseSingleQuote(lexerInfo, token);
        }
        else if (IsString(c))
        {
            ParseString(lexerInfo, token);
        }
        else if (IsCommentSL(lexerInfo))
        {
            ParseCommentSL(lexerInfo, token);
        }
        else if (IsCommentML_Start(lexerInfo))
        {
            ParseCommentML(lexerInfo, token);
        }
        else
        {
            ParsePunctuation(lexerInfo, token);
        }

        if (token.kind == Token::Kind::None)
        {
            token.nameHash.length = 1;
            lexerInfo.Error(&token, "Failed to parse token");
        }
    }

    void Lexer::Advance(LexerInfo& lexerInfo, u32 count)
    {
        while (count--)
        {
            char c = lexerInfo.PeekBuffer();
            if (c == 0)
                return;

            lexerInfo.SkipBuffer();
            if (c == '\r')
            {
                char curr = lexerInfo.PeekBuffer();
                if (curr == '\n')
                    lexerInfo.SkipBuffer();

                c = '\n';
            }

            if (c == '\n')
            {
                lexerInfo.line++;
                lexerInfo.column = 1;
            }
            else
            {
                lexerInfo.column++;
            }
        }
    }

    void Lexer::SkipWhitespaces(LexerInfo& lexerInfo)
    {
        while (IsWhitespace(lexerInfo.PeekBuffer()))
            Advance(lexerInfo);
    }

    bool Lexer::IsWhitespace(char c)
    {
        return c == ' ' || c == '\n' || c == '\r' || c == '\t';
    }

    bool Lexer::IsCharacter(char c)
    {
        if (c == '_')
            return true;

        // Convert C to lowercase char
        char tmp = c | (1 << 5);
        return tmp >= 'a' && tmp <= 'z';
    }

    bool Lexer::IsNumber(char c)
    {
        return c >= '0' && c <= '9';
    }

    bool Lexer::IsSingleQuote(char c)
    {
        return c == '\'';
    }

    bool Lexer::IsString(char c)
    {
        return c == '"';
    }

    bool Lexer::IsCommentSL(LexerInfo& lexerInfo)
    {
        char c = lexerInfo.PeekBuffer();
        char next = lexerInfo.PeekBuffer(1);
        return c == '/' && next == '/';
    }

    bool Lexer::IsCommentML_Start(LexerInfo& lexerInfo)
    {
        char c = lexerInfo.PeekBuffer();
        char next = lexerInfo.PeekBuffer(1);

        return c == '/' && next == '*';
    }
    bool Lexer::IsCommentML_End(LexerInfo& lexerInfo)
    {
        char c = lexerInfo.PeekBuffer();
        char next = lexerInfo.PeekBuffer(1);

        return c == '*' && next == '/';
    }

    bool Lexer::CharToNumber(char c, u8& number)
    {
        // Handle 0 - 9
        if (IsNumber(c))
        {
            number = c - '0';
            return true;
        }

        // Convert C to lowercase char
        c |= (1 << 5);

        if (c >= 'a' && c <= 'f')
        {
            number = 10 + c - 'a';
            return true;
        }

        return false;
    }

    void Lexer::ParseIdentifier(LexerInfo& lexerInfo, Token& token)
    {
        u64 startIndex = lexerInfo.index;

        while (char c = lexerInfo.PeekBuffer())
        {
            if (!IsCharacter(c) && !IsNumber(c))
                break;

            Advance(lexerInfo);
        }

        u64 endIndex = lexerInfo.index;

        const char* startPtr = &lexerInfo.buffer[startIndex];
        const char* endPtr = &lexerInfo.buffer[endIndex];

        token.kind = Token::Kind::Identifier;
        token.nameHash.SetNameHash(startPtr, endPtr - startPtr);

        auto itr = keywords.find(token.nameHash.hash);
        if (itr != keywords.end())
        {
            token.kind = itr->second;
        }
    }

    void Lexer::ParseNumber(LexerInfo& lexerInfo, Token& token)
    {
        u64 startIndex = lexerInfo.index;
        const char* startPtr = &lexerInfo.buffer[startIndex];

        u32 base = 10;

        char c = lexerInfo.PeekBuffer();
        if (c == '0')
        {
            char next = lexerInfo.PeekBuffer(1);
            if (next == 'x')
            {
                base = 16;
            }
            else if (next == 'o')
            {
                base = 8;
            }
            else if (next == 'b')
            {
                base = 2;
            }
            else if (IsNumber(next))
            {
                token.nameHash.length = 2;
                lexerInfo.Error(&token, "A number cannot start with 0 unless it is 0");
            }

            if (base != 10)
            {
                Advance(lexerInfo, 2);

                u8 tmp = 0;
                if (!CharToNumber(lexerInfo.PeekBuffer(), tmp))
                {
                    token.nameHash.length = 3;
                    lexerInfo.Error(&token, "A number must contain at least 1 valid digit");
                }
            }
        }

        u8 tmp = 0;
        u64 number = 0;
        while (CharToNumber(lexerInfo.PeekBuffer(), tmp))
        {
            if (tmp >= base)
            {
                token.nameHash.length = static_cast<u32>((lexerInfo.index - startIndex) + 1);
                lexerInfo.Error(&token, "Encountered digit with a value greater than base (Base: %u, Digit: %u)", base, tmp);
            }

            number = number * base + tmp;
            Advance(lexerInfo);
        }

        token.kind = Token::Kind::Number;
        token.nameHash.SetNameHash(startPtr, lexerInfo.index - startIndex);
        token.number = number;
    }

    void Lexer::ParseSingleQuote(LexerInfo& lexerInfo, Token& token)
    {
        // Skip Single Quote
        Advance(lexerInfo);

        char character = lexerInfo.PeekBuffer();

        token.kind = Token::Kind::Number;
        token.nameHash.SetNameHash(&lexerInfo.buffer[lexerInfo.index], 1);
        token.number = character;

        // Skip Character
        Advance(lexerInfo, 1);

        // Ensure there is only one char between single quotes
        char tmp = lexerInfo.PeekBuffer();
        if (tmp != '\'')
        {
            lexerInfo.Error(&token, "Encountered single quote with more than 1 character within");
        }

        // Skip Single Quote
        Advance(lexerInfo, 1);
    }

    void Lexer::ParseString(LexerInfo& lexerInfo, Token& token)
    {
        Advance(lexerInfo); // Skip start quote

        u64 startIndex = lexerInfo.index;
        const char* startPtr = &lexerInfo.buffer[startIndex];

        while (char c = lexerInfo.PeekBuffer())
        {
            if (IsString(c))
                break;

            Advance(lexerInfo);
        }

        token.kind = Token::Kind::String;
        token.nameHash.SetNameHash(startPtr, lexerInfo.index - startIndex);

        char c = lexerInfo.PeekBuffer();
        if (c == 0)
        {
            lexerInfo.Error(&token, "Unterminated String");
        }

        Advance(lexerInfo); // Skip end quote
    }

    void Lexer::ParsePunctuation(LexerInfo& lexerInfo, Token& token)
    {
        u64 startIndex = lexerInfo.index;
        const char* startPtr = &lexerInfo.buffer[startIndex];

        char c = lexerInfo.PeekBuffer();
        char next = lexerInfo.PeekBuffer(1);
        switch (c)
        {
            case ':':
            {
                if (next == ':')
                {
                    SkipPunctuation(lexerInfo, token, Token::Kind::DoubleColon, 2);
                }
                else
                {
                    SkipPunctuation(lexerInfo, token, Token::Kind::Colon, 1);
                }
                break;
            }
            case ';':
            {
                SkipPunctuation(lexerInfo, token, Token::Kind::Semicolon, 1);
                break;
            }
            case '(':
            {
                SkipPunctuation(lexerInfo, token, Token::Kind::Parenthesis_Open, 1);
                break;
            }
            case ')':
            {
                SkipPunctuation(lexerInfo, token, Token::Kind::Parenthesis_Close, 1);
                break;
            }
            case '[':
            {
                SkipPunctuation(lexerInfo, token, Token::Kind::Bracket_Open, 1);
                break;
            }
            case ']':
            {
                SkipPunctuation(lexerInfo, token, Token::Kind::Bracket_Close, 1);
                break;
            }
            case '{':
            {
                SkipPunctuation(lexerInfo, token, Token::Kind::Curley_Bracket_Open, 1);
                break;
            }
            case '}':
            {
                SkipPunctuation(lexerInfo, token, Token::Kind::Curley_Bracket_Close, 1);
                break;
            }
            case ',':
            {
                SkipPunctuation(lexerInfo, token, Token::Kind::Comma, 1);
                break;
            }
            case '.':
            {
                SkipPunctuation(lexerInfo, token, Token::Kind::Dot, 1);
                break;
            }
            case '#':
            {
                SkipPunctuation(lexerInfo, token, Token::Kind::Hashtag, 1);
                break;
            }
            case '+':
            {
                SkipPunctuation(lexerInfo, token, Token::Kind::Op_Add, 1);
                break;
            }
            case '-':
            {
                if (next == '>')
                {
                    SkipPunctuation(lexerInfo, token, Token::Kind::Arrow, 2);
                }
                else
                {
                    SkipPunctuation(lexerInfo, token, Token::Kind::Op_Subtract, 1);
                }
                break;
            }
            case '*':
            {
                SkipPunctuation(lexerInfo, token, Token::Kind::Op_Multiply, 1);
                break;
            }
            case '/':
            {
                SkipPunctuation(lexerInfo, token, Token::Kind::Op_Divide, 1);
                break;
            }
            case '%':
            {
                SkipPunctuation(lexerInfo, token, Token::Kind::Op_Modulo, 1);
                break;
            }
            case '=':
            {
                if (next == '=')
                {
                    SkipPunctuation(lexerInfo, token, Token::Kind::Op_Equal, 2);
                }
                else
                {
                    SkipPunctuation(lexerInfo, token, Token::Kind::Op_Assign, 1);
                }
                break;
            }
            case '!':
            {
                if (next == '=')
                {
                    SkipPunctuation(lexerInfo, token, Token::Kind::Op_NotEqual, 2);
                }
                break;
            }
            case '<':
            {
                if (next == '=')
                {
                    SkipPunctuation(lexerInfo, token, Token::Kind::Op_LessEqual, 2);
                }
                else
                {
                    SkipPunctuation(lexerInfo, token, Token::Kind::Op_LessThan, 1);
                }
                break;
            }
            case '>':
            {
                if (next == '=')
                {
                    SkipPunctuation(lexerInfo, token, Token::Kind::Op_GreaterEqual, 2);
                }
                else
                {
                    SkipPunctuation(lexerInfo, token, Token::Kind::Op_GreaterThan, 1);
                }

                break;
            }
            case '&':
            {
                if (next == '&')
                {
                    SkipPunctuation(lexerInfo, token, Token::Kind::Op_And, 2);
                }
                else
                {
                    SkipPunctuation(lexerInfo, token, Token::Kind::Op_Bitwise_And, 1);
                }
                break;
            }
            case '|':
            {
                if (next == '|')
                {
                    SkipPunctuation(lexerInfo, token, Token::Kind::Op_Or, 2);
                }
                else
                {
                    SkipPunctuation(lexerInfo, token, Token::Kind::Op_Bitwise_Or, 1);
                }
                break;
            }
            case '@':
            {
                SkipPunctuation(lexerInfo, token, Token::Kind::Op_At, 1);
                break;
            }

            default:
                return;
        }

        token.nameHash.SetNameHash(startPtr, lexerInfo.index - startIndex);
    }

    void Lexer::SkipPunctuation(LexerInfo& lexerInfo, Token& token, Token::Kind kind, u32 count)
    {
        token.kind = kind;

        lexerInfo.SkipBuffer(count);
        lexerInfo.column += count;
    }

    void Lexer::ParseCommentSL(LexerInfo& lexerInfo, Token& token)
    {
        Advance(lexerInfo, 2); // Skip initial comment characters

        u64 startIndex = lexerInfo.index;
        const char* startPtr = &lexerInfo.buffer[startIndex];

        while (char c = lexerInfo.PeekBuffer())
        {
            if (c == '\r' || c == '\n')
                break;

            Advance(lexerInfo);
        }

        token.kind = Token::Kind::Comment;
        token.nameHash.SetNameHash(startPtr, lexerInfo.index - startIndex);

        Advance(lexerInfo); // Skip new line
    }

    void Lexer::ParseCommentML(LexerInfo& lexerInfo, Token& token)
    {
        Advance(lexerInfo, 2); // Skip initial comment characters

        u64 startIndex = lexerInfo.index;
        const char* startPtr = &lexerInfo.buffer[startIndex - 2];

        u32 nestedLevel = 1;
        while (char c = lexerInfo.PeekBuffer())
        {
            if (IsCommentML_Start(lexerInfo))
            {
                nestedLevel++;
            }
            else if (IsCommentML_End(lexerInfo))
            {
                if (--nestedLevel == 0)
                    break;
            }

            Advance(lexerInfo);
        }

        token.kind = Token::Kind::Comment;
        token.nameHash.SetNameHash(startPtr, lexerInfo.index - startIndex);

        if (nestedLevel != 0)
        {
            lexerInfo.Error(&token, "Multi Line Comment with no closure");
        }

        Advance(lexerInfo, 2); // Skip end comment characters
    }
}