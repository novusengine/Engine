#include "Parser.h"

namespace TypeParser
{
    void Parser::Process(Module& module)
    {
        NC_LOG_INFO("Parser : Processing (Tokens: {0})", module.lexerInfo.tokens.size());

        while (true)
        {
            if (!TryParseType(module.lexerInfo, module.parserInfo))
                break;
        }

        NC_LOG_INFO("Parser : Processed (Tokens: {0}, Types: {1})", module.lexerInfo.tokens.size(), module.parserInfo.parsedTypes.size());
    }

    bool Parser::TryParseType(LexerInfo& lexerInfo, ParserInfo& parserInfo)
    {
        if (lexerInfo.PeekToken()->kind == Token::Kind::End_of_File)
            return false;

        Token* startToken = lexerInfo.ExpectToken(Token::Kind::Keyword_Type);
        Token* identifierToken = lexerInfo.ExpectToken(Token::Kind::Identifier);
        lexerInfo.ExpectToken(Token::Kind::Op_LessThan);

        Token* typeKindToken = lexerInfo.ConsumeToken();
        if (typeKindToken->kind != Token::Kind::Keyword_ClientDB && typeKindToken->kind != Token::Kind::Keyword_Command && typeKindToken->kind != Token::Kind::Keyword_Packet && typeKindToken->kind != Token::Kind::Keyword_Enum)
        {
            lexerInfo.Error(typeKindToken, "Expecting 'ClientDB', 'Command', 'Packet' or 'Enum' but got '%s'", Token::GetTokenKindName(typeKindToken->kind));
        }

        lexerInfo.ExpectToken(Token::Kind::Op_GreaterThan);
        lexerInfo.ExpectToken(Token::Kind::Op_Assign);

        ParsedType& parsedType = parserInfo.parsedTypes.emplace_back();
        parsedType.startToken = startToken;
        parsedType.identifierToken = identifierToken;
        parsedType.kindToken = typeKindToken;

        switch (parsedType.kindToken->kind)
        {
            case Token::Kind::Keyword_ClientDB:
            {
                ParseClientDB(lexerInfo, parsedType);
                break;
            }

            case Token::Kind::Keyword_Command:
            {
                ParseCommand(lexerInfo, parsedType);
                break;
            }
            case Token::Kind::Keyword_Packet:
            {
                parsedType.kind = ParsedTypeKind::Packet;
                break;
            }
            case Token::Kind::Keyword_Enum:
            {
                ParseEnum(lexerInfo, parsedType);
                break;
            }

            default: break;
        }

        return true;
    }

    void Parser::ParseClientDB(LexerInfo& lexerInfo, ParsedType& parsedType)
    {
        parsedType.kind = ParsedTypeKind::ClientDB;
        parsedType.properties.reserve(8);
        parsedType.propertyHashToIndex.reserve(8);

        parsedType.startToken = lexerInfo.ExpectToken(Token::Kind::Curley_Bracket_Open);
        {
            while (lexerInfo.PeekToken()->kind != Token::Kind::Curley_Bracket_Close)
            {
                ParseProperty(lexerInfo, parsedType);
            }
        }
        parsedType.endToken = lexerInfo.ExpectToken(Token::Kind::Curley_Bracket_Close);

        if (lexerInfo.PeekToken()->kind == Token::Kind::Semicolon)
            lexerInfo.ConsumeToken();
    }

    void Parser::ParseCommand(LexerInfo& lexerInfo, ParsedType& parsedType)
    {
        parsedType.kind = ParsedTypeKind::Command;
        parsedType.properties.reserve(8);
        parsedType.propertyHashToIndex.reserve(8);

        parsedType.startToken = lexerInfo.ExpectToken(Token::Kind::Curley_Bracket_Open);
        {
            while (lexerInfo.PeekToken()->kind != Token::Kind::Curley_Bracket_Close)
            {
                ParseProperty(lexerInfo, parsedType);
            }
        }
        parsedType.endToken = lexerInfo.ExpectToken(Token::Kind::Curley_Bracket_Close);
        
        if (lexerInfo.PeekToken()->kind == Token::Kind::Semicolon)
            lexerInfo.ConsumeToken();
    }

