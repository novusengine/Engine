#include "Parser.h"

#include <Base/Util/DebugHandler.h>
#include <Base/Util/StringUtils.h>

#include <robinhood/robinhood.h>

#include <execution>
#include <stack>
#include <sstream>

namespace fs = std::filesystem;

namespace ShaderCooker
{
    enum class ParseRuleSet
    {
        NONE,
        INCLUDE,
        INCLUDE_NAME,

        PERMUTATION,
        PERMUTATION_NAME,
        PERMUTATION_VALUE,
        PERMUTATION_VALUE_SEQUENCE,

        ASSIGNMENT,
        PARAM_SEPERATOR,
        OPEN_BRACKET,
        CLOSE_BRACKET,
        END_OF_LINE
    };

    bool Parser::CheckSyntax(Shader& shader)
    {
        // Only check syntax if we have permutations to check

        int errorCount = 0;

        for (Include& include : shader.includes)
        {
            int numTokens = static_cast<int>(include.tokens.size());

            if (numTokens > 0)
            {
                if (!CheckSyntaxInclude(shader, include, numTokens))
                {
                    errorCount++;
                }
            }
        }

        for (PermutationGroup& permutationGroup : shader.permutationGroups)
        {
            int numTokens = static_cast<int>(permutationGroup.tokens.size());

            if (numTokens > 0)
            {
                if (!CheckSyntaxPermutation(permutationGroup, numTokens))
                {
                    errorCount++;
                }
            }
        }

        if (errorCount > 0)
        {
            DebugHandler::PrintError("Shader (%s) failed to pass syntax check", shader.name.c_str());
        }

        return errorCount == 0;
    }

    bool Parser::ResolveIncludes(ShaderCompiler* compiler, Shader& shader)
    {
        int numIncludes = static_cast<int>(shader.includes.size());
        int includesFound = 0;

        for (int i = 0; i < numIncludes; i++)
        {
            Include& include = shader.includes[i];

            fs::path baseIncludePath = (shader.parentPath / include.name);

            if (fs::exists(baseIncludePath))
            {
                include.path = baseIncludePath;
                include.pathHash = StringUtils::fnv1a_32(baseIncludePath.string().c_str(), baseIncludePath.string().length());

                compiler->AddInclude(shader.path, baseIncludePath);
                includesFound++;
            }
            else
            {
                fs::path sourceIncludePath = (compiler->GetSourceDirPath() / include.name).make_preferred();
                if (fs::exists(sourceIncludePath))
                {
                    include.path = sourceIncludePath;
                    include.pathHash = StringUtils::fnv1a_32(sourceIncludePath.string().c_str(), sourceIncludePath.string().length());

                    compiler->AddInclude(shader.path, sourceIncludePath);
                    includesFound++;
                }
                else
                {
                    DebugHandler::PrintError("Shader (%s) includes (%s) the include could not be resolved", shader.name.c_str(), include.name.c_str());
                }
            }
        }

        return numIncludes == includesFound;
    }

    bool Parser::CheckSyntaxInclude(Shader& shader, Include& include, const int& numTokens)
    {
        std::stack<ParseRuleSet> ruleSetStack;
        std::stack<Token::Type> tokenTypeStack;

        ruleSetStack.push(ParseRuleSet::INCLUDE);
        tokenTypeStack.push(Token::Type::NONE);

        for (int tokenIndex = 0; tokenIndex < numTokens;)
        {
            const Token& currentToken = include.Peek(tokenIndex);
            const Token::Type& expectedTokenType = tokenTypeStack.top();

            if (currentToken.type == expectedTokenType)
            {
                tokenIndex += 1;
                tokenTypeStack.pop();
                continue;
            }

            const ParseRuleSet& rule = ruleSetStack.top();
            ruleSetStack.pop();

            switch (rule)
            {
            case ParseRuleSet::INCLUDE:
            {
                switch (currentToken.type)
                {
                case Token::Type::KEYWORD_INCLUDE:
                    tokenTypeStack.push(Token::Type::KEYWORD_INCLUDE);

                    ruleSetStack.push(ParseRuleSet::INCLUDE_NAME);
                    break;

                default:
                    return false;
                }

                break;
            }

            case ParseRuleSet::INCLUDE_NAME:
            {
                switch (currentToken.type)
                {
                case Token::Type::STRING:
                    tokenTypeStack.push(Token::Type::STRING);

                    include.name = currentToken.stringview;
                    include.hash = currentToken.hash;
                    break;

                default:
                    return false;
                }

                break;
            }

            default:
                return false;
            }
        }

        return tokenTypeStack.size() == 1 && ruleSetStack.size() == 0;
    }

