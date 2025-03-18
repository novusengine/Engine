#pragma once
#include "TypeParser/Types.h"

#include "Base/Types.h"

namespace TypeParser
{
    struct Module;
    struct LexerInfo;
    struct ParserInfo;
    struct ParsedType;
    class Parser
    {
    public:
        static void Process(Module& module);

    private:
        static bool TryParseType(LexerInfo& lexerInfo, ParserInfo& parserInfo);
        static void ParseClientDB(LexerInfo& lexerInfo, ParsedType& parsedType);
        static void ParseCommand(LexerInfo& lexerInfo, ParsedType& parsedType);
        static void ParseEnum(LexerInfo& lexerInfo, ParsedType& parsedType);
        static void ParseProperty(LexerInfo& lexerInfo, ParsedType& parsedType);
        static void ParsePropertyValue(LexerInfo& lexerInfo, ParsedTypeProperty& parsedTypeProperty);
        static void ParsePropertyType(LexerInfo& lexerInfo, ParsedTypeProperty& parsedTypeProperty, TypeProperty& property);
    };
}