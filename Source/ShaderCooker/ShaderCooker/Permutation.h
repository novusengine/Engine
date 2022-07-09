#pragma once
#include "Token.h"

#include <Base/Types.h>

#include <vector>
#include <cassert>
#include <filesystem>

struct DxcDefine;

namespace ShaderCooker
{
    struct Permutation
    {
        std::vector<DxcDefine*> defines;
    };

    struct PermutationGroup
    {
        std::vector<Token> tokens;
        std::vector<Token> types;

        std::string name;

        const char* startPtr;
        const char* endPtr;

        size_t startLineNum;
        size_t startColumnNum;

        size_t endOffset;

        Token& Peek(int index) { assert(index < static_cast<int>(tokens.size())); return tokens[index]; }
        void Eat(int& index) { index += 1; }
    };

    struct Include
    {
        std::string name;
        u32 hash;

        std::filesystem::path path;
        u32 pathHash;

        const char* startPtr;
        const char* endPtr;

        size_t startLineNum = 0;
        size_t startColumnNum = 0;

        std::vector<Token> tokens;

        Token& Peek(int index) { assert(index < static_cast<int>(tokens.size())); return tokens[index]; }
        void Eat(int& index) { index += 1; }
    };

    struct Shader
    {
        std::string name;

        std::filesystem::path path;
        u32 pathHash;

        std::filesystem::path parentPath;

        std::string source;

        std::vector<Include> includes;
        std::vector<PermutationGroup> permutationGroups;

        std::vector<Permutation> permutations;

        char* fileBuffer = nullptr;
        size_t fileBufferSize = 0;

        bool hasResolvedIncludes = false;
        bool markedForChange = false;
    };
}