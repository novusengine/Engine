#pragma once
#include "ShaderCooker/ShaderCompiler.h"
#include "ShaderCooker/Permutation.h"
#include "ShaderCooker/Token.h"

namespace ShaderCooker
{
    class Parser
    {
    public:
        static bool CheckSyntax(Shader& shader);
        static bool ResolveIncludes(ShaderCompiler* compiler, Shader& shader);

    public:
        // Check Syntax Helper Functions
        static bool CheckSyntaxInclude(Shader& shader, Include& include, const int& numTokens);
        static bool CheckSyntaxPermutation(PermutationGroup& permutationGroup, const int& numTokens);

    private:
        Parser() { }

        template<typename... Args>
        static void ReportError(int errorCode, std::string str, Args... args)
        {
            std::stringstream ss;
            ss << "Parser Error " << errorCode << ": " << str << std::endl;

            printf_s(ss.str().c_str(), args...);
        }
    };
}