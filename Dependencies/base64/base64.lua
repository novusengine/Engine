local dep = Solution.Util.CreateDepTable("base64", {})

Solution.Util.CreateDep(dep.Name, dep.Dependencies, function()
    Solution.Util.SetIncludes(dep.Path)
end)