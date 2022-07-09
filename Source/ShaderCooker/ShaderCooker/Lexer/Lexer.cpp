#include "Lexer.h"
#include <Base/Util/StringUtils.h>

#include <iostream>
#include <execution>

namespace ShaderCooker
{
    const char* KEYWORD_PERMUTATION = (char*)"permutation";
    const char* KEYWORD_INCLUDE = (char*)"#include";
    const char* KEYWORD_SINGLE_COMMENT = (char*)"//";
    const char* KEYWORD_MULTI_COMMENT_START = (char*)"/*";
    const char* KEYWORD_MULTI_COMMENT_END = (char*)"*/";

    const char NEWLINE = '\n';
    const char SPACE = ' ';
    const char STRING_SYMBOL = '"';

    const size_t permutationLen = strlen(KEYWORD_PERMUTATION);
    const size_t includeLen = strlen(KEYWORD_INCLUDE);
    const size_t singleCommentLen = strlen(KEYWORD_SINGLE_COMMENT);
    const size_t multiCommentLen = strlen(KEYWORD_MULTI_COMMENT_START);

    robin_hood::unordered_map<std::string_view, Token::Type> Lexer::_keywordStringToType =
    {
        { KEYWORD_PERMUTATION, Token::Type::KEYWORD_PERMUTATION },
        { KEYWORD_INCLUDE, Token::Type::KEYWORD_INCLUDE },
    };

    bool Lexer::Process(Shader& shader)
    {
        if (!GatherUnits(shader))
            return false;

        if (!ProcessUnits(shader))
            return false;

        return true;
    }

    bool Lexer::GatherUnits(Shader& shader)
    {
        size_t lineNum = 1;
        size_t colNum = 1;

        for (size_t bufferPosition = 0; bufferPosition < shader.fileBufferSize;)
        {
            const char& character = shader.fileBuffer[bufferPosition];

            // Skip New Lines or White Spaces
            if (character == NEWLINE)
            {
                bufferPosition++;
                lineNum += 1;
                colNum = 1;

                continue;
            }
            else if (character == SPACE)
            {
                bufferPosition++;
                colNum += 1;

                continue;
            }

            // Handle Comments Here
            if (strncmp(&character, KEYWORD_SINGLE_COMMENT, singleCommentLen) == 0)
            {
                bufferPosition += singleCommentLen;
                colNum += singleCommentLen;

                SkipSingleComment(shader.fileBuffer, shader.fileBufferSize, bufferPosition, lineNum, colNum);
                continue;
            }
            else if (strncmp(&character, KEYWORD_MULTI_COMMENT_START, multiCommentLen) == 0)
            {
                bufferPosition += multiCommentLen;
                colNum += singleCommentLen;

                SkipMultiComment(shader.fileBuffer, shader.fileBufferSize, bufferPosition, lineNum, colNum);
                continue;
            }

            // Handle Include Definition
            if (strncmp(&character, KEYWORD_INCLUDE, includeLen) == 0)
            {
                Include& include = shader.includes.emplace_back();
                include.startPtr = &character;
                include.startLineNum = lineNum;
                include.startColumnNum = colNum;

                for (; bufferPosition < shader.fileBufferSize; bufferPosition++)
                {
                    const char& subCharacter = shader.fileBuffer[bufferPosition];

                    if (subCharacter == NEWLINE)
                    {
                        lineNum += 1;
                        colNum = 1;

                        bufferPosition += 1;
                        break;
                    }
                    else if (subCharacter == SPACE)
                    {
                        colNum += 1;
                        continue;
                    }
                }

                include.endPtr = &shader.fileBuffer[bufferPosition - 1];
                continue;
            }

            // Handle functions, structs and enums here
            if (strncmp(&character, KEYWORD_PERMUTATION, permutationLen) == 0)
            {
                bufferPosition += permutationLen;
                colNum += permutationLen;

                PermutationGroup& permutationGroup = shader.permutationGroups.emplace_back();
                permutationGroup.startPtr = &character;
                permutationGroup.startLineNum = lineNum;
                permutationGroup.startColumnNum = colNum;

                if (!GatherUnit(shader, permutationGroup, bufferPosition, lineNum, colNum))
                    return false;

                continue;
            }

            bufferPosition += 1;
        }

        return true;
    }

