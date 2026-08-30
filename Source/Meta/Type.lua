local M = OrderedTable()

--[[
    ENUM Available Fields
    - archetype (ENUM/Archetype)
    - type
    - fields (array)

    STRUCT Available Fields
    - archetype (STRUCT/Archetype)
    - fields (array)

    FIELD Available Attributes
    - serialize (boolean, defaults to true; false explicitly excludes the field from Component.Serialization)
    - luaPush (boolean, defaults to true; false explicitly excludes the field from Component.LuaSerialization)
    - debug (boolean, defaults to true; false explicitly excludes the field from Component.Debug)
--]]

M.ENUM = 1
M.STRUCT = 2

local function CheckedCommandReadExpression(typeInfo, parser, cppParamIndex, isUnsigned)
    local parameter = "parameters[" .. tostring(cppParamIndex) .. "]"
    local expression = "[](const std::string& value) -> " .. typeInfo.name .. " { "
    if isUnsigned then
        expression = expression .. "if (!value.empty() && value.front() == '-') throw std::out_of_range(\"negative unsigned value\"); "
    end
    expression = expression .. "const auto parsed = std::" .. parser .. "(value); "
    expression = expression .. "if (!std::in_range<" .. typeInfo.name .. ">(parsed)) throw std::out_of_range(\"numeric value out of range\"); "
    expression = expression .. "return static_cast<" .. typeInfo.name .. ">(parsed); }(" .. parameter .. ")"
    return expression
end

M.VOID =
{
    name = "void",
    size = 0,

    ToString = function(self, fieldInfo, attributes)
        return "std::string(\"" .. fieldInfo.name .. " : void\")"
    end
}

M.STRING =
{
    name = "std::string",
    size = 40,
    flags =
    {
        pod = false
    },

    ToString = function(self, fieldInfo, attributes)
        return "std::string(\"" .. fieldInfo.name .. " : \") + " .. fieldInfo.name .. ""
    end,

    SerializeExpr = function(self, fieldInfo, attributes)
        return "PutString(" .. fieldInfo.name .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "GetString(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return "static_cast<u32>(" .. fieldInfo.name .. ".size()) + 1"
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_pushstring", { "state", fieldInfo.name .. ".c_str()" }))
    end,

    CommandReadExpr = function(self, fieldInfo, attributes, cppParamIndex, context)
        return "std::move(parameters[" .. tostring(cppParamIndex) .. "])"
    end
}

M.STRING_VIEW =
{
    name = "std::string_view",
    size = 16,
    flags =
    {
        pod = false,
        serializable = false
    },

    ToString = function(self, fieldInfo, attributes)
        return "std::string(\"" .. fieldInfo.name .. " : \") + " .. fieldInfo.name .. ""
    end,

    SerializeExpr = function(self, fieldInfo, attributes)
        return "PutString(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return "static_cast<u32>(" .. fieldInfo.name .. ".size()) + 1"
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_pushlstring", { "state", fieldInfo.name .. ".data()", fieldInfo.name .. ".size()" }))
    end
}

M.VECTOR =
{
    name = "std::vector",
    size = 32,
    flags =
    {
        pod = false,
        serializable = false
    },

    ToString = function(self, fieldInfo, attributes)
        return "std::string(\"" .. fieldInfo.name .. " : Vectors are unsupported\")"
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_newtable", { "state" }))

        local fieldType = attributes.type
        if fieldType.EmitLuaPush == nil then
            return
        end

        context.cpp:Block("for (u32 i = 0; i < static_cast<u32>(" .. fieldInfo.name .. ".size()); i++)", function()
            local elementFieldInfo = { name = fieldInfo.name .. "[i]", type = fieldType }
            fieldType:EmitLuaPush(elementFieldInfo, nil, context)
            context.cpp:BlankLine()
            context.cpp:Statement(context.cpp:Call("lua_rawseti", { "state", "-2", "i + 1" }))
        end)
    end
}

