local mod = Solution.Util.CreateDepTable("dxcompiler", {})

Solution.Util.CreateDep(mod.Name, mod.Dependencies, function()
    Solution.Util.SetIncludes(mod.Path .. "/include")
    
    local link = iif(os.istarget("windows"), mod.Path .. "/lib/windows/dxcompiler.lib", mod.Path .. "/lib/linux/libdxcompiler.so")
    Solution.Util.SetLinks(link)
end)

local libPath = iif(os.istarget("windows"), mod.Path .. "/lib/windows/dxcompiler.dll", mod.Path .. "/lib/linux/libdxcompiler.so")
BuildSettings:Add("DXCompiler Dynamic Lib Path", libPath)