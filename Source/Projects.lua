-- Engine Projects
Engine.projectsDir = path.getabsolute("Source/", Engine.rootDir)

print("-- Creating Modules --")

if (Engine.isRoot) then
    group "[Build System]"
    include("Generate/Generate.lua")
end

group "Engine/[Modules]"
include("Base/Base.lua")
include("Network/Network.lua")
include("FileFormat/FileFormat.lua")
include("Input/Input.lua")
include("ShaderCooker/ShaderCooker.lua")
include("Renderer/Renderer.lua")
include("UnitTests/UnitTests.lua")
group "Engine"
print("-- Finished with Modules --\n")