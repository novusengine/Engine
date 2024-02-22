local function Win64Include(srcFolder)
    local files =
    {
        -- Windows Only
        srcFolder .. "/win32_platform.h",
        srcFolder .. "/win32_joystick.h",
        srcFolder .. "/wgl_context.h",
        srcFolder .. "/egl_context.h",
        srcFolder .. "/osmesa_context.h",

        srcFolder .. "/win32_init.c",
        srcFolder .. "/win32_joystick.c",
        srcFolder .. "/win32_monitor.c",
        srcFolder .. "/win32_time.c",
        srcFolder .. "/win32_thread.c",
        srcFolder .. "/win32_window.c",
        srcFolder .. "/wgl_context.c",
        srcFolder .. "/egl_context.c",
        srcFolder .. "/osmesa_context.c"
    }

    AddFiles(files)

    local defines =
    {
        "_GLFW_WIN32",
        "_CRT_SECURE_NO_WARNINGS"
    }

    AddDefines(defines)
end

local function SetupLib()
    local basePath = path.getabsolute("glfw/", Engine.dependencyDir)
    local dependencies = { }
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    ProjectTemplate("Glfw", "StaticLib", nil, Engine.binDir, dependencies, defines)

    local sourceDir = path.getabsolute("src/", basePath)
    local includeDir = path.getabsolute("include/", basePath)
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
    AddFiles(files)
    AddIncludeDirs(includeDir)

    filter "platforms:Win64"
        Win64Include(sourceDir)

    filter "platforms:Linux"
        AddLinks("pthread")
end
SetupLib()

local function Include()
    local includeDir = path.getabsolute("glfw/include/", Engine.dependencyDir)
    AddIncludeDirs(includeDir)

    filter "platforms:Win64"
        AddDefines({ "_GLFW_WIN32", "_CRT_SECURE_NO_WARNINGS" })

    AddLinks("Glfw")
end
CreateDep("glfw", Include)