M.ARRAY =
{
    name = "std::array",
    size = 0,
    flags =
    {
        pod = false
    },

    ToString = function(self, fieldInfo, attributes)
        return "std::string(\"" .. fieldInfo.name .. " : Arrays are unsupported\")"
    end,

    ValidateSerialization = function(self, fieldInfo, attributes)
        if attributes == nil or attributes.type == nil or attributes.count == nil then
            error("Serialization : array field '" .. fieldInfo.name .. "' requires element type and count attributes")
        end

        if attributes.type.flags == nil or attributes.type.flags.pod ~= true then
            error("Serialization : array field '" .. fieldInfo.name .. "' cannot use raw byte serialization for non-POD element type '" .. attributes.type.name .. "'")
        end
    end,

    SerializeExpr = function(self, fieldInfo, attributes)
        return "PutBytes(" .. fieldInfo.name .. ".data(), " .. self:SerializedSizeExpr(fieldInfo, attributes) .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "GetBytes(" .. fieldInfo.name .. ".data(), " .. self:SerializedSizeExpr(fieldInfo, attributes) .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return tostring(attributes.count) .. " * sizeof(" .. attributes.type.name .. ")"
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_newtable", { "state" }))

        local fieldType = attributes.type
        if fieldType.EmitLuaPush == nil then
            return
        end

        context.cpp:Block("for (u32 i = 0; i < static_cast<u32>(" .. fieldInfo.name .. ".size()); i++)", function()
            local elementFieldInfo = { name = fieldInfo.name .. "[i]", type = fieldType }
            fieldType:EmitLuaPush(elementFieldInfo, nil, context)
            context.cpp:BlankLine()
            context.cpp:Statement(context.cpp:Call("lua_rawseti", { "state", "-2", "i + 1" }))
        end)
    end
}

M.PAIR =
{
    name = "std::pair",
    size = 0,
    flags =
    {
        pod = false
    },

    ToString = function(self, fieldInfo, attributes)
        return "std::string(\"" .. fieldInfo.name .. " : std::pair is unsupported\")"
    end
}

M.BOOL =
{
    name = "bool",
    size = 1,
    flags =
    {
        pod = true
    },
    
    min = 0,
    max = 0x1,

    Validate = function(self, value)
        if value < self.min or value > self.max then
            error("Value out of range for " .. self.name .. ": " .. tostring(value))
        end
    end,

    ToString = function(self, fieldInfo, attributes)
        return "\"" .. fieldInfo.name .. " : \" + std::to_string(" .. fieldInfo.name .. ")"
    end,

    SerializeExpr = function(self, fieldInfo, attributes)
        return "Put(" .. fieldInfo.name .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "Get(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_pushboolean", { "state", fieldInfo.name }))
    end,

    CommandReadExpr = function(self, fieldInfo, attributes, cppParamIndex, context)
        local parameter = "parameters[" .. tostring(cppParamIndex) .. "]"
        return "[](const std::string& value) { if (value == \"1\" || value == \"true\") return true; if (value == \"0\" || value == \"false\") return false; throw std::invalid_argument(\"invalid boolean value\"); }(" .. parameter .. ")"
    end
}

M.I8 =
{
    name = "i8",
    size = 1,
    flags =
    {
        pod = true
    },
    
    suffix = "l",
    min = -0x80,
    max = 0x7F,

    Validate = function(self, value)
        if value < self.min or value > self.max then
            error("Value out of range for " .. self.name .. ": " .. tostring(value))
        end
    end,

    ToString = function(self, fieldInfo, attributes)
        return "\"" .. fieldInfo.name .. " : \" + std::to_string(" .. fieldInfo.name .. ")"
    end,

    SerializeExpr = function(self, fieldInfo, attributes)
        return "PutI8(" .. fieldInfo.name .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "GetI8(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_pushinteger64", { "state", "static_cast<i64>(" .. fieldInfo.name .. ")" }))
    end,

    CommandReadExpr = function(self, fieldInfo, attributes, cppParamIndex, context)
        return CheckedCommandReadExpression(self, "stoll", cppParamIndex, false)
    end
}

