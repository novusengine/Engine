-- Modules
Solution.Util.Print("-- Creating Modules --")
Solution.Util.ClearFilter()

Solution.Util.SetGroupRaw(Solution.BuildSystemGroup)
if Solution.Projects.Current.IsRoot then
    include("Generate/Generate.lua")
end
include("Gen-Meta/Gen-Meta.lua")

Solution.Util.SetGroup(Solution.ModuleGroup)
local modules =
{
    "Base/Base.lua",
    "FileFormat/FileFormat.lua",
    "Scripting/Scripting.lua",
    "Meta/Meta.lua",
    "Network/Network.lua",
    "Input/Input.lua",
    "Gameplay/Gameplay.lua",
    "ShaderCooker/ShaderCooker.lua",
    "Renderer/Renderer.lua",
    "TypeParser/TypeParser.lua",
    "TypeGenerator/TypeGenerator.lua",
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