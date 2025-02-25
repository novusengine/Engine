local dep = Solution.Util.CreateDepTable("Imgui", { "vulkan", "glfw", "glm"})

Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS", "IMGUI_DEFINE_MATH_OPERATORS" }

    Solution.Util.SetLanguage("C++")
    Solution.Util.SetCppDialect(20)

    local sourceDir = dep.Path .. "/imgui"
    local includeDirs = { dep.Path, sourceDir }
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
    Solution.Util.SetFiles(files)
    Solution.Util.SetIncludes(includeDirs)
    Solution.Util.SetDefines(defines)
end)

Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
    local sourceDir = dep.Path .. "/imgui"
    Solution.Util.SetIncludes({dep.Path, sourceDir})
    Solution.Util.SetLinks(dep.Name)
end)