M.I16 =
{
    name = "i16",
    size = 2,
    flags =
    {
        pod = true
    },

    suffix = "l",
    min = -0x8000,
    max = 0x7FFF,

    Validate = function(self, value)
        if value < self.min or value > self.max then
            error("Value out of range for " .. self.name .. ": " .. tostring(value))
        end
    end,

    ToString = function(self, fieldInfo, attributes)
        return "\"" .. fieldInfo.name .. " : \" + std::to_string(" .. fieldInfo.name .. ")"
    end,

    SerializeExpr = function(self, fieldInfo, attributes)
        return "PutI16(" .. fieldInfo.name .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "GetI16(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_pushinteger64", { "state", "static_cast<i64>(" .. fieldInfo.name .. ")" }))
    end,

    CommandReadExpr = function(self, fieldInfo, attributes, cppParamIndex, context)
        return CheckedCommandReadExpression(self, "stoll", cppParamIndex, false)
    end
}

M.I32 =
{
    name = "i32",
    size = 4,
    flags =
    {
        pod = true
    },

    suffix = "l",
    min = -0x80000000,
    max = 0x7FFFFFFF,

    Validate = function(self, value)
        if value < self.min or value > self.max then
            error("Value out of range for " .. self.name .. ": " .. tostring(value))
        end
    end,

    ToString = function(self, fieldInfo, attributes)
        return "\"" .. fieldInfo.name .. " : \" + std::to_string(" .. fieldInfo.name .. ")"
    end,
    
    SerializeExpr = function(self, fieldInfo, attributes)
        return "PutI32(" .. fieldInfo.name .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "GetI32(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_pushinteger64", { "state", "static_cast<i64>(" .. fieldInfo.name .. ")" }))
    end,

    CommandReadExpr = function(self, fieldInfo, attributes, cppParamIndex, context)
        return CheckedCommandReadExpression(self, "stoll", cppParamIndex, false)
    end
}

M.I64 =
{
    name = "i64",
    size = 8,
    flags =
    {
        pod = true
    },

    suffix = "ll",
    min = -0x7FFFFFFFFFFFFFFF - 1,
    max = 0x7FFFFFFFFFFFFFFF,

    Validate = function(self, value)
        if value < self.min or value > self.max then
            error("Value out of range for " .. self.name .. ": " .. tostring(value))
        end
    end,

    ToString = function(self, fieldInfo, attributes)
        return "\"" .. fieldInfo.name .. " : \" + std::to_string(" .. fieldInfo.name .. ")"
    end,

    SerializeExpr = function(self, fieldInfo, attributes)
        return "PutI64(" .. fieldInfo.name .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "GetI64(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_pushinteger64", { "state", fieldInfo.name }))
    end,

    CommandReadExpr = function(self, fieldInfo, attributes, cppParamIndex, context)
        return CheckedCommandReadExpression(self, "stoll", cppParamIndex, false)
    end
}

M.U8 =
{
    name = "u8",
    size = 1,
    flags =
    {
        pod = true
    },
    
    suffix = "u",
    min = 0,
    max = 0xFF,

    Validate = function(self, value)
        if value < self.min or value > self.max then
            error("Value out of range for " .. self.name .. ": " .. tostring(value), 2)
        end
    end,

    ToString = function(self, fieldInfo, attributes)
        return "\"" .. fieldInfo.name .. " : \" + std::to_string(" .. fieldInfo.name .. ")"
    end,

    SerializeExpr = function(self, fieldInfo, attributes)
        return "PutU8(" .. fieldInfo.name .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "GetU8(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_pushinteger64", { "state", "static_cast<i64>(" .. fieldInfo.name .. ")" }))
    end,

    CommandReadExpr = function(self, fieldInfo, attributes, cppParamIndex, context)
        return CheckedCommandReadExpression(self, "stoull", cppParamIndex, true)
    end
}

