#include "Types.h"

namespace TypeParser
{
    const char* TypePropertyKindNames[] =
    {
        "None",

        "Auto",
        "Value",
        "bool",
        "i8",
        "i16",
        "i32",
        "i64",

        "u8",
        "u16",
        "u32",
        "u64",

        "f32",
        "f64",

        "vec2",
        "vec3",
        "vec4",

        "ivec2",
        "ivec3",
        "ivec4",

        "uvec2",
        "uvec3",
        "uvec4",

        "Identifier",
        "string",
        "StringRef",
        "array"
    };

    TypePropertyKind ParsedTypeProperty::GetTypePropertyKindFromNameHash(u32 nameHash)
    {
        TypePropertyKind kind = TypePropertyKind::None;

        switch (nameHash)
        {
            case "Auto"_djb2:       kind = TypePropertyKind::Auto; break;
            case "Value"_djb2:      kind = TypePropertyKind::Value; break;
            case "boolean"_djb2:    kind = TypePropertyKind::boolean; break;
            case "i8"_djb2:         kind = TypePropertyKind::i8; break;
            case "i16"_djb2:        kind = TypePropertyKind::i16; break;
            case "i32"_djb2:        kind = TypePropertyKind::i32; break;
            case "i64"_djb2:        kind = TypePropertyKind::i64; break;
            case "u8"_djb2:         kind = TypePropertyKind::u8; break;
            case "u16"_djb2:        kind = TypePropertyKind::u16; break;
            case "u32"_djb2:        kind = TypePropertyKind::u32; break;
            case "u64"_djb2:        kind = TypePropertyKind::u64; break;
            case "f32"_djb2:        kind = TypePropertyKind::f32; break;
            case "f64"_djb2:        kind = TypePropertyKind::f64; break;
            case "vec2"_djb2:       kind = TypePropertyKind::vec2; break;
            case "vec3"_djb2:       kind = TypePropertyKind::vec3; break;
            case "vec4"_djb2:       kind = TypePropertyKind::vec4; break;
            case "ivec2"_djb2:      kind = TypePropertyKind::ivec2; break;
            case "ivec3"_djb2:      kind = TypePropertyKind::ivec3; break;
            case "ivec4"_djb2:      kind = TypePropertyKind::ivec4; break;
            case "uvec2"_djb2:      kind = TypePropertyKind::uvec2; break;
            case "uvec3"_djb2:      kind = TypePropertyKind::uvec3; break;
            case "uvec4"_djb2:      kind = TypePropertyKind::uvec4; break;
            case "string"_djb2:     kind = TypePropertyKind::string; break;
            case "stringref"_djb2:  kind = TypePropertyKind::StringRef; break;
            case "array"_djb2:      kind = TypePropertyKind::array; break;
        }

        return kind;
    }

    const char* ParsedTypeProperty::GetTypePropertyKindName(TypePropertyKind kind)
    {
        return TypePropertyKindNames[static_cast<u32>(kind)];
    }
}