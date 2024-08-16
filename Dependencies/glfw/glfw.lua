local dep = Solution.Util.CreateDepTable("Glfw", {})

Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS", "_CRT_SECURE_NO_WARNINGS" }

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
        sourceDir .. "/null_platform.h",
        sourceDir .. "/null_joystick.h",
        
        sourceDir .. "/context.c",
        sourceDir .. "/init.c",
        sourceDir .. "/input.c",
        sourceDir .. "/monitor.c",
        sourceDir .. "/platform.c",
        sourceDir .. "/vulkan.c",
        sourceDir .. "/window.c",
        sourceDir .. "/egl_context.c",
        sourceDir .. "/osmesa_context.c",
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
        Solution.Util.SetDefines({ "_GLFW_WIN32" })
    end)

    Solution.Util.SetFilter("system:linux", function()
        local useXorg, useWayland = BuildSettings:Get("Using X11"), BuildSettings:Get("Using Wayland")
        
        local files =
        {
            sourceDir .. "/posix_module.c",
            sourceDir .. "/posix_poll.c",

            sourceDir .. "/posix_time.c",
            sourceDir .. "/posix_thread.c",
            sourceDir .. "/glx_context.c",
            sourceDir .. "/linux_joystick.c",
            sourceDir .. "/xkb_unicode.c",
        }
        
        Solution.Util.SetFiles(files)
        Solution.Util.SetLinks({ "pthread" })
        
        if(useXorg) then
            local files = 
            {
                sourceDir .. "/x11_platform.h",
                
                sourceDir .. "/x11_init.c",
                sourceDir .. "/x11_monitor.c",
                sourceDir .. "/x11_window.c",
            }
            Solution.Util.SetFiles(files)
            Solution.Util.SetDefines({ "_GLFW_X11" })
            Solution.Util.SetLinks({ "X11" })
        end
        
        if(useWayland and not useXorg) then
            local files = 
            {
                sourceDir .. "/wl_platform.h",
                
                sourceDir .. "/wl_init.c",
                sourceDir .. "/wl_monitor.c",
                sourceDir .. "/wl_window.c",
            }
            Solution.Util.SetFiles(files)
            Solution.Util.SetDefines({ "_GLFW_WAYLAND" })
            Solution.Util.SetLinks({ "wayland-dev" })
        end
    end)
end)

local function populateDepCache(dep)
    local cachedData = Solution.Util.GetDepCache(dep, "cache")
    
    if not cachedData then
        local def = { "GLFW_INCLUDE_VULKAN", "_CRT_SECURE_NO_WARNINGS" }
        local link = {}
        
        if os.target() == "windows" then
            -- add win32 defines
            table.insert(def, "GLFW_EXPOSE_NATIVE_WIN32")
            table.insert(def, "_GLFW_WIN32")
        else
            local useXorg, useWayland = BuildSettings:Get("Using X11"), BuildSettings:Get("Using Wayland")
            
            if(useXorg) then
                -- add x11 defines and libraries
                table.insert(def, "GLFW_EXPOSE_NATIVE_X11")
                table.insert(def, "_GLFW_X11")
                table.insert(link, "X11")
            end
            
            if(useWayland and not useXorg) then
                -- add wayland defines and libraries
                table.insert(def, "GLFW_EXPOSE_NATIVE_WAYLAND")
                table.insert(def, "_GLFW_WAYLAND")
                table.insert(link, "wayland-dev")
            end
            
            table.insert(link, "pthread")
        end
        cachedData = { defines = def, links = link }
        Solution.Util.SetDepCache(dep, "cache", cachedData)
    end
    
    return cachedData
end

Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
    -- get our own internal dependency table, and not the parent one
    local self = Solution.Util.GetDepTable(dep.NameLow)
    
    -- get our own local cache
    local cachedData = Solution.Util.GetDepCache(self, "cache")
    if not cachedData then
        cachedData = populateDepCache(self)
    end
    
    local defines, links = cachedData.defines, cachedData.links
    Solution.Util.SetIncludes(dep.Path .. "/include")
    Solution.Util.SetLinks({ dep.Name, links })
    Solution.Util.SetDefines(defines)
end)