M.U16 =
{
    name = "u16",
    size = 2,
    flags =
    {
        pod = true
    },

    suffix = "u",
    min = 0,
    max = 0xFFFF,

    Validate = function(self, value)
        if value < self.min or value > self.max then
            error("Value out of range for " .. self.name .. ": " .. tostring(value))
        end
    end,

    ToString = function(self, fieldInfo, attributes)
        return "\"" .. fieldInfo.name .. " : \" + std::to_string(" .. fieldInfo.name .. ")"
    end,
    
    SerializeExpr = function(self, fieldInfo, attributes)
        return "PutU16(" .. fieldInfo.name .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "GetU16(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_pushinteger64", { "state", "static_cast<i64>(" .. fieldInfo.name .. ")" }))
    end,

    CommandReadExpr = function(self, fieldInfo, attributes, cppParamIndex, context)
        return CheckedCommandReadExpression(self, "stoull", cppParamIndex, true)
    end
}

M.U32 =
{
    name = "u32",
    size = 4,
    flags =
    {
        pod = true
    },

    suffix = "u",
    min = 0,
    max = 0xFFFFFFFF,

    Validate = function(self, value)
        if value < self.min or value > self.max then
            error("Value out of range for " .. self.name .. ": " .. tostring(value))
        end
    end,

    ToString = function(self, fieldInfo, attributes)
        return "\"" .. fieldInfo.name .. " : \" + std::to_string(" .. fieldInfo.name .. ")"
    end,

    SerializeExpr = function(self, fieldInfo, attributes)
        return "PutU32(" .. fieldInfo.name .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "GetU32(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_pushinteger64", { "state", "static_cast<i64>(" .. fieldInfo.name .. ")" }))
    end,

    CommandReadExpr = function(self, fieldInfo, attributes, cppParamIndex, context)
        return CheckedCommandReadExpression(self, "stoull", cppParamIndex, true)
    end
}

M.U64 =
{
    name = "u64",
    size = 8,
    flags =
    {
        pod = true
    },

    suffix = "ull",
    min = 0,
    max = 18446744073709551615,

    Validate = function(self, value)
        if value < self.min or value > self.max then
            error("Value out of range for " .. self.name .. ": " .. tostring(value))
        end
    end,

    ToString = function(self, fieldInfo, attributes)
        return "\"" .. fieldInfo.name .. " : \" + std::to_string(" .. fieldInfo.name .. ")"
    end,

    SerializeExpr = function(self, fieldInfo, attributes)
        return "PutU64(" .. fieldInfo.name .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "GetU64(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_pushinteger64", { "state", "std::bit_cast<i64>(" .. fieldInfo.name .. ")" }))
    end,

    CommandReadExpr = function(self, fieldInfo, attributes, cppParamIndex, context)
        return CheckedCommandReadExpression(self, "stoull", cppParamIndex, true)
    end
}

M.F32 =
{
    name = "f32",
    size = 4,
    min = -3.402823466e38,
    max = 3.402823466e38,
    flags =
    {
        pod = true
    },

    Validate = function(self, value)
        if value < self.min or value > self.max then
            error("Value out of range for " .. self.name .. ": " .. tostring(value))
        end
    end,

    ToString = function(self, fieldInfo, attributes)
        return "\"" .. fieldInfo.name .. " : \" + std::to_string(" .. fieldInfo.name .. ")"
    end,

    SerializeExpr = function(self, fieldInfo, attributes)
        return "PutF32(" .. fieldInfo.name .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "GetF32(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_pushnumber", { "state", fieldInfo.name }))
    end,

    CommandReadExpr = function(self, fieldInfo, attributes, cppParamIndex, context)
        return "std::stof(parameters[" .. tostring(cppParamIndex) .. "])"
    end
}

