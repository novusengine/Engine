local dep = Solution.Util.CreateDepTable("glm", {})

Solution.Util.CreateDep(dep.Name, dep.Dependencies, function()
    local defines = { "GLM_FORCE_LEFT_HANDED", "GLM_FORCE_DEPTH_ZERO_TO_ONE" }

    Solution.Util.SetIncludes(dep.Path)
    Solution.Util.SetDefines(defines)
end)