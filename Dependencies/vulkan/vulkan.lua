local function getVulkanInfo()
    local envName = "VULKAN_SDK"
    local includeDirs = {}
    local libs = {}
    
    if os.target() == "windows" then
        local vulkanSDK = os.getenv(envName)
        if vulkanSDK then
            table.insert(includeDirs, vulkanSDK .. "/include")
            table.insert(libs, vulkanSDK .. "/lib/vulkan-1.lib")
        end
    else
        local vulkanSDK = io.popen("echo $"..envName):read("*a"):gsub("\n", "")
        if vulkanSDK ~= "" then
            table.insert(includeDirs, vulkanSDK .. "/include")
            table.insert(libs, "vulkan")
        end
    end
    
    if #libs == 0 or #includeDirs == 0 then
        Solution.Util.PrintError("Failed to find Vulkan SDK with system variable '" .. envName .. "'. Please ensure Vulkan is installed and configured properly")
    else
        print("Found Vulkan SDK at: "..table.unpack(includeDirs))
    end
    
    return includeDirs, libs
end

local dep = Solution.Util.CreateDepTable("vulkan", {})

Solution.Util.CreateDep(dep.Name, dep.Dependencies, function()
    local includeDirs, libs = getVulkanInfo()
    Solution.Util.SetIncludes(includeDirs)
    Solution.Util.SetLinks(libs)
    Solution.Util.SetDefines({ "_CRT_SECURE_NO_WARNINGS" })
end)