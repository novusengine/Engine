local Type = require("Type")

local M = {}

local function QuoteString(value)
    local text = tostring(value)
    text = text:gsub("\\", "\\\\"):gsub("\"", "\\\"")
    text = text:gsub("\0", "\\0"):gsub("\a", "\\a"):gsub("\b", "\\b"):gsub("\f", "\\f")
    text = text:gsub("\n", "\\n"):gsub("\r", "\\r"):gsub("\t", "\\t"):gsub("\v", "\\v")
    return "\"" .. text .. "\""
end

local function TypeName(typeInfo)
    return type(typeInfo) == "string" and typeInfo or typeInfo.name
end

local function Join(values)
    local result = {}
    for index, value in ipairs(values or {}) do result[index] = tostring(value) end
    return table.concat(result, ", ")
end

function M.Create()
    local writer = { currentIndent = 0, currentFile = nil }

    function writer:Write(message, additionalIndent)
        local indent = self.currentIndent + (type(additionalIndent) == "number" and additionalIndent or 0)
        self.currentFile:write(string.rep("    ", indent) .. message)
    end
    function writer:GetIndent() return self.currentIndent end
    function writer:AddIndent() self.currentIndent = self.currentIndent + 1 end
    function writer:SubIndent() self.currentIndent = self.currentIndent - 1 end
    function writer:NewLine() self.currentFile:write("\n") end

    local cpp = { writer = writer }

    function cpp:Type(typeInfo) return TypeName(typeInfo) end
    function cpp:String(value) return QuoteString(value) end
    function cpp:Raw(text) return tostring(text) end
    function cpp:Template(name, arguments) return TypeName(name) .. "<" .. Join(arguments) .. ">" end
    function cpp:Call(name, arguments) return tostring(name) .. "(" .. Join(arguments) .. ")" end
    function cpp:CallMember(object, name, arguments) return tostring(object) .. "." .. self:Call(name, arguments) end
    function cpp:CallPointer(object, name, arguments) return tostring(object) .. "->" .. self:Call(name, arguments) end
    function cpp:Member(object, name) return tostring(object) .. "." .. tostring(name) end
    function cpp:Index(object, index) return tostring(object) .. "[" .. tostring(index) .. "]" end
    function cpp:Cast(typeInfo, expression) return "static_cast<" .. TypeName(typeInfo) .. ">(" .. expression .. ")" end
    function cpp:InitList(values) return "{ " .. Join(values) .. " }" end

    function cpp:Line(text) writer:Write((text or "") .. "\n") end
    function cpp:BlankLine() writer:NewLine() end
    function cpp:Statement(expression, comment)
        local text = tostring(expression) .. ";"
        if comment ~= nil then text = text .. " // " .. comment end
        self:Line(text)
    end
    function cpp:Return(expression) self:Statement("return " .. expression) end
    function cpp:Assign(destination, expression) self:Statement(destination .. " = " .. expression) end
    function cpp:Using(name, expression) self:Statement("using " .. name .. " = " .. expression) end

    function cpp:Variable(typeInfo, name, initializer, options)
        options = options or {}
        local parts = {}
        if options.static then table.insert(parts, "static") end
        if options.inline then table.insert(parts, "inline") end
        if options.constexpr then table.insert(parts, "constexpr") end
        if options.const then table.insert(parts, "const") end
        local typeText = TypeName(typeInfo)
        if options.pointer then typeText = typeText .. "*"
        elseif options.reference then typeText = typeText .. "&" end
        table.insert(parts, typeText)
        local declaration = table.concat(parts, " ") .. " " .. name
        if initializer ~= nil then declaration = declaration .. " = " .. initializer end
        self:Statement(declaration)
    end

    function cpp:Block(header, callback, suffix, terminateLine)
        self:Line(header)
        self:Line("{")
        writer:AddIndent()
        callback()
        writer:SubIndent()
        local closing = "}" .. (suffix or "")
        if terminateLine == false then writer:Write(closing) else self:Line(closing) end
    end
    function cpp:Struct(name, callback, terminateLine) self:Block("struct " .. name, callback, ";", terminateLine) end

    local function Parameter(parameter)
        local typeText = TypeName(parameter.type)
        if parameter.type == Type.ARRAY or parameter.type == Type.VECTOR then
            local arguments = { TypeName(parameter.subType) }
            if parameter.type == Type.ARRAY then table.insert(arguments, parameter.count) end
            typeText = typeText .. "<" .. Join(arguments) .. ">"
        end
        local flags = parameter.flags or {}
        if flags.ptr then typeText = typeText .. "*"
        elseif flags.ref then typeText = typeText .. "&" end
        return typeText .. " " .. parameter.name
    end

    function cpp:Function(specification, callback)
        local parameters = {}
        for index, parameter in ipairs(specification.parameters or {}) do parameters[index] = Parameter(parameter) end
        local header = specification.static and "static " or ""
        header = header .. TypeName(specification.returns or Type.VOID) .. " " .. specification.name .. "(" .. Join(parameters) .. ")"
        if specification.const then header = header .. " const" end
        self:Block(header, callback)
    end

    return { writer = writer, cpp = cpp }
end

return M
