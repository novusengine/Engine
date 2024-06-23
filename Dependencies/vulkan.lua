local osEnvName = "VULKAN_SDK"
local vulkanSDK = os.getenv(osEnvName)

if not vulkanSDK then
    Solution.Util.PrintError("Failed to find System Environment Variable '" .. osEnvName .. ". Please ensure Vulkan is installed and configured properly")
end

local dep = Solution.Util.CreateDepTable("vulkan", {})

Solution.Util.CreateDep(dep.Name, dep.Dependencies, function()
    Solution.Util.SetIncludes(vulkanSDK .. "/include")
    Solution.Util.SetLinks(vulkanSDK .. "/lib/vulkan-1.lib")
    Solution.Util.SetDefines({ "_CRT_SECURE_NO_WARNINGS" })
end)