local dependencies = { "vulkan" }

local function SetupLib()
    local basePath = path.getabsolute("FidelityFX-SDK/", Engine.dependencyDir)
    local dependencies = { "vulkan" }
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    ProjectTemplate("FidelityFX", "StaticLib", nil, Engine.binDir, dependencies, defines)

    local sourceDir = path.getabsolute("FidelityFX/sdk/src", basePath)
    local includeDir = 
    {
        path.getabsolute("FidelityFX/sdk/include/", basePath),
        path.getabsolute("FidelityFX/sdk/permutations/", basePath),
        path.getabsolute("FidelityFX/sdk/src/components", basePath),
        path.getabsolute("FidelityFX/sdk/src/backends/shared", basePath),
        path.getabsolute("FidelityFX/sdk/src/shared", basePath),
    }

    local files =
    {
        sourceDir .. "/**.cpp",
        sourceDir .. "/**.h",
        basePath .. "/FidelityFX/sdk/include/**.h"
    }
    AddFiles(files)
    AddIncludeDirs(includeDir)

    AddDefines("FFX_CACAO")

end
SetupLib()

local function Include()
    local basePath = path.getabsolute("FidelityFX-SDK/", Engine.dependencyDir)
    local includeDir = {
        path.getabsolute("FidelityFX/sdk/include/", basePath),
        path.getabsolute("FidelityFX/sdk/src/backends/shared", basePath),
    }
    AddIncludeDirs(includeDir)

    AddDefines("FFX_CACAO")

    AddLinks("FidelityFX")
end
CreateDep("fidelityfx", Include, dependencies)