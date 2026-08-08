local Archetype = require("Archetype")
local Component = require("Component")
local D = require("Definition")
local Type = require("Type")

local M = {}

local PARAMETER_COUNT = 6
local I32_MIN = -2147483648
local I32_MAX = 2147483647

M.OwnerType =
{
    None = 0,
    Spell = 1,
    Aura = 2
}

M.TargetMode =
{
    None = 0,
    Required = 1
}

M.TargetKind =
{
    None = 0,
    Unit = 1,
    Location = 2,
    Item = 3
}

M.TargetState =
{
    Any = 0,
    Alive = 1,
    Dead = 2
}

local validOwners =
{
    [M.OwnerType.None] = "None",
    [M.OwnerType.Spell] = "Spell",
    [M.OwnerType.Aura] = "Aura"
}

local validTargetModes =
{
    [M.TargetMode.None] = "None",
    [M.TargetMode.Required] = "Required"
}

local validTargetKinds =
{
    [M.TargetKind.None] = "None",
    [M.TargetKind.Unit] = "Unit",
    [M.TargetKind.Location] = "Location",
    [M.TargetKind.Item] = "Item"
}

local validTargetStates =
{
    [M.TargetState.Any] = "Any",
    [M.TargetState.Alive] = "Alive",
    [M.TargetState.Dead] = "Dead"
}

local validParameterKinds =
{
    Integer = true,
    Boolean = true,
    Enum = true,
    Flags = true,
    SpellID = true,
    FactionID = true,
    Standing = true,
    DurationMilliseconds = true,
    IntervalMilliseconds = true
}

local function Fail(definition, message)
    error("Spell effect catalog '" .. definition.name .. "': " .. message)
end

local function ValidateInteger(definition, value, description)
    if type(value) ~= "number" or value % 1 ~= 0 or value < I32_MIN or value > I32_MAX then
        Fail(definition, description .. " must be an i32")
    end
end

local function ValidateParameter(definition, effectName, parameter, parameterIndex)
    if type(parameter) ~= "table" then
        Fail(definition, "effect '" .. effectName .. "' parameter #" .. parameterIndex .. " must be created with Parameter")
    end
    if type(parameter.name) ~= "string" or parameter.name == "" then
        Fail(definition, "effect '" .. effectName .. "' parameter #" .. parameterIndex .. " requires a name")
    end
    if not validParameterKinds[parameter.kind] then
        Fail(definition, "effect '" .. effectName .. "' parameter '" .. parameter.name .. " has invalid kind '" .. tostring(parameter.kind) .. "'")
    end
    if type(parameter.semanticType) ~= "string" or parameter.semanticType == "" then
        Fail(definition, "effect '" .. effectName .. "' parameter '" .. parameter.name .. "' requires a semantic type")
    end

    local minimum = parameter.minimum == nil and I32_MIN or parameter.minimum
    local maximum = parameter.maximum == nil and I32_MAX or parameter.maximum
    local default = parameter.default == nil and 0 or parameter.default
    local parameterDescription = "effect '" .. effectName .. "' parameter '" .. parameter.name .. "'"
    ValidateInteger(definition, minimum, parameterDescription .. " minimum")
    ValidateInteger(definition, maximum, parameterDescription .. " maximum")
    ValidateInteger(definition, default, parameterDescription .. " default")
    if minimum > maximum then
        Fail(definition, "effect '" .. effectName .. "' parameter '" .. parameter.name .. "' has minimum greater than maximum")
    end
    if default < minimum or default > maximum then
        Fail(definition, "effect '" .. effectName .. "' parameter '" .. parameter.name .. "' has a default outside its range")
    end
    if parameter.kind == "Boolean" and (minimum ~= 0 or maximum ~= 1) then
        Fail(definition, "boolean parameter '" .. effectName .. "." .. parameter.name .. "' must use the range 0 through 1")
    end

    if type(parameter.options) ~= "table" then
        Fail(definition, "effect '" .. effectName .. "' parameter '" .. parameter.name .. "' must declare options as a table")
    end

    local supportsOptions = parameter.kind == "Enum" or parameter.kind == "Flags"
    if supportsOptions and #parameter.options == 0 then
        Fail(definition, "effect '" .. effectName .. "' parameter '" .. parameter.name .. "' requires at least one option")
    elseif not supportsOptions and #parameter.options ~= 0 then
        Fail(definition, "effect '" .. effectName .. "' parameter '" .. parameter.name .. "' does not support options")
    end

    local optionValues = {}
    local optionNames = {}
    local hasDefaultOption = false
    for optionIndex, option in ipairs(parameter.options) do
        if type(option) ~= "table" then
            Fail(definition, "effect '" .. effectName .. "' parameter '" .. parameter.name .. "' option #" .. optionIndex .. " must be created with Option")
        end
        ValidateInteger(definition, option.value,
            parameterDescription .. " option #" .. optionIndex .. " value")
        if type(option.name) ~= "string" or option.name == "" then
            Fail(definition, "effect '" .. effectName .. "' parameter '" .. parameter.name .. "' option #" .. optionIndex .. " requires a name")
        end
        if option.value < minimum or option.value > maximum then
            Fail(definition, "effect '" .. effectName .. "' parameter '" .. parameter.name .. "' option '" .. option.name .. "' is outside its range")
        end
        if optionValues[option.value] then
            Fail(definition, "effect '" .. effectName .. "' parameter '" .. parameter.name .. "' has duplicate option value " .. option.value)
        end
        if optionNames[option.name] then
            Fail(definition, "effect '" .. effectName .. "' parameter '" .. parameter.name .. "' has duplicate option name '" .. option.name .. "'")
        end

        optionValues[option.value] = true
        optionNames[option.name] = true
        hasDefaultOption = hasDefaultOption or option.value == default
    end

    if parameter.kind == "Enum" and not hasDefaultOption then
        Fail(definition, "effect '" .. effectName .. "' enum parameter '" .. parameter.name .. "' requires an option matching its default")
    end
