local UTIL_VERSION = 1

local function Print(msg)
    print(msg)
end

local function PrintError(msg)
    local debugInfo = debug.getinfo(2)
    if not debugInfo then
        return
    end

    local callerName = debugInfo.name
    error("[" .. callerName .. "]" .. " : " .. msg, 2)
end

--[[ DumpObject(object, [limit], [indent])   Recursively print arbitrary data. 
	Set limit (default 100) to stanch infinite loops.
	Indents tables as [KEY] VALUE, nested tables as [KEY] [KEY]...[KEY] VALUE
	Set indent ("") to prefix each line:    Mytable [KEY] [KEY]...[KEY] VALUE
--]]
local function DumpObject(object, l, i)
	l = (l) or 100; i = i or "";	-- default item limit, indent string
	if (l<1) then PrintError "ERROR: Item limit reached."; return l-1 end;
	local ts = type(object);
	if (ts ~= "table") then Print (i,ts,object); return l-1 end
	print (i,ts);           -- print "table"
	for k,v in pairs(object) do  -- print "[KEY] VALUE"
		l = DumpObject(v, l, i.."\t["..tostring(k).."]");
		if (l < 0) then break end
	end
	return l
end

if Solution.Util then
    if Solution.Util.Version >= UTIL_VERSION then
        return
    end
end

systemToExecutableExtensionMap =
{
    windows = "exe",
    linux = "sh"
}

systemToDynamicLibExtensionMap =
{
    windows = "dll",
    linux = "so"
}

Solution.Util = {}
Solution.Util.Version = UTIL_VERSION
Solution.Util.ActiveProject = ""

Solution.Util.Print = Print
Solution.Util.DumpObject = DumpObject
Solution.Util.PrintError = PrintError

Solution.Util.Create = function(identifier, root)
    Solution.NumProjects = Solution.NumProjects + 1

    Solution.Projects[identifier] = 
    {
        Index = Solution.NumProjects,
        Name = identifier,
        BaseDir = "",
        BinDir = "",
        IsRoot = root
    }

    return Solution.Projects[identifier]
end

Solution.Util.CreateModuleTable = function(name, dependencies)
    local module = { Name = name }

    module.NameLow = string.lower(module.Name)
    module.Path = path.getabsolute(module.Name .. "/", Solution.Projects.Current.ModulesDir)
    module.Dependencies = dependencies

    return module
end

Solution.Util.CreateDepTable = function(name, dependencies)
    local dependency = { Name = name }

    dependency.NameLow = string.lower(dependency.Name)
    dependency.Path = path.getabsolute(dependency.NameLow .. "/", Solution.Projects.Current.DependencyDir)
    dependency.Dependencies = dependencies

    return dependency
end

Solution.Util.IncludeSubmodule = function(name, rootDir, binDir)
    local submoduleRootDir = path.getabsolute("Submodules/".. name .. "/", rootDir)
    local submoduleBuildDir = path.getabsolute("Build/".. name .. "/", rootDir)
    local submodulePremakeFile = path.getabsolute("premake5.lua", submoduleRootDir)

    include(submodulePremakeFile)

    Solution.Projects[name]:Init(submoduleRootDir, submoduleBuildDir, binDir)
end

Solution.Util.CreateProject = function(name, projectType, binDir, dependencies, callback)
    local internalName = "Project-" .. name
    if _G[internalName] ~= nil then
        Solution.Util.PrintError("Project with name '" .. name .. "' already exists")
    end

    Solution.Util.Print("Creating Project '" .. name .. "'")
    Solution.Util.ActiveProject = name

    local projectTable = { }
    projectTable.deps = {}

    -- Default Args here --
    dependencies = dependencies or {}

    local resolvedDependencies = { }
    local needToResolve = true

    for _, v in pairs(dependencies) do
        projectTable.deps[v] = 
        {
            deps = {}
        }

        resolvedDependencies[v] =
        {
            parent = projectTable.deps[v]
        }
    end

    while needToResolve do
        local numAddedDependencies = 0

        for k, v in pairs(resolvedDependencies) do
            local depInternalName = "Dependency-" .. k

            local dep = _G[depInternalName]
            if (dep == nil) then
                Solution.Util.PrintError("'" .. name .. "' use undeclared dependency '" .. k .. "'")
            end

            if dep.Dependencies then
                local deps = nil

                if type(dep.Dependencies) == "function" then
                    deps = dep.Dependencies()
                elseif type(dep.Dependencies) == "table" or type(dep.Dependencies) == "string" then
                    deps = dep.Dependencies
                end

                if deps then
                    for _, newDep in pairs(deps) do
                        if not resolvedDependencies[newDep] then
                            v.parent.deps[newDep] =
                            {
                                deps = {}
                            }

                            resolvedDependencies[newDep] =
                            {
                                parent = v.parent.deps[newDep]
                            }

                            numAddedDependencies = numAddedDependencies + 1
                        end
                    end
                end
            end
        end

        needToResolve = numAddedDependencies > 0
    end
    
    project (name)
        kind (projectType)
        targetdir (binDir .. "/%{cfg.buildcfg}")

        characterset ("ASCII")
        editandcontinue "Off"

        filter "configurations:Debug"
            runtime "Debug"
            symbols "On"
            defines { "_DEBUG", "NC_DEBUG" }

        filter "configurations:RelDebug"
            runtime "Release"
            symbols "On"
            optimize "On"
            defines { "NDEBUG", "NC_RELEASE"}

        filter "configurations:Release"
            runtime "Release"
            symbols "Off"
            optimize "Full"
            defines { "NDEBUG", "NC_RELEASE"}

        filter "platforms:Win64"
            system "Windows"
            architecture "x86_64"
            defines { "WIN32", "WINDOWS" }

        filter { }

    for k, v in pairs(resolvedDependencies) do
        local depInternalName = "Dependency-" .. k
        if (_G[depInternalName].Callback ~= nil) then
            _G[depInternalName].Callback()
            filter {}
        end
    end

    local isMSVC = BuildSettings:Get("Using MSVC")
    local multithreadedCompilation = BuildSettings:Get("Multithreaded Compilation")
    local multithreadedCoreCount = BuildSettings:Get("Multithreaded Core Count")

    if multithreadedCompilation then
        if isMSVC then
            local cores = multithreadedCoreCount or 0
            if cores > 0 then
                buildoptions { "/MP" .. tostring(cores) }
            else
                buildoptions { "/MP" }
            end
        end
    end

    -- START TEST ONLY START --
    --DumpObject(projectTable)
    -- END TEST ONLY END ---

    _G[internalName] = projectTable

    if callback then
        callback()
    end
end

Solution.Util.CreateStaticLib = function(name, binDir, dependencies, callback)
    Solution.Util.CreateProject(name, "StaticLib", binDir, dependencies, callback)
end

Solution.Util.CreateDynamicLib = function(name, binDir, dependencies, callback)
    Solution.Util.CreateProject(name, "DynamicLib", binDir, dependencies, callback)
end

Solution.Util.CreateConsoleApp = function(name, binDir, dependencies, callback)
    Solution.Util.CreateProject(name, "ConsoleApp", binDir, dependencies, callback)
end

Solution.Util.CreateDep = function(name, dependencies, callback)
    local internalName = "Dependency-" .. name
    if _G[internalName] ~= nil then
        Solution.Util.PrintError("Dependency with name '" .. name .. "' already exists")
    end

    Solution.Util.Print("Creating Dependency '" .. name .. "'")
    
    -- Default Args here --
    dependencies = dependencies or {}

    if type(callback) ~= "function" then
        Solution.Util.PrintError("Dependency '" .. name .. "' specified incorrect callback parameter type. Type must be 'function'")
    end

    local dependencyTable = 
    {
        Callback = callback,
        Dependencies = dependencies
    }

    _G[internalName] = dependencyTable
end

Solution.Util.SetLanguage = function(lang)
    if lang ~= "C" and lang ~= "C++" then
        Solution.Util.PrintError("Attempted to set unsupported language '" .. lang .. "' for project '" .. Solution.Util.ActiveProject .. "'. Supported Languages ('C', 'C++')")
    end

    language (lang)
end

Solution.Util.SetCppDialect = function(dialect)
    local cppVersion = dialect
    
    if cppVersion == nil or cppVersion == "" then
        cppVersion = "C++20"
    else
        local version = tonumber(cppVersion)

        if version ~= nil then
            cppVersion = tostring(version)
        end

        if cppVersion == "03" then
            cppVersion = "C++03"
        elseif cppVersion == "11" then
            cppVersion = "C++11"
        elseif cppVersion == "14" then
            cppVersion = "C++14"
        elseif cppVersion == "17" then
            cppVersion = "C++17"
        elseif cppVersion == "20" then
            cppVersion = "C++20"
        elseif cppVersion == "23" then
            cppVersion = "C++23"
        end

        if not cppVersion:find("^C++") then
            cppVersion = "C++20"
        end
    end

    cppdialect (cppVersion)
end

Solution.Util.SetFiles = function(filesToAdd)
    files (filesToAdd)
end

Solution.Util.SetIncludes = function(includesToAdd)
    includedirs (includesToAdd)
end

Solution.Util.SetDefines = function(definesToAdd)
    defines (definesToAdd)
end

Solution.Util.SetLinks = function(linksToAdd)
    links (linksToAdd)
end

Solution.Util.GetFilesForCpp = function(basePath)
    local files = 
    {
        (basePath .. "/**.h"),
        (basePath .. "/**.hpp"),
        (basePath .. "/**.c"),
        (basePath .. "/**.cpp"),
    }

    return files
end

Solution.Util.SetGroup = function(name)
    local currentProject = Solution.Projects.Current

    local projectIdentifier = currentProject.Index .. ". " .. currentProject.Name
    if not name or name == "" then
        Solution.ActiveGroup = projectIdentifier
    else
        Solution.ActiveGroup = projectIdentifier .. "/" .. name
    end

    group (Solution.ActiveGroup)
end

Solution.Util.SetGroupRaw = function(name)
    Solution.ActiveGroup = name
    group (Solution.ActiveGroup)
end

Solution.Util.SetFilter = function(value, callback)
    filter(value)

    if callback then
        callback()
        Solution.Util.ClearFilter()
    end
end

Solution.Util.ClearFilter = function()
    filter { }
end

if InitBuildSettings == nil then
    function InitBuildSettings(silentFailOnDuplicateSetting)
        if BuildSettings == nil then
            BuildSettings = { }

            BuildSettings.Has = function(settings, name)
                return settings[name] ~= nil
            end

            BuildSettings.Add = function(settings, name, value)
                local silentFail = settings:ShouldSilentFailOnDuplicateSetting()

                if settings:Has(name) then
                    if silentFail then
                        return
                    end

                    Solution.Util.PrintError("Tried to call BuildSettings:Add with name '" .. name .. '" but a setting with that name already exists')
                end

                settings[name] = value
            end

            BuildSettings.Get = function(settings, name)
                if not settings:Has(name) then
                    return nil
                end

                return settings[name]
            end

            BuildSettings.SetSilentFailOnDuplicateSetting = function(settings, silentFailOnDuplicateSetting)
                BuildSettings["SilentFailOnDuplicateSetting"] = silentFailOnDuplicateSetting
            end

            BuildSettings.ShouldSilentFailOnDuplicateSetting = function(settings)
                local option = BuildSettings["SilentFailOnDuplicateSetting"]

                if option == nil then
                    return false
                end

                return option
            end
        end

        BuildSettings:SetSilentFailOnDuplicateSetting(silentFailOnDuplicateSetting)
    end
end