local dep = Solution.Util.CreateDepTable("dxcompiler", {})

Solution.Util.CreateDep(dep.Name, dep.Dependencies, function()
    local cachedData = Solution.Util.GetDepCache(dep, "cache")
    
    local libPath, lib
    if cachedData then
        libPath, lib = cachedData.libPaths, cachedData.libs
    else
        libPath = iif(os.istarget("windows"), dep.Path .. "/lib/windows", dep.Path .. "/lib/linux")
        lib = iif(os.istarget("windows"), libPath .. "/dxcompiler.lib", libPath .. "/dxcompiler")
        Solution.Util.SetDepCache(dep, "cache", { libPaths = libPath, libs = lib })
    end
    
    Solution.Util.SetLibDirs(libPath)
    Solution.Util.SetLinks(lib)
    Solution.Util.SetIncludes(dep.Path .. "/include")
end)

local libPath = iif(os.istarget("windows"), dep.Path .. "/lib/windows/dxcompiler.dll", dep.Path .. "/lib/linux/libdxcompiler.so")
BuildSettings:Add("DXCompiler Dynamic Lib Path", libPath)
