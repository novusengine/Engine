local function Win64Link()
    local links =
    {
        path.getabsolute("dxcompiler/lib/windows/dxcompiler.lib", Engine.dependencyDir)
    }

    AddLinks(links)
end
local function LinuxLink()
    local links =
    {
        path.getabsolute("dxcompiler/lib/linux/dxcompiler.so", Engine.dependencyDir)
    }

    AddLinks(links)
end

local function Include()
    local includeDir = path.getabsolute("dxcompiler/include", Engine.dependencyDir)
    AddIncludeDirs(includeDir)

    filter "platforms:Win64"
        Win64Link()

    filter "platforms:Linux"
        LinuxLink()
end

CreateDep("dxcompiler", Include)

if os.istarget("windows") then
    local libPath = path.getabsolute("dxcompiler/lib/windows/dxcompiler.dll", Engine.dependencyDir)
    BuildSettings:Add("DXCompiler Dynamic Lib Path", libPath)
else
    local libPath = path.getabsolute("dxcompiler/lib/linux/dxcompiler.so", Engine.dependencyDir)
    BuildSettings:Add("DXCompiler Dynamic Lib Path", libPath)
end