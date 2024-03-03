Engine = { }
Engine.name = "Engine"
Engine.rootDir = ""
Engine.buildDir = ""
Engine.binDir = ""
Engine.isRoot = false

Engine.Init = function(self, rootDir, buildDir, binDir)
    print("-- Configuring (" .. self.name .. ") --\n")

    self.rootDir = rootDir
    self.buildDir = buildDir
    self.binDir = binDir .. "/" .. self.name

    local projectUtils = path.getabsolute("Premake/ProjectUtil.lua", self.rootDir)
    include(projectUtils)

    local buildSettings = path.getabsolute("Premake/BuildSettings.lua", self.rootDir)
    local silentFailOnDuplicateSetting = not self.isRoot
    InitBuildSettings(silentFailOnDuplicateSetting)
    include(buildSettings)

    print("\n-- Directory Info (" .. self.name .. ") --")
    print(" Root Directory : " .. self.rootDir)
    print(" Build Directory : " .. self.buildDir)
    print(" Bin Directory : " .. self.binDir)
    print("--\n")

    if self.isRoot then
        workspace (self.name)
            location (buildDir)
            configurations { "Debug", "RelDebug", "Release" }

            filter "system:Windows"
                system "windows"
                platforms "Win64"

            filter "system:Unix"
                system "linux"
                platforms "Linux"
    end

    local deps = path.getabsolute("Dependencies/Dependencies.lua", self.rootDir)
    local projects = path.getabsolute("Source/Projects.lua", self.rootDir)
    include(deps)
    include(projects)

    print("-- Done (" .. self.name .. ") --")
end

if HasRoot == nil then
    HasRoot = true

    local rootDir = path.getabsolute(".")
    local buildDir = path.getabsolute("Build/", rootDir)
    local binDir = path.getabsolute("Bin/", buildDir)

    Engine.isRoot = true
    Engine:Init(rootDir, buildDir, binDir)
end