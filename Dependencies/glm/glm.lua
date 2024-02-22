local function Include()
    local includeDir = path.getabsolute("glm/", Engine.dependencyDir)
    AddIncludeDirs(includeDir)

    local defines = { "GLM_FORCE_LEFT_HANDED", "GLM_FORCE_DEPTH_ZERO_TO_ONE" }
    AddDefines(defines)
end

CreateDep("glm", Include)