#include <Base/Types.h>
#include <Base/Memory/Bytebuffer.h>
#include <Base/Memory/FileReader.h>
#include <Base/Memory/FileWriter.h>
#include <Base/Util/DebugHandler.h>
#include <Base/Util/StringUtils.h>

#include <TypeParser/Types.h>
#include <TypeParser/Lexer/Lexer.h>
#include <TypeParser/Parser/Parser.h>

#include <quill/Backend.h>

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <execution>

struct Packet
{
public:
    u16 id;
    std::string name;
};
struct PacketList
{
public:
    u16 nextID = 0;
    std::vector<Packet> packets;
};

void WriteContent(std::string& fileContent, const std::string& content, u32 indent = 0)
{
    for (u32 i = 0; i < indent; i++)
        fileContent += "    ";

    fileContent += content;
}

void WriteArrayField(std::string& fileContent, const TypeParser::TypeArrayInfo& arrayInfo, u32 indent)
{
    WriteContent(fileContent, "std::array<", indent);
    WriteContent(fileContent, TypeParser::ParsedTypeProperty::GetTypePropertyKindName(arrayInfo.kind));
    WriteContent(fileContent, ", ");
    WriteContent(fileContent, std::to_string(arrayInfo.count));
    WriteContent(fileContent, ">");
};

void WriteMetaName(std::string& fileContent, const std::string& name, u32 indent)
{
    WriteContent(fileContent, "static inline std::string Name = \"", indent);
    WriteContent(fileContent, name);
    WriteContent(fileContent, "\";\n");
};
void WriteMetaNameHash(std::string& fileContent, const std::string& name, u32 indent)
{
    u32 hash = StringUtils::fnv1a_32(name.c_str(), name.size());

    WriteContent(fileContent, "static inline u32 NameHash = ", indent);
    WriteContent(fileContent, std::to_string(hash));
    WriteContent(fileContent, ";\n\n");
};
void WriteDefaultConstructors(std::string& fileContent, const std::string& structName, u32 indent)
{
    // Default Constructor
    {
        WriteContent(fileContent, "// Default Constructor\n", indent);
        WriteContent(fileContent, structName, indent);
        WriteContent(fileContent, "() = default;\n\n");
    }

    // Copy/Move Constructor
    {
        WriteContent(fileContent, "// Default Copy/Move Constructors/Operators\n", indent);
        WriteContent(fileContent, structName, indent);
        WriteContent(fileContent, "(const ");
        WriteContent(fileContent, structName);
        WriteContent(fileContent, "& other) = default;\n");

        WriteContent(fileContent, structName, indent);
        WriteContent(fileContent, "(");
        WriteContent(fileContent, structName);
        WriteContent(fileContent, "&& other) = default;\n");

        WriteContent(fileContent, structName, indent);
        WriteContent(fileContent, "& operator=(const ");
        WriteContent(fileContent, structName);
        WriteContent(fileContent, "& other) = default;\n");
        WriteContent(fileContent, structName, indent);
        WriteContent(fileContent, "& operator=(");
        WriteContent(fileContent, structName);
        WriteContent(fileContent, "&& other) = default;\n\n");
    }
};

bool WriteClientDBField(std::string& fileContent, const std::string& name, const TypeParser::TypeProperty& property, u32 indent)
{
    switch (property.kind)
    {
        case TypeParser::TypePropertyKind::string:
        {
            NC_LOG_ERROR("ClientDB Record '{0}' has a string field which is not allowed (Did you mean to use 'stringref')", name);
            return false;
        }

        case TypeParser::TypePropertyKind::array:
        {
            WriteArrayField(fileContent, *property.arrayInfo, indent);
            break;
        }
        default:
        {
            WriteContent(fileContent, TypeParser::ParsedTypeProperty::GetTypePropertyKindName(property.kind), indent);
            break;
        }
    }

    WriteContent(fileContent, " ");
    WriteContent(fileContent, property.name);
    WriteContent(fileContent, ";\n");
    return true;
};
void WriteClientDBMetaField(std::string& fileContent, const TypeParser::TypeProperty& property, u32 indent)
{
    WriteContent(fileContent, "{ \"", indent);
    WriteContent(fileContent, property.name);
    WriteContent(fileContent, "\", ClientDB::FieldType::");

    switch (property.kind)
    {
        case TypeParser::TypePropertyKind::boolean:
        {
            WriteContent(fileContent, "i8");
            break;
        }

        case TypeParser::TypePropertyKind::array:
        {
            WriteContent(fileContent, TypeParser::ParsedTypeProperty::GetTypePropertyKindName(property.arrayInfo->kind));
            WriteContent(fileContent, ", ");
            WriteContent(fileContent, std::to_string(property.arrayInfo->count));
            break;
        }
        default:
        {
            WriteContent(fileContent, TypeParser::ParsedTypeProperty::GetTypePropertyKindName(property.kind));
            break;
        }
    }

    WriteContent(fileContent, " }");
};
void WriteClientDBMetaFields(std::string& fileContent, const TypeParser::ParsedTypeProperty& parameterProperties, u32 numParameters, u32 indent)
{
    WriteContent(fileContent, "static inline std::vector<ClientDB::FieldInfo> FieldInfo =\n", indent);
    WriteContent(fileContent, "{\n", indent);

    indent++;
    {
        for (u32 valueIndex = 0; valueIndex < numParameters; valueIndex++)
        {
            const TypeParser::TypeProperty& property = parameterProperties.values[valueIndex];

            WriteClientDBMetaField(fileContent, property, indent);

            if (valueIndex != numParameters - 1)
                WriteContent(fileContent, ",");

            WriteContent(fileContent, "\n");
        }
    }
    indent--;

    WriteContent(fileContent, "};\n\n", indent);
};
void WriteClientDBSerializeFunction(std::string& fileContent, u32 indent)
{
    WriteContent(fileContent, "bool Serialize(Bytebuffer* buffer) const\n", indent);
    WriteContent(fileContent, "{\n", indent);

    indent++;
    {
        WriteContent(fileContent, "bool result = buffer->Put(*this);\n", indent);
        WriteContent(fileContent, "return result;\n", indent);
    }
    indent--;

    WriteContent(fileContent, "}\n\n", indent);
};
void WriteClientDBDeserializeFunction(std::string& fileContent, u32 indent)
{
    WriteContent(fileContent, "bool Deserialize(Bytebuffer* buffer)\n", indent);
    WriteContent(fileContent, "{\n", indent);

    indent++;
    {
        WriteContent(fileContent, "bool result = buffer->Get(*this);\n", indent);
        WriteContent(fileContent, "return result;\n", indent);
    }
    indent--;

    WriteContent(fileContent, "}\n", indent);
};

