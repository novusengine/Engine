local warningsAsErrors = BuildSettings:Get("Slang Warnings as Errors")
local basePath = path.getabsolute("slang/slang", Solution.Projects.Current.DependencyDir)

local Slang = { }

local function OverrideOptimizationSettings()
    filter "configurations:Debug"
        runtime "Release"
        symbols "Off"
        optimize "Full"
        defines { "NDEBUG", "NC_RELEASE"}

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

    filter { }
end

-- Deps
Slang.SetupAnkerl = function()
    local dep = Solution.Util.CreateDepTable("Slang-Ankerl", {})

    Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
        Solution.Util.SetIncludes(basePath .. "/external/unordered_dense/include/")
    end)
end
Slang.SetupMiniz = function()
    local dep = Solution.Util.CreateDepTable("Slang-Miniz", {})

    local sourceDir = basePath .. "/external/miniz"
    local includeDir = basePath .. "/external/miniz"

    Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
        OverrideOptimizationSettings()
        local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS", }

        Solution.Util.SetLanguage("C++")
        Solution.Util.SetCppDialect(20)

        local files =
        {
            sourceDir .. "/miniz.h",
            sourceDir .. "/miniz.c",
        }
        Solution.Util.SetFiles(files)
        Solution.Util.SetDefines(defines)
    end)

    Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetLinks(dep.Name)
    end)
end
Slang.SetupLZ4 = function()
    local dep = Solution.Util.CreateDepTable("Slang-LZ4", {})
    local sourceDir = basePath .. "/external/lz4/lib"
    local includeDir = basePath .. "/external/lz4/lib"

    Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
        OverrideOptimizationSettings()
        local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS", }

        Solution.Util.SetLanguage("C++")
        Solution.Util.SetCppDialect(20)

        local files =
        {
            sourceDir .. "/lz4.h",
            sourceDir .. "/lz4.c",
        }
        Solution.Util.SetFiles(files)
        Solution.Util.SetDefines(defines)
    end)

    Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetLinks(dep.Name)
    end)
end
Slang.SetupGlslang = function()
    local vkSdk = os.getenv("VULKAN_SDK")
    local includeDirs = {
        vkSdk .. "/Include",
        vkSdk .. "/Include/glslang"
    }
    local links = {
        "glslang"
    }

    local dep = Solution.Util.CreateDepTable("Slang-Glslang", {})

    Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
        Solution.Util.SetIncludes(includeDirs)
        Solution.Util.SetLibDirs(vkSdk .. "/Lib")
        Solution.Util.SetLinks(links)
    end)
end
Slang.SetupSlangGlslang = function()
    local dep = Solution.Util.CreateDepTable("Slang-Glslang-Wrapper", { "slang-glslang" })
    local sourceDir = basePath .. "/source/slang-glslang"
    local includeDir = basePath .. "/include"

    Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
        OverrideOptimizationSettings()
        local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS", }

        Solution.Util.SetLanguage("C++")
        Solution.Util.SetCppDialect(20)

        local files =
        {
            sourceDir .. "/*.h",
            sourceDir .. "/*.cpp",
        }
        Solution.Util.SetFiles(files)
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetDefines(defines)
    end)

    Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
        Solution.Util.SetLinks(dep.Name)
    end)
end
Slang.SetupSpirvHeaders = function()
    local dep = Solution.Util.CreateDepTable("Slang-SPIRV-Headers", {})
    local sourceDir = basePath .. "/external/spirv-headers/include"

    Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
        Solution.Util.SetIncludes(sourceDir)
    end)
end
-- Slang Core
Slang.SetupCore = function()
    local dep = Solution.Util.CreateDepTable("Slang-Core", { "slang-ankerl", "slang-miniz", "slang-lz4" })
    local sourceDir = basePath .. "/source/core"
    local includeDir = basePath .. "/include"

    Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
        OverrideOptimizationSettings()
        local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS", "SLANG_STATIC", "MINIZ_STATIC_DEFINE", "UNICODE" }

        Solution.Util.SetLanguage("C++")
        Solution.Util.SetCppDialect(20)

        local files =
        {
            sourceDir .. "/*.h",
            sourceDir .. "/*.cpp",
            sourceDir .. "/windows/*.cpp"
        }
        Solution.Util.SetFiles(files)
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetDefines(defines)
    end)

    Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetLinks(dep.Name)
    end)
