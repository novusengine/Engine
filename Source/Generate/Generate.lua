local dependencies = { }
local defines = { }
ProjectTemplate("Generate", "StaticLib", ".", Engine.binDir, dependencies, defines)

local solutionType = BuildSettings:Get("Solution Type")
postbuildcommands
{
    "cd " .. Engine.rootDir,
    "premake5 " .. solutionType
}