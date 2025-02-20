-- Modules
Solution.Util.Print("-- Creating Modules --")
Solution.Util.ClearFilter()

if Solution.Projects.Current.IsRoot then
    Solution.Util.SetGroupRaw(Solution.BuildSystemGroup)
    include("Generate/Generate.lua")
end

Solution.Util.SetGroup(Solution.ModuleGroup)
local modules =
{
    "Base/Base.lua",
    "Network/Network.lua",
    "FileFormat/FileFormat.lua",
    "Input/Input.lua",
    "Gameplay/Gameplay.lua",
    "ShaderCooker/ShaderCooker.lua",
    "Renderer/Renderer.lua",
    "Audio/Audio.lua"
}

for _, v in pairs(modules) do
    include(v)
    Solution.Util.ClearFilter()
end

Solution.Util.SetGroup(Solution.TestGroup)
local tests =
{
    "Engine-Tests/Engine-Tests.lua"
}

for _, v in pairs(tests) do
    include(v)
    Solution.Util.ClearFilter()
end

Solution.Util.SetGroup("")
Solution.Util.Print("-- Finished with Modules --\n")