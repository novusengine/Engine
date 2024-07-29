local dep = Solution.Util.CreateDepTable("FidelityFX", { "vulkan" })

Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS", "FFX_CACAO" }

    Solution.Util.SetLanguage("C++")
    Solution.Util.SetCppDialect(20)

    local basePath = dep.Path .. "/" .. dep.Name
    local sourceDir = basePath .. "/sdk/src"
    local includeDirs =
    {
        basePath .. "/sdk/include",
        basePath .. "/sdk/permutations",
        basePath .. "/sdk/src/components",
        basePath .. "/sdk/src/backends/shared",
        basePath .. "/sdk/src/shared",
    }

    local files =
    {
        sourceDir .. "/**.cpp",
        sourceDir .. "/**.h",
        basePath .. "/sdk/include/**.h"
    }

    Solution.Util.SetFiles(files)
    Solution.Util.SetIncludes(includeDirs)
    Solution.Util.SetDefines(defines)
        
    Solution.Util.SetFilter("system:linux", function()
        Solution.Util.SetDefines("FFX_GCC")
    end)
end)

Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
    local basePath = dep.Path .. "/" .. dep.Name
    Solution.Util.SetIncludes({ basePath .. "/sdk/include", basePath .. "/sdk/src/backends/shared" })
    Solution.Util.SetDefines("FFX_CACAO")
    Solution.Util.SetLinks(dep.Name)

    Solution.Util.SetFilter("system:linux", function()
        Solution.Util.SetDefines("FFX_GCC")
    end)
end)