bool WriteCommandField(std::string& fileContent, const std::string& name, const TypeParser::TypeProperty& property, u32 indent)
{
    switch (property.kind)
    {
        case TypeParser::TypePropertyKind::array:
        {
            NC_LOG_ERROR("Command '{0}' has an array field which is currently not supported", name);
            break;
        }

        case TypeParser::TypePropertyKind::StringRef:
        {
            NC_LOG_ERROR("Command '{0}' has a StringRef field which is not allowed (Did you mean to use 'string')", name);
            return false;
        }


        case TypeParser::TypePropertyKind::string:
        {
            WriteContent(fileContent, "std::string", indent);
            break;
        }

        default:
        {
            WriteContent(fileContent, TypeParser::ParsedTypeProperty::GetTypePropertyKindName(property.kind), indent);
            break;
        }
    }

    WriteContent(fileContent, " ");
    WriteContent(fileContent, property.name);
    WriteContent(fileContent, ";\n");

    return true;
};
bool WriteCommandFields(std::string& fileContent, const std::string& name, const TypeParser::ParsedTypeProperty& parameterProperties, u32 numParameters, u32 indent)
{
    for (u32 valueIndex = 0; valueIndex < numParameters; valueIndex++)
    {
        const TypeParser::TypeProperty& property = parameterProperties.values[valueIndex];

        if (!WriteCommandField(fileContent, name, property, indent))
            return false;

        if (valueIndex == numParameters - 1)
            WriteContent(fileContent, "\n");
    }

    return true;
};
void WriteCommandMetaParameters(std::string& fileContent, u32 numParameters, u32 numOptionalParameters, u32 indent)
{
    WriteContent(fileContent, "static inline constexpr u32 NumParameters = ", indent);
    WriteContent(fileContent, std::to_string(numParameters));
    WriteContent(fileContent, ";\n");

    WriteContent(fileContent, "static inline constexpr u32 NumParametersOptional = ", indent);
    WriteContent(fileContent, std::to_string(numOptionalParameters));
    WriteContent(fileContent, ";\n\n");
};
void WriteCommandMetaNameList(std::string& fileContent, const TypeParser::ParsedTypeProperty& namesProperties, u32 numCommandNames, u32 indent)
{
    WriteContent(fileContent, "static inline constexpr std::array<std::string_view, ", indent);
    WriteContent(fileContent, std::to_string(numCommandNames));
    WriteContent(fileContent, "> CommandNameList = {");

    indent++;
    {
        for (u32 i = 0; i < numCommandNames; i++)
        {
            if (i == 0)
                WriteContent(fileContent, " ");

            WriteContent(fileContent, "\"");
            WriteContent(fileContent, namesProperties.values[i].name);
            WriteContent(fileContent, "\"");

            if (i == numCommandNames - 1)
            {
                WriteContent(fileContent, " ");
            }
            else
            {
                WriteContent(fileContent, ", ");
            }
        }
    }
    indent--;

    WriteContent(fileContent, "};\n");
};
void WriteCommandMetaHelp(std::string& fileContent, const TypeParser::ParsedTypeProperty& parameterProperties, u32 numParameters, u32 indent)
{
    WriteContent(fileContent, "static inline constexpr std::string_view CommandHelp = \"(", indent);

    for (u32 i = 0; i < numParameters; i++)
    {
        WriteContent(fileContent, parameterProperties.values[i].name);
        WriteContent(fileContent, " : ");
        WriteContent(fileContent, TypeParser::ParsedTypeProperty::GetTypePropertyKindName(parameterProperties.values[i].kind));

        if (i != numParameters - 1)
            WriteContent(fileContent, ", ");
    }

    WriteContent(fileContent, ")\";\n\n");
};
void WriteCommandMetaParameterNameList(std::string& fileContent, const TypeParser::ParsedTypeProperty& parameterProperties, u32 numParameters, u32 indent)
{
    WriteContent(fileContent, "static inline constexpr std::array<std::string_view, ", indent);
    WriteContent(fileContent, std::to_string(numParameters));
    WriteContent(fileContent, "> ParameterNameList = {");

    indent++;
    {
        for (u32 i = 0; i < numParameters; i++)
        {
            if (i == 0)
                WriteContent(fileContent, " ");

            WriteContent(fileContent, "\"");
            WriteContent(fileContent, parameterProperties.values[i].name);
            WriteContent(fileContent, "\"");

            if (i == numParameters - 1)
            {
                WriteContent(fileContent, " ");
            }
            else
            {
                WriteContent(fileContent, ", ");
            }
        }
    }
    indent--;

    WriteContent(fileContent, "};\n");
};
void WriteCommandMetaParameterTypeList(std::string& fileContent, const TypeParser::ParsedTypeProperty& parameterProperties, u32 numParameters, u32 indent)
{
    WriteContent(fileContent, "using ParameterTypeList = std::tuple<", indent);

    for (u32 parameterIndex = 0; parameterIndex < numParameters; parameterIndex++)
    {
        const TypeParser::TypeProperty& property = parameterProperties.values[parameterIndex];

        switch (property.kind)
        {
            case TypeParser::TypePropertyKind::string:
            {
                WriteContent(fileContent, "std::string");
                break;
            }

            default:
            {
                WriteContent(fileContent, TypeParser::ParsedTypeProperty::GetTypePropertyKindName(property.kind));
                break;
            }
        }

        if (parameterIndex != numParameters - 1)
            WriteContent(fileContent, ", ");
    }
    WriteContent(fileContent, ">;\n\n");
};
void WriteCommandReadFunction(std::string& fileContent, const std::string& structName, const TypeParser::ParsedTypeProperty& parameterProperties, u32 numParameters, u32 indent)
{
    WriteContent(fileContent, "static bool Read(std::vector<std::string>& parameters, ", indent);
    WriteContent(fileContent, structName);
    WriteContent(fileContent, "& result)\n");
    WriteContent(fileContent, "{\n", indent);
    indent++;
    {
        if (numParameters > 0)
        {
            WriteContent(fileContent, "static constexpr u32 NumRequiredParameters = NumParameters - NumParametersOptional;\n\n", indent);
            WriteContent(fileContent, "u32 numParams = static_cast<u32>(parameters.size());\n", indent);
            WriteContent(fileContent, "if (numParams < NumRequiredParameters) return false;\n", indent);
            WriteContent(fileContent, "if (numParams > NumRequiredParameters + NumParametersOptional) return false;\n\n", indent);

            WriteContent(fileContent, "try\n", indent);
            WriteContent(fileContent, "{\n", indent);

            indent++;
            {
                WriteContent(fileContent, structName, indent);
                WriteContent(fileContent, " cmd = {};\n");

                for (u32 parameterIndex = 0; parameterIndex < numParameters; parameterIndex++)
                {
                    const TypeParser::TypeProperty& property = parameterProperties.values[parameterIndex];

                    if (property.kind == TypeParser::TypePropertyKind::None)
                        continue;

                    WriteContent(fileContent, "cmd.", indent);
                    WriteContent(fileContent, property.name);
                    WriteContent(fileContent, " = ");

                    const std::string paramIndexStr = std::to_string(parameterIndex);;

                    switch (property.kind)
                    {
                        case TypeParser::TypePropertyKind::boolean:
                        {
                            WriteContent(fileContent, "(parameters[");
                            WriteContent(fileContent, paramIndexStr);
                            WriteContent(fileContent, "].size() > 0 && parameters[");
                            WriteContent(fileContent, paramIndexStr);
                            WriteContent(fileContent, "][0] == '1') || parameters[");
                            WriteContent(fileContent, paramIndexStr);
                            WriteContent(fileContent, "] == \"true\";\n");
                            break;
                        }
                        case TypeParser::TypePropertyKind::i8:
                        {
                            WriteContent(fileContent, "static_cast<i8>(std::stoi(parameters[");
                            WriteContent(fileContent, paramIndexStr);
                            WriteContent(fileContent, "]));\n");
                            break;
                        }
                        case TypeParser::TypePropertyKind::i16:
                        {
                            WriteContent(fileContent, "static_cast<i16>(std::stoi(parameters[");
                            WriteContent(fileContent, paramIndexStr);
                            WriteContent(fileContent, "]));\n");
                            break;
                        }
                        case TypeParser::TypePropertyKind::i32:
                        {
                            WriteContent(fileContent, "std::stoi(parameters[");
                            WriteContent(fileContent, paramIndexStr);
                            WriteContent(fileContent, "]);\n");
                            break;
                        }
                        case TypeParser::TypePropertyKind::i64:
                        {
                            WriteContent(fileContent, "std::stoll(parameters[");
                            WriteContent(fileContent, paramIndexStr);
                            WriteContent(fileContent, "]);\n");
                            break;
                        }
                        case TypeParser::TypePropertyKind::u8:
                        {
                            WriteContent(fileContent, "static_cast<u8>(std::stoul(parameters[");
                            WriteContent(fileContent, paramIndexStr);
                            WriteContent(fileContent, "]));\n");
                            break;
                        }
                        case TypeParser::TypePropertyKind::u16:
                        {
                            WriteContent(fileContent, "static_cast<u16>(std::stoul(parameters[");
                            WriteContent(fileContent, paramIndexStr);
                            WriteContent(fileContent, "]));\n");
                            break;
                        }
                        case TypeParser::TypePropertyKind::u32:
                        {
                            WriteContent(fileContent, "std::stoul(parameters[");
                            WriteContent(fileContent, paramIndexStr);
                            WriteContent(fileContent, "]);\n");
                            break;
                        }
                        case TypeParser::TypePropertyKind::u64:
                        {
                            WriteContent(fileContent, "std::stoull(parameters[");
                            WriteContent(fileContent, paramIndexStr);
                            WriteContent(fileContent, "]);\n");
                            break;
                        }
                        case TypeParser::TypePropertyKind::f32:
                        {
                            WriteContent(fileContent, "std::stof(parameters[");
                            WriteContent(fileContent, paramIndexStr);
                            WriteContent(fileContent, "]);\n");
                            break;
                        }
                        case TypeParser::TypePropertyKind::f64:
                        {
                            WriteContent(fileContent, "std::stod(parameters[");
                            WriteContent(fileContent, paramIndexStr);
                            WriteContent(fileContent, "]);\n");
                            break;
                        }
                        case TypeParser::TypePropertyKind::string:
                        {
                            WriteContent(fileContent, "std::move(parameters[");
                            WriteContent(fileContent, paramIndexStr);
                            WriteContent(fileContent, "]);\n");
                            break;
                        }
                    }

                    if (parameterIndex == numParameters - 1)
                        WriteContent(fileContent, "\n");
                }

                WriteContent(fileContent, "result = std::move(cmd);\n", indent);
            }

            indent--;

            WriteContent(fileContent, "}\n", indent);
            WriteContent(fileContent, "catch (const std::exception&) { return false; }\n\n", indent);
        }
        else
        {
            WriteContent(fileContent, "result = {};\n", indent);
        }

        WriteContent(fileContent, "return true;\n", indent);
    }
    indent--;

    WriteContent(fileContent, "}\n", indent);
};