M.F64 =
{
    name = "f64",
    size = 8,
    min = -1.797693134862315708e308,
    max = 1.797693134862315708e308,
    flags =
    {
        pod = true
    },

    Validate = function(self, value)
        if value < self.min or value > self.max then
            error("Value out of range for " .. self.name .. ": " .. tostring(value))
        end
    end,

    ToString = function(self, fieldInfo, attributes)
        return "\"" .. fieldInfo.name .. " : \" + std::to_string(" .. fieldInfo.name .. ")"
    end,

    SerializeExpr = function(self, fieldInfo, attributes)
        return "PutF64(" .. fieldInfo.name .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "GetF64(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_pushnumber", { "state", fieldInfo.name }))
    end,

    CommandReadExpr = function(self, fieldInfo, attributes, cppParamIndex, context)
        return "std::stod(parameters[" .. tostring(cppParamIndex) .. "])"
    end
}

M.VEC2 =
{
    name = "vec2",
    size = 8,
    flags =
    {
        pod = true
    },

    ToString = function(self, fieldInfo, attributes)
        return "std::string(\"" .. fieldInfo.name .. " : \") + \"(X : \" + std::to_string(" .. fieldInfo.name .. ".x) + \", Y : \" + std::to_string(" .. fieldInfo.name .. ".y) + \")\""
    end,

    SerializeExpr = function(self, fieldInfo, attributes)
        return "Put(" .. fieldInfo.name .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "Get(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_pushvector", { "state", fieldInfo.name .. ".x", fieldInfo.name .. ".y", "0.0f" }))
    end
}

M.VEC3 =
{
    name = "vec3",
    size = 12,
    flags =
    {
        pod = true
    },

    ToString = function(self, fieldInfo, attributes)
        return "std::string(\"" .. fieldInfo.name .. " : \") + \"(X : \" + std::to_string(" .. fieldInfo.name .. ".x) + \", Y : \" + std::to_string(" .. fieldInfo.name .. ".y) + \", Z : \" + std::to_string(" .. fieldInfo.name .. ".z) + \")\""
    end,

    SerializeExpr = function(self, fieldInfo, attributes)
        return "Put(" .. fieldInfo.name .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "Get(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_pushvector", { "state", fieldInfo.name .. ".x", fieldInfo.name .. ".y", fieldInfo.name .. ".z" }))
    end
}

M.VEC4 =
{
    name = "vec4",
    size = 16,
    flags =
    {
        pod = true
    },

    ToString = function(self, fieldInfo, attributes)
        return "std::string(\"" .. fieldInfo.name .. " : \") + \"(X : \" + std::to_string(" .. fieldInfo.name .. ".x) + \", Y : \" + std::to_string(" .. fieldInfo.name .. ".y) + \", Z : \" + std::to_string(" .. fieldInfo.name .. ".z) + \",  W : \" + std::to_string(" .. fieldInfo.name .. ".w) + \")\""
    end,

    SerializeExpr = function(self, fieldInfo, attributes)
        return "Put(" .. fieldInfo.name .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "Get(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_pushvector", { "state", fieldInfo.name .. ".x", fieldInfo.name .. ".y", fieldInfo.name .. ".z" }))
    end
}

M.IVEC2 =
{
    name = "ivec2",
    size = 8,
    flags =
    {
        pod = true
    },

    ToString = function(self, fieldInfo, attributes)
        return "std::string(\"" .. fieldInfo.name .. " : \") + \"(X : \" + std::to_string(" .. fieldInfo.name .. ".x) + \", Y : \" + std::to_string(" .. fieldInfo.name .. ".y) + \")\""
    end,

    SerializeExpr = function(self, fieldInfo, attributes)
        return "Put(" .. fieldInfo.name .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "Get(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_pushvector", { "state", fieldInfo.name .. ".x", fieldInfo.name .. ".y", "0.0f" }))
    end
}

M.IVEC3 =
{
    name = "ivec3",
    size = 12,
    flags =
    {
        pod = true
    },

    ToString = function(self, fieldInfo, attributes)
        return "std::string(\"" .. fieldInfo.name .. " : \") + \"(X : \" + std::to_string(" .. fieldInfo.name .. ".x) + \", Y : \" + std::to_string(" .. fieldInfo.name .. ".y) + \", Z : \" + std::to_string(" .. fieldInfo.name .. ".z) + \")\""
    end,

    SerializeExpr = function(self, fieldInfo, attributes)
        return "Put(" .. fieldInfo.name .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "Get(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_pushvector", { "state", fieldInfo.name .. ".x", fieldInfo.name .. ".y", fieldInfo.name .. ".z" }))
    end
}

