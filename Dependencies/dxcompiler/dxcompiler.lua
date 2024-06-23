local mod = Solution.Util.CreateDepTable("dxcompiler", {})

Solution.Util.CreateDep(mod.Name, mod.Dependencies, function()
    Solution.Util.SetIncludes(mod.Path .. "/include")

    Solution.Util.SetFilter("platforms:Win64", function()
        local link = mod.Path .. "/lib/windows/dxcompiler.lib"
        Solution.Util.SetLinks(link)
    end)
    
    Solution.Util.SetFilter("platforms:Linux", function()
        local link = mod.Path .. "/lib/linux/dxcompiler.so"
        Solution.Util.SetLinks(link)
    end)
end)

local libPath = iif(os.istarget("windows"), mod.Path .. "/lib/windows/dxcompiler.dll", mod.Path .. "/lib/linux/dxcompiler.so")
BuildSettings:Add("DXCompiler Dynamic Lib Path", libPath)