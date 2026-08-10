local dep = Solution.Util.CreateDepTable("libsodium", {})

Solution.Util.CreateDep(dep.Name, dep.Dependencies, function()
    if os.istarget("windows") then
        local libPath = dep.Path .. "/libsodium/lib/windows"
        local lib = libPath .. "/libsodium.lib"

        Solution.Util.SetDefines("SODIUM_STATIC")
        Solution.Util.SetLibDirs(libPath)
        Solution.Util.SetLinks(lib)
    else
        Solution.Util.SetLinks("sodium")
    end

    Solution.Util.SetIncludes(dep.Path)
end)