local mod = Solution.Util.CreateModuleTable("Meta", { "base", "fileformat", "luau-compiler", "luau-vm" })

local originalPackagePath = package.path
include(mod.Path .. '/Init')
package.path = originalPackagePath

Solution.Util.CreateStaticLib(mod.Name, Solution.Projects.Current.BinDir, mod.Dependencies, function()
    local defines = { "_CRT_SECURE_NO_WARNINGS", "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    Solution.Util.SetLanguage("C++")
    Solution.Util.SetCppDialect(20)

    local projFile = mod.Path .. "/" .. mod.Name .. ".lua"
    local files = Solution.Util.GetFilesForCpp(mod.Path .. "/MetaGen")
    table.insert(files, projFile)
    
    Solution.Util.SetFiles(files)
    Solution.Util.SetIncludes(mod.Path)
    Solution.Util.SetDefines(defines)

    vpaths {
        ["/*"] = { "*.lua", "MetaGen/**" }
    }
end)

Solution.Util.CreateDep(mod.NameLow, mod.Dependencies, function()
    Solution.Util.SetIncludes(mod.Path)
end)