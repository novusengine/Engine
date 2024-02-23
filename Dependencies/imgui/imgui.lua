local function SetupLib()
    local basePath = path.getabsolute("imgui/", Engine.dependencyDir)
    local dependencies = { "vulkan", "glfw", "glm" }
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS", "SPDLOG_COMPILED_LIB" }

    ProjectTemplate("Imgui", "StaticLib", nil, Engine.binDir, dependencies, defines)

    local sourceDir = path.getabsolute("imgui/", basePath)
    local includeDir = { basePath, sourceDir }
    local files =
    {
        sourceDir .. "/imgui.h",
        sourceDir .. "/imgui.cpp",
        sourceDir .. "/imgui_demo.cpp",
        sourceDir .. "/imgui_draw.cpp",
        sourceDir .. "/imgui_widgets.cpp",
        sourceDir .. "/imgui_tables.cpp",
        sourceDir .. "/backends/imgui_impl_vulkan.h",
        sourceDir .. "/backends/imgui_impl_vulkan.cpp",
        sourceDir .. "/backends/imgui_impl_glfw.h",
        sourceDir .. "/backends/imgui_impl_glfw.cpp",
        sourceDir .. "/misc/cpp/imgui_stdlib.cpp",

        sourceDir .. "/implot.h",
        sourceDir .. "/implot.cpp",
        sourceDir .. "/implot_demo.cpp",
        sourceDir .. "/implot_items.cpp",

        sourceDir .. "/imguizmo/ImGuizmo.h",
        sourceDir .. "/imguizmo/ImGuizmo.cpp",
        sourceDir .. "/imguizmo/GraphEditor.h",
        sourceDir .. "/imguizmo/GraphEditor.cpp",
        sourceDir .. "/imguizmo/ImCurveEdit.h",
        sourceDir .. "/imguizmo/ImCurveEdit.cpp",
        sourceDir .. "/imguizmo/ImGradient.h",
        sourceDir .. "/imguizmo/ImGradient.cpp",
        sourceDir .. "/imguizmo/ImSequencer.h",
        sourceDir .. "/imguizmo/ImSequencer.cpp",

        sourceDir .. "/imnodes.h",
        sourceDir .. "/imnodes.cpp"
    }
    AddFiles(files)

    AddIncludeDirs(includeDir)
end
SetupLib()

local dependencies = { "vulkan", "glfw", "glm" }
local function Include()
    local imguiFolder = path.getabsolute("imgui/", Engine.dependencyDir)

    local includeDirs = { imguiFolder }
    AddIncludeDirs(includeDirs)

    AddLinks("Imgui")
end
CreateDep("imgui", Include, dependencies)