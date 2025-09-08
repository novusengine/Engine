local currentProject = Solution.Projects.Current

local dependencies = { }
Solution.Util.CreateProject("Gen-Meta", "Utility", currentProject.BinDir, dependencies)
dependson { "Meta", "TypeGenerator", "TypeParser" }
if os.target() == "windows" then
    fastuptodate "Off"
end

local metaGenDir = currentProject.RootDir .. "/Source/Meta/Meta"
local typeGenFolder = ""
if currentProject.IsRoot then
    typeGenFolder = "%{wks.location}/bin/%{wks.name}/%{cfg.buildcfg}/TypeGenerator"
else
    typeGenFolder = "%{wks.location}/bin/%{wks.name}/Engine/%{cfg.buildcfg}/TypeGenerator"
end

local solutionType = BuildSettings:Get("Solution Type")
prebuildcommands
{
    "if exist \"" .. metaGenDir .. "/Generated\" rmdir /S /Q \"" .. metaGenDir .. "/Generated\"",
    
    "\"" .. typeGenFolder .. "\" " ..
    "\"" .. metaGenDir .. "/Source\" " ..
    "\"" .. metaGenDir .. "/Generated\" ",

    "cd " .. currentProject.RootDir,
    "premake5 " .. solutionType
}