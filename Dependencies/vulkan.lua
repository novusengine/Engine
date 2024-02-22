local osEnvName = "VULKAN_SDK"
local vulkanSDK = os.getenv(osEnvName)

if not vulkanSDK then
    error("Failed to find System Environment Variable '" .. osEnvName .. ". Please ensure Vulkan is installed and properly configured")
end

local function Include()
    local includeDir = vulkanSDK .. "/include"
    AddIncludeDirs(includeDir)

    local link = vulkanSDK .. "/lib/vulkan-1.lib"
    AddLinks(link)

    local define = { "_CRT_SECURE_NO_WARNINGS" }
    AddDefines(define)
end

CreateDep("vulkan", Include)