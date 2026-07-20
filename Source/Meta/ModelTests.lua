local info = debug.getinfo(1, "S")
local scriptPath = info.source:sub(2)
local root = scriptPath:match("(.*/)")
package.path = root .. "?.lua;" .. package.path

require("OrderedTable")

local Type = require("Type")
local D = require("Definition")
local Model = require("Model")
local Backend = require("Backend")
local CppBackend = require("CppBackend")
local CppEmitter = require("CppEmitter")
local Component = require("Component")
local Output = require("Output")
local TestSuite = require("TestSuite")

local function Source(definitions)
    return
    {
        path = "ModelTests.lua",
        namespace = "MetaGen::Tests",
        defines = definitions
    }
end

local function ExpectFailure(name, expectedText, callback)
    local succeeded, message = pcall(callback)
    if succeeded then
        error("Expected test '" .. name .. "' to fail")
    end

    if tostring(message):find(expectedText, 1, true) == nil then
        error("Test '" .. name .. "' failed with unexpected message: " .. tostring(message))
    end
end

local function AssertContains(text, expected)
    assert(text:find(expected, 1, true), "Missing exact generated text:\n" .. expected)
end

local function EmitInMemory(model)
    local generation = CppEmitter.Create()
    local outputs = {}
    local context =
    {
        sourceRootDir = "Definitions",
        stagingRootDir = "memory",
        binRootDir = "MetaGen",
        namespaceBase = "MetaGen",
        pathSort = function(a, b) return a < b end,
        hash = function(value)
            local hashes = { FirstPacket = 101, SecondPacket = 102, ClientRows = 103, LuaPayload = 104, SpecialRows = 105 }
            return hashes[value] or 0
        end,
        writer = generation.writer,
        cpp = generation.cpp,
        makeDirectory = function() end,
        openOutput = function(path)
            local chunks = {}
            outputs[path] = chunks
            return
            {
                write = function(_, value) table.insert(chunks, value) end,
                close = function() end
            }
        end
    }

    Backend.ValidateAll(model, context)
    Backend.EmitAll(model, context)
    local result = {}
    for path, chunks in pairs(outputs) do result[path] = table.concat(chunks) end
    return result
end

local function Run()
    ExpectFailure("legacy definition map", "must return D.Definitions", function()
        Model.BuildFromSources({ Source(OrderedTable()) })
    end)
    ExpectFailure("raw definition entry", "typed Definition constructor", function()
        D.Definitions({ { "LegacyPacket", { fields = {} } } })
    end)

    local validDefinitions = D.Definitions
    {
        D.Packet("TestPacket",
        {
            D.Field("id", Type.U32),
            D.Field("values", Type.ARRAY, { type = Type.U16, count = 4 }),
            D.Field("runtimeValues", Type.VECTOR, { type = Type.U8, serialize = false, debug = false })
        }),
        D.Enum("TestEnum", Type.U8,
        {
            D.Field("Invalid"),
            D.Field("First"),
            D.Field("FirstAlias", "First")
        })
    }

    local model = Model.BuildFromSources({ Source(validDefinitions) })
    assert(#model.definitions == 2)
    assert(#model.definitionsByKind.packet == 1)
    assert(#model.definitionsByKind.enum == 1)
    assert(#model.definitionsByTarget.cpp == 2)
    assert(model.namespaces["MetaGen::Tests"].definitions.TestPacket.fields[2].attributes.count == 4)
    assert(model.namespaces["MetaGen::Tests"].definitions.TestEnum.fields[3].resolvedValue == 1)

    Backend.Register("cpp", CppBackend)
    Backend.ValidateTargets(model)
    local firstSelection = CppBackend.SelectNamespaces(model)
    local secondSelection = CppBackend.SelectNamespaces(model)
    local firstNames = {}
    local secondNames = {}
    for name in pairs(firstSelection) do table.insert(firstNames, name) end
    for name in pairs(secondSelection) do table.insert(secondNames, name) end
    assert(table.concat(firstNames, "|") == table.concat(secondNames, "|"))
    assert(firstNames[1] == "MetaGen::Tests")

    Component.PacketMetaData.nextPacketID = 99
    Component.PacketMetaData.packets = { "stale" }
    Component.EnumMetaData.nextEnumID = 99
    Component.EnumMetaData.enums = { { name = "stale" } }
    Component.EnumMetaData.enumFilesToInclude = { stale = 1 }
    Component.LuaEventMetaData.nextStructID = 99
    CppBackend.ResetGenerationState()
    assert(Component.PacketMetaData.nextPacketID == 1 and #Component.PacketMetaData.packets == 0)
    assert(Component.EnumMetaData.nextEnumID == 0 and #Component.EnumMetaData.enums == 0)
    assert(next(Component.EnumMetaData.enumFilesToInclude) == nil)
    assert(Component.LuaEventMetaData.nextStructID == 1)

    local firstWriter = CppEmitter.Create()
    local secondWriter = CppEmitter.Create()
    assert(firstWriter.writer ~= secondWriter.writer)
    assert(firstWriter.cpp ~= secondWriter.cpp)
    assert(firstWriter.cpp.writer == firstWriter.writer)
    assert(secondWriter.cpp.writer == secondWriter.writer)

    local backendContext =
    {
        writer = firstWriter.writer,
        cpp = firstWriter.cpp,
        pathSort = function(a, b) return a < b end,
        hash = function() return 0 end
    }
    Backend.ValidateAll(model, backendContext)
    local firstPlan = Backend.PlanOutputs(model, backendContext)
    local secondPlan = Backend.PlanOutputs(model, backendContext)
    assert(table.concat(firstPlan, "|") == "EnumTraits.h|PacketList.h|Tests/Tests.h")
    assert(table.concat(firstPlan, "|") == table.concat(secondPlan, "|"))

    local function TestBackend(outputs)
        return
        {
            Validate = function() end,
            PlanOutputs = function() return outputs end,
            Emit = function() end
        }
    end
    Backend.Register("testOutputA", TestBackend({ "shared.out" }))
    Backend.Register("testOutputB", TestBackend({ "shared.out" }))
    local overlappingOutputModel = Model.BuildFromSources({ Source(D.Definitions
    {
        D.Packet("OverlappingOutput", {}, { targets = { testOutputA = true, testOutputB = true } })
    }) })
    ExpectFailure("overlapping backend outputs", "planned by both", function()
        Backend.PlanOutputs(overlappingOutputModel, {})
    end)

    Backend.Register("testUnsafeOutput", TestBackend({ "../unsafe.out" }))
    local unsafeOutputModel = Model.BuildFromSources({ Source(D.Definitions
    {
        D.Packet("UnsafeOutput", {}, { targets = { testUnsafeOutput = true } })
    }) })
    ExpectFailure("unsafe backend output", "planned unsafe output path", function()
        Backend.PlanOutputs(unsafeOutputModel, {})
    end)

    Backend.Register("testSparseOutput", TestBackend({ [1] = "first.out", [3] = "third.out" }))
    local sparseOutputModel = Model.BuildFromSources({ Source(D.Definitions
    {
        D.Packet("SparseOutput", {}, { targets = { testSparseOutput = true } })
    }) })
    ExpectFailure("sparse backend output", "must not contain holes", function()
        Backend.PlanOutputs(sparseOutputModel, {})
    end)

    local dslModel = Model.BuildFromSources({ Source(D.Definitions
    {
        D.Packet("DslPacket", { D.Field("value", Type.U32) }),
        D.Enum("DslEnum", Type.U8, { "Invalid", "Count" }),
        D.LuaEnum("DslLuaEnum", Type.U8, { "Invalid", "Count" }),
        D.NetField("DslNetField", Type.U8, { "Value" })
    }) })
    assert(#dslModel.definitions == 4)

    local targetModel = Model.BuildFromSources({ Source(D.Definitions
    {
        D.DatabaseTable("PostgresOnly",
        {
            D.Field("values", Type.VECTOR, { type = Type.U8, persistentId = "test.postgres_only.values" })
        }, { persistentId = "test.postgres_only" })
    }) })
    assert(targetModel.definitionsByTarget.cpp == nil)
    assert(#targetModel.definitionsByTarget.postgres == 1)
    ExpectFailure("unsupported backend", "unsupported target 'postgres'", function()
        Backend.ValidateTargets(targetModel)
    end)

    ExpectFailure("duplicate field", "duplicate field 'value'", function()
        Model.BuildFromSources({ Source(D.Definitions
        {
            D.Packet("DuplicateField", { D.Field("value", Type.U8), D.Field("value", Type.U16) })
        }) })
    end)

    ExpectFailure("invalid array", "requires a positive integer count", function()
        Model.BuildFromSources({ Source(D.Definitions
        {
            D.Packet("InvalidArray", { D.Field("values", Type.ARRAY, { type = Type.U8, count = 0 }) })
        }) })
    end)

    ExpectFailure("missing container attributes", "requires a valid element type", function()
        Model.BuildFromSources({ Source(D.Definitions
        {
            D.Packet("MissingContainerAttributes", { D.Field("values", Type.ARRAY) })
        }) })
    end)

    ExpectFailure("non-serializable field", "uses explicitly non-serializable type", function()
        local invalidModel = Model.BuildFromSources({ Source(D.Definitions
        {
            D.Packet("InvalidSerialization", { D.Field("values", Type.VECTOR, { type = Type.U8 }) })
        }) })
        Backend.ValidateAll(invalidModel, backendContext)
    end)

    ExpectFailure("unknown enum alias", "references unknown or later field", function()
        Model.BuildFromSources({ Source(D.Definitions
        {
            D.Enum("InvalidEnum", Type.U8, { D.Field("Alias", "Missing") })
        }) })
    end)

    ExpectFailure("non-integer enum", "must be an integer MetaGen type", function()
        Model.BuildFromSources({ Source(D.Definitions
        {
            D.Enum("FloatEnum", Type.F32, { D.Field("Invalid") })
        }) })
    end)

    ExpectFailure("invalid identifier", "is not a valid C++ identifier", function()
        local invalidModel = Model.BuildFromSources({ Source(D.Definitions
        {
            D.Packet("InvalidIdentifier", { D.Field("not-valid", Type.U8) })
        }) })
        Backend.ValidateAll(invalidModel, backendContext)
    end)

    local backendNeutralNames = Model.BuildFromSources({ Source(D.Definitions
    {
        D.DatabaseTable("not-valid-cpp",
        {
            D.Field("also-not-valid-cpp", Type.U32, { persistentId = "test.neutral.field" })
        }, { persistentId = "test.neutral", targets = { postgres = true } })
    }) })
    assert(backendNeutralNames.definitions[1].name == "not-valid-cpp")

    local mutationModel = Model.BuildFromSources({ Source(D.Definitions
    {
        D.Packet("MutationTest", { D.Field("value", Type.U32) })
    }) })
    mutationModel.definitions[1].fields[1].name = "changed"
    ExpectFailure("model mutation detection", "mutated the normalized model", function()
        Model.AssertUnchanged(mutationModel, "test")
    end)

    Backend.Register("testMutation", {
        Validate = function(_, definitions) definitions[1].name = "changed" end,
        PlanOutputs = function() return {} end,
        Emit = function() end
    })
    local backendMutationModel = Model.BuildFromSources({ Source(D.Definitions
    {
        D.Packet("BackendMutation", {}, { targets = { testMutation = true } })
    }) })
    ExpectFailure("backend mutation isolation", "backend 'testMutation' validation", function()
        Backend.ValidateAll(backendMutationModel, {})
    end)

    ExpectFailure("duplicate command alias", "is already used by", function()
        Model.BuildFromSources({ Source(D.Definitions
        {
            D.GameCommand("FirstCommand", { "shared alias" }, {}),
            D.GameCommand("SecondCommand", { "shared alias" }, {})
        }) })
    end)

    ExpectFailure("duplicate persistent id", "is already used by", function()
        Model.BuildFromSources({ Source(D.Definitions
        {
            D.DatabaseTable("FirstTable",
            {
                D.Field("id", Type.U32, { persistentId = "test.shared.id" })
            }, { persistentId = "test.first" }),
            D.DatabaseTable("SecondTable",
            {
                D.Field("id", Type.U32, { persistentId = "test.shared.id" })
            }, { persistentId = "test.second" })
        }) })
    end)

    ExpectFailure("ClientDB generated suffix", "must not include the generated Record suffix", function()
        EmitInMemory(Model.BuildFromSources({ Source(D.Definitions
        {
            D.ClientDB("LegacyRecord", {})
        }) }))
    end)

    local goldenModel = Model.BuildFromSources({ Source(D.Definitions
    {
        D.Packet("FirstPacket",
        {
            D.Field("escaped", Type.STRING, { default = "quote\" slash\\ line\n" }),
            D.Field("serialized", Type.U16),
            D.Field("runtimeOnly", Type.U32, { serialize = false, debug = false })
        }),
        D.Packet("SecondPacket", {}),
        D.Enum("FirstEnum", Type.U8,
        {
            D.Field("Invalid"),
            D.Field("Explicit", 7),
            D.Field("Alias", "Explicit")
        }),
        D.Enum("SecondEnum", Type.U16, { "Zero", "One" }),
        D.ClientDB("ClientRows",
        {
            D.Field("id", Type.U32),
            D.Field("values", Type.ARRAY, { type = Type.I16, count = 2 })
        }),
        D.ClientDB("SpecialRows", {}, { recordName = "SpecialRow" }),
        D.LuaEvent("LuaPayload",
        {
            D.Field("values", Type.VECTOR, { type = Type.U8 }),
            D.Field("hidden", Type.U32, { luaPush = false })
        }),
        D.GameCommand("CheckedCommand", { "Checked", "C" },
        {
            D.Field("count", Type.U8),
            D.Field("enabled", Type.BOOL)
        })
    }) })

    local firstGolden = EmitInMemory(goldenModel)
    local secondGolden = EmitInMemory(goldenModel)
    for path, output in pairs(firstGolden) do assert(output == secondGolden[path], "Repeated emission changed " .. path) end

    local header = firstGolden["memory/Tests/Tests.h"]
    AssertContains(header, "#include <Base/Types.h>\n#include <FileFormat/Novus/ClientDB/ClientDB.h>\n#include <array>\n")
    AssertContains(header, "string escaped = \"quote\\\" slash\\\\ line\\n\";")
    AssertContains(header, "failed |= !buffer->PutU16(serialized);")
    assert(not header:find("PutU32(runtimeOnly)", 1, true))
    assert(not header:find("runtimeOnly : ", 1, true))
    AssertContains(header, "enum class FirstEnum : u8\n    {\n        Invalid = 0,\n        Explicit = 7,\n        Alias = Explicit\n    };")
    AssertContains(header, "static constexpr u16 ENUM_ID = 0;")
    AssertContains(header, "static constexpr u16 ENUM_ID = 1;")
    AssertContains(header, "struct ClientRowsRecord")
    AssertContains(header, "static inline std::string NAME = \"ClientRows\";")
    AssertContains(header, "static constexpr u32 NAME_HASH = 103;")
    AssertContains(header, "struct SpecialRowRecord")
    AssertContains(header, "static inline std::string NAME = \"SpecialRows\";")
    AssertContains(header, "static constexpr u32 NAME_HASH = 105;")
    AssertContains(header, "::ClientDB::FieldType::u32")
    AssertContains(header, "::ClientDB::FieldType::i16, 2")
    AssertContains(header, "for (u32 i = 0; i < static_cast<u32>(values.size()); i++)")
    AssertContains(header, "lua_rawseti(state, -2, i + 1);")
    assert(not header:find("lua_pushstring(state, \"hidden\")", 1, true))
    AssertContains(header, "const auto parsed = std::stoull(value);")
    AssertContains(header, "}(parameters[0])")
    AssertContains(header, "std::in_range<u8>(parsed)")
    AssertContains(header, "value == \"1\" || value == \"true\"")

    local packetList = firstGolden["memory/PacketList.h"]
    AssertContains(packetList, "FirstPacket = 1,\n        SecondPacket = 2,\n        Count = 3")
    local enumTraits = firstGolden["memory/EnumTraits.h"]
    AssertContains(enumTraits, "using Meta = MetaGen::Tests::FirstEnumMeta;")
    AssertContains(enumTraits, "using Meta = MetaGen::Tests::SecondEnumMeta;")

    local cleanupGeneration = CppEmitter.Create()
    local cleanupClosed = false
    cleanupGeneration.writer.currentIndent = 3
    ExpectFailure("output cleanup", "emission failed", function()
        Output.Write(
        {
            writer = cleanupGeneration.writer,
            cpp = cleanupGeneration.cpp,
            openOutput = function()
                return { write = function() end, close = function() cleanupClosed = true end }
            end
        }, "memory/failure.h", function()
            cleanupGeneration.writer:AddIndent()
            error("emission failed")
        end)
    end)
    assert(cleanupClosed)
    assert(cleanupGeneration.writer.currentFile == nil and cleanupGeneration.writer.currentIndent == 3)

    print("MetaGen model tests passed")
end

TestSuite.Register("model", Run)

newaction
{
    trigger = "metagen-tests",
    description = "Run MetaGen model validation tests",
    execute = Run
}
