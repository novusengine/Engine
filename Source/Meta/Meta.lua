local mod = Solution.Util.CreateModuleTable("Meta", { "base", "fileformat", "luau-compiler", "luau-vm" })

include(mod.Path .. '/Profile')
MetaGen.RegisterProvider {
    name = "Engine.ClientDB",
    definitionRoot = mod.Path .. "/Definitions/Shared/ClientDB",
    namespace = "MetaGen.Shared.ClientDB"
}
MetaGen.RegisterProvider {
    name = "Engine.Gameplay",
    sources = {
        { root = mod.Path .. "/Definitions/Shared/ProximityTrigger", namespace = "MetaGen.Shared.ProximityTrigger" },
        { root = mod.Path .. "/Definitions/Shared/Spell", namespace = "MetaGen.Shared.Spell" },
        { root = mod.Path .. "/Definitions/Shared/Unit", namespace = "MetaGen.Shared.Unit" }
    }
}
MetaGen.RegisterProvider {
    name = "Engine.Protocol",
    sources = {
        { root = mod.Path .. "/Definitions/Shared/Cheat", namespace = "MetaGen.Shared.Cheat" },
        { root = mod.Path .. "/Definitions/Shared/CombatLog", namespace = "MetaGen.Shared.CombatLog" },
        { root = mod.Path .. "/Definitions/Shared/NetField", namespace = "MetaGen.Shared.NetField" },
        { root = mod.Path .. "/Definitions/Shared/Packet", namespace = "MetaGen.Shared.Packet" }
    },
    dependencies = { "Engine.Gameplay" }
}
local originalPackagePath = package.path
MetaGen.Finalize(function()
    include(mod.Path .. '/Init')
end)
package.path = originalPackagePath

Solution.Util.CreateStaticLib(mod.Name, Solution.Projects.Current.BinDir, mod.Dependencies, function()
    local defines = { "_CRT_SECURE_NO_WARNINGS", "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    Solution.Util.SetLanguage("C++")
    Solution.Util.SetCppDialect(20)

    local generatedRoot = MetaGen.GetGeneratedIncludeRoot()
    local activeProfile = MetaGen.ResolveProject()
    local files = Solution.Util.GetFilesForCpp(generatedRoot .. "/MetaGen")
    table.insert(files, mod.Path .. "/" .. mod.Name .. ".lua")
    for _, provider in ipairs(activeProfile.providers) do
        for _, source in ipairs(provider.sources) do
            for _, file in ipairs(os.matchfiles(source.root .. "/**.lua")) do table.insert(files, file) end
        end
        for _, extension in ipairs(provider.extensions) do table.insert(files, extension) end
    end
    
    Solution.Util.SetFiles(files)
    Solution.Util.SetIncludes(generatedRoot)
    Solution.Util.SetDefines(defines)

    vpaths {
        ["/*"] = { "Meta.lua" },
        ["MetaGen/*"] = { generatedRoot .. "/MetaGen/**" },
        ["Definitions/*"] = { "Definitions/**.lua" }
    }
end)

Solution.Util.CreateDep(mod.NameLow, mod.Dependencies, function()
    Solution.Util.SetIncludes(MetaGen.GetGeneratedIncludeRoot())
end)
