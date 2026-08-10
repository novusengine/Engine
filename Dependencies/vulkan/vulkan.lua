local function getVulkanInfo()
    local includeDirs = {}
    local libDirs = {}
    local libs = {}

    if os.target() == "linux" then
        if os.host() == "linux" then
            local vulkanSDK = os.getenv("VULKAN_SDK")
            local includeDir
            local libDir

            if vulkanSDK then
                includeDir = vulkanSDK .. "/include"
                libDir = vulkanSDK .. "/lib/VulkanLoader/lib"

                if not os.isfile(includeDir .. "/vulkan/vulkan.h") then
                    Solution.Util.PrintError(
                        "VULKAN_SDK does not contain Vulkan headers: '" .. includeDir .. "'."
                    )
                end

                if not os.isfile(libDir .. "/libvulkan.so") then
                    Solution.Util.PrintError(
                        "VULKAN_SDK does not contain the Vulkan loader: '" .. libDir .. "'."
                    )
                end
            else
                includeDir = os.findheader("vulkan/vulkan.h")
                libDir = os.findlib("vulkan")
            end

            if not includeDir then
                Solution.Util.PrintError(
                    "Failed to find the Vulkan headers in the system include paths. " ..
                    "Source the Vulkan SDK's setup-env.sh, or install the Vulkan development packages."
                )
            end

            if not libDir then
                Solution.Util.PrintError(
                    "Failed to find the Vulkan loader in the system library paths. " ..
                    "Source the Vulkan SDK's setup-env.sh, or install the Vulkan development packages."
                )
            end

            table.insert(includeDirs, includeDir)
            table.insert(libDirs, libDir)
        end

        table.insert(libs, "vulkan")
    else
        local envName = "VULKAN_SDK"
        local vulkanSDK = os.getenv(envName)
        if not vulkanSDK then
            Solution.Util.PrintError(
                "Failed to find the Vulkan SDK with system variable '" .. envName .. "'. " ..
                "Please ensure Vulkan is installed and configured properly."
            )
        end

        table.insert(includeDirs, vulkanSDK .. "/include")

        if os.target() == "windows" then
            table.insert(libs, vulkanSDK .. "/lib/vulkan-1.lib")
        else
            table.insert(libs, "vulkan")
        end
    end

    return includeDirs, libDirs, libs
end

local dep = Solution.Util.CreateDepTable("vulkan", {})

Solution.Util.CreateDep(dep.Name, dep.Dependencies, function()
    local cachedData = Solution.Util.GetDepCache(dep.Name, "cache")

    local includeDirs, libDirs, libs
    if cachedData then
        includeDirs, libDirs, libs = cachedData.includes, cachedData.libDirs, cachedData.libs
    else
        includeDirs, libDirs, libs = getVulkanInfo()
        Solution.Util.SetDepCache(dep.Name, "cache", {
            includes = includeDirs,
            libDirs = libDirs,
            libs = libs
        })
    end

    Solution.Util.SetIncludes(includeDirs)
    Solution.Util.SetLibDirs(libDirs)
    Solution.Util.SetLinks(libs)
    Solution.Util.SetDefines({ "_CRT_SECURE_NO_WARNINGS" })
end)