    bool Lexer::ProcessUnits(Shader& shader)
    {
        for (Include& include : shader.includes)
        {
            include.tokens.reserve(32);
            GatherTokens(include.startPtr, include.endPtr, include.tokens, include.startLineNum, include.startColumnNum);
        }

        for (PermutationGroup& permutationGroup : shader.permutationGroups)
        {
            permutationGroup.tokens.reserve(1024);

            GatherTokens(permutationGroup.startPtr, permutationGroup.endPtr, permutationGroup.tokens, permutationGroup.startLineNum, permutationGroup.startColumnNum);

            int numTokens = static_cast<int>(permutationGroup.tokens.size());
            for (int i = 0; i < numTokens; i++)
            {
                const Token& token = permutationGroup.tokens[i];
                if (token.type == Token::Type::KEYWORD_PERMUTATION)
                {
                    if (i + 1 < numTokens)
                    {
                        const Token& nameToken = permutationGroup.tokens[i + 1];
                        permutationGroup.name = nameToken.stringview;
                    }
                    break;
                }
            }
        }

        return true;
    }

    void Lexer::SkipSingleComment(const char* buffer, size_t bufferSize, size_t& bufferPosition, size_t& lineNum, size_t& colNum)
    {
        size_t subBufferPosition = bufferPosition;
        for (; subBufferPosition < bufferSize;)
        {
            const char& subCharacter = buffer[subBufferPosition];

            if (subCharacter == NEWLINE)
            {
                lineNum += 1;
                colNum = 1;

                subBufferPosition += 1;
                break;
            }
            else
            {
                colNum += 1;
                subBufferPosition += 1;
            }
        }

        bufferPosition = subBufferPosition;
    }

    void Lexer::SkipMultiComment(const char* buffer, size_t bufferSize, size_t& bufferPosition, size_t& lineNum, size_t& colNum)
    {
        const size_t multiCommentStartLen = strlen(KEYWORD_MULTI_COMMENT_START);
        const size_t multiCommentEndLen = strlen(KEYWORD_MULTI_COMMENT_START);

        for (; bufferPosition < bufferSize;)
        {
            const char& subCharacter = buffer[bufferPosition];

            // Skip New Lines
            if (subCharacter == NEWLINE)
            {
                lineNum += 1;
                colNum = 1;

                bufferPosition += 1;
            }
            else if (strncmp(&subCharacter, KEYWORD_MULTI_COMMENT_START, multiCommentStartLen) == 0)
            {
                bufferPosition += multiCommentStartLen;
                colNum += multiCommentStartLen;

                SkipMultiComment(buffer, bufferSize, bufferPosition, lineNum, colNum);
            }
            else if (strncmp(&subCharacter, KEYWORD_MULTI_COMMENT_END, multiCommentEndLen) == 0)
            {
                bufferPosition += multiCommentEndLen;
                colNum += multiCommentEndLen;
                break;
            }
            else
            {
                colNum += 1;
                bufferPosition += 1;
            }
        }
    }

