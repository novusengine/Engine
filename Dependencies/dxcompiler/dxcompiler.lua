local mod = Solution.Util.CreateDepTable("dxcompiler", {})
local libPath = iif(os.istarget("windows"), mod.Path .. "/lib/windows/dxcompiler.dll", mod.Path .. "/lib/linux/libdxcompiler.so")

Solution.Util.CreateDep(mod.Name, mod.Dependencies, function()
    Solution.Util.SetIncludes(mod.Path .. "/include")
    Solution.Util.SetLinks(libPath)
end)

BuildSettings:Add("DXCompiler Dynamic Lib Path", libPath)