bool WritePacketField(std::string& fileContent, const std::string& name, const TypeParser::TypeProperty& property, u32 indent)
{
    switch (property.kind)
    {
        case TypeParser::TypePropertyKind::None:
        case TypeParser::TypePropertyKind::Auto:
        case TypeParser::TypePropertyKind::Value:
        case TypeParser::TypePropertyKind::Identifier:
        case TypeParser::TypePropertyKind::StringRef:
        {
            NC_LOG_ERROR("Packet '{0}' has a {1} field which is not allowed", name, TypeParser::ParsedTypeProperty::GetTypePropertyKindName(property.kind));
            return false;
        }

        case TypeParser::TypePropertyKind::string:
        {
            WriteContent(fileContent, "std::string", indent);
            break;
        }

        case TypeParser::TypePropertyKind::array:
        {
            WriteArrayField(fileContent, *property.arrayInfo, indent);
            break;
        }

        default:
        {
            WriteContent(fileContent, TypeParser::ParsedTypeProperty::GetTypePropertyKindName(property.kind), indent);
            break;
        }
    }

    WriteContent(fileContent, " ");
    WriteContent(fileContent, property.name);
    WriteContent(fileContent, ";\n");

    return true;
};
bool WritePacketFields(std::string& fileContent, const std::string& name, const TypeParser::ParsedTypeProperty& fieldProperties, u32 numFields, u32 indent)
{
    for (u32 valueIndex = 0; valueIndex < numFields; valueIndex++)
    {
        const TypeParser::TypeProperty& property = fieldProperties.values[valueIndex];
        
        if (!WritePacketField(fileContent, name, property, indent))
            return false;

        if (valueIndex == numFields - 1)
            WriteContent(fileContent, "\n");
    }

    return true;
};
void WritePacketSerializeFunction(std::string& fileContent, const TypeParser::ParsedTypeProperty& fieldProperties, u32 numFields, u32 indent)
{
    WriteContent(fileContent, "bool Serialize(Bytebuffer* buffer) const\n", indent);
    WriteContent(fileContent, "{\n", indent);

    indent++;
    {
        if (numFields == 0)
        {
            WriteContent(fileContent, "return true;\n", indent);
        }
        else
        {
            WriteContent(fileContent, "bool failed = false;\n\n", indent);

            for (u32 valueIndex = 0; valueIndex < numFields; valueIndex++)
            {
                const TypeParser::TypeProperty& property = fieldProperties.values[valueIndex];

                WriteContent(fileContent, "failed |= !buffer->", indent);

                switch (property.kind)
                {
                    case TypeParser::TypePropertyKind::i8:
                    {
                        WriteContent(fileContent, "PutI8(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    case TypeParser::TypePropertyKind::i16:
                    {
                        WriteContent(fileContent, "PutI16(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    case TypeParser::TypePropertyKind::i32:
                    {
                        WriteContent(fileContent, "PutI32(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    case TypeParser::TypePropertyKind::i64:
                    {
                        WriteContent(fileContent, "PutI64(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    case TypeParser::TypePropertyKind::u8:
                    {
                        WriteContent(fileContent, "PutU8(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    case TypeParser::TypePropertyKind::u16:
                    {
                        WriteContent(fileContent, "PutU16(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    case TypeParser::TypePropertyKind::u32:
                    {
                        WriteContent(fileContent, "PutU32(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    case TypeParser::TypePropertyKind::u64:
                    {
                        WriteContent(fileContent, "PutU64(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    case TypeParser::TypePropertyKind::f32:
                    {
                        WriteContent(fileContent, "PutF32(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    case TypeParser::TypePropertyKind::f64:
                    {
                        WriteContent(fileContent, "PutF64(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    case TypeParser::TypePropertyKind::string:
                    {
                        WriteContent(fileContent, "PutString(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    case TypeParser::TypePropertyKind::objectguid:
                    {
                        WriteContent(fileContent, "Serialize(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    default:
                    {
                        WriteContent(fileContent, "Put(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }
                }

                WriteContent(fileContent, ");\n");
            }

            WriteContent(fileContent, "\n");
            WriteContent(fileContent, "return !failed;\n", indent);
        }
    }
    indent--;

    WriteContent(fileContent, "}\n", indent);
};
void WritePacketDeserializeFunction(std::string& fileContent, const TypeParser::ParsedTypeProperty& fieldProperties, u32 numFields, u32 indent)
{
    WriteContent(fileContent, "bool Deserialize(Bytebuffer* buffer)\n", indent);
    WriteContent(fileContent, "{\n", indent);

    indent++;
    {
        if (numFields == 0)
        {
            WriteContent(fileContent, "return true;\n", indent);
        }
        else
        {
            WriteContent(fileContent, "bool failed = false;\n\n", indent);

            for (u32 valueIndex = 0; valueIndex < numFields; valueIndex++)
            {
                const TypeParser::TypeProperty& property = fieldProperties.values[valueIndex];

                WriteContent(fileContent, "failed |= !buffer->", indent);

                switch (property.kind)
                {
                    case TypeParser::TypePropertyKind::i8:
                    {
                        WriteContent(fileContent, "GetI8(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    case TypeParser::TypePropertyKind::i16:
                    {
                        WriteContent(fileContent, "GetI16(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    case TypeParser::TypePropertyKind::i32:
                    {
                        WriteContent(fileContent, "GetI32(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    case TypeParser::TypePropertyKind::i64:
                    {
                        WriteContent(fileContent, "GetI64(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    case TypeParser::TypePropertyKind::u8:
                    {
                        WriteContent(fileContent, "GetU8(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    case TypeParser::TypePropertyKind::u16:
                    {
                        WriteContent(fileContent, "GetU16(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    case TypeParser::TypePropertyKind::u32:
                    {
                        WriteContent(fileContent, "GetU32(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    case TypeParser::TypePropertyKind::u64:
                    {
                        WriteContent(fileContent, "GetU64(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    case TypeParser::TypePropertyKind::f32:
                    {
                        WriteContent(fileContent, "GetF32(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    case TypeParser::TypePropertyKind::f64:
                    {
                        WriteContent(fileContent, "GetF64(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    case TypeParser::TypePropertyKind::string:
                    {
                        WriteContent(fileContent, "GetString(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    case TypeParser::TypePropertyKind::objectguid:
                    {
                        WriteContent(fileContent, "Deserialize(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    default:
                    {
                        WriteContent(fileContent, "Get(");
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }
                }

                WriteContent(fileContent, ");\n");
            }

            WriteContent(fileContent, "\n");
            WriteContent(fileContent, "return !failed;\n", indent);
        }
    }
    indent--;

    WriteContent(fileContent, "}\n", indent);
};
void WritePacketGetSerializedSizeFunction(std::string& fileContent, const TypeParser::ParsedTypeProperty& fieldProperties, u32 numFields, u32 indent)
{
    WriteContent(fileContent, "u16 GetSerializedSize() const\n", indent);
    WriteContent(fileContent, "{\n", indent);

    indent++;
    {
        if (numFields == 0)
        {
            WriteContent(fileContent, "return 0;\n", indent);
        }
        else
        {
            WriteContent(fileContent, "u16 size = 0;\n\n", indent);

            u32 size = 0;

            for (u32 valueIndex = 0; valueIndex < numFields; valueIndex++)
            {
                const TypeParser::TypeProperty& property = fieldProperties.values[valueIndex];

                switch (property.kind)
                {
                    case TypeParser::TypePropertyKind::boolean:
                    case TypeParser::TypePropertyKind::i8:
                    case TypeParser::TypePropertyKind::u8:
                    {
                        size += 1;
                        break;
                    }

                    case TypeParser::TypePropertyKind::i16:
                    case TypeParser::TypePropertyKind::u16:
                    {
                        size += 2;
                        break;
                    }

                    case TypeParser::TypePropertyKind::i32:
                    case TypeParser::TypePropertyKind::u32:
                    case TypeParser::TypePropertyKind::f32:
                    {
                        size += 4;
                        break;
                    }

                    case TypeParser::TypePropertyKind::i64:
                    case TypeParser::TypePropertyKind::u64:
                    case TypeParser::TypePropertyKind::f64:
                    case TypeParser::TypePropertyKind::vec2:
                    case TypeParser::TypePropertyKind::ivec2:
                    case TypeParser::TypePropertyKind::uvec2:
                    {
                        size += 8;
                        break;
                    }

                    case TypeParser::TypePropertyKind::vec3:
                    case TypeParser::TypePropertyKind::ivec3:
                    case TypeParser::TypePropertyKind::uvec3:
                    {
                        size += 12;
                        break;
                    }

                    case TypeParser::TypePropertyKind::vec4:
                    case TypeParser::TypePropertyKind::ivec4:
                    case TypeParser::TypePropertyKind::uvec4:
                    {
                        size += 16;
                        break;
                    }

                    case TypeParser::TypePropertyKind::string:
                    {
                        WriteContent(fileContent, "size += static_cast<u16>(", indent);
                        WriteContent(fileContent, property.name.c_str());
                        WriteContent(fileContent, ".length() + 1);\n");
                        break;
                    }

                    case TypeParser::TypePropertyKind::array:
                    {
                        WriteContent(fileContent, "size += static_cast<u16>(", indent);
                        WriteContent(fileContent, property.name.c_str());
                        WriteContent(fileContent, ".size() * sizeof(");
                        WriteContent(fileContent, TypeParser::ParsedTypeProperty::GetTypePropertyKindName(property.arrayInfo->kind));
                        WriteContent(fileContent, "));\n");
                        break;
                    }

                    case TypeParser::TypePropertyKind::objectguid:
                    {
                        WriteContent(fileContent, "size += static_cast<u16>(", indent);
                        WriteContent(fileContent, property.name.c_str());
                        WriteContent(fileContent, ".GetCounterBytesUsed() + 1);\n");
                        break;
                    }
                }
            }

            if (size > 0)
            {
                WriteContent(fileContent, "size += ", indent);
                WriteContent(fileContent, std::to_string(size));
                WriteContent(fileContent, ";\n");
            }

            WriteContent(fileContent, "\n");
            WriteContent(fileContent, "return size;\n", indent);
        }
    }
    indent--;

    WriteContent(fileContent, "}\n", indent);
};

bool WriteLuaStructField(std::string& fileContent, const std::string& name, const TypeParser::TypeProperty& property, u32 indent)
{
    switch (property.kind)
    {
        case TypeParser::TypePropertyKind::None:
        case TypeParser::TypePropertyKind::Auto:
        case TypeParser::TypePropertyKind::Value:
        case TypeParser::TypePropertyKind::Identifier:
        case TypeParser::TypePropertyKind::StringRef:
        case TypeParser::TypePropertyKind::vec4:
        case TypeParser::TypePropertyKind::ivec4:
        case TypeParser::TypePropertyKind::uvec4:
        case TypeParser::TypePropertyKind::array:
        {
            NC_LOG_ERROR("LuaStruct '{0}' has a {1} field which is not allowed", name, TypeParser::ParsedTypeProperty::GetTypePropertyKindName(property.kind));
            return false;
        }

        case TypeParser::TypePropertyKind::string:
        {
            WriteContent(fileContent, "std::string", indent);
            break;
        }

        default:
        {
            WriteContent(fileContent, TypeParser::ParsedTypeProperty::GetTypePropertyKindName(property.kind), indent);
            break;
        }
    }

    WriteContent(fileContent, " ");
    WriteContent(fileContent, property.name);
    WriteContent(fileContent, ";\n");

    return true;
};
bool WriteLuaStructFields(std::string& fileContent, const std::string& name, const TypeParser::ParsedTypeProperty& fieldProperties, u32 numFields, u32 indent)
{
    for (u32 valueIndex = 0; valueIndex < numFields; valueIndex++)
    {
        const TypeParser::TypeProperty& property = fieldProperties.values[valueIndex];

        if (!WritePacketField(fileContent, name, property, indent))
            return false;

        if (valueIndex == numFields - 1)
            WriteContent(fileContent, "\n");
    }

    return true;
};
void WriteLuaStructPushFunction(std::string& fileContent, const TypeParser::ParsedTypeProperty& fieldProperties, u32 numFields, u32 indent)
{
    WriteContent(fileContent, "void Push(lua_State* state) const\n", indent);
    WriteContent(fileContent, "{\n", indent);

    indent++;
    {
        if (numFields == 0)
        {
            WriteContent(fileContent, "lua_pushnil(state);\n", indent);
        }
        else
        {
            WriteContent(fileContent, "lua_newtable(state);\n\n", indent);

            for (u32 valueIndex = 0; valueIndex < numFields; valueIndex++)
            {
                const TypeParser::TypeProperty& property = fieldProperties.values[valueIndex];

                WriteContent(fileContent, "lua_pushstring(state, \"", indent);
                WriteContent(fileContent, property.name);
                WriteContent(fileContent, "\");\n");

                switch (property.kind)
                {
                    case TypeParser::TypePropertyKind::boolean:
                    {
                        WriteContent(fileContent, "lua_pushboolean(state, ", indent);
                        WriteContent(fileContent, property.name.c_str());
                        break;
                    }

                    case TypeParser::TypePropertyKind::i8:
                    case TypeParser::TypePropertyKind::i16:
                    case TypeParser::TypePropertyKind::i32:
                    case TypeParser::TypePropertyKind::i64:
                    case TypeParser::TypePropertyKind::u8:
                    case TypeParser::TypePropertyKind::u16:
                    case TypeParser::TypePropertyKind::u32:
                    case TypeParser::TypePropertyKind::u64:
                    case TypeParser::TypePropertyKind::f32:
                    case TypeParser::TypePropertyKind::f64:
                    {
                        WriteContent(fileContent, "lua_pushnumber(state, static_cast<f64>(", indent);
                        WriteContent(fileContent, property.name.c_str());
                        WriteContent(fileContent, ")");
                        break;
                    }

                    case TypeParser::TypePropertyKind::string:
                    {
                        WriteContent(fileContent, "lua_pushstring(state, ", indent);
                        WriteContent(fileContent, property.name.c_str());
                        WriteContent(fileContent, ".c_str()");
                        break;
                    }

                    case TypeParser::TypePropertyKind::objectguid:
                    {
                        WriteContent(fileContent, "lua_pushnumber(", indent);
                        WriteContent(fileContent, property.name.c_str());
                        WriteContent(fileContent, ".GetData()");

                        break;
                    }


                    case TypeParser::TypePropertyKind::vec2:
                    case TypeParser::TypePropertyKind::ivec2:
                    case TypeParser::TypePropertyKind::uvec2:
                    {
                        WriteContent(fileContent, "lua_pushvector(state, ", indent);

                        WriteContent(fileContent, property.name.c_str());
                        WriteContent(fileContent, ".x, ");
                        WriteContent(fileContent, property.name.c_str());
                        WriteContent(fileContent, ".y, ");
                        WriteContent(fileContent, "0.0f");

                        break;
                    }

                    case TypeParser::TypePropertyKind::vec3:
                    case TypeParser::TypePropertyKind::ivec3:
                    case TypeParser::TypePropertyKind::uvec3:
                    {
                        WriteContent(fileContent, "lua_pushvector(state, ", indent);

                        WriteContent(fileContent, property.name.c_str());
                        WriteContent(fileContent, ".x, ");
                        WriteContent(fileContent, property.name.c_str());
                        WriteContent(fileContent, ".y, ");
                        WriteContent(fileContent, property.name.c_str());
                        WriteContent(fileContent, ".z");

                        break;
                    }

                    default:
                    {
                        break;
                    }
                }

                WriteContent(fileContent, ");\n");
                WriteContent(fileContent, "lua_settable(state, -3);\n", indent);
            }
        }
    }
    indent--;

    WriteContent(fileContent, "}\n", indent);
};

bool GenerateCommand(const TypeParser::ParsedType& parsedType, std::string& fileContent)
{
    std::string name = std::string(parsedType.identifierToken->nameHash.name, parsedType.identifierToken->nameHash.length);
    std::string structName = name + "Command";

    u32 indent = 1;
    u32 structIndentLevel = indent;

    WriteContent(fileContent, "struct ", indent);
    WriteContent(fileContent, structName);

    WriteContent(fileContent, "\n");
    WriteContent(fileContent, "{\n", indent);

    indent++;
    {
        u32 propertyNamesHash = "Names"_djb2;
        u32 propertyParametersHash = "Parameters"_djb2;

        if (!parsedType.propertyHashToIndex.contains(propertyNamesHash))
        {
            NC_LOG_ERROR("Command '{0}' missing 'Names' property", name);
            return false;
        }

        if (!parsedType.propertyHashToIndex.contains(propertyParametersHash))
        {
            NC_LOG_ERROR("Command '{0}' missing 'Parameters' property", name);
            return false;
        }

        u32 namePropertyIndex = parsedType.propertyHashToIndex.at(propertyNamesHash);
        const TypeParser::ParsedTypeProperty& namesProperties = parsedType.properties[namePropertyIndex];

        u32 parameterPropertyIndex = parsedType.propertyHashToIndex.at(propertyParametersHash);
        const TypeParser::ParsedTypeProperty& parameterProperties = parsedType.properties[parameterPropertyIndex];

        u32 numCommandNames = static_cast<u32>(namesProperties.values.size());
        if (numCommandNames == 0)
        {
            NC_LOG_ERROR("Command '{0}' 'Names' property has no values", name);
            return false;
        }

        u32 numParameters = static_cast<u32>(parameterProperties.values.size());
        u32 numOptionalParameters = 0;
        u32 numAutoParameters = 0;

        for (u32 i = 0; i < numParameters; i++)
        {
            numOptionalParameters += parameterProperties.values[i].isOptional;
            numAutoParameters += parameterProperties.values[i].kind == TypeParser::TypePropertyKind::Auto;
        }

        if (numAutoParameters != 0)
        {
            NC_LOG_ERROR("Command '{0}' has Auto fields which is not allowed", name);
            return false;
        }

        // Parameters
        {
            if (numParameters > 0)
            {
                WriteContent(fileContent, "public:\n", structIndentLevel);
                WriteCommandFields(fileContent, name, parameterProperties, numParameters, indent);
            }
        }

        // Meta Values
        {
            WriteContent(fileContent, "public:\n", structIndentLevel);

            // Command Names / Parameter Names & Types / Help
            {
                WriteCommandMetaParameters(fileContent, numParameters, numOptionalParameters, indent);
                WriteCommandMetaNameList(fileContent, namesProperties, numCommandNames, indent);
                WriteCommandMetaHelp(fileContent, parameterProperties, numParameters, indent);
                WriteCommandMetaParameterNameList(fileContent, parameterProperties, numParameters, indent);
                WriteCommandMetaParameterTypeList(fileContent, parameterProperties, numParameters, indent);
            }
        }

        // Functions
        {
            WriteContent(fileContent, "public:\n", structIndentLevel);

            // Constructors
            {
                WriteDefaultConstructors(fileContent, structName, indent);
            }

            // Helper Functions
            {
                WriteContent(fileContent, "// Helper Functions\n", indent);

                WriteCommandReadFunction(fileContent, structName, parameterProperties, numParameters, indent);
            }
        }
    }
    indent--;

    WriteContent(fileContent, "};\n", indent);
    return true;
}
bool GenerateClientDB(const TypeParser::ParsedType& parsedType, std::string& fileContent)
{
    std::string name = std::string(parsedType.identifierToken->nameHash.name, parsedType.identifierToken->nameHash.length);
    std::string structName = name + "Record";

    u32 indent = 1;
    u32 structIndentLevel = indent;

    WriteContent(fileContent, "struct ", indent);
    WriteContent(fileContent, structName);

    WriteContent(fileContent, "\n");
    WriteContent(fileContent, "{\n", indent);

    indent++;
    {
        u32 propertyNamesHash = "Name"_djb2;
        u32 propertyParametersHash = "Fields"_djb2;

        if (!parsedType.propertyHashToIndex.contains(propertyNamesHash))
        {
            NC_LOG_ERROR("ClientDB '{0}' missing 'Name' property", name);
            return false;
        }

        if (!parsedType.propertyHashToIndex.contains(propertyParametersHash))
        {
            NC_LOG_ERROR("ClientDB '{0}' missing 'Fields' property", name);
            return false;
        }

        u32 namePropertyIndex = parsedType.propertyHashToIndex.at(propertyNamesHash);
        const TypeParser::ParsedTypeProperty& namesProperties = parsedType.properties[namePropertyIndex];

        u32 parameterPropertyIndex = parsedType.propertyHashToIndex.at(propertyParametersHash);
        const TypeParser::ParsedTypeProperty& parameterProperties = parsedType.properties[parameterPropertyIndex];

        u32 numCommandNames = static_cast<u32>(namesProperties.values.size());
        if (numCommandNames == 0)
        {
            NC_LOG_ERROR("ClientDB '{0}' 'Name' property has no values", name);
            return false;
        }

        u32 numParameters = static_cast<u32>(parameterProperties.values.size());
        u32 numOptionalParameters = 0;
        u32 numAutoParameters = 0;

        for (u32 i = 0; i < numParameters; i++)
        {
            numOptionalParameters += parameterProperties.values[i].isOptional;
            numAutoParameters += parameterProperties.values[i].kind == TypeParser::TypePropertyKind::Auto;
        }

        if (numParameters == 0)
        {
            NC_LOG_ERROR("ClientDB Record '{0}' has no fields which is not allowed", name);
            return false;
        }

        if (numOptionalParameters != 0)
        {
            NC_LOG_ERROR("ClientDB Record '{0}' has optional fields which is not allowed", name);
            return false;
        }

        if (numAutoParameters != 0)
        {
            NC_LOG_ERROR("ClientDB Record '{0}' has Auto fields which is not allowed", name);
            return false;
        }

        // Fields
        {
            WriteContent(fileContent, "public:\n", structIndentLevel);

            for (u32 valueIndex = 0; valueIndex < numParameters; valueIndex++)
            {
                const TypeParser::TypeProperty& property = parameterProperties.values[valueIndex];

                if (!WriteClientDBField(fileContent, name, property, indent))
                    return false;
            }

            WriteContent(fileContent, "\n");
        }

        // Meta Values
        {
            WriteContent(fileContent, "public:\n", structIndentLevel);

            // Name
            {
                WriteMetaName(fileContent, name, indent);
                WriteMetaNameHash(fileContent, name, indent);
            }
            
            WriteClientDBMetaFields(fileContent, parameterProperties, numParameters, indent);
        }

        // Functions
        {
            WriteContent(fileContent, "public:\n", structIndentLevel);

            // Constructors
            {
                WriteDefaultConstructors(fileContent, structName, indent);

                // Helper Functions
                {
                    WriteContent(fileContent, "// Helper Functions\n", indent);

                    // Serialize / Deserialize
                    {
                        WriteClientDBSerializeFunction(fileContent, indent);
                        WriteClientDBDeserializeFunction(fileContent, indent);
                    }
                }
            }
        }
    }
    indent--;

    WriteContent(fileContent, "};\n", indent);
    return true;
}
bool GenerateEnum(const TypeParser::ParsedType& parsedType, std::string& fileContent, u16& currentMaxEnumID)
{
    std::string name = std::string(parsedType.identifierToken->nameHash.name, parsedType.identifierToken->nameHash.length);
    const std::string& enumName = name + "Enum";

    u32 indent = 1;
    u32 enumIndentLevel = indent;

    u32 propertyNameHash = "Name"_djb2;
    u32 propertyTypeHash = "Type"_djb2;

    if (!parsedType.propertyHashToIndex.contains(propertyNameHash))
    {
        NC_LOG_ERROR("ClientDB '{0}' missing 'Name' property", name);
        return false;
    }
    if (!parsedType.propertyHashToIndex.contains(propertyTypeHash))
    {
        NC_LOG_ERROR("ClientDB '{0}' missing 'Type' property", name);
        return false;
    }

    u32 namePropertyIndex = parsedType.propertyHashToIndex.at(propertyNameHash);
    const TypeParser::ParsedTypeProperty& nameProperty = parsedType.properties[namePropertyIndex];

    u32 typePropertyIndex = parsedType.propertyHashToIndex.at(propertyTypeHash);
    const TypeParser::ParsedTypeProperty& typeProperty = parsedType.properties[typePropertyIndex];

    switch (typeProperty.type.kind)
    {
        case TypeParser::TypePropertyKind::i8:
        case TypeParser::TypePropertyKind::i16:
        case TypeParser::TypePropertyKind::i32:
        case TypeParser::TypePropertyKind::i64:
        case TypeParser::TypePropertyKind::u8:
        case TypeParser::TypePropertyKind::u16:
        case TypeParser::TypePropertyKind::u32:
        case TypeParser::TypePropertyKind::u64: break;

        default:
        {
            NC_LOG_ERROR("Enum '{0}' has an invalid type '{1}' (Enums only support (i8, i16, i32, i64, u8, u16, u32, u64)", name, TypeParser::ParsedTypeProperty::GetTypePropertyKindName(typeProperty.type.kind));
            return false;
        }
    }

    WriteContent(fileContent, "enum class ", indent);
    WriteContent(fileContent, enumName);
    WriteContent(fileContent, " : ");
    WriteContent(fileContent, TypeParser::ParsedTypeProperty::GetTypePropertyKindName(typeProperty.type.kind));

    WriteContent(fileContent, "\n");
    WriteContent(fileContent, "{\n", indent);

    u32 fieldsPropertyHash = "Fields"_djb2;

    if (!parsedType.propertyHashToIndex.contains(fieldsPropertyHash))
    {
        NC_LOG_ERROR("ClientDB '{0}' missing 'Fields' property", name);
        return false;
    }

    u32 fieldsPropertyIndex = parsedType.propertyHashToIndex.at(fieldsPropertyHash);
    const TypeParser::ParsedTypeProperty& fieldsProperties = parsedType.properties[fieldsPropertyIndex];

    u32 numFields = static_cast<u32>(fieldsProperties.values.size());
    if (numFields == 0)
    {
        NC_LOG_ERROR("Enum '{0}' 'Fields' property has no values", name);
        return false;
    }

    std::vector<u64> fieldValues;
    bool isSigned = typeProperty.type.kind >= TypeParser::TypePropertyKind::i8 && typeProperty.type.kind <= TypeParser::TypePropertyKind::i64;
    bool isUnsigned = typeProperty.type.kind >= TypeParser::TypePropertyKind::u8 && typeProperty.type.kind <= TypeParser::TypePropertyKind::u64;

    indent++;
    {
        i64 minValueForType = 0;

        union
        {
            i64 signedMaxValueForType;
            u64 unsignedMaxValueForType;
        };

        switch (typeProperty.type.kind)
        {
            case TypeParser::TypePropertyKind::i8:
            {
                minValueForType = std::numeric_limits<i8>::min();
                signedMaxValueForType = std::numeric_limits<i8>::max();
                break;
            }

            case TypeParser::TypePropertyKind::i16:
            {
                minValueForType = std::numeric_limits<i16>::min();
                signedMaxValueForType = std::numeric_limits<i16>::max();
                break;
            }

            case TypeParser::TypePropertyKind::i32:
            {
                minValueForType = std::numeric_limits<i32>::min();
                signedMaxValueForType = std::numeric_limits<i32>::max();
                break;
            }

            case TypeParser::TypePropertyKind::i64:
            {
                minValueForType = std::numeric_limits<i64>::min();
                signedMaxValueForType = std::numeric_limits<i64>::max();
                break;
            }

            case TypeParser::TypePropertyKind::u8:  unsignedMaxValueForType = std::numeric_limits<u8>::max(); break;
            case TypeParser::TypePropertyKind::u16: unsignedMaxValueForType = std::numeric_limits<u16>::max(); break;
            case TypeParser::TypePropertyKind::u32: unsignedMaxValueForType = std::numeric_limits<u32>::max(); break;
            case TypeParser::TypePropertyKind::u64: unsignedMaxValueForType = std::numeric_limits<u64>::max(); break;
        }

        union
        {
            i64 signedMaxValue;
            u64 unsignedMaxValue;
        };

        union
        {
            i64 signedValue;
            u64 unsignedValue;
        };

        if (isSigned)
        {
            signedMaxValue = minValueForType;
            signedValue = 0;
        }
        else if (isUnsigned)
        {
            unsignedMaxValue = 0;
            unsignedValue = 0;
        }

        for (u32 valueIndex = 0; valueIndex < numFields; valueIndex++)
        {
            const TypeParser::TypeProperty& property = fieldsProperties.values[valueIndex];

            if (property.kind != TypeParser::TypePropertyKind::Auto && property.kind != TypeParser::TypePropertyKind::Value)
            {
                NC_LOG_ERROR("Enum '{0}' has a field which is not of kind 'Auto' or 'Value'", name);
                return false;
            }

            bool isManualValue = property.kind == TypeParser::TypePropertyKind::Value;

            if (isSigned)
            {
                i64 valueSigned = isManualValue ? static_cast<i64>(property.arrayInfo->count) : signedValue;
                if (isManualValue && property.arrayInfo->isNegative)
                    valueSigned = -valueSigned;

                if (valueSigned < minValueForType)
                {
                    NC_LOG_ERROR("Enum '{0}' has field '{1}' with a value '{2}' less than the minimum value '{3}' for the type '{4}'", name, property.name, valueSigned, minValueForType, TypeParser::ParsedTypeProperty::GetTypePropertyKindName(typeProperty.type.kind));
                    return false;
                }

                if (valueSigned > signedMaxValueForType)
                {
                    NC_LOG_ERROR("Enum '{0}' has field '{1}' with a value '{2}' greater than the maximum value '{3}' for the type '{4}'", name, property.name, valueSigned, signedMaxValueForType, TypeParser::ParsedTypeProperty::GetTypePropertyKindName(typeProperty.type.kind));
                    return false;
                }

                if (valueSigned < signedMaxValue)
                {
                    NC_LOG_ERROR("Enum '{0}' has field '{1}' with a value '{2}' less than or equal to the previous field '{4}'", name, property.name, valueSigned, signedMaxValue);
                    return false;
                }

                signedValue = valueSigned;
                signedMaxValue = valueSigned;

                fieldValues.push_back(static_cast<u64>(valueSigned));
            }
            else if (isUnsigned)
            {
                u64 valueUnsigned = isManualValue ? property.arrayInfo->count : unsignedValue;
                if (isManualValue && property.arrayInfo->isNegative)
                {
                    NC_LOG_ERROR("Enum '{0}' has field '{1}' with a negative value '{2}' which is not permitted for the type {3}", name, property.name, -static_cast<i64>(valueUnsigned), TypeParser::ParsedTypeProperty::GetTypePropertyKindName(typeProperty.type.kind));
                    return false;
                }

                if (valueUnsigned > unsignedMaxValueForType)
                {
                    NC_LOG_ERROR("Enum '{0}' has field '{1}' with a value '{2}' greater than the maximum value '{3}' for the type '{4}'", name, property.name, valueUnsigned, unsignedMaxValueForType, TypeParser::ParsedTypeProperty::GetTypePropertyKindName(typeProperty.type.kind));
                    return false;
                }

                if (valueUnsigned < unsignedMaxValue)
                {
                    NC_LOG_ERROR("Enum '{0}' has field '{1}' with a value '{2}' less than or equal to the previous field '{3}'", name, property.name, valueUnsigned, unsignedMaxValue);
                    return false;
                }

                unsignedValue = valueUnsigned;
                unsignedMaxValue = valueUnsigned;

                fieldValues.push_back(valueUnsigned);
            }

            WriteContent(fileContent, property.name, indent);
            WriteContent(fileContent, " = ");

            if (isSigned)
            {
                WriteContent(fileContent, std::to_string(signedValue++));
            }
            else if (isUnsigned)
            {
                WriteContent(fileContent, std::to_string(unsignedValue++));
            }

            if (valueIndex == numFields - 1)
                WriteContent(fileContent, "\n");
            else
                WriteContent(fileContent, ",\n");
        }

        WriteContent(fileContent, "};\n", indent - 1);
    }
    indent--;

    std::string structMetaName = enumName + "Meta";
    WriteContent(fileContent, "struct ", indent);
    WriteContent(fileContent, structMetaName);
    WriteContent(fileContent, "\n");
    WriteContent(fileContent, "{\n", indent);
    WriteContent(fileContent, "public:\n", indent);

    indent++;
    {
        if (currentMaxEnumID == std::numeric_limits<u16>::max())
        {
            NC_LOG_ERROR("Enum '{0}' exceeded maximum number of enums (65536)", name);
            return false;
        }

        WriteContent(fileContent, "using Type = ", indent);
        WriteContent(fileContent, TypeParser::ParsedTypeProperty::GetTypePropertyKindName(typeProperty.type.kind));
        WriteContent(fileContent, ";\n\n");

        WriteContent(fileContent, "static inline constexpr u16 EnumID = ", indent);
        WriteContent(fileContent, std::to_string(currentMaxEnumID++));
        WriteContent(fileContent, ";\n");

        WriteContent(fileContent, "static inline constexpr std::string_view EnumName = \"", indent);
        WriteContent(fileContent, nameProperty.values[0].name);
        WriteContent(fileContent, "\";\n");

        WriteContent(fileContent, "static inline constexpr std::array<std::pair<std::string_view, ", indent);
        WriteContent(fileContent, TypeParser::ParsedTypeProperty::GetTypePropertyKindName(typeProperty.type.kind));
        WriteContent(fileContent, ">, ");
        WriteContent(fileContent, std::to_string(numFields));
        WriteContent(fileContent, "> EnumList = {");
        indent++;
        {
            for (u32 i = 0; i < numFields; i++)
            {
                if (i == 0)
                    WriteContent(fileContent, " ");

                u64 fieldValue = fieldValues[i];


                WriteContent(fileContent, "std::pair(\"");
                WriteContent(fileContent, fieldsProperties.values[i].name);
                WriteContent(fileContent, "\", ");

                if (isSigned)
                {
                    WriteContent(fileContent, std::to_string(static_cast<i64>(fieldValue)));
                }
                else if (isUnsigned)
                {
                    WriteContent(fileContent, std::to_string(fieldValue));
                }

                WriteContent(fileContent, ")");

                if (i == numFields - 1)
                {
                    WriteContent(fileContent, " ");
                }
                else
                {
                    WriteContent(fileContent, ", ");
                }
            }
        }
        indent--;
        WriteContent(fileContent, "};\n");

        WriteContent(fileContent, "};\n", indent - 1);
    }
    indent--;

    {
        WriteContent(fileContent, "template <>\n", indent);
        WriteContent(fileContent, "struct EnumTraits<", indent);
        WriteContent(fileContent, enumName);
        WriteContent(fileContent, ">\n");
        WriteContent(fileContent, "{\n", indent);

        indent++;
        {
            WriteContent(fileContent, "using Meta = ", indent);
            WriteContent(fileContent, structMetaName);
            WriteContent(fileContent, ";\n");
        }
        indent--;

        WriteContent(fileContent, "};\n", indent);
    }

    {
        WriteContent(fileContent, "DECLARE_GENERIC_BITWISE_OPERATORS(", indent);
        WriteContent(fileContent, enumName);
        WriteContent(fileContent, ");\n");
    }

    return true;
}
bool GeneratePacket(const TypeParser::ParsedType& parsedType, std::string& fileContent, PacketList& packetList)
{
    std::string name = std::string(parsedType.identifierToken->nameHash.name, parsedType.identifierToken->nameHash.length);

    u32 indent = 1;
    u32 structIndentLevel = indent;

    u32 propertyNameHash = "Name"_djb2;
    u32 propertyTypeHash = "Type"_djb2;

    if (!parsedType.propertyHashToIndex.contains(propertyNameHash))
    {
        NC_LOG_ERROR("Packet '{0}' missing 'Name' property", name);
        return false;
    }

    u32 namePropertyIndex = parsedType.propertyHashToIndex.at(propertyNameHash);
    const TypeParser::ParsedTypeProperty& nameProperty = parsedType.properties[namePropertyIndex];

    u32 numNames = static_cast<u32>(nameProperty.values.size());
    if (numNames == 0)
    {
        NC_LOG_ERROR("Packet '{0}' 'Name' property has no values", name);
        return false;
    }

    if (numNames > 1)
    {
        NC_LOG_ERROR("Packet '{0}' 'Name' property has multiple values, only one is allowed", name);
        return false;
    }

    const std::string& packetName = nameProperty.values[0].name;
    if (packetName.empty() || !std::isalpha(packetName[0]) || !StringUtils::StringIsAlphaNumeric(packetName))
    {
        NC_LOG_ERROR("Packet '{0}' field 'Name' is malformed. Name must start with a non numeric character and be strictly alpha numeric", name);
        return false;
    }

    std::string structName = packetName + "Packet";
    WriteContent(fileContent, "struct ", indent);
    WriteContent(fileContent, structName);
    WriteContent(fileContent, "\n");
    WriteContent(fileContent, "{\n", indent);

    u32 fieldsPropertyHash = "Fields"_djb2;

    if (!parsedType.propertyHashToIndex.contains(fieldsPropertyHash))
    {
        NC_LOG_ERROR("Packet '{0}' missing 'Fields' property", name);
        return false;
    }

    if (packetList.nextID == std::numeric_limits<u16>::max())
    {
        NC_LOG_ERROR("Packet '{0}' exceeded maximum number of packets (65536)", name);
        return false;
    }

    u16 packetID = packetList.nextID++;
    u32 fieldsPropertyIndex = parsedType.propertyHashToIndex.at(fieldsPropertyHash);
    const TypeParser::ParsedTypeProperty& fieldsProperties = parsedType.properties[fieldsPropertyIndex];

    indent++;
    {
        WriteContent(fileContent, "public:\n", indent - 1);
        WriteContent(fileContent, "static constexpr u16 PACKET_ID = ", indent);
        WriteContent(fileContent, std::to_string(packetID));
        WriteContent(fileContent, ";\n\n");

        u32 numFields = static_cast<u32>(fieldsProperties.values.size());
        if (numFields > 0)
        {
            {
                if (!WritePacketFields(fileContent, name, fieldsProperties, numFields, indent))
                    return false;
            }
        }

        {
            WriteContent(fileContent, "public:\n", indent - 1);

            WritePacketSerializeFunction(fileContent, fieldsProperties, numFields, indent);
            WritePacketDeserializeFunction(fileContent, fieldsProperties, numFields, indent);
            WritePacketGetSerializedSizeFunction(fileContent, fieldsProperties, numFields, indent);
        }
    }
    indent--;

    WriteContent(fileContent, "};\n", indent);

    packetList.packets.push_back({ packetID, packetName });

    return true;
}

bool GenerateLuaStruct(const TypeParser::ParsedType& parsedType, std::string& fileContent, u16& currentMaxLuaStructID)
{
    std::string name = std::string(parsedType.identifierToken->nameHash.name, parsedType.identifierToken->nameHash.length);

    u32 indent = 1;
    u32 structIndentLevel = indent;

    u32 propertyTypeHash = "Type"_djb2;

    const std::string& luaStructName = name;
    if (luaStructName.empty() || !std::isalpha(luaStructName[0]) || !StringUtils::StringIsAlphaNumeric(luaStructName))
    {
        NC_LOG_ERROR("LuaStruct '{0}' field 'Name' is malformed. Name must start with a non numeric character and be strictly alpha numeric", name);
        return false;
    }

    std::string structName = luaStructName;
    WriteContent(fileContent, "struct ", indent);
    WriteContent(fileContent, structName);
    WriteContent(fileContent, "\n");
    WriteContent(fileContent, "{\n", indent);

    u32 fieldsPropertyHash = "Fields"_djb2;

    if (!parsedType.propertyHashToIndex.contains(fieldsPropertyHash))
    {
        NC_LOG_ERROR("Packet '{0}' missing 'Fields' property", name);
        return false;
    }

    if (currentMaxLuaStructID == std::numeric_limits<u16>::max())
    {
        NC_LOG_ERROR("LuaStruct '{0}' exceeded maximum number of lua structs (65536)", name);
        return false;
    }

    u16 structID = currentMaxLuaStructID++;

    u32 fieldsPropertyIndex = parsedType.propertyHashToIndex.at(fieldsPropertyHash);
    const TypeParser::ParsedTypeProperty& fieldsProperties = parsedType.properties[fieldsPropertyIndex];

    indent++;
    {
        u32 numFields = static_cast<u32>(fieldsProperties.values.size());

        WriteContent(fileContent, "public:\n", indent - 1);
        WriteContent(fileContent, "static constexpr u16 StructID = ", indent);
        WriteContent(fileContent, std::to_string(structID));
        WriteContent(fileContent, ";\n");
        WriteContent(fileContent, "static constexpr u16 NumParameters = ", indent);
        WriteContent(fileContent, std::to_string(numFields));
        WriteContent(fileContent, ";\n\n");

        if (numFields > 0)
        {
            {
                WriteLuaStructFields(fileContent, name, fieldsProperties, numFields, indent);
            }
        }

        {
            WriteContent(fileContent, "public:\n", indent - 1);
            WriteLuaStructPushFunction(fileContent, fieldsProperties, numFields, indent);
        }
    }
    indent--;

    WriteContent(fileContent, "};\n", indent);

    {
        WriteContent(fileContent, "template <>\n", indent);
        WriteContent(fileContent, "struct LuaEventDataTraits<", indent);
        WriteContent(fileContent, std::to_string(structID));
        WriteContent(fileContent, ">\n");
        WriteContent(fileContent, "{\n", indent);

        indent++;
        {
            WriteContent(fileContent, "using Type = ", indent);
            WriteContent(fileContent, structName);
            WriteContent(fileContent, ";\n");
        }
        indent--;

        WriteContent(fileContent, "};\n", indent);
    }

    return true;
}

bool GenerateParsedType(const TypeParser::ParsedType& parsedType, std::string& fileContent, u16& currentMaxEnumID, PacketList& packetList, u16& currentMaxLuaStructID)
{
    switch (parsedType.kind)
    {
        case TypeParser::ParsedTypeKind::Command:
        {
            return GenerateCommand(parsedType, fileContent);
        }

        case TypeParser::ParsedTypeKind::ClientDB:
        {
            return GenerateClientDB(parsedType, fileContent);
        }

        case TypeParser::ParsedTypeKind::Enum:
        {
            return GenerateEnum(parsedType, fileContent, currentMaxEnumID);
        }

        case TypeParser::ParsedTypeKind::Packet:
        {
            return GeneratePacket(parsedType, fileContent, packetList);
        }

        case TypeParser::ParsedTypeKind::LuaStruct:
        {
            return GenerateLuaStruct(parsedType, fileContent, currentMaxLuaStructID);
        }

        default: break;
    }

    return false;
}

bool GeneratePacketEnum(std::string& fileContent, const PacketList& packetList, u16& currentMaxEnumID)
{
    u32 indent = 0;

    WriteContent(fileContent, "#pragma once\n", indent);
    WriteContent(fileContent, "#include <Base/Types.h>\n\n", indent);

    WriteContent(fileContent, "#include <array>\n\n", indent);

    WriteContent(fileContent, "namespace Generated");
    WriteContent(fileContent, "\n{\n");

    indent++;
    {
        u32 numPackets = static_cast<u32>(packetList.packets.size());

        std::string name = "PacketList";
        std::string enumName = name + "Enum";

        WriteContent(fileContent, "enum class PacketListEnum : u16\n", indent);
        WriteContent(fileContent, "{\n", indent);

        indent++;
        {
            for (u32 i = 0; i < numPackets; i++)
            {
                const auto& packet = packetList.packets[i];

                WriteContent(fileContent, packet.name, indent);
                WriteContent(fileContent, " = ");
                WriteContent(fileContent, std::to_string(packet.id));

                if (i < numPackets - 1)
                    WriteContent(fileContent, ",");

                WriteContent(fileContent, "\n");
            }
        }
        indent--;

        WriteContent(fileContent, "};\n", indent);

        std::string structMetaName = enumName + "Meta";
        WriteContent(fileContent, "struct ", indent);
        WriteContent(fileContent, structMetaName);
        WriteContent(fileContent, "\n");
        WriteContent(fileContent, "{\n", indent);
        WriteContent(fileContent, "public:\n", indent);

        indent++;
        {
            if (currentMaxEnumID == std::numeric_limits<u16>::max())
            {
                NC_LOG_ERROR("Enum '{0}' exceeded maximum number of enums (65536)", name);
                return false;
            }

            WriteContent(fileContent, "static inline constexpr u16 EnumID = ", indent);
            WriteContent(fileContent, std::to_string(currentMaxEnumID++));
            WriteContent(fileContent, ";\n");

            WriteContent(fileContent, "static inline constexpr std::string_view EnumName = \"PacketList\";\n", indent);

            WriteContent(fileContent, "static inline constexpr std::array<std::pair<std::string_view, u16>, ", indent);
            WriteContent(fileContent, std::to_string(numPackets));
            WriteContent(fileContent, "> EnumList = {");
            indent++;
            {
                for (u32 i = 0; i < numPackets; i++)
                {
                    const auto& packet = packetList.packets[i];

                    if (i == 0)
                        WriteContent(fileContent, " ");

                    WriteContent(fileContent, "std::pair(\"");
                    WriteContent(fileContent, packet.name);
                    WriteContent(fileContent, "\", ");
                    WriteContent(fileContent, std::to_string(packet.id));

                    WriteContent(fileContent, ")");

                    if (i < numPackets - 1)
                    {
                        WriteContent(fileContent, ",");
                    }

                    WriteContent(fileContent, " ");
                }
            }
            indent--;
            WriteContent(fileContent, "};\n");

            WriteContent(fileContent, "};\n", indent - 1);
        }
        indent--;

        {
            WriteContent(fileContent, "template <>\n", indent);
            WriteContent(fileContent, "struct EnumTraits<", indent);
            WriteContent(fileContent, enumName);
            WriteContent(fileContent, ">\n");
            WriteContent(fileContent, "{\n", indent);

            indent++;
            {
                WriteContent(fileContent, "using Meta = ", indent);
                WriteContent(fileContent, structMetaName);
                WriteContent(fileContent, ";\n");
            }
            indent--;

            WriteContent(fileContent, "};\n", indent);
        }

        {
            WriteContent(fileContent, "DECLARE_GENERIC_BITWISE_OPERATORS(", indent);
            WriteContent(fileContent, enumName);
            WriteContent(fileContent, ");\n");
        }
    }
    indent--;

    WriteContent(fileContent, "}\n");

    return true;
}

i32 main(int argc, char* argv[])
{
    quill::Backend::start();

    quill::ConsoleColours colors;
    auto console_sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("console_sink_1", colors, "stderr");
    quill::Logger* logger = quill::Frontend::create_or_get_logger("root", std::move(console_sink), "%(time:<16) LOG_%(log_level:<11) %(message)", "%H:%M:%S.%Qms", quill::Timezone::LocalTime, quill::ClockSourceType::System);

    if (argc != 3)
    {
        NC_LOG_ERROR("Expected two parameters, got {0}. Usage: <type_source_dir> <type_generated_dir>", argc);
        return -1;
    }

    std::string sourceDir = argv[1];
    std::string generatedDir = argv[2];

    if (!std::filesystem::exists(sourceDir))
    {
        NC_LOG_ERROR("SourceDir does not exist");
        return -1;
    }

    if (!std::filesystem::exists(generatedDir))
        std::filesystem::create_directories(generatedDir);

    if (std::filesystem::is_symlink(sourceDir))
        sourceDir = std::filesystem::read_symlink(sourceDir).string();

    if (std::filesystem::is_symlink(generatedDir))
        generatedDir = std::filesystem::read_symlink(generatedDir).string();

    NC_LOG_INFO("Source Dir: {0}", sourceDir);
    NC_LOG_INFO("Generated Dir: {0}", generatedDir);

    std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();

    u32 numLuauFiles = 0;
    u32 numLuauFilesCompiled = 0;

    std::vector<TypeParser::Module> modules;
    modules.reserve(128);

    PacketList packetList;
    packetList.packets.reserve(1024);

    std::string fileContent;
    fileContent.reserve(32768);

    u16 currentMaxEnumID = 0;
    u16 currentMaxPacketID = 0;
    u16 currentMaxLuaStructID = 0;

    // Find all luau files in the source directory
    for (auto& dirEntry : std::filesystem::recursive_directory_iterator(sourceDir))
    {
        std::filesystem::path path = dirEntry.path();
        path = path.make_preferred();

        // Skip non files
        if (!dirEntry.is_regular_file())
            continue;

        // Skip non .luau files
        if (!StringUtils::EndsWith(path.filename().string(), ".luau"))
            continue;

        numLuauFiles++;

        try
        {
            FileReader fileReader(path.string());
            if (!fileReader.Open())
            {
                NC_LOG_ERROR("Failed to open file {0}", path.string());
                break;
            }

            std::shared_ptr<Bytebuffer> bytebuffer = Bytebuffer::BorrowRuntime(fileReader.Length());
            fileReader.Read(bytebuffer.get(), fileReader.Length());

            auto& module = modules.emplace_back();
            module.lexerInfo.buffer = reinterpret_cast<const char*>(bytebuffer->GetDataPointer());
            module.lexerInfo.size = bytebuffer->writtenData;
            module.lexerInfo.tokens.reserve(module.lexerInfo.size);
            module.parserInfo.parsedTypes.reserve(1024);

            TypeParser::Lexer::Process(module);
            TypeParser::Parser::Process(module);

            numLuauFilesCompiled++;

            // Generate File Content
            fileContent.clear();

            u32 numCommands = 0;
            u32 numClientDBs = 0;
            u32 numEnums = 0;
            u32 numPackets = 0;
            u32 numLuaStructs = 0;

            u32 numParsedTypes = static_cast<u32>(module.parserInfo.parsedTypes.size());
            for (u32 parsedTypeIndex = 0; parsedTypeIndex < numParsedTypes; parsedTypeIndex++)
            {
                const TypeParser::ParsedType& parsedType = module.parserInfo.parsedTypes[parsedTypeIndex];

                numClientDBs += 1 * (parsedType.kind == TypeParser::ParsedTypeKind::ClientDB);
                numCommands += 1 * (parsedType.kind == TypeParser::ParsedTypeKind::Command);
                numEnums += 1 * (parsedType.kind == TypeParser::ParsedTypeKind::Enum);
                numPackets += 1 * (parsedType.kind == TypeParser::ParsedTypeKind::Packet);
                numLuaStructs += 1 * (parsedType.kind == TypeParser::ParsedTypeKind::LuaStruct);
            }

            WriteContent(fileContent, "#pragma once\n");
            WriteContent(fileContent, "#include <Base/Types.h>\n\n");

            if (numClientDBs > 0 || numPackets > 0)
            {
                WriteContent(fileContent, "#include <Base/Memory/Bytebuffer.h>");
                WriteContent(fileContent, "\n");
            }

            if (numClientDBs > 0)
            {
                WriteContent(fileContent, "#include <FileFormat/Novus/ClientDB/ClientDB.h>\n");
                WriteContent(fileContent, "\n");
            }

            if (numLuaStructs > 0)
            {
                WriteContent(fileContent, "#include <Scripting/Defines.h>\n\n");

                WriteContent(fileContent, "#include <lua.h>\n\n");
            }

            if (numCommands > 0 || numEnums > 0 || numPackets > 0 || numLuaStructs > 0)
            {
                WriteContent(fileContent, "#include <array>\n");
            }

            if (numCommands > 0)
            {
                WriteContent(fileContent, "#include <numeric>\n");
                WriteContent(fileContent, "#include <vector>\n");
                WriteContent(fileContent, "#include <string>\n");
                WriteContent(fileContent, "#include <tuple>\n");
            }

            if (numEnums > 0 || numLuaStructs > 0)
            {
                WriteContent(fileContent, "#include <utility>\n");
            }

            if (numCommands > 0 || numEnums > 0 || numPackets > 0 || numLuaStructs > 0)
            {
                WriteContent(fileContent, "\n");
            }

            WriteContent(fileContent, "namespace Generated");
            WriteContent(fileContent, "\n{\n");

            bool failed = false;

            for (u32 parsedTypeIndex = 0; parsedTypeIndex < numParsedTypes; parsedTypeIndex++)
            {
                const TypeParser::ParsedType& parsedType = module.parserInfo.parsedTypes[parsedTypeIndex];
                failed |= !GenerateParsedType(parsedType, fileContent, currentMaxEnumID, packetList, currentMaxLuaStructID);

                if (failed)
                    break;

                if (parsedTypeIndex != numParsedTypes - 1)
                    WriteContent(fileContent, "\n");
            }

            if (failed)
            {
                NC_LOG_ERROR("Failed to generate types for file {0}", path.string());
                continue;
            }

            WriteContent(fileContent, "}");

            if (fileContent.length() == 0)
                continue;

            // Get relative path to source dir
            std::filesystem::path relativePath = std::filesystem::relative(path, sourceDir);
            std::filesystem::path generatedPath = std::filesystem::path(generatedDir) / relativePath;
            if (!std::filesystem::exists(generatedPath.parent_path()))
                std::filesystem::create_directories(generatedPath.parent_path());

            generatedPath.replace_extension(".h");
            std::ofstream output(generatedPath, std::ofstream::out | std::ofstream::binary);
            if (!output)
            {
                NC_LOG_ERROR("Failed to create file ({0}). Check admin permissions", generatedPath.string());
                break;
            }

            output.write(fileContent.c_str(), fileContent.length());
            output.close();
        }
        catch (const std::exception&)
        {
            NC_LOG_ERROR("Failed to compile file {0}", path.string());
            break;
        }
    }

    u32 numPackets = static_cast<u32>(packetList.packets.size());
    if (numPackets > 0)
    {
        fileContent.clear();

        if (GeneratePacketEnum(fileContent, packetList, currentMaxEnumID))
        {
            std::filesystem::path generatedPath = std::filesystem::path(generatedDir) / "Shared/PacketList.h";
            if (!std::filesystem::exists(generatedPath.parent_path()))
                std::filesystem::create_directories(generatedPath.parent_path());

            std::ofstream output(generatedPath, std::ofstream::out | std::ofstream::binary);
            if (output)
            {
                output.write(fileContent.c_str(), fileContent.length());
                output.close();
            }
            else
            {
                NC_LOG_ERROR("Failed to create file ({0}). Check admin permissions", generatedPath.string());
            }
        }
    }

    std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> duration = endTime - startTime;

    NC_LOG_INFO("Compiled {0}/{1} type files in {2}s", numLuauFilesCompiled, numLuauFiles, duration.count());
    return 0;
}