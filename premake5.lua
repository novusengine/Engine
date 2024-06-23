Solution = Solution or
{
    NumProjects = 0,
    Projects = {},

    ActiveGroup = "",
    BuildSystemGroup = "0. [Build System]",
    ModuleGroup = "0. [Modules]",
    TestGroup = "1. [Tests]",
    DependencyGroup = "2. [Dependencies]"
}

local projectUtils = "Premake/ProjectUtil.lua"
include(projectUtils)

local projectName = "Engine"
local projectIsRoot = Solution.NumProjects == 0
local project = Solution.Util.Create(projectName, projectIsRoot)
local submodules = { }

project.Init = function(self, rootDir, buildDir, binDir)
    Solution.Util.Print("-- Initializing (" .. self.Name .. ") --\n")

    self.RootDir = rootDir
    self.BuildDir = buildDir
    self.BinDir = binDir .. "/" .. self.Name

    self.DependencyDir = path.getabsolute("Dependencies/", self.RootDir)
    self.ModulesDir = path.getabsolute("Source/", self.RootDir)
    
    local buildSettings = path.getabsolute("Premake/BuildSettings.lua", self.RootDir)
    local silentFailOnDuplicateSetting = not self.IsRoot
    InitBuildSettings(silentFailOnDuplicateSetting)
    include(buildSettings)

    if self.IsRoot then
        workspace (self.Name)
            location (self.BuildDir)
            configurations { "Debug", "RelDebug", "Release" }

            Solution.Util.SetFilter("system:Windows", function()
                system "windows"
                platforms "Win64"
            end)

            Solution.Util.SetFilter("system:Unix", function()
                system "linux"
                platforms "Linux"
            end)
    end

    for _, v in pairs(submodules) do
        Solution.Util.Print("-- Initializing Submodule : " .. v)
        Solution.Util.IncludeSubmodule(v, self.RootDir, self.BinDir)
    end

    Solution.Util.Print("\n-- Directory Info (" .. self.Name .. ") --")
    Solution.Util.Print(" Root Directory : " .. self.RootDir)
    Solution.Util.Print(" Build Directory : " .. self.BuildDir)
    Solution.Util.Print(" Bin Directory : " .. self.BinDir)
    Solution.Util.Print("--\n")

    Solution.Projects.Current = project

    local deps = path.getabsolute("Dependencies/Dependencies.lua", self.RootDir)
    local projects = path.getabsolute("Source/Modules.lua", self.RootDir)
    include(deps)
    include(projects)

    Solution.Util.Print("-- Done (" .. self.Name .. ") --")
end

if projectIsRoot then
    local rootDir = path.getabsolute(".")
    local buildDir = path.getabsolute("Build/", rootDir)
    local binDir = path.getabsolute("Bin/", buildDir)

    project:Init(rootDir, buildDir, binDir)
end