    bool Parser::CheckSyntaxPermutation(PermutationGroup& permutationGroup, const int& numTokens)
    {
        std::stack<ParseRuleSet> ruleSetStack;
        std::stack<Token::Type> tokenTypeStack;

        ruleSetStack.push(ParseRuleSet::PERMUTATION);
        tokenTypeStack.push(Token::Type::NONE);

        for (int tokenIndex = 0; tokenIndex < numTokens;)
        {
            const Token& currentToken = permutationGroup.Peek(tokenIndex);
            const Token::Type& expectedTokenType = tokenTypeStack.top();

            if (currentToken.type == expectedTokenType)
            {
                tokenIndex += 1;
                tokenTypeStack.pop();
                continue;
            }

            const ParseRuleSet& rule = ruleSetStack.top();
            ruleSetStack.pop();

            switch (rule)
            {
            case ParseRuleSet::PERMUTATION:
            {
                switch (currentToken.type)
                {
                case Token::Type::KEYWORD_PERMUTATION:
                    tokenTypeStack.push(Token::Type::KEYWORD_PERMUTATION);

                    ruleSetStack.push(ParseRuleSet::END_OF_LINE);
                    ruleSetStack.push(ParseRuleSet::CLOSE_BRACKET);
                    ruleSetStack.push(ParseRuleSet::PERMUTATION_VALUE_SEQUENCE);
                    ruleSetStack.push(ParseRuleSet::PERMUTATION_VALUE);
                    ruleSetStack.push(ParseRuleSet::OPEN_BRACKET);
                    ruleSetStack.push(ParseRuleSet::ASSIGNMENT);
                    ruleSetStack.push(ParseRuleSet::PERMUTATION_NAME);
                    break;

                default:
                    return false;
                }

                break;
            }
            case ParseRuleSet::PERMUTATION_VALUE:
            {
                switch (currentToken.type)
                {
                case Token::Type::IDENTIFIER:
                    tokenTypeStack.push(Token::Type::IDENTIFIER);

                    permutationGroup.types.push_back(currentToken);
                    break;

                default:
                    ReportError(1, "Expected to find identifier for permutation (%s). (Line: %d, Col: %d)", permutationGroup.name.c_str(), currentToken.lineNum, currentToken.colNum);
                    return false;
                }

                break;
            }
            case ParseRuleSet::PERMUTATION_VALUE_SEQUENCE:
            {
                switch (currentToken.type)
                {
                case Token::Type::PARAM_SEPERATOR:
                    tokenTypeStack.push(Token::Type::PARAM_SEPERATOR);

                    ruleSetStack.push(ParseRuleSet::PERMUTATION_VALUE_SEQUENCE);
                    ruleSetStack.push(ParseRuleSet::PERMUTATION_VALUE);
                    break;
                case Token::Type::RBRACKET:
                    break;

                default:
                    ReportError(2, "Expected to find ',' or ']' for permutation (%s). (Line: %d, Col: %d)", permutationGroup.name.c_str(), currentToken.lineNum, currentToken.colNum);
                    return false;
                }

                break;
            }
            case ParseRuleSet::ASSIGNMENT:
            {
                switch (currentToken.type)
                {
                case Token::Type::ASSIGN:
                    tokenTypeStack.push(Token::Type::ASSIGN);
                    break;

                default:
                    ReportError(3, "Expected to find '=' for permutation (%s). (Line: %d, Col: %d)", permutationGroup.name.c_str(), currentToken.lineNum, currentToken.colNum);
                    return false;
                }
                break;
            }
            case ParseRuleSet::PERMUTATION_NAME:
            {
                switch (currentToken.type)
                {
                case Token::Type::IDENTIFIER:
                    tokenTypeStack.push(Token::Type::IDENTIFIER);
                    break;

                default:
                    ReportError(4, "Expected to find identifier for permutation name but found (%s). (Line: %d, Col: %d)", permutationGroup.name.c_str(), currentToken.lineNum, currentToken.colNum);
                    return false;
                }
                break;
            }
            case ParseRuleSet::OPEN_BRACKET:
            {
                switch (currentToken.type)
                {
                case Token::Type::LBRACKET:
                    tokenTypeStack.push(Token::Type::LBRACKET);
                    break;

                default:
                    ReportError(5, "Expected to find '[' for permutation (%s). (Line: %d, Col: %d)", permutationGroup.name.c_str(), currentToken.lineNum, currentToken.colNum);
                    return false;
                }
                break;
            }
            case ParseRuleSet::CLOSE_BRACKET:
            {
                switch (currentToken.type)
                {
                case Token::Type::RBRACKET:
                    tokenTypeStack.push(Token::Type::RBRACKET);
                    break;

                default:
                    ReportError(6, "Expected to find ']' for permutation (%s). (Line: %d, Col: %d)", permutationGroup.name.c_str(), currentToken.lineNum, currentToken.colNum);
                    return false;
                }
                break;
            }
            case ParseRuleSet::END_OF_LINE:
            {
                switch (currentToken.type)
                {
                case Token::Type::END_OF_LINE:
                    tokenTypeStack.push(Token::Type::END_OF_LINE);
                    break;

                default:
                    ReportError(7, "Expected to find ';' for permutation (%s). (Line: %d, Col: %d)", permutationGroup.name.c_str(), currentToken.lineNum, currentToken.colNum);
                    return false;
                }
                break;
            }

            default:
                ReportError(8, "Internal compiler error for permutation (%s), please contact Pursche. (Line: %d, Col: %d)", permutationGroup.name.c_str(), currentToken.lineNum, currentToken.colNum);
                return false;
            }
        }

        return tokenTypeStack.size() == 1 && ruleSetStack.size() == 0;
    }
}