end
Slang.SetupCompilerCore = function()
    local dep = Solution.Util.CreateDepTable("Slang-Compiler-Core", { "slang-core", "slang-spirv-headers" })
    local sourceDir = basePath .. "/source/compiler-core"
    local includeDir = basePath .. "/source"

    Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
        OverrideOptimizationSettings()
        local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

        Solution.Util.SetLanguage("C++")
        Solution.Util.SetCppDialect(20)

        local files =
        {
            sourceDir .. "/*.h",
            sourceDir .. "/*.cpp",
            sourceDir .. "/windows/*.cpp"
        }
        Solution.Util.SetFiles(files)
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetDefines(defines)
    end)

    Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
        Solution.Util.SetLinks(dep.Name)
    end)
end
Slang.SetupEmbeddedCoreModule = function()
    local dep = Solution.Util.CreateDepTable("Slang-Embedded-Core-Module", { "slang-core" })
    local sourceFile = basePath .. "/source/slang-core-module/slang-embedded-core-module.cpp"
    local includeDir = basePath .. "/generated"

    Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
        OverrideOptimizationSettings()
        local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS", "SLANG_STATIC", "SLANG_EMBED_CORE_MODULE" }

        Solution.Util.SetLanguage("C++")
        Solution.Util.SetCppDialect(20)

        Solution.Util.SetFiles(sourceFile)
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetDefines(defines)
    end)

    Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
        Solution.Util.SetLinks(dep.Name)
    end)
end
Slang.GenerateVersion = function()
    local inputVersionFile  = basePath .. "/../version.txt"
    local outputVersionHeader = basePath .. "/generated/slang-tag-version.h"

    local file = io.open(inputVersionFile, "r")
    if not file then
        error("Failed to open version.txt at: " .. inputVersionFile)
    end

    local version = file:read("*l")
    file:close()

    if not version or version == "" then
        error("version.txt is empty!")
    end

    os.mkdir(path.getdirectory(outputVersionHeader))

    local out = io.open(outputVersionHeader, "w+")
    if not out then
        error("Failed to open output header for writing: " .. outputVersionHeader)
    end

    out:write('#define SLANG_TAG_VERSION "' .. version .. '"\n')
    out:close()
end
-- Actual Slang library
Slang.SetupSlang = function()
    local dep = Solution.Util.CreateDepTable("Slang-Slang", { "slang-compiler-core", "slang-embedded-core-module" })
    local sourceDir = basePath .. "/source/slang"
    local replayDir = basePath .. "/source/slang-record-replay"
    local generatedDir = basePath .. "/source/generated"
    local includeDirs = {
        basePath .. "/generated",
        basePath .. "/source",
        basePath .. "/source/slang"
    }
    local exportIncludeDir = basePath .. "/include"

    Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
        OverrideOptimizationSettings()
        local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS", "SLANG_STATIC", "SLANG_EMBED_CORE_MODULE_SOURCE" }

        Solution.Util.SetLanguage("C++")
        Solution.Util.SetCppDialect(20)

        local files =
        {
            sourceDir .. "/**.h",
            sourceDir .. "/**.cpp",
            replayDir .. "/**.h",
            replayDir .. "/**.cpp",
            generatedDir .. "/**.h",
            generatedDir .. "/**.cpp",
            basePath .. "/source/slang-core-module/slang-embedded-core-module-source.cpp"
        }
        Solution.Util.SetFiles(files)
        Solution.Util.SetIncludes(includeDirs)
        Solution.Util.SetDefines(defines)
    end)

    Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
        Solution.Util.SetLinks(dep.Name)
        Solution.Util.SetIncludes(exportIncludeDir)
    end)
end

