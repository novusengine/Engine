local mod = Solution.Util.CreateModuleTable("Scripting", { "base", "fileformat", "luau-compiler", "luau-vm", "luau-codegen" })

Solution.Util.CreateStaticLib(mod.Name, Solution.Projects.Current.BinDir, mod.Dependencies, function()
    local defines = { "_CRT_SECURE_NO_WARNINGS", "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    Solution.Util.SetLanguage("C++")
    Solution.Util.SetCppDialect(20)

    local projFile = mod.Path .. "/" .. mod.Name .. ".lua"
    local files = Solution.Util.GetFilesForCpp(mod.Path)
    table.insert(files, projFile)

    Solution.Util.SetFiles(files)
    Solution.Util.SetIncludes(mod.Path)
    Solution.Util.SetDefines(defines)

    vpaths {
        ["/*"] = { "*.lua", mod.Name .. "/**" }
    }
end)

Solution.Util.CreateDep(mod.NameLow, mod.Dependencies, function()
    Solution.Util.SetIncludes(mod.Path)
    Solution.Util.SetLinks(mod.Name)
end)