    bool Lexer::GatherUnit(Shader& shader, PermutationGroup& permutationGroup, size_t& bufferPosition, size_t& lineNum, size_t& colNum)
    {
        // Gather Unit
        bool endOfLineFound = false;

        for (; bufferPosition < shader.fileBufferSize; bufferPosition++)
        {
            const char& subCharacter = shader.fileBuffer[bufferPosition];

            if (subCharacter == NEWLINE)
            {
                ReportError(3, "Failed to find end of line ';' for permutation. (Line: %d, Col: %d)", lineNum, colNum);

                bufferPosition++;
                lineNum += 1;
                colNum = 1;
                return false;
            }
            else if (subCharacter == SPACE)
            {
                colNum += 1;

                continue;
            }

            if (subCharacter == ';')
            {
                permutationGroup.endPtr = &subCharacter;
                permutationGroup.endOffset = bufferPosition;

                bufferPosition++;
                endOfLineFound = true;
                break;
            }

            colNum += 1;
        }

        if (endOfLineFound == false)
        {
            ReportError(3, "Failed to find end of line ';'. (Line: %d, Col: %d)", lineNum, colNum);
            return false;
        }

        return true;
    }

    void Lexer::GatherTokens(const char* startPtr, const char* endPtr, std::vector<Token>& tokens, size_t& lineNum, size_t& colNum)
    {
        size_t bufferStartPosition = 0;
        size_t bufferSize = (endPtr - startPtr) + 1;

        for (size_t bufferPosition = bufferStartPosition; bufferPosition < bufferSize;)
        {
            const char& character = startPtr[bufferPosition];
            size_t remainingBytes = bufferSize - bufferPosition;

            // Skip New Lines or White Spaces
            if (character == NEWLINE || character == SPACE)
            {
                if (character == NEWLINE)
                {
                    lineNum += 1;
                    colNum = 1;
                }
                else
                {
                    colNum += 1;
                }

                bufferPosition += 1;
                continue;
            }

            // Handle Comments Here
            if (remainingBytes >= singleCommentLen && strncmp(&character, KEYWORD_SINGLE_COMMENT, singleCommentLen) == 0)
            {
                bufferPosition += singleCommentLen;
                colNum += singleCommentLen;

                SkipSingleComment(startPtr, bufferSize, bufferPosition, lineNum, colNum);
                continue;
            }
            else if (remainingBytes >= multiCommentLen && strncmp(&character, KEYWORD_MULTI_COMMENT_START, multiCommentLen) == 0)
            {
                bufferPosition += multiCommentLen;
                colNum += singleCommentLen;

                SkipMultiComment(startPtr, bufferSize, bufferPosition, lineNum, colNum);
                continue;
            }

            if (TryParseString(tokens, startPtr, bufferSize, bufferPosition, lineNum, colNum))
                continue;

            if (TryParseSpecialToken(tokens, startPtr, bufferSize, bufferPosition, lineNum, colNum))
                continue;

            if (TryParseKeyword(tokens, startPtr, bufferSize, bufferPosition, lineNum, colNum))
                continue;

            if (TryParseIdentifier(tokens, startPtr, bufferSize, bufferPosition, lineNum, colNum))
                continue;

            // We should never reach this
            ReportError(8, "Failed to Analyse Character Sequence");
            break;
        }
    }

    bool Lexer::TryParseString(std::vector<Token>& tokens, const char* buffer, size_t bufferSize, size_t& bufferPosition, size_t& lineNum, size_t& colNum)
    {
        const char& character = buffer[bufferPosition];

        if (character != STRING_SYMBOL)
            return false;

        // Parse String Here
        bool foundEndSymbol = false;

        size_t subBufferPosition = bufferPosition + 1;
        for (; subBufferPosition < bufferSize; subBufferPosition++)
        {
            const char& subCharacter = buffer[subBufferPosition];

            if (subCharacter == NEWLINE)
                break;

            if (subCharacter == STRING_SYMBOL)
            {
                foundEndSymbol = true;
                break;
            }
        }

        if (foundEndSymbol == false)
            return false;

        size_t length = subBufferPosition - bufferPosition;
        Token& token = tokens.emplace_back();

        token.type = Token::Type::STRING;
        token.stringview = std::string_view(&character + 1, length - 1);
        token.hash = StringUtils::hash_djb2(token.stringview.data(), token.stringview.size());
        token.lineNum = lineNum;
        token.colNum = colNum;

        bufferPosition = subBufferPosition + 1;
        colNum += length;
        return true;
    }

