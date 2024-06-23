local dep = Solution.Util.CreateDepTable("Glfw", {})

Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    Solution.Util.SetLanguage("C++")
    Solution.Util.SetCppDialect(20)

    local sourceDir = dep.Path .. "/src"
    local includeDir = dep.Path .. "/include"

    local files =
    {
        includeDir .. "/GLFW/glfw3.h",
        includeDir .. "/GLFW/glfw3native.h",

        sourceDir .. "/context.c",
        sourceDir .. "/init.c",
        sourceDir .. "/input.c",
        sourceDir .. "/monitor.c",
        sourceDir .. "/vulkan.c",
        sourceDir .. "/window.c"
    }
    Solution.Util.SetFiles(files)
    Solution.Util.SetIncludes(dep.Path)
    Solution.Util.SetDefines(defines)

    Solution.Util.SetFilter("platforms:Win64", function()
        local files =
        {
            sourceDir .. "/win32_platform.h",
            sourceDir .. "/win32_joystick.h",
            sourceDir .. "/wgl_context.h",
            sourceDir .. "/egl_context.h",
            sourceDir .. "/osmesa_context.h",

            sourceDir .. "/win32_init.c",
            sourceDir .. "/win32_joystick.c",
            sourceDir .. "/win32_monitor.c",
            sourceDir .. "/win32_time.c",
            sourceDir .. "/win32_thread.c",
            sourceDir .. "/win32_window.c",
            sourceDir .. "/wgl_context.c",
            sourceDir .. "/egl_context.c",
            sourceDir .. "/osmesa_context.c"
        }
        
        Solution.Util.SetFiles(files)
        Solution.Util.SetDefines({ "_GLFW_WIN32", "_CRT_SECURE_NO_WARNINGS" })
    end)
    
    Solution.Util.SetFilter("platforms:Linux", function()
        Solution.Util.SetLinks({ "pthread" })
    end)
end)

Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
    Solution.Util.SetIncludes(dep.Path .. "/include")
    Solution.Util.SetLinks(dep.Name)
    
    Solution.Util.SetFilter("platforms:Win64", function()
        Solution.Util.SetDefines({ "_GLFW_WIN32", "_CRT_SECURE_NO_WARNINGS" })
    end)
end)