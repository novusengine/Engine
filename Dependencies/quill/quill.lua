local dep = Solution.Util.CreateDepTable("Quill", {})

Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
    Solution.Util.SetIncludes(dep.Path)
    Solution.Util.SetDefines({"_CRT_SECURE_NO_WARNINGS"})
end)