M.IVEC4 =
{
    name = "ivec4",
    size = 16,
    flags =
    {
        pod = true
    },

    ToString = function(self, fieldInfo, attributes)
        return "std::string(\"" .. fieldInfo.name .. " : \") + \"(X : \" + std::to_string(" .. fieldInfo.name .. ".x) + \", Y : \" + std::to_string(" .. fieldInfo.name .. ".y) + \", Z : \" + std::to_string(" .. fieldInfo.name .. ".z) + \",  W : \" + std::to_string(" .. fieldInfo.name .. ".w) + \")\""
    end,

    SerializeExpr = function(self, fieldInfo, attributes)
        return "Put(" .. fieldInfo.name .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "Get(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_pushvector", { "state", fieldInfo.name .. ".x", fieldInfo.name .. ".y", fieldInfo.name .. ".z" }))
    end
}

M.UVEC2 =
{
    name = "uvec2",
    size = 8,
    flags =
    {
        pod = true
    },

    ToString = function(self, fieldInfo, attributes)
        return "std::string(\"" .. fieldInfo.name .. " : \") + \"(X : \" + std::to_string(" .. fieldInfo.name .. ".x) + \", Y : \" + std::to_string(" .. fieldInfo.name .. ".y) + \")\""
    end,

    SerializeExpr = function(self, fieldInfo, attributes)
        return "Put(" .. fieldInfo.name .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "Get(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_pushvector", { "state", fieldInfo.name .. ".x", fieldInfo.name .. ".y", "0.0f" }))
    end
}

M.UVEC3 =
{
    name = "uvec3",
    size = 12,
    flags =
    {
        pod = true
    },

    ToString = function(self, fieldInfo, attributes)
        return "std::string(\"" .. fieldInfo.name .. " : \") + \"(X : \" + std::to_string(" .. fieldInfo.name .. ".x) + \", Y : \" + std::to_string(" .. fieldInfo.name .. ".y) + \", Z : \" + std::to_string(" .. fieldInfo.name .. ".z) + \")\""
    end,

    SerializeExpr = function(self, fieldInfo, attributes)
        return "Put(" .. fieldInfo.name .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "Get(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_pushvector", { "state", fieldInfo.name .. ".x", fieldInfo.name .. ".y", fieldInfo.name .. ".z" }))
    end
}

M.UVEC4 =
{
    name = "uvec4",
    size = 16,
    flags =
    {
        pod = true
    },

    ToString = function(self, fieldInfo, attributes)
        return "std::string(\"" .. fieldInfo.name .. " : \") + \"(X : \" + std::to_string(" .. fieldInfo.name .. ".x) + \", Y : \" + std::to_string(" .. fieldInfo.name .. ".y) + \", Z : \" + std::to_string(" .. fieldInfo.name .. ".z) + \",  W : \" + std::to_string(" .. fieldInfo.name .. ".w) + \")\""
    end,

    SerializeExpr = function(self, fieldInfo, attributes)
        return "Put(" .. fieldInfo.name .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "Get(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_pushvector", { "state", fieldInfo.name .. ".x", fieldInfo.name .. ".y", fieldInfo.name .. ".z" }))
    end
}

M.STRINGREF = 
{
    name = "StringRef",
    size = 4,
    flags =
    {
        pod = true
    },

    ToString = function(self, fieldInfo, attributes)
        return "\"" .. fieldInfo.name .. " : \" + std::to_string(" .. fieldInfo.name .. ")"
    end,

    SerializeExpr = function(self, fieldInfo, attributes)
        return "PutU32(" .. fieldInfo.name .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "GetU32(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    EmitLuaPush = function(self, fieldInfo, attributes, context)
        context.cpp:Statement(context.cpp:Call("lua_pushinteger64", { "state", "static_cast<i64>(" .. fieldInfo.name .. ")" }))
    end
}

M.BYTEBUFFER =
{
    name = "Bytebuffer",
    size = 40,
    flags =
    {
        pod = false,
        serializable = false
    },

    ToString = function(self, fieldInfo, attributes)
        return "\"" .. fieldInfo.name .. " : Unsupported\""
    end,

    -- Bytebuffer needs an explicit framing policy before it can be embedded in another serialized type.
}

M.OBJECTGUID =
{
    name = "ObjectGUID",
    size = 8,
    flags =
    {
        pod = false
    },

    ToString = function(self, fieldInfo, attributes)
        return "\"" .. fieldInfo.name .. " : \" + " .. fieldInfo.name .. ".ToString()"
    end,

    SerializeExpr = function(self, fieldInfo, attributes)
        return "Serialize(" .. fieldInfo.name .. ")"
    end,
    DeserializeExpr = function(self, fieldInfo, attributes)
        return "Deserialize(" .. fieldInfo.name .. ")"
    end,
    SerializedSizeExpr = function(self, fieldInfo, attributes)
        return fieldInfo.name .. ".GetCounterBytesUsed() + 1"
    end
}

M.LUASTATE =
{
    name = "lua_State",
    size = 8,
    flags =
    {
        pod = false,
        serializable = false
    },

    ToString = function(self, fieldInfo, attributes)
        return "\"" .. fieldInfo.name .. " : lua_State unsupported\""
    end
}

local function SetSystemIncludes(typeInfo, ...)
    typeInfo.include = typeInfo.include or {}
    typeInfo.include.system = { ... }
end

SetSystemIncludes(M.STRING, "string")
SetSystemIncludes(M.STRING_VIEW, "string_view")
SetSystemIncludes(M.VECTOR, "vector")
SetSystemIncludes(M.ARRAY, "array")
SetSystemIncludes(M.PAIR, "utility")

for _, typeInfo in ipairs(
{
    M.BOOL,
    M.I8, M.I16, M.I32, M.I64,
    M.U8, M.U16, M.U32, M.U64,
    M.F32, M.F64,
    M.VEC2, M.VEC3, M.VEC4,
    M.IVEC2, M.IVEC3, M.IVEC4,
    M.UVEC2, M.UVEC3, M.UVEC4,
    M.STRINGREF,
    M.OBJECTGUID
}) do
    SetSystemIncludes(typeInfo, "Base/Types.h")
end

SetSystemIncludes(M.BYTEBUFFER, "Base/Memory/Bytebuffer.h")
SetSystemIncludes(M.LUASTATE, "lua.h")

local function SetIntegerInfo(typeInfo, signed, bits)
    typeInfo.kind = "integer"
    typeInfo.signed = signed
    typeInfo.bits = bits
end

SetIntegerInfo(M.I8, true, 8)
SetIntegerInfo(M.I16, true, 16)
SetIntegerInfo(M.I32, true, 32)
SetIntegerInfo(M.I64, true, 64)
SetIntegerInfo(M.U8, false, 8)
SetIntegerInfo(M.U16, false, 16)
SetIntegerInfo(M.U32, false, 32)
SetIntegerInfo(M.U64, false, 64)

M.BOOL.kind = "boolean"
M.F32.kind = "float"
M.F32.bits = 32
M.F64.kind = "float"
M.F64.bits = 64
M.STRING.kind = "string"
M.STRING_VIEW.kind = "stringView"
M.STRINGREF.kind = "stringReference"
M.ARRAY.kind = "array"
M.VECTOR.kind = "vector"
M.PAIR.kind = "pair"
M.BYTEBUFFER.kind = "byteBuffer"
M.OBJECTGUID.kind = "objectGuid"
M.LUASTATE.kind = "luaState"

for _, typeInfo in ipairs({ M.VEC2, M.VEC3, M.VEC4, M.IVEC2, M.IVEC3, M.IVEC4, M.UVEC2, M.UVEC3, M.UVEC4 }) do
    typeInfo.kind = "mathVector"
end

return M;
