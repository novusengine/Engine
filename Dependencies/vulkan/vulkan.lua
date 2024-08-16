local function getVulkanInfo()
    local envName = "VULKAN_SDK"
    local includeDirs = {}
    local libs = {}
    
    local vulkanSDK = os.getenv(envName)
    if not vulkanSDK then
        Solution.Util.PrintError("Failed to find Vulkan SDK with system variable '" .. envName .. "'. Please ensure Vulkan is installed and configured properly")
    end
    
    -- Add Includes path
    table.insert(includeDirs, vulkanSDK .. "/include")
    
    -- Add Library
    if os.target() == "windows" then
        table.insert(libs, vulkanSDK .. "/lib/vulkan-1.lib")
    else
        table.insert(libs, "vulkan")
    end
    
    return includeDirs, libs
end

local dep = Solution.Util.CreateDepTable("vulkan", {})

Solution.Util.CreateDep(dep.Name, dep.Dependencies, function()
    local cachedData = Solution.Util.GetDepCache(dep.Name, "cache")

    local includeDirs, libs
    if cachedData then
        includeDirs, libs = cachedData.includes, cachedData.libs
    else
        includeDirs, libs = getVulkanInfo()
        Solution.Util.SetDepCache(dep.Name, "cache", { includes = includeDirs, libs = libs })
    end

    Solution.Util.SetIncludes(includeDirs)
    Solution.Util.SetLinks(libs)
    Solution.Util.SetDefines({ "_CRT_SECURE_NO_WARNINGS" })
end)