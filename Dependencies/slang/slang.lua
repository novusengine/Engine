local function getSlangInfo()
    if os.target() == "linux" then
        local includeDirs = {}
        local libDirs = {}
        local runtimeLibraries = {}

        if os.host() == "linux" then
            local slangSDK = os.getenv("SLANG_SDK") or os.getenv("VULKAN_SDK")
            local includeDir
            local libDir

            if slangSDK then
                includeDir = slangSDK .. "/include"
                libDir = slangSDK .. "/lib"

                if not os.isfile(includeDir .. "/slang/slang.h") then
                    Solution.Util.PrintError(
                        "The configured SDK does not contain Slang headers: '" .. includeDir .. "'."
                    )
                end

                if not os.isfile(libDir .. "/libslang.so") then
                    Solution.Util.PrintError(
                        "The configured SDK does not contain the Slang library: '" .. libDir .. "'."
                    )
                end
            else
                includeDir = os.findheader("slang/slang.h")
                libDir = os.findlib("slang")
            end

            if not includeDir then
                Solution.Util.PrintError(
                    "Failed to find the Slang headers in the system include paths. " ..
                    "Source the Vulkan SDK's setup-env.sh, set SLANG_SDK, or install Slang system-wide."
                )
            end

            if not libDir then
                Solution.Util.PrintError(
                    "Failed to find the Slang shared library in the system library paths. " ..
                    "Source the Vulkan SDK's setup-env.sh, set SLANG_SDK, or install Slang system-wide."
                )
            end

            table.insert(includeDirs, includeDir)
            table.insert(libDirs, libDir)

            if slangSDK then
                runtimeLibraries = os.matchfiles(libDir .. "/libslang*.so*")
            end
        end

        -- Requiring slang/slang.h distinguishes Shader Slang from the unrelated
        -- S-Lang terminal library commonly installed by Linux distributions.
        return {
            includeDirs = includeDirs,
            libDirs = libDirs,
            runtimeLibraries = runtimeLibraries
        }
    end

    -- Newer Vulkan SDKs bundle Slang. SLANG_SDK also permits a standalone Slang
    -- installation without forcing Vulkan and Slang to share an SDK root.
    local slangSDK = os.getenv("SLANG_SDK") or os.getenv("VULKAN_SDK")
    if not slangSDK then
        Solution.Util.PrintError(
            "Failed to find Slang. Please set SLANG_SDK to the Slang SDK root, " ..
            "or VULKAN_SDK when using a Vulkan SDK that bundles Slang."
        )
    end

    local runtimeLibraries
    if os.target() == "windows" then
        runtimeLibraries = { slangSDK .. "/bin/slang.dll" }
    else
        runtimeLibraries = os.matchfiles(slangSDK .. "/lib/libslang*.so*")
    end

    return {
        includeDirs = { slangSDK .. "/include" },
        libDirs = { slangSDK .. "/lib" },
        runtimeLibraries = runtimeLibraries
    }
end

local function copyRuntimeLibraries(runtimeLibraries)
    if not runtimeLibraries or #runtimeLibraries == 0 then
        return
    end

    local binDir = Solution.Projects.Current.BinDir
    local configs = { "Debug", "RelDebug", "Release" }

    for _, runtimeLibrary in ipairs(runtimeLibraries) do
        local runtimeLibraryName = path.getname(runtimeLibrary)

        for _, cfg in ipairs(configs) do
            local destDir = binDir .. "/" .. cfg
            os.mkdir(destDir)

            local destPath = destDir .. "/" .. runtimeLibraryName
            local success, err = os.copyfile(runtimeLibrary, destPath)
            if success then
                Solution.Util.Print("Copied " .. runtimeLibraryName .. " to " .. destDir)
            else
                Solution.Util.PrintError(
                    "Failed to copy " .. runtimeLibraryName .. " from '" .. runtimeLibrary .. "': " ..
                    (err or "unknown error")
                )
            end
        end
    end
end

local slangInfo = getSlangInfo()
copyRuntimeLibraries(slangInfo.runtimeLibraries)

local dep = Solution.Util.CreateDepTable("Slang-Slang", {})

Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
    Solution.Util.SetIncludes(slangInfo.includeDirs)
    Solution.Util.SetLibDirs(slangInfo.libDirs)
    Solution.Util.SetLinks("slang")
end)
