#pragma once
#include "ShaderCooker/Permutation.h"
#include "ShaderCooker/Token.h"

#include <robinhood/robinhood.h>

#include <string>
#include <vector>
#include <cassert>
#include <sstream>

namespace ShaderCooker
{
    class Lexer
    {
    public:
        static bool Process(Shader& shader);

    private:
        Lexer() { }

        static bool GatherUnits(Shader& shader);
        static bool ProcessUnits(Shader& shader);

        static void SkipSingleComment(const char* buffer, size_t bufferSize, size_t& bufferPosition, size_t& lineNum, size_t& colNum);
        static void SkipMultiComment(const char* buffer, size_t bufferSize, size_t& bufferPosition, size_t& lineNum, size_t& colNum);

        static bool GatherUnit(Shader& shader, PermutationGroup& permutationGroup, size_t& bufferPosition, size_t& lineNum, size_t& colNum);
        static void GatherTokens(const char* startPtr, const char* endPtr, std::vector<Token>& tokens, size_t& lineNum, size_t& colNum);

        static bool TryParseString(std::vector<Token>& tokens, const char* buffer, size_t bufferSize, size_t& bufferPosition, size_t& lineNum, size_t& colNum);
        static bool TryParseSpecialToken(std::vector<Token>& tokens, const char* buffer, size_t bufferSize, size_t& bufferPosition, size_t& lineNum, size_t& colNum);
        static bool TryParseKeyword(std::vector<Token>& tokens, const char* buffer, size_t bufferSize, size_t& bufferPosition, size_t& lineNum, size_t& colNum);
        static bool TryParseIdentifier(std::vector<Token>& tokens, const char* buffer, size_t bufferSize, size_t& bufferPosition, size_t& lineNum, size_t& colNum);

        template<typename... Args>
        static void ReportError(int errorCode, std::string str, Args... args)
        {
            std::stringstream ss;
            ss << "Lexer Error " << errorCode << ": " << str << std::endl;

            printf_s(ss.str().c_str(), args...);
        }

        static robin_hood::unordered_map<std::string_view, Token::Type> _keywordStringToType;
    };
}