end

local function FindParameterIndex(parameters, parameterName)
    for parameterIndex, parameter in ipairs(parameters) do
        if parameter.name == parameterName then
            return parameterIndex
        end
    end

    return nil
end

local function ValidateConstraint(definition, effectName, effect, constraint, constraintIndex)
    if type(constraint) ~= "table" or constraint.kind ~= "LessThanOrEqual" then
        Fail(definition, "effect '" .. effectName .. "' constraint #" .. constraintIndex .. " has an invalid kind")
    end

    if not FindParameterIndex(effect.parameters, constraint.left) then
        Fail(definition, "effect '" .. effectName .. "' constraint #" .. constraintIndex .. " references unknown parameter '" .. tostring(constraint.left) .. "'")
    end
    if not FindParameterIndex(effect.parameters, constraint.right) then
        Fail(definition, "effect '" .. effectName .. "' constraint #" .. constraintIndex .. " references unknown parameter '" .. tostring(constraint.right) .. "'")
    end
end

local function ParameterInitializer(parameter, optionsSymbol)
    return "SpellEffectParameterDescriptor{ " ..
        "\"" .. parameter.name .. "\", " ..
        "SpellEffectParameterKind::" .. parameter.kind .. ", " ..
        "\"" .. parameter.semanticType .. "\", " ..
        tostring(parameter.default) .. ", " .. tostring(parameter.minimum) .. ", " .. tostring(parameter.maximum) .. ", " ..
        (optionsSymbol or "{}") .. " }"
end

