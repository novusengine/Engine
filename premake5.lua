Engine = { }
Engine.name = "Engine"
Engine.isRoot = false

Engine.Init = function(self, rootDir, buildDir, binDir)
    print("-- Configuring (" .. self.name .. ") --\n")

    self.rootDir = rootDir
    self.buildDir = buildDir
    self.binDir = binDir

    local projectUtils = path.getabsolute("ProjectUtil.lua", self.rootDir)
    include(projectUtils)

    local buildSettings = path.getabsolute("BuildSettings.lua", self.rootDir)
    local silentFailOnDuplicateSetting = not Engine.isRoot
    InitBuildSettings(silentFailOnDuplicateSetting)
    include(buildSettings)

    print("\n-- Directory Info (" .. self.name .. ") --")
    print(" Root Directory : " .. self.rootDir)
    print(" Build Directory : " .. self.buildDir)
    print(" Bin Directory : " .. self.binDir)
    print("--\n")

    if (Engine.isRoot) then
        workspace "Engine"
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