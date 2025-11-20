local basePath = path.getabsolute("recastnavigation", Solution.Projects.Current.DependencyDir)

local RecastNavigation = { }
RecastNavigation.SetupRecast = function()
    local dep = Solution.Util.CreateDepTable("RecastNavigation-Recast", { })
    local sourceDir = basePath .. "/Recast"

    Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
        local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS", "DT_POLYREF64" }

        Solution.Util.SetLanguage("C++")
        Solution.Util.SetCppDialect(17)

        local files =
        {
            sourceDir .. "/Recast/**.h",
            sourceDir .. "/Recast/**.c",
            sourceDir .. "/Recast/**.cpp"
        }
        Solution.Util.SetFiles(files)
        Solution.Util.SetIncludes(sourceDir)
        Solution.Util.SetDefines(defines)
    end)

    Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
        Solution.Util.SetIncludes(sourceDir)
        Solution.Util.SetLinks(dep.Name)
    end)
end
RecastNavigation.SetupDetour = function()
    local dep = Solution.Util.CreateDepTable("RecastNavigation-Detour", { })
    local sourceDir = basePath .. "/Detour"

    Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
        local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS", "DT_POLYREF64" }

        Solution.Util.SetLanguage("C++")
        Solution.Util.SetCppDialect(17)

        local files =
        {
            sourceDir .. "/Detour/**.h",
            sourceDir .. "/Detour/**.c",
            sourceDir .. "/Detour/**.cpp"
        }
        Solution.Util.SetFiles(files)
        Solution.Util.SetIncludes(sourceDir)
        Solution.Util.SetDefines(defines)
    end)

    Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
        Solution.Util.SetIncludes(sourceDir)
        Solution.Util.SetLinks(dep.Name)
        Solution.Util.SetDefines({"DT_POLYREF64"})
    end)
end

Solution.Util.SetGroup(Solution.DependencyGroup .. "/RecastNavigation")
RecastNavigation.SetupRecast()
RecastNavigation.SetupDetour()
Solution.Util.SetGroup(Solution.DependencyGroup)