    bool Lexer::TryParseSpecialToken(std::vector<Token>& tokens, const char* buffer, size_t /*bufferSize*/, size_t& bufferPosition, size_t& lineNum, size_t& colNum)
    {
        Token::Type type = Token::Type::NONE;
        int specialCharacterLength = 1;
        const char* character = &buffer[bufferPosition];

        switch (*character++)
        {
        case ',':
        {
            type = Token::Type::COMMA;
            break;
        }

        case ';':
        {
            type = Token::Type::SEMICOLON;
            break;
        }

        case '=':
        {
            type = Token::Type::ASSIGN;
            break;
        }

        case '[':
        {
            type = Token::Type::LBRACKET;
            break;
        }

        case ']':
        {
            type = Token::Type::RBRACKET;
            break;
        }

        default:
            return false;
        }

        Token& token = tokens.emplace_back();
        token.type = type;
        token.stringview = std::string_view(&buffer[bufferPosition], specialCharacterLength);
        token.hash = StringUtils::hash_djb2(token.stringview.data(), token.stringview.size());
        token.lineNum = lineNum;
        token.colNum = colNum;

        bufferPosition += specialCharacterLength;
        colNum += specialCharacterLength;

        return true;
    }

    bool Lexer::TryParseKeyword(std::vector<Token>& tokens, const char* buffer, size_t bufferSize, size_t& bufferPosition, size_t& lineNum, size_t& colNum)
    {
        const char& character = buffer[bufferPosition];

        size_t subBufferPosition = bufferPosition + 1;
        for (; subBufferPosition < bufferSize; subBufferPosition++)
        {
            const char& subCharacter = buffer[subBufferPosition];

            if (subCharacter == NEWLINE || subCharacter == SPACE)
                break;

            if (subCharacter != '_' && !isalpha(subCharacter) && !isdigit(subCharacter))
                break;
        }

        size_t length = subBufferPosition - bufferPosition;
        std::string_view stringView = std::string_view(&character, length);

        // Keyword
        auto itr = _keywordStringToType.find(stringView);
        if (itr == _keywordStringToType.end())
        {
            return false;
        }

        Token& token = tokens.emplace_back();
        token.stringview = stringView;
        token.hash = StringUtils::hash_djb2(token.stringview.data(), token.stringview.size());
        token.lineNum = lineNum;
        token.colNum = colNum;

        token.type = itr->second;
        bufferPosition = subBufferPosition;
        colNum += length;
        return true;
    }

    bool Lexer::TryParseIdentifier(std::vector<Token>& tokens, const char* buffer, size_t bufferSize, size_t& bufferPosition, size_t& lineNum, size_t& colNum)
    {
        const char& character = buffer[bufferPosition];

        if (character != '_' && !isalpha(character) && !isdigit(character))
            return false;

        size_t subBufferPosition = bufferPosition + 1;
        for (; subBufferPosition < bufferSize; subBufferPosition++)
        {
            const char& subCharacter = buffer[subBufferPosition];

            if (subCharacter == NEWLINE || subCharacter == SPACE)
                break;

            if (subCharacter != '_' && !isalpha(subCharacter) && !isdigit(subCharacter))
                break;
        }

        size_t length = subBufferPosition - bufferPosition;

        Token& token = tokens.emplace_back();
        token.type = Token::Type::IDENTIFIER;
        token.stringview = std::string_view(&character, length);
        token.hash = StringUtils::hash_djb2(token.stringview.data(), token.stringview.size());
        token.lineNum = lineNum;
        token.colNum = colNum;

        // Keyword
        auto itr = _keywordStringToType.find(token.stringview);
        if (itr != _keywordStringToType.end())
        {
            token.type = itr->second;
        }

        bufferPosition = subBufferPosition;
        colNum += length;
        return true;
    }
}