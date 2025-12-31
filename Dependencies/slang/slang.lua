local vkSdk = os.getenv("VULKAN_SDK")
if not vkSdk then
    Solution.Util.PrintError("VULKAN_SDK not found. Please ensure Vulkan SDK is installed.")
end

-- Determine platform-specific library paths
local libName, libSrcPath
if os.target() == "windows" then
    libName = "slang.dll"
    libSrcPath = vkSdk .. "/bin/" .. libName
else
    libName = "libslang.so"
    libSrcPath = vkSdk .. "/lib/" .. libName
end

-- Copy shared library (release version always) during premake generation
local binDir = Solution.Projects.Current.BinDir
local configs = { "Debug", "RelDebug", "Release" }

for _, cfg in ipairs(configs) do
    local destDir = binDir .. "/" .. cfg
    os.mkdir(destDir)
    
    local destPath = destDir .. "/" .. libName
    local success, err = os.copyfile(libSrcPath, destPath)
    if success then
        Solution.Util.Print("Copied " .. libName .. " to " .. destDir)
    else
        Solution.Util.PrintError("Failed to copy " .. libName .. ": " .. (err or "unknown error"))
    end
end

-- Create dependency
local dep = Solution.Util.CreateDepTable("Slang-Slang", {})

Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
    Solution.Util.SetIncludes(vkSdk .. "/include")
    Solution.Util.SetLibDirs(vkSdk .. "/lib")
    Solution.Util.SetLinks("slang")
end)
