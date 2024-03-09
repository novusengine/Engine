-- Projects
Engine.projectsDir = path.getabsolute("Source/", Engine.rootDir)

print("-- Creating Modules --")

if (Engine.isRoot) then
    group "[Build System]"
    filter { }
    include("Generate/Generate.lua")
end

group (Engine.name .. "/[Modules]")
local modules =
{
    "Base/Base.lua",
    "Network/Network.lua",
    "FileFormat/FileFormat.lua",
    "Input/Input.lua",
    "ShaderCooker/ShaderCooker.lua",
    "Renderer/Renderer.lua",
    "UnitTests/UnitTests.lua",
}

for k,v in pairs(modules) do
    filter { }
    include(v)
end

filter { }
group (Engine.name)

print("-- Finished with Modules --\n")