local SpellEffectCatalogComponent =
{
    target = "cpp",
    include =
    {
        system =
        {
            "Base/Types.h",
            "array",
            "span",
            "string_view"
        }
    },
    flags =
    {
        supportsEnum = true,
        supportsStruct = false
    },

    Validate = function(self, definition)
        local catalog = definition.options.catalog
        if type(catalog) ~= "table" then
            Fail(definition, "requires catalog entries")
        end

        for _, field in ipairs(definition.fields) do
            local effect = catalog[field.name]
            if type(effect) ~= "table" then
                Fail(definition, "enum field '" .. field.name .. "' has no catalog entry")
            end
            if not validOwners[effect.owner] then
                Fail(definition, "effect '" .. field.name .. "' must use a value from OwnerType")
            end
            if type(effect.periodic) ~= "boolean" then
                Fail(definition, "effect '" .. field.name .. "' must declare whether it is periodic")
            end
            if type(effect.target) ~= "table" then
                Fail(definition, "effect '" .. field.name .. "' must declare a target contract")
            end
            if not validTargetModes[effect.target.mode] then
                Fail(definition, "effect '" .. field.name .. "' target mode must use a value from TargetMode")
            end
            if not validTargetKinds[effect.target.kind] then
                Fail(definition, "effect '" .. field.name .. "' target kind must use a value from TargetKind")
            end
            if not validTargetStates[effect.target.state] then
                Fail(definition, "effect '" .. field.name .. "' target state must use a value from TargetState")
            end
            if effect.target.mode == M.TargetMode.None and
                (effect.target.kind ~= M.TargetKind.None or effect.target.state ~= M.TargetState.Any) then
                Fail(definition, "effect '" .. field.name .. "' without a target must use target kind None and state Any")
            end
            if effect.target.mode == M.TargetMode.Required and effect.target.kind == M.TargetKind.None then
                Fail(definition, "effect '" .. field.name .. "' with a required target must declare a target kind")
            end
            if effect.target.kind ~= M.TargetKind.Unit and effect.target.state ~= M.TargetState.Any then
                Fail(definition, "effect '" .. field.name .. "' can only constrain target state for unit targets")
            end
            if type(effect.parameters) ~= "table" or #effect.parameters > PARAMETER_COUNT then
                Fail(definition, "effect '" .. field.name .. "' must declare at most " .. PARAMETER_COUNT .. " parameters")
            end

            local names = {}
            for parameterIndex, parameter in ipairs(effect.parameters) do
                ValidateParameter(definition, field.name, parameter, parameterIndex)
                if names[parameter.name] then
                    Fail(definition, "effect '" .. field.name .. "' has duplicate parameter '" .. parameter.name .. "'")
                end
                names[parameter.name] = true
            end

            if type(effect.constraints) ~= "table" then
                Fail(definition, "effect '" .. field.name .. "' must declare constraints as a table")
            end
            for constraintIndex, constraint in ipairs(effect.constraints) do
                ValidateConstraint(definition, field.name, effect, constraint, constraintIndex)
            end
        end

        for effectName in pairs(catalog) do
            local found = false
            for _, field in ipairs(definition.fields) do
                found = found or field.name == effectName
            end
            if not found then
                Fail(definition, "catalog entry '" .. effectName .. "' has no enum field")
            end
        end
    end,

    PostGenerate = function(self, definition, context)
        local cpp = context.cpp
        local writer = context.writer
        local catalog = definition.model.options.catalog

        cpp:Line("enum class SpellEffectOwner : u8")
        cpp:Line("{")
        writer:AddIndent()
        cpp:Line("None = 0,")
        cpp:Line("Spell = 1,")
        cpp:Line("Aura = 2")
        writer:SubIndent()
        cpp:Line("};")
        cpp:BlankLine()

        cpp:Line("enum class SpellEffectParameterKind : u8")
        cpp:Line("{")
        writer:AddIndent()
        cpp:Line("Unused = 0,")
        cpp:Line("Integer = 1,")
        cpp:Line("Boolean = 2,")
        cpp:Line("Enum = 3,")
        cpp:Line("Flags = 4,")
        cpp:Line("SpellID = 5,")
        cpp:Line("FactionID = 6,")
        cpp:Line("Standing = 7,")
        cpp:Line("DurationMilliseconds = 8,")
        cpp:Line("IntervalMilliseconds = 9")
        writer:SubIndent()
        cpp:Line("};")
        cpp:BlankLine()

        cpp:Line("enum class SpellEffectTargetMode : u8")
        cpp:Line("{")
        writer:AddIndent()
        cpp:Line("None = 0,")
        cpp:Line("Required = 1")
        writer:SubIndent()
        cpp:Line("};")
        cpp:BlankLine()

        cpp:Line("enum class SpellEffectTargetKind : u8")
        cpp:Line("{")
        writer:AddIndent()
        cpp:Line("None = 0,")
        cpp:Line("Unit = 1,")
        cpp:Line("Location = 2,")
        cpp:Line("Item = 3")
        writer:SubIndent()
        cpp:Line("};")
        cpp:BlankLine()

        cpp:Line("enum class SpellEffectTargetState : u8")
        cpp:Line("{")
        writer:AddIndent()
        cpp:Line("Any = 0,")
        cpp:Line("Alive = 1,")
        cpp:Line("Dead = 2")
        writer:SubIndent()
        cpp:Line("};")
        cpp:BlankLine()

        cpp:Line("struct SpellEffectTargetDescriptor")
        cpp:Line("{")
        cpp:Line("public:")
        writer:AddIndent()
        cpp:Line("SpellEffectTargetMode mode;")
        cpp:Line("SpellEffectTargetKind kind;")
        cpp:Line("SpellEffectTargetState state;")
        writer:SubIndent()
        cpp:Line("};")
        cpp:BlankLine()

        cpp:Line("constexpr std::string_view GetSpellEffectTargetModeName(SpellEffectTargetMode mode)")
        cpp:Line("{")
        writer:AddIndent()
        cpp:Line("switch (mode)")
        cpp:Line("{")
        writer:AddIndent()
        cpp:Line("case SpellEffectTargetMode::None: return \"None\";")
        cpp:Line("case SpellEffectTargetMode::Required: return \"Required\";")
        cpp:Line("default: return \"Unknown\";")
        writer:SubIndent()
        cpp:Line("}")
        writer:SubIndent()
        cpp:Line("}")
        cpp:BlankLine()

        cpp:Line("constexpr std::string_view GetSpellEffectTargetKindName(SpellEffectTargetKind kind)")
        cpp:Line("{")
        writer:AddIndent()
        cpp:Line("switch (kind)")
        cpp:Line("{")
        writer:AddIndent()
        cpp:Line("case SpellEffectTargetKind::None: return \"None\";")
        cpp:Line("case SpellEffectTargetKind::Unit: return \"Unit\";")
        cpp:Line("case SpellEffectTargetKind::Location: return \"Location\";")
        cpp:Line("case SpellEffectTargetKind::Item: return \"Item\";")
        cpp:Line("default: return \"Unknown\";")
        writer:SubIndent()
        cpp:Line("}")
        writer:SubIndent()
        cpp:Line("}")
        cpp:BlankLine()

        cpp:Line("constexpr std::string_view GetSpellEffectTargetStateName(SpellEffectTargetState state)")
        cpp:Line("{")
        writer:AddIndent()
        cpp:Line("switch (state)")
        cpp:Line("{")
        writer:AddIndent()
        cpp:Line("case SpellEffectTargetState::Any: return \"Any\";")
        cpp:Line("case SpellEffectTargetState::Alive: return \"Alive\";")
        cpp:Line("case SpellEffectTargetState::Dead: return \"Dead\";")
        cpp:Line("default: return \"Unknown\";")
        writer:SubIndent()
        cpp:Line("}")
        writer:SubIndent()
        cpp:Line("}")
        cpp:BlankLine()

        cpp:Line("enum class SpellEffectParameterValidationError : u8")
        cpp:Line("{")
        writer:AddIndent()
        cpp:Line("None = 0,")
        cpp:Line("UnusedParameter = 1,")
        cpp:Line("BelowMinimum = 2,")
        cpp:Line("AboveMaximum = 3,")
        cpp:Line("InvalidEnumValue = 4,")
        cpp:Line("UnsupportedFlagValue = 5,")
        cpp:Line("ConstraintViolation = 6")
        writer:SubIndent()
        cpp:Line("};")
        cpp:BlankLine()

        cpp:Line("struct SpellEffectParameterValidationResult")
        cpp:Line("{")
        cpp:Line("public:")
        writer:AddIndent()
        cpp:Line("constexpr bool IsValid() const { return error == SpellEffectParameterValidationError::None; }")
        writer:SubIndent()
        cpp:BlankLine()
        cpp:Line("public:")
        writer:AddIndent()
        cpp:Line("SpellEffectParameterValidationError error = SpellEffectParameterValidationError::None;")
        cpp:Line("u8 parameterIndex = 0xFF;")
        cpp:Line("u8 relatedParameterIndex = 0xFF;")
        cpp:Line("i32 value = 0;")
        cpp:Line("i32 minimumValue = 0;")
        cpp:Line("i32 maximumValue = 0;")
        writer:SubIndent()
        cpp:Line("};")
        cpp:BlankLine()

        cpp:Line("constexpr std::string_view GetSpellEffectParameterValidationErrorName(SpellEffectParameterValidationError error)")
        cpp:Line("{")
        writer:AddIndent()
        cpp:Line("switch (error)")
        cpp:Line("{")
        writer:AddIndent()
        cpp:Line("case SpellEffectParameterValidationError::None: return \"None\";")
        cpp:Line("case SpellEffectParameterValidationError::UnusedParameter: return \"UnusedParameter\";")
        cpp:Line("case SpellEffectParameterValidationError::BelowMinimum: return \"BelowMinimum\";")
        cpp:Line("case SpellEffectParameterValidationError::AboveMaximum: return \"AboveMaximum\";")
        cpp:Line("case SpellEffectParameterValidationError::InvalidEnumValue: return \"InvalidEnumValue\";")
        cpp:Line("case SpellEffectParameterValidationError::UnsupportedFlagValue: return \"UnsupportedFlagValue\";")
        cpp:Line("case SpellEffectParameterValidationError::ConstraintViolation: return \"ConstraintViolation\";")
        cpp:Line("default: return \"Unknown\";")
        writer:SubIndent()
        cpp:Line("}")
        writer:SubIndent()
        cpp:Line("}")
        cpp:BlankLine()

        cpp:Line("struct SpellEffectParameterOption")
        cpp:Line("{")
        cpp:Line("public:")
        writer:AddIndent()
        cpp:Line("i32 value;")
        cpp:Line("std::string_view name;")
        writer:SubIndent()
        cpp:Line("};")
        cpp:BlankLine()

        cpp:Line("struct SpellEffectParameterDescriptor")
        cpp:Line("{")
        cpp:Line("public:")
        writer:AddIndent()
        cpp:Line("std::string_view name;")
        cpp:Line("SpellEffectParameterKind kind;")
        cpp:Line("std::string_view semanticType;")
        cpp:Line("i32 defaultValue;")
        cpp:Line("i32 minimumValue;")
        cpp:Line("i32 maximumValue;")
        cpp:Line("std::span<const SpellEffectParameterOption> options;")
        writer:SubIndent()
        cpp:Line("};")
        cpp:BlankLine()

        cpp:Line("struct SpellEffectDescriptor")
        cpp:Line("{")
        cpp:Line("public:")
        writer:AddIndent()
        cpp:Line(definition.name .. " type;")
        cpp:Line("std::string_view name;")
        cpp:Line("SpellEffectOwner owner;")
        cpp:Line("bool periodic;")
        cpp:Line("SpellEffectTargetDescriptor target;")
        cpp:Line("u8 parameterCount;")
        cpp:Line("std::array<SpellEffectParameterDescriptor, " .. PARAMETER_COUNT .. "> parameters;")
        writer:SubIndent()
        cpp:Line("};")
        cpp:BlankLine()

        local optionsSymbols = {}
        for fieldIndex, field in ipairs(definition.fields) do
            local effect = catalog[field.name]
            optionsSymbols[field.name] = {}
            for parameterIndex, parameter in ipairs(effect.parameters) do
                if #parameter.options > 0 then
                    local symbol = "SPELL_EFFECT_PARAMETER_OPTIONS_" .. fieldIndex .. "_" .. parameterIndex
                    optionsSymbols[field.name][parameterIndex] = symbol
                    cpp:Line("inline constexpr std::array<SpellEffectParameterOption, " .. #parameter.options .. "> " .. symbol .. " =")
                    cpp:Line("{")
                    writer:AddIndent()
                    for optionIndex, option in ipairs(parameter.options) do
                        local suffix = optionIndex < #parameter.options and "," or ""
                        cpp:Line("SpellEffectParameterOption{ " .. option.value .. ", \"" .. option.name .. "\" }" .. suffix)
                    end
                    writer:SubIndent()
                    cpp:Line("};")
                    cpp:BlankLine()
                end
            end
        end

        cpp:Line("inline constexpr std::array<SpellEffectDescriptor, " .. #definition.fields .. "> SPELL_EFFECT_CATALOG =")
        cpp:Line("{")
        writer:AddIndent()
        for fieldIndex, field in ipairs(definition.fields) do
            local effect = catalog[field.name]
            cpp:Line("SpellEffectDescriptor{")
            writer:AddIndent()
            cpp:Line("." .. "type = " .. definition.name .. "::" .. field.name .. ",")
            cpp:Line(".name = \"" .. field.name .. "\",")
            cpp:Line(".owner = SpellEffectOwner::" .. validOwners[effect.owner] .. ",")
            cpp:Line(".periodic = " .. tostring(effect.periodic) .. ",")
            cpp:Line(".target = { SpellEffectTargetMode::" .. validTargetModes[effect.target.mode] ..
                ", SpellEffectTargetKind::" .. validTargetKinds[effect.target.kind] ..
                ", SpellEffectTargetState::" .. validTargetStates[effect.target.state] .. " },")
            cpp:Line(".parameterCount = " .. #effect.parameters .. ",")
            cpp:Line(".parameters = {")
            writer:AddIndent()
            for parameterIndex = 1, PARAMETER_COUNT do
                local parameter = effect.parameters[parameterIndex]
                local optionsSymbol = optionsSymbols[field.name][parameterIndex]
                local initializer = parameter and ParameterInitializer(parameter, optionsSymbol) or
                    "SpellEffectParameterDescriptor{ \"\", SpellEffectParameterKind::Unused, \"\", 0, 0, 0, {} }"
                if parameterIndex < PARAMETER_COUNT then initializer = initializer .. "," end
                cpp:Line(initializer)
            end
            writer:SubIndent()
            cpp:Line("}")
            writer:SubIndent()
            local suffix = fieldIndex < #definition.fields and "}," or "}"
            cpp:Line(suffix)
        end
        writer:SubIndent()
        cpp:Line("};")
        cpp:BlankLine()

        cpp:Line("constexpr const SpellEffectDescriptor* GetSpellEffectDescriptor(" .. definition.name .. " type)")
        cpp:Line("{")
        writer:AddIndent()
        cpp:Line("for (const SpellEffectDescriptor& descriptor : SPELL_EFFECT_CATALOG)")
        cpp:Line("{")
        writer:AddIndent()
        cpp:Line("if (descriptor.type == type)")
        writer:AddIndent()
        cpp:Line("return &descriptor;")
        writer:SubIndent()
        writer:SubIndent()
        cpp:Line("}")
        cpp:BlankLine()
        cpp:Line("return nullptr;")
        writer:SubIndent()
        cpp:Line("}")
        cpp:BlankLine()

        cpp:Line("constexpr SpellEffectParameterValidationResult ValidateSpellEffectParameters(const SpellEffectDescriptor& descriptor, const std::array<i32, " .. PARAMETER_COUNT .. ">& parameters)")
        cpp:Line("{")
        writer:AddIndent()
        cpp:Line("for (u32 parameterIndex = 0; parameterIndex < parameters.size(); ++parameterIndex)")
        cpp:Line("{")
        writer:AddIndent()
        cpp:Line("const SpellEffectParameterDescriptor& parameter = descriptor.parameters[parameterIndex];")
        cpp:Line("const i32 value = parameters[parameterIndex];")
        cpp:Line("if (parameter.kind == SpellEffectParameterKind::Unused)")
        cpp:Line("{")
        writer:AddIndent()
        cpp:Line("if (value != 0)")
        writer:AddIndent()
        cpp:Line("return { .error = SpellEffectParameterValidationError::UnusedParameter, .parameterIndex = static_cast<u8>(parameterIndex), .value = value };")
        writer:SubIndent()
        cpp:Line("continue;")
        writer:SubIndent()
        cpp:Line("}")
        cpp:BlankLine()
        cpp:Line("if (value < parameter.minimumValue)")
        writer:AddIndent()
        cpp:Line("return { .error = SpellEffectParameterValidationError::BelowMinimum, .parameterIndex = static_cast<u8>(parameterIndex), .value = value, .minimumValue = parameter.minimumValue, .maximumValue = parameter.maximumValue };")
        writer:SubIndent()
        cpp:Line("if (value > parameter.maximumValue)")
        writer:AddIndent()
        cpp:Line("return { .error = SpellEffectParameterValidationError::AboveMaximum, .parameterIndex = static_cast<u8>(parameterIndex), .value = value, .minimumValue = parameter.minimumValue, .maximumValue = parameter.maximumValue };")
        writer:SubIndent()
        cpp:BlankLine()
        cpp:Line("if (parameter.kind == SpellEffectParameterKind::Enum)")
        cpp:Line("{")
        writer:AddIndent()
        cpp:Line("bool found = false;")
        cpp:Line("for (const SpellEffectParameterOption& option : parameter.options)")
        cpp:Line("{")
        writer:AddIndent()
        cpp:Line("found = found || option.value == value;")
        writer:SubIndent()
        cpp:Line("}")
        cpp:Line("if (!found)")
        writer:AddIndent()
        cpp:Line("return { .error = SpellEffectParameterValidationError::InvalidEnumValue, .parameterIndex = static_cast<u8>(parameterIndex), .value = value, .minimumValue = parameter.minimumValue, .maximumValue = parameter.maximumValue };")
        writer:SubIndent()
        writer:SubIndent()
        cpp:Line("}")
        cpp:Line("else if (parameter.kind == SpellEffectParameterKind::Flags)")
        cpp:Line("{")
        writer:AddIndent()
        cpp:Line("u32 supportedFlags = 0;")
        cpp:Line("for (const SpellEffectParameterOption& option : parameter.options)")
        cpp:Line("{")
        writer:AddIndent()
        cpp:Line("supportedFlags |= static_cast<u32>(option.value);")
        writer:SubIndent()
        cpp:Line("}")
        cpp:Line("if ((static_cast<u32>(value) & ~supportedFlags) != 0)")
        writer:AddIndent()
        cpp:Line("return { .error = SpellEffectParameterValidationError::UnsupportedFlagValue, .parameterIndex = static_cast<u8>(parameterIndex), .value = value, .maximumValue = static_cast<i32>(supportedFlags) };")
        writer:SubIndent()
        writer:SubIndent()
        cpp:Line("}")
        writer:SubIndent()
        cpp:Line("}")
        cpp:BlankLine()

        for _, field in ipairs(definition.fields) do
            local effect = catalog[field.name]
            for _, constraint in ipairs(effect.constraints) do
                local leftIndex = FindParameterIndex(effect.parameters, constraint.left) - 1
                local rightIndex = FindParameterIndex(effect.parameters, constraint.right) - 1
                cpp:Line("if (descriptor.type == " .. definition.name .. "::" .. field.name ..
                    " && parameters[" .. leftIndex .. "] > parameters[" .. rightIndex .. "])")
                writer:AddIndent()
                cpp:Line("return { .error = SpellEffectParameterValidationError::ConstraintViolation, .parameterIndex = " .. leftIndex .. ", .relatedParameterIndex = " .. rightIndex .. ", .value = parameters[" .. leftIndex .. "], .minimumValue = descriptor.parameters[" .. leftIndex .. "].minimumValue, .maximumValue = parameters[" .. rightIndex .. "] };")
                writer:SubIndent()
            end
        end

        cpp:BlankLine()
        cpp:Line("return {};")
        writer:SubIndent()
        cpp:Line("}")

        return true
    end
}

local SpellEffectCatalogArchetype =
{
    kind = "enum",
    base = Type.ENUM,
    components = { Component.EnumMetaData, SpellEffectCatalogComponent }
}

function M.Parameter(name, kind, options)
    options = options or {}
    return
    {
        name = name,
        kind = kind,
        semanticType = options.semanticType or kind,
        default = options.default == nil and 0 or options.default,
        minimum = options.minimum == nil and I32_MIN or options.minimum,
        maximum = options.maximum == nil and I32_MAX or options.maximum,
        options = options.values or {}
    }
end

function M.Integer(name, options)
    return M.Parameter(name, "Integer", options)
end

function M.Percentage(name, options)
    options = options or {}
    options.semanticType = "Percent"
    options.minimum = options.minimum == nil and 0 or options.minimum
    options.maximum = options.maximum == nil and 100 or options.maximum
    return M.Integer(name, options)
end

function M.Boolean(name, options)
    options = options or {}
    options.semanticType = "bool"
    options.minimum = 0
    options.maximum = 1
    return M.Parameter(name, "Boolean", options)
end

function M.Enum(name, semanticType, options)
    options = options or {}
    options.semanticType = semanticType
    return M.Parameter(name, "Enum", options)
end

function M.Flags(name, semanticType, options)
    options = options or {}
    options.semanticType = semanticType
    return M.Parameter(name, "Flags", options)
end

function M.SpellID(name, options)
    options = options or {}
    options.semanticType = "SpellID"
    return M.Parameter(name, "SpellID", options)
end

function M.FactionID(name, options)
    options = options or {}
    options.semanticType = "Gameplay::Faction::FactionID"
    return M.Parameter(name, "FactionID", options)
end

function M.Standing(name, options)
    options = options or {}
    options.semanticType = "FactionStanding"
    return M.Parameter(name, "Standing", options)
end

function M.DurationMilliseconds(name, options)
    options = options or {}
    options.semanticType = "Milliseconds"
    return M.Parameter(name, "DurationMilliseconds", options)
end

function M.IntervalMilliseconds(name, options)
    options = options or {}
    options.semanticType = "Milliseconds"
    return M.Parameter(name, "IntervalMilliseconds", options)
end

function M.Option(value, name)
    return
    {
        value = value,
        name = name
    }
end

function M.Target(mode, kind, state)
    return
    {
        mode = mode,
        kind = kind,
        state = state
    }
end

function M.Effect(value, name, options)
    options = options or {}
    local periodic = options.periodic
    if periodic == nil then
        periodic = false
    end

    return
    {
        name = name,
        value = value,
        owner = options.owner,
        periodic = periodic,
        target = options.target or M.Target(M.TargetMode.None, M.TargetKind.None, M.TargetState.Any),
        parameters = options.parameters or {},
        constraints = options.constraints or {}
    }
end

function M.LessThanOrEqual(left, right)
    return
    {
        kind = "LessThanOrEqual",
        left = left,
        right = right
    }
end

function M.Catalog(name, enumType, effects)
    local fields = {}
    local catalog = {}
    for _, effect in ipairs(effects) do
        fields[#fields + 1] = D.Field(effect.name, effect.value)
        catalog[effect.name] = effect
    end

    return D.Enum(name, enumType, fields,
    {
        archetype = SpellEffectCatalogArchetype,
        catalog = catalog
    })
end

return M
