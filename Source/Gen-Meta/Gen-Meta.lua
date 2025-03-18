local currentProject = Solution.Projects.Current

local dependencies = { }
Solution.Util.CreateProject("Gen-Meta", "Utility", currentProject.BinDir, dependencies)
dependson { "TypeGenerator" }
if os.target() == "windows" then
    fastuptodate "Off"
end

prebuildcommands
{
    "if exist \"" .. Solution.Projects.Current.RootDir .. "/Source/Meta/Meta/Generated\" rmdir /S /Q \"" .. Solution.Projects.Current.RootDir .. "/Source/Meta/Meta/Generated\"",
    
    "\"%{wks.location}/bin/%{wks.name}/Engine/%{cfg.buildcfg}/TypeGenerator\" " ..
    "\"" .. Solution.Projects.Current.RootDir .. "/Source/Meta/Meta/Source\" " ..
    "\"" .. Solution.Projects.Current.RootDir .. "/Source/Meta/Meta/Generated\" "
}