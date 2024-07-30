local osEnvName = "VULKAN_SDK"
local vulkanSDK = os.getenv(osEnvName)

if not vulkanSDK then
    Solution.Util.PrintError("Failed to find System Environment Variable '" .. osEnvName .. ". Please ensure Vulkan is installed and configured properly")
end

local dep = Solution.Util.CreateDepTable("vulkan", {})
local libPath = iif(os.istarget("windows"), vulkanSDK .. "/lib/vulkan-1.lib", "vulkan")

Solution.Util.CreateDep(dep.Name, dep.Dependencies, function()
    Solution.Util.SetIncludes(vulkanSDK .. "/include")
    Solution.Util.SetLinks(libPath)
    Solution.Util.SetDefines({ "_CRT_SECURE_NO_WARNINGS" })
end)