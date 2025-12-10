local M = OrderedTable()

--[[
    ENUM Available Fields
    - archetype (ENUM/Archetype)
    - type
    - fields (array)

    STRUCT Available Fields
    - archetype (STRUCT/Archetype)
    - fields (array)
--]]

M.ENUM = 1
M.STRUCT = 2

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

    Serialize = function(self, fieldInfo, attributes)
        return "PutString(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "GetString(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return "static_cast<u32>(" .. fieldInfo.name .. ".size()) + 1"
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushstring(state, " .. fieldInfo.name .. ".c_str());"):Flush()
    end,

    CommandRead = function(self, fieldInfo, attributes, cppParamIndex)
        MetaGenBuilder:Unknown("std::move(parameters[" .. tostring(cppParamIndex) .. "])")
    end
}

M.STRING_VIEW =
{
    name = "std::string_view",
    size = 16,
    flags =
    {
        pod = false
    },

    ToString = function(self, fieldInfo, attributes)
        return "std::string(\"" .. fieldInfo.name .. " : \") + " .. fieldInfo.name .. ""
    end,

    Serialize = function(self, fieldInfo, attributes)
        return "PutString(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "GetString(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return "static_cast<u32>(" .. fieldInfo.name .. ".size()) + 1"
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushstring(state, " .. fieldInfo.name .. ".data());"):Flush()
    end
}

M.VECTOR =
{
    name = "std::vector",
    size = 32,
    flags =
    {
        pod = false
    },

    ToString = function(self, fieldInfo, attributes)
        return "std::string(\"" .. fieldInfo.name .. " : Vectors are unsupported\")"
    end,

    Serialize = function(self, fieldInfo, attributes)
        return "PutBytes(" .. fieldInfo.name .. ".data(), " .. fieldInfo.name .. ".size())"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "GetBytes(" .. fieldInfo.name .. ".data(), " .. fieldInfo.name .. ".size())"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_newtable(state)"):End()

        local fieldType = attributes.type
        if fieldType.Push == nil then
            return
        end

        MetaGenBuilder:Unknown("for (u32 i = 0; i < static_cast<u32>(" .. fieldInfo.name .. ".size()); i++)"):Scope(function()
            local elementFieldInfo = { name = fieldInfo.name .. "[i]", type = fieldType }
            fieldType:Push(elementFieldInfo, nil)
            MetaGenBuilder:NewLine()
            MetaGenBuilder:Unknown("lua_rawseti(state, -2, i);"):Flush()
        end):NewLine():Flush()
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

    Serialize = function(self, fieldInfo, attributes)
        return "PutBytes(" .. fieldInfo.name .. ".data(), " .. fieldInfo.name .. ".size())"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "GetBytes(" .. fieldInfo.name .. ".data(), " .. self:GetSerializedSize(fieldInfo, attributes) .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return tostring(attributes.count) .. " * sizeof(" .. attributes.type.name .. ")"
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_newtable(state)"):End()

        local fieldType = attributes.type
        if fieldType.Push == nil then
            return
        end

        MetaGenBuilder:Unknown("for (u32 i = 0; i < static_cast<u32>(" .. fieldInfo.name .. ".size()); i++)"):Scope(function()
            local elementFieldInfo = { name = fieldInfo.name .. "[i]", type = fieldType }
            fieldType:Push(elementFieldInfo, nil)
            MetaGenBuilder:NewLine()
            MetaGenBuilder:Unknown("lua_rawseti(state, -2, i);"):Flush()
        end):NewLine():Flush()
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

    Serialize = function(self, fieldInfo, attributes)
        return "Put(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "Get(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushboolean(state, " .. fieldInfo.name .. ");"):Flush()
    end,

    CommandRead = function(self, fieldInfo, attributes, cppParamIndex)
        local paramIndexText = "parameters[" .. tostring(cppParamIndex) .. "]"
        MetaGenBuilder:Unknown("(" .. paramIndexText .. ".size() > 0 && (" .. paramIndexText .. "[0] == '1' || " .. paramIndexText .. " == \"true\"))")
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
    min = 0x80,
    max = 0x7F,

    Validate = function(self, value)
        if value < self.min or value > self.max then
            error("Value out of range for " .. self.name .. ": " .. tostring(value))
        end
    end,

    ToString = function(self, fieldInfo, attributes)
        return "\"" .. fieldInfo.name .. " : \" + std::to_string(" .. fieldInfo.name .. ")"
    end,

    Serialize = function(self, fieldInfo, attributes)
        return "PutI8(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "GetI8(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushnumber(state, " .. fieldInfo.name .. ");"):Flush()
    end,

    CommandRead = function(self, fieldInfo, attributes, cppParamIndex)
        MetaGenBuilder:Unknown("static_cast<i8>(std::stoi(parameters[" .. tostring(cppParamIndex) .. "]))")
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
    min = 0x8000,
    max = 0x7FFF,

    Validate = function(self, value)
        if value < self.min or value > self.max then
            error("Value out of range for " .. self.name .. ": " .. tostring(value))
        end
    end,

    ToString = function(self, fieldInfo, attributes)
        return "\"" .. fieldInfo.name .. " : \" + std::to_string(" .. fieldInfo.name .. ")"
    end,

    Serialize = function(self, fieldInfo, attributes)
        return "PutI16(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "GetI16(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushnumber(state, " .. fieldInfo.name .. ");"):Flush()
    end,

    CommandRead = function(self, fieldInfo, attributes, cppParamIndex)
        MetaGenBuilder:Unknown("static_cast<i16>(std::stoi(parameters[" .. tostring(cppParamIndex) .. "]))")
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
    min = 0x80000000,
    max = 0x7FFFFFFF,

    Validate = function(self, value)
        if value < self.min or value > self.max then
            error("Value out of range for " .. self.name .. ": " .. tostring(value))
        end
    end,

    ToString = function(self, fieldInfo, attributes)
        return "\"" .. fieldInfo.name .. " : \" + std::to_string(" .. fieldInfo.name .. ")"
    end,
    
    Serialize = function(self, fieldInfo, attributes)
        return "PutI32(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "GetI32(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushnumber(state, " .. fieldInfo.name .. ");"):Flush()
    end,

    CommandRead = function(self, fieldInfo, attributes, cppParamIndex)
        MetaGenBuilder:Unknown("std::stoi(parameters[" .. tostring(cppParamIndex) .. "])")
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
    min = 0x8000000000000000,
    max = 0x7FFFFFFFFFFFFFFF,

    Validate = function(self, value)
        if value < self.min or value > self.max then
            error("Value out of range for " .. self.name .. ": " .. tostring(value))
        end
    end,

    ToString = function(self, fieldInfo, attributes)
        return "\"" .. fieldInfo.name .. " : \" + std::to_string(" .. fieldInfo.name .. ")"
    end,

    Serialize = function(self, fieldInfo, attributes)
        return "PutI64(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "GetI64(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushnumber(state, " .. fieldInfo.name .. ");"):Flush()
    end,

    CommandRead = function(self, fieldInfo, attributes, cppParamIndex)
        MetaGenBuilder:Unknown("std::stoll(parameters[" .. tostring(cppParamIndex) .. "])")
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

    Serialize = function(self, fieldInfo, attributes)
        return "PutU8(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "GetU8(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushnumber(state, " .. fieldInfo.name .. ");"):Flush()
    end,

    CommandRead = function(self, fieldInfo, attributes, cppParamIndex)
        MetaGenBuilder:Unknown("static_cast<u8>(std::stoul(parameters[" .. tostring(cppParamIndex) .. "]))")
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
    
    Serialize = function(self, fieldInfo, attributes)
        return "PutU16(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "GetU16(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushnumber(state, " .. fieldInfo.name .. ");"):Flush()
    end,

    CommandRead = function(self, fieldInfo, attributes, cppParamIndex)
        MetaGenBuilder:Unknown("static_cast<u16>(std::stoul(parameters[" .. tostring(cppParamIndex) .. "]))")
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

    Serialize = function(self, fieldInfo, attributes)
        return "PutU32(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "GetU32(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushnumber(state, " .. fieldInfo.name .. ");"):Flush()
    end,

    CommandRead = function(self, fieldInfo, attributes, cppParamIndex)
        MetaGenBuilder:Unknown("std::stoul(parameters[" .. tostring(cppParamIndex) .. "])")
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

    Serialize = function(self, fieldInfo, attributes)
        return "PutU64(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "GetU64(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushnumber(state, " .. fieldInfo.name .. ");"):Flush()
    end,

    CommandRead = function(self, fieldInfo, attributes, cppParamIndex)
        MetaGenBuilder:Unknown("std::stoull(parameters[" .. tostring(cppParamIndex) .. "])")
    end
}

M.F32 =
{
    name = "f32",
    size = 4,
    min = 1.401298464e-45,
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

    Serialize = function(self, fieldInfo, attributes)
        return "PutF32(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "GetF32(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushnumber(state, " .. fieldInfo.name .. ");"):Flush()
    end,

    CommandRead = function(self, fieldInfo, attributes, cppParamIndex)
        MetaGenBuilder:Unknown("std::stof(parameters[" .. tostring(cppParamIndex) .. "])")
    end
}

M.F64 =
{
    name = "f64",
    size = 8,
    min = 5e-324,
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

    Serialize = function(self, fieldInfo, attributes)
        return "PutF64(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "GetF64(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushnumber(state, " .. fieldInfo.name .. ");"):Flush()
    end,

    CommandRead = function(self, fieldInfo, attributes, cppParamIndex)
        MetaGenBuilder:Unknown("std::stod(parameters[" .. tostring(cppParamIndex) .. "])")
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

    Serialize = function(self, fieldInfo, attributes)
        return "Put(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "Get(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushvector(state, " .. fieldInfo.name .. ".x, " .. fieldInfo.name .. ".y, 0.0f);"):Flush()
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

    Serialize = function(self, fieldInfo, attributes)
        return "Put(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "Get(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushvector(state, " .. fieldInfo.name .. ".x, " .. fieldInfo.name .. ".y, " .. fieldInfo.name .. ".z);"):Flush()
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

    Serialize = function(self, fieldInfo, attributes)
        return "Put(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "Get(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushvector(state, " .. fieldInfo.name .. ".x, " .. fieldInfo.name .. ".y, " .. fieldInfo.name .. ".z);"):Flush()
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

    Serialize = function(self, fieldInfo, attributes)
        return "Put(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "Get(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushvector(state, " .. fieldInfo.name .. ".x, " .. fieldInfo.name .. ".y, 0.0f);"):Flush()
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

    Serialize = function(self, fieldInfo, attributes)
        return "Put(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "Get(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushvector(state, " .. fieldInfo.name .. ".x, " .. fieldInfo.name .. ".y, " .. fieldInfo.name .. ".z);"):Flush()
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

    Serialize = function(self, fieldInfo, attributes)
        return "Put(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "Get(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushvector(state, " .. fieldInfo.name .. ".x, " .. fieldInfo.name .. ".y, " .. fieldInfo.name .. ".z);"):Flush()
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

    Serialize = function(self, fieldInfo, attributes)
        return "Put(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "Get(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushvector(state, " .. fieldInfo.name .. ".x, " .. fieldInfo.name .. ".y, 0.0f);"):Flush()
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

    Serialize = function(self, fieldInfo, attributes)
        return "Put(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "Get(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushvector(state, " .. fieldInfo.name .. ".x, " .. fieldInfo.name .. ".y, " .. fieldInfo.name .. ".z);"):Flush()
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

    Serialize = function(self, fieldInfo, attributes)
        return "Put(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "Get(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushvector(state, " .. fieldInfo.name .. ".x, " .. fieldInfo.name .. ".y, " .. fieldInfo.name .. ".z);"):Flush()
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

    Serialize = function(self, fieldInfo, attributes)
        return "PutU32(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "GetU32(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return tostring(self.size)
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushnumber(state, " .. fieldInfo.name .. ");"):Flush()
    end
}

M.BYTEBUFFER =
{
    name = "Bytebuffer",
    size = 40,
    flags =
    {
        pod = false
    },

    ToString = function(self, fieldInfo, attributes)
        return "\"" .. fieldInfo.name .. " : Unsupported\""
    end,

    Serialize = function(self, fieldInfo, attributes)
        return "PutBytes(" .. fieldInfo.name .. ".GetDataPointer(), " .. fieldInfo.name .. ".writtenData)"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "GetBytes(" .. fieldInfo.name .. ".GetDataPointer(), " .. fieldInfo.name .. ".writtenData)"
    end,

    GetSerializedSize = function(self, fieldInfo, attributes)
        return fieldInfo.name .. ".writtenData"
    end
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

    Serialize = function(self, fieldInfo, attributes)
        return "Serialize(" .. fieldInfo.name .. ")"
    end,
    Deserialize = function(self, fieldInfo, attributes)
        return "Deserialize(" .. fieldInfo.name .. ")"
    end,
    GetSerializedSize = function(self, fieldInfo, attributes)
        return fieldInfo.name .. ".GetCounterBytesUsed() + 1"
    end,

    Push = function(self, fieldInfo, attributes)
        MetaGenBuilder:Unknown("lua_pushnumber(state, " .. fieldInfo.name .. ").GetData();"):Flush()
    end
}

M.LUASTATE =
{
    name = "lua_State",
    size = 8,
    flags =
    {
        pod = false
    },

    ToString = function(self, fieldInfo, attributes)
        return "\"" .. fieldInfo.name .. " : lua_State unsupported\""
    end
}

return M;