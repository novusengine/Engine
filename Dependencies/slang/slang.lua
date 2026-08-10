local vkSdk = os.getenv("VULKAN_SDK")
if not vkSdk then
    Solution.Util.PrintError("VULKAN_SDK not found. Please ensure Vulkan SDK is installed.")
end

-- Determine platform-specific library paths
-- On Linux the slang runtime is split across several libraries (libslang.so is a
-- symlink to libslang-compiler.so.<version>, which the executable's SONAME reference
-- points at), so the full libslang* family must be copied, not just libslang.so.
local libSrcPaths
if os.target() == "windows" then
    libSrcPaths = { vkSdk .. "/bin/slang.dll" }
else
    libSrcPaths = os.matchfiles(vkSdk .. "/lib/libslang*.so*")
    if #libSrcPaths == 0 then
        Solution.Util.PrintError("No libslang*.so* found in '" .. vkSdk .. "/lib'. Please ensure the Vulkan SDK ships slang")
    end
end

-- Copy shared libraries (release version always) during premake generation
-- Copy into both the Engine bin dir and its parent (the root project's bin dir),
-- since executables built by the root project load these from their own directory.
local binDir = Solution.Projects.Current.BinDir
local binDirs = { binDir }
local parentBinDir = path.getdirectory(binDir)
if parentBinDir and parentBinDir ~= binDir then
    table.insert(binDirs, parentBinDir)
end
local configs = { "Debug", "RelDebug", "Release" }

for _, cfgDir in ipairs(binDirs) do
for _, cfg in ipairs(configs) do
    local destDir = cfgDir .. "/" .. cfg
    os.mkdir(destDir)

    for _, libSrcPath in ipairs(libSrcPaths) do
        local libName = path.getname(libSrcPath)
        local destPath = destDir .. "/" .. libName
        local success, err = os.copyfile(libSrcPath, destPath)
        if success then
            Solution.Util.Print("Copied " .. libName .. " to " .. destDir)
        else
            Solution.Util.PrintError("Failed to copy " .. libName .. ": " .. (err or "unknown error"))
        end
    end
end
end

-- Create dependency
local dep = Solution.Util.CreateDepTable("Slang-Slang", {})

Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
    Solution.Util.SetIncludes(vkSdk .. "/include")
    Solution.Util.SetLibDirs(vkSdk .. "/lib")
    Solution.Util.SetLinks("slang")
end)