    void Parser::ParseEnum(LexerInfo& lexerInfo, ParsedType& parsedType)
    {
        parsedType.kind = ParsedTypeKind::Enum;
        parsedType.properties.reserve(8);
        parsedType.propertyHashToIndex.reserve(8);

        parsedType.startToken = lexerInfo.ExpectToken(Token::Kind::Curley_Bracket_Open);
        {
            while (lexerInfo.PeekToken()->kind != Token::Kind::Curley_Bracket_Close)
            {
                ParseProperty(lexerInfo, parsedType);
            }
        }
        parsedType.endToken = lexerInfo.ExpectToken(Token::Kind::Curley_Bracket_Close);

        if (lexerInfo.PeekToken()->kind == Token::Kind::Semicolon)
            lexerInfo.ConsumeToken();
    }

    void Parser::ParseProperty(LexerInfo& lexerInfo, ParsedType& parsedType)
    {
        Token* identifierToken = lexerInfo.ExpectToken(Token::Kind::Identifier);
        if (identifierToken->nameHash.hash == 0)
            lexerInfo.Error(identifierToken, "Invalid identifier");

        if (parsedType.propertyHashToIndex.contains(identifierToken->nameHash.hash))
            lexerInfo.Error(identifierToken, "Property '%.*s' already exists", identifierToken->nameHash.length, identifierToken->nameHash.name);

        u32 propertyIndex = static_cast<u32>(parsedType.properties.size());
        ParsedTypeProperty& property = parsedType.properties.emplace_back();
        property.name = std::string(identifierToken->nameHash.name, identifierToken->nameHash.length);
        parsedType.propertyHashToIndex[identifierToken->nameHash.hash] = propertyIndex;

        lexerInfo.ExpectToken(Token::Kind::Colon);

        Token* nextToken = lexerInfo.PeekToken();
        if (nextToken->kind == Token::Kind::Curley_Bracket_Open)
        {
            lexerInfo.ConsumeToken();
            {
                while (lexerInfo.PeekToken()->kind != Token::Kind::Curley_Bracket_Close)
                {
                    ParsePropertyValue(lexerInfo, property);
                }
            }
            lexerInfo.ExpectToken(Token::Kind::Curley_Bracket_Close);
        }
        else
        {
            ParsePropertyType(lexerInfo, property, property.type);
        }

        if (lexerInfo.PeekToken()->kind == Token::Kind::Semicolon)
            lexerInfo.ConsumeToken();
    }

    void Parser::ParsePropertyValue(LexerInfo& lexerInfo, ParsedTypeProperty& parsedTypeProperty)
    {
        Token* valueToken = lexerInfo.PeekToken();
        if (valueToken->kind != Token::Kind::Identifier && valueToken->kind != Token::Kind::String)
            lexerInfo.Error(valueToken, "Expecting identifier or string but got '%s'", Token::GetTokenKindName(valueToken->kind));

        lexerInfo.ConsumeToken();

        switch (valueToken->kind)
        {
            case Token::Kind::Identifier:
            {
                if (valueToken->nameHash.hash == 0)
                    lexerInfo.Error(valueToken, "Invalid identifier");

                if (parsedTypeProperty.type.kind != TypePropertyKind::None && parsedTypeProperty.type.kind != TypePropertyKind::Identifier)
                    lexerInfo.Error(valueToken, "Expecting '%s' but got identifier", ParsedTypeProperty::GetTypePropertyKindName(parsedTypeProperty.type.kind));

                if (parsedTypeProperty.type.kind == TypePropertyKind::None)
                    parsedTypeProperty.type.kind = TypePropertyKind::Identifier;

                lexerInfo.ExpectToken(Token::Kind::Colon);

                Token* kindToken = lexerInfo.PeekToken();
                if (kindToken->kind != Token::Kind::Identifier)
                    lexerInfo.Error(kindToken, "Expecting identifier but got '%s'", Token::GetTokenKindName(kindToken->kind));

                TypeProperty& value = parsedTypeProperty.values.emplace_back();
                value.name = std::string(valueToken->nameHash.name, valueToken->nameHash.length);
                value.kind = ParsedTypeProperty::GetTypePropertyKindFromNameHash(kindToken->nameHash.hash);

                ParsePropertyType(lexerInfo, parsedTypeProperty, value);

                break;
            }
                
            case Token::Kind::String:
            {
                if (valueToken->nameHash.length == 0)
                    lexerInfo.Error(valueToken, "Invalid string");

                if (parsedTypeProperty.type.kind != TypePropertyKind::None && parsedTypeProperty.type.kind != TypePropertyKind::string)
                    lexerInfo.Error(valueToken, "Expecting '%s' but got string", ParsedTypeProperty::GetTypePropertyKindName(parsedTypeProperty.type.kind));

                if (parsedTypeProperty.type.kind == TypePropertyKind::None)
                    parsedTypeProperty.type.kind = TypePropertyKind::string;

                TypeProperty& value = parsedTypeProperty.values.emplace_back();
                value.name = std::string(valueToken->nameHash.name, valueToken->nameHash.length);
                value.kind = TypePropertyKind::string;

                break;
            }
        }

        if (lexerInfo.PeekToken()->kind == Token::Kind::Comma)
            lexerInfo.ConsumeToken();
    }

    void Parser::ParsePropertyType(LexerInfo& lexerInfo, ParsedTypeProperty& parsedTypeProperty, TypeProperty& property)
    {
        Token* kindToken = lexerInfo.PeekToken();
        if (kindToken->kind != Token::Kind::Identifier && kindToken->kind != Token::Kind::String)
            lexerInfo.Error(kindToken, "Expecting identifier or string but got '%s'", Token::GetTokenKindName(kindToken->kind));

        lexerInfo.ConsumeToken();

        switch (kindToken->kind)
        {
            case Token::Kind::Identifier:
            {
                property.kind = ParsedTypeProperty::GetTypePropertyKindFromNameHash(kindToken->nameHash.hash);

                if (property.kind == TypePropertyKind::None)
                {
                    lexerInfo.Error(kindToken, "Invalid type '%.*s'", kindToken->nameHash.length, kindToken->nameHash.name);
                }
                else if (property.kind == TypePropertyKind::Value)
                {
                    lexerInfo.ExpectToken(Token::Kind::Op_LessThan);
                    {
                        bool isNegative = false;
                        Token* peekToken = lexerInfo.PeekToken();

                        if (peekToken->kind == Token::Kind::Op_Subtract)
                        {
                            isNegative = true;
                            lexerInfo.ConsumeToken();
                        }

                        Token* valueToken = lexerInfo.ExpectToken(Token::Kind::Number);
                        property.arrayInfo = TypeArrayInfo{ TypePropertyKind::Value, valueToken->number, isNegative };
                    }
                    lexerInfo.ExpectToken(Token::Kind::Op_GreaterThan);
                }
                else if (property.kind == TypePropertyKind::array)
                {
                    lexerInfo.ExpectToken(Token::Kind::Op_LessThan);
                    {
                        Token* arrayKindToken = lexerInfo.ExpectToken(Token::Kind::Identifier);

                        TypePropertyKind arrayKind = ParsedTypeProperty::GetTypePropertyKindFromNameHash(arrayKindToken->nameHash.hash);
                        if (arrayKind == TypePropertyKind::None || arrayKind == TypePropertyKind::array)
                            lexerInfo.Error(arrayKindToken, "Invalid type '%.*s'", arrayKindToken->nameHash.length, arrayKindToken->nameHash.name);

                        lexerInfo.ExpectToken(Token::Kind::Comma);

                        Token* arrayCountToken = lexerInfo.ExpectToken(Token::Kind::Number);
                        if (arrayCountToken->number == 0)
                            lexerInfo.Error(arrayCountToken, "Invalid array count");

                        property.arrayInfo = TypeArrayInfo{ ParsedTypeProperty::GetTypePropertyKindFromNameHash(arrayKindToken->nameHash.hash), arrayCountToken->number };
                    }
                    lexerInfo.ExpectToken(Token::Kind::Op_GreaterThan);
                }

                break;
            }

            case Token::Kind::String:
            {
                if (kindToken->nameHash.length == 0)
                    lexerInfo.Error(kindToken, "Invalid string");

                if (parsedTypeProperty.type.kind != TypePropertyKind::None && parsedTypeProperty.type.kind != TypePropertyKind::string)
                    lexerInfo.Error(kindToken, "Expecting '%s' but got string", ParsedTypeProperty::GetTypePropertyKindName(parsedTypeProperty.type.kind));

                if (parsedTypeProperty.type.kind == TypePropertyKind::None)
                    parsedTypeProperty.type.kind = TypePropertyKind::string;

                TypeProperty& value = parsedTypeProperty.values.emplace_back();
                value.name = std::string(kindToken->nameHash.name, kindToken->nameHash.length);
                value.kind = TypePropertyKind::string;

                break;
            }
        }
    }
}