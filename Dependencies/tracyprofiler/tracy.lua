local dep = Solution.Util.CreateDepTable("TracyProfiler", {})

Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    Solution.Util.SetLanguage("C++")
    Solution.Util.SetCppDialect(20)

    local sourceDir = dep.Path .. "/tracy/"
    
    local files =
    {
        path.getabsolute("tracy/Tracy.hpp", sourceDir),
        path.getabsolute("tracy/TracyVulkan.hpp", sourceDir),
        path.getabsolute("TracyClient.cpp", sourceDir)
    }

    Solution.Util.SetFiles(files)
    Solution.Util.SetIncludes(dep.Path)
    Solution.Util.SetDefines(defines)
    
    local isEnabled = BuildSettings:Get("Enable Tracy")
    if isEnabled then
        Solution.Util.SetDefines({ "TRACY_ENABLE" })
    end
end)

Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
    local sourceDir = dep.Path .. "/tracy/"

    Solution.Util.SetIncludes({ dep.Path, sourceDir })
    Solution.Util.SetLinks(dep.Name)

    local isEnabled = BuildSettings:Get("Enable Tracy")
    if isEnabled then
        Solution.Util.SetDefines({ "TRACY_ENABLE" })
    end
end)