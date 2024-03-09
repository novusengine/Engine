local function Include()
    local includeDir = path.getabsolute("typesafe/include", Engine.dependencyDir)
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
end

CreateDep("typesafe", Include)