-- This function is intended to be run only once after upgrading Slang to clean up unneeded files from the Slang folder.
-- After running it once, remember to comment out the call to Slang.OneTimeCleanup() below.
Slang.OneTimeCleanup = function()
    -- Paths to delete inside slang/external/
    local externalToDelete = {
        "external/glm",
        "external/glslang",
        "external/glslang-generated",
        "external/imgui",
        "external/lua",
        "external/metal-cpp",
        "external/optix-dev",
        "external/slang-binaries",
        "external/slang-rhi",
        "external/spirv",
        "external/spirv-tools",
        "external/spirv-tools-generated",
        "external/stb",
        "external/tinyobjloader",
        "external/vulkan",
        "external/WindowsToolchain",
        -- unordered_dense cleanup (keep only include/)
        "external/unordered_dense/.clang-format",
        "external/unordered_dense/.clang-tidy",
        "external/unordered_dense/.fuzz-corpus-base-dir",
        "external/unordered_dense/.git",
        "external/unordered_dense/.github",
        "external/unordered_dense/.gitignore",
        "external/unordered_dense/cmake",
        "external/unordered_dense/CMakeLists.txt",
        "external/unordered_dense/CODE_OF_CONDUCT.md",
        "external/unordered_dense/CONTRIBUTING.md",
        "external/unordered_dense/data",
        "external/unordered_dense/doc",
        "external/unordered_dense/example",
        "external/unordered_dense/LICENSE",
        "external/unordered_dense/meson.build",
        "external/unordered_dense/README.md",
        "external/unordered_dense/scripts",
        "external/unordered_dense/src",
        "external/unordered_dense/subprojects",
        "external/unordered_dense/test"
    }

    -- Paths to delete (top-level folders, files, and source folders)
    local otherToDelete = {
        -- Top-level folders
        ".claude",
        ".github",
        ".reuse",
        "cmake",
        "core-module-meta",
        "docs",
        "examples",
        "extras",
        "prelude",
        "tests",
        "typings",
        -- Tools subfolders (keep tools/platform)
        "tools/benchmark",
        "tools/CMakeLists.txt",
        "tools/copy-hlsl-libs.bat",
        "tools/gfx",
        "tools/gfx-unit-test",
        "tools/render-test",
        "tools/slang-capability-generator",
        "tools/slang-cpp-parser",
        "tools/slang-embed",
        "tools/slang-fiddle",
        "tools/slang-generate",
        "tools/slang-lookup-generator",
        "tools/slang-profile",
        "tools/slang-reflection-test",
        "tools/slang-replay",
        "tools/slang-spirv-embed-generator",
        "tools/slang-test",
        "tools/slang-unit-test",
        "tools/slangc-test",
        "tools/slangd",
        "tools/slangi",
        "tools/test-process",
        "tools/test-server",
        "tools/unit-test",
        "tools/vk-pipeline-create",
        -- Top-level files
        ".clang-format",
        ".clangd",
        ".editorconfig",
        ".gitattributes",
        ".gitignore",
        ".gitmodules",
        ".lldbinit",
        ".mailmap",
        "CMakeLists.txt",
        "CMakePresets.json",
        "CODE_OF_CONDUCT.md",
        "CONTRIBUTING.md",
        "flake.lock",
        "flake.nix",
        "README.md",
        "slang-tag-version.h.in",
        "CLAUDE.local.md",
        "CLAUDE.md",
        -- Source folders
        "source/slangc"
    }

    local function deleteIfExists(p)
        if os.isdir(p) then
            print("Deleting directory: " .. p)
            os.rmdir(p)
        elseif os.isfile(p) then
            print("Deleting file: " .. p)
            os.remove(p)
        end
    end

    for _, item in ipairs(externalToDelete) do
        deleteIfExists(basePath .. "/" .. item)
    end

    for _, item in ipairs(otherToDelete) do
        deleteIfExists(basePath .. "/" .. item)
    end

    print("Slang cleanup complete!")
end
--Slang.OneTimeCleanup() -- Uncomment to run once and clean up the Slang folder

Solution.Util.SetGroup(Solution.DependencyGroup .. "/Slang")
Slang.SetupAnkerl()
Slang.SetupMiniz()
Slang.SetupLZ4()
Slang.SetupGlslang()
Slang.SetupSlangGlslang()
Slang.SetupCore()
Slang.SetupSpirvHeaders()
Slang.SetupCompilerCore()
Slang.SetupEmbeddedCoreModule()
Slang.GenerateVersion()
Slang.SetupSlang()
Solution.Util.SetGroup(Solution.DependencyGroup)