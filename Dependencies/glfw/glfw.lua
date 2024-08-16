local function populateDepCache(dep)
    local cachedData = Solution.Util.GetDepCache(dep, "cache")
    
    if not cachedData then
        -- static library only uses libDefine, dependency uses both libDefine and depDefine
        local libDefine = { "_CRT_SECURE_NO_WARNINGS" }
        local depDefine = { "GLFW_INCLUDE_VULKAN" }
        local link = {}
        
        if os.target() == "windows" then
            -- add win32 defines
            Solution.Util.MergeIntoTable(libDefine, { "_GLFW_WIN32" })
            Solution.Util.MergeIntoTable(depDefine, { "GLFW_EXPOSE_NATIVE_WIN32" })
        else
            local useXorg, useWayland = BuildSettings:Get("Using X11"), BuildSettings:Get("Using Wayland")
            if(useXorg) then
                -- add x11 defines and libraries
                Solution.Util.MergeIntoTable(libDefine, { "_GLFW_X11" })
                Solution.Util.MergeIntoTable(depDefine, { "GLFW_EXPOSE_NATIVE_X11" })
                Solution.Util.MergeIntoTable(link, { "X11" })
            end
            
            if(useWayland and not useXorg) then
                -- add wayland defines and libraries
                Solution.Util.MergeIntoTable(libDefine, { "_GLFW_WAYLAND" })
                Solution.Util.MergeIntoTable(depDefine, { "GLFW_EXPOSE_NATIVE_WAYLAND" })
                Solution.Util.MergeIntoTable(link, { "wayland-dev" })
            end
            
            Solution.Util.MergeIntoTable(link, { "pthread" })
        end
        cachedData = { libDefines = libDefine, depDefines = depDefine, links = link }
        Solution.Util.SetDepCache(dep, "cache", cachedData)
    end
    
    return cachedData
end

local dep = Solution.Util.CreateDepTable("Glfw", {})

Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
    Solution.Util.SetLanguage("C++")
    Solution.Util.SetCppDialect(20)

    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }
    local links = {}

    -- either get or generate cached defines and links
    local cachedData = Solution.Util.GetDepCache(dep, "cache")
    if not cachedData then
        cachedData = populateDepCache(dep)
    end
    
    -- merge cached data into local tables
    Solution.Util.MergeIntoTable(defines, cachedData.libDefines)
    Solution.Util.MergeIntoTable(links, cachedData.links)

    -- gather generic and platform specific source files
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

    if os.target() == "windows" then
        local platformFiles =
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
        Solution.Util.MergeIntoTable(files, platformFiles)
    else -- Linux
        local platformFiles =
        {
            sourceDir .. "/posix_module.c",
            sourceDir .. "/posix_poll.c",
            sourceDir .. "/posix_time.c",
            sourceDir .. "/posix_thread.c",
            sourceDir .. "/glx_context.c",
            sourceDir .. "/linux_joystick.c",
            sourceDir .. "/xkb_unicode.c",
        }
        Solution.Util.MergeIntoTable(files, platformFiles)
        
        local useXorg, useWayland = BuildSettings:Get("Using X11"), BuildSettings:Get("Using Wayland")
        if(useXorg) then
            local platformFiles = 
            {
                sourceDir .. "/x11_platform.h",
                
                sourceDir .. "/x11_init.c",
                sourceDir .. "/x11_monitor.c",
                sourceDir .. "/x11_window.c",
            }
            Solution.Util.MergeIntoTable(files, platformFiles)
        end
        
        if(useWayland and not useXorg) then
            local platformFiles = 
            {
                sourceDir .. "/wl_platform.h",
                
                sourceDir .. "/wl_init.c",
                sourceDir .. "/wl_monitor.c",
                sourceDir .. "/wl_window.c",
            }
            Solution.Util.MergeIntoTable(files, platformFiles)
        end
    end

    -- set library data
    Solution.Util.SetDefines(defines)
    Solution.Util.SetLinks(links)
    Solution.Util.SetFiles(files)
    Solution.Util.SetIncludes(dep.Path)
end)

Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
    -- get cache from parent dependency table
    local cachedData = Solution.Util.GetDepCache(dep, "cache")
    if not cachedData then
        cachedData = populateDepCache(dep)
    end

    -- set dependency data
    local libDefines, depDefines, links = cachedData.libDefines, cachedData.depDefines, cachedData.links
    Solution.Util.SetIncludes(dep.Path .. "/include")
    Solution.Util.SetLinks({ dep.Name, links })
    Solution.Util.SetDefines({ libDefines, depDefines })
end)
