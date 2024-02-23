local function SetupLib()
    local basePath = path.getabsolute("typesafe/", Engine.dependencyDir)
    local dependencies = { }
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    ProjectTemplate("Typesafe", "StaticLib", nil, Engine.binDir, dependencies, defines)

    local sourceDir = path.getabsolute("include/type_safe", basePath)
    local includeDir = path.getabsolute("include/", basePath)
    local files =
    {
        -- Detail Header Files
        sourceDir .. "/detail/aligned_union.hpp",
        sourceDir .. "/detail/all_of.hpp",
        sourceDir .. "/detail/assert.hpp",
        sourceDir .. "/detail/assign_or_construct.hpp",
        sourceDir .. "/detail/constant_parser.hpp",
        sourceDir .. "/detail/copy_move_control.hpp",
        sourceDir .. "/detail/force_inline.hpp",
        sourceDir .. "/detail/is_nothrow_swappable.hpp",
        sourceDir .. "/detail/map_invoke.hpp",
        sourceDir .. "/detail/variant_impl.hpp",

        -- Typesafe Header Files
        sourceDir .. "/config.hpp",
        sourceDir .. "/arithmetic_policy.hpp",
        sourceDir .. "/boolean.hpp",
        sourceDir .. "/bounded_type.hpp",
        sourceDir .. "/compact_optional.hpp",
        sourceDir .. "/constrained_type.hpp",
        sourceDir .. "/deferred_construction.hpp",
        sourceDir .. "/downcast.hpp",
        sourceDir .. "/flag.hpp",
        sourceDir .. "/flag_set.hpp",
        sourceDir .. "/floating_point.hpp",
        sourceDir .. "/index.hpp",
        sourceDir .. "/integer.hpp",
        sourceDir .. "/narrow_cast.hpp",
        sourceDir .. "/optional.hpp",
        sourceDir .. "/optional_ref.hpp",
        sourceDir .. "/output_parameter.hpp",
        sourceDir .. "/reference.hpp",
        sourceDir .. "/strong_typedef.hpp",
        sourceDir .. "/tagged_union.hpp",
        sourceDir .. "/types.hpp",
        sourceDir .. "/variant.hpp",
        sourceDir .. "/visitor.hpp"
    }
    AddFiles(files)
    AddIncludeDirs(includeDir)

    local enableAsserts = BuildSettings:Get("Typesafe Enable Assertions")
    local enablePreconditionChecks = BuildSettings:Get("Typesafe Enable Precondition Checks")
    local enableWrapper = BuildSettings:Get("Typesafe Enable Wrapper")
    local arithmeticPolicy = BuildSettings:Get("Typesafe Arithmetic Policy")
    local defines =
    {
        "TYPE_SAFE_ENABLE_ASSERTIONS=" .. tostring(enableAsserts),
        "TYPE_SAFE_ENABLE_PRECONDITION_CHECKS=" .. tostring(enablePreconditionChecks),
        "TYPE_SAFE_ENABLE_WRAPPER=" .. tostring(enableWrapper),
        "TYPE_SAFE_ARITHMETIC_POLICY=" .. tostring(airthemticPolicy),
    }
    AddDefines(defines)

    AddLinks("debug_assert")
end
SetupLib()

local function Include()
    local includeDir = path.getabsolute("typesafe/include", Engine.dependencyDir)
    AddIncludeDirs(includeDir)

    AddLinks("Typesafe")
end

CreateDep("typesafe", Include)