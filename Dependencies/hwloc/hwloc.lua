local dep = Solution.Util.CreateDepTable("hwloc", {})

Solution.Util.CreateDep(dep.Name, dep.Dependencies, function()
    if os.target() == "linux" then
        Solution.Util.SetIncludes({ "/usr/include/hwloc" })
        Solution.Util.SetLinks({ "hwloc" })
    end
end)