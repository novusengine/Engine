local currentProject = Solution.Projects.Current

local dependencies = { }
Solution.Util.CreateProject("Generate", "StaticLib", currentProject.BinDir, dependencies)

local solutionType = BuildSettings:Get("Solution Type")
postbuildcommands
{
    "cd " .. currentProject.RootDir,
    "premake5 " .. solutionType
}