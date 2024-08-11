local mod = Solution.Util.CreateDepTable("dxcompiler", {})

Solution.Util.CreateDep(mod.Name, mod.Dependencies, function()
    Solution.Util.SetIncludes(mod.Path .. "/include")
    
    local libPath = iif(os.istarget("windows"), mod.Path .. "/lib/windows", mod.Path .. "/lib/linux")
    Solution.Util.SetLibDirs(libPath)
    local link = iif(os.istarget("windows"), libPath .. "/dxcompiler.lib", libPath .. "/dxcompiler")
    Solution.Util.SetLinks(link)
end)

local libPath = iif(os.istarget("windows"), mod.Path .. "/lib/windows/dxcompiler.dll", mod.Path .. "/lib/linux/libdxcompiler.so")
BuildSettings:Add("DXCompiler Dynamic Lib Path", libPath)
