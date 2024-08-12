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
        
        sourceDir .. "/internal.h",
        sourceDir .. "/platform.h",
        sourceDir .. "/mappings.h",
        sourceDir .. "/context.c",
        sourceDir .. "/init.c",
        sourceDir .. "/input.c",
        sourceDir .. "/monitor.c",
        sourceDir .. "/platform.c",
        sourceDir .. "/vulkan.c",
        sourceDir .. "/window.c",
        sourceDir .. "/egl_context.c",
        sourceDir .. "/osmesa_context.c",
        sourceDir .. "/null_platform.h",
        sourceDir .. "/null_joystick.h",
        sourceDir .. "/null_init.c",

        sourceDir .. "/null_monitor.c",
        sourceDir .. "/null_window.c",
        sourceDir .. "/null_joystick.c",
    }
    Solution.Util.SetFiles(files)
    Solution.Util.SetIncludes(dep.Path)
    Solution.Util.SetDefines(defines)

    Solution.Util.SetFilter("platforms:Win64", function()
        local files =
        {
            sourceDir .. "/win32_thread.h",
            sourceDir .. "/win32_time.h",

            sourceDir .. "/win32_init.c",
            sourceDir .. "/win32_module.c",
            sourceDir .. "/win32_joystick.c",
            sourceDir .. "/win32_monitor.c",
            sourceDir .. "/win32_time.c",
            sourceDir .. "/win32_thread.c",
            sourceDir .. "/win32_window.c",
            sourceDir .. "/wgl_context.c",
        }
        
        Solution.Util.SetFiles(files)
        Solution.Util.SetDefines({ "_GLFW_WIN32", "_CRT_SECURE_NO_WARNINGS" })
    end)
    
    Solution.Util.SetFilter("system:linux", function()
        local files =
        {
            sourceDir .. "/x11_init.c",
            sourceDir .. "/posix_module.c",
            sourceDir .. "/posix_poll.c",
            sourceDir .. "/x11_monitor.c",
            sourceDir .. "/x11_window.c",
            sourceDir .. "/xkb_unicode.c",
            sourceDir .. "/posix_time.c",
            sourceDir .. "/posix_thread.c",
            sourceDir .. "/glx_context.c",
            sourceDir .. "/linux_joystick.c"
        }
        
        Solution.Util.SetFiles(files)
        Solution.Util.SetDefines({ "_GLFW_X11", "_CRT_SECURE_NO_WARNINGS" })
        Solution.Util.SetLinks({ "pthread" })
    end)
end)

Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
    Solution.Util.SetIncludes(dep.Path .. "/include")
    Solution.Util.SetLinks(dep.Name)
    
    Solution.Util.SetFilter("platforms:Win64", function()
        Solution.Util.SetDefines({ "_GLFW_WIN32", "_CRT_SECURE_NO_WARNINGS" })
    end)

    Solution.Util.SetFilter("system:linux", function()
        Solution.Util.SetDefines({ "_GLFW_X11", "_CRT_SECURE_NO_WARNINGS" })
    end)
end)
