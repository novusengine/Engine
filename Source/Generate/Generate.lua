local currentProject = Solution.Projects.Current

local dependencies = { }
Solution.Util.CreateProject("Generate", "StaticLib", currentProject.BinDir, dependencies)

-- TODO: This needs to be fixed to work properly on Linux. Dirty fix to make runner build properly.
if os.target() == "windows" then
    local solutionType = BuildSettings:Get("Solution Type")
    postbuildcommands
    {
        "cd " .. currentProject.RootDir,
        "premake5 " .. solutionType
    }
end