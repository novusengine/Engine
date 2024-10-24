local dep = Solution.Util.CreateDepTable("msdf-atlas-gen", { "msdfgen" })

Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
    Solution.Util.SetLanguage("C++")
    Solution.Util.SetCppDialect(20)

    local srcDir = dep.Path .. "/msdf-atlas-gen"
    
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }
    local links = {}
    local files =
    {
        srcDir .. "/AtlasGenerator.h",
        srcDir .. "/AtlasStorage.h",
        srcDir .. "/BitmapAtlasStorage.h",
        srcDir .. "/BitmapAtlasStorage.hpp",
        srcDir .. "/bitmap-blit.cpp",
        srcDir .. "/bitmap-blit.h",
        srcDir .. "/Charset.cpp",
        srcDir .. "/Charset.h",
        srcDir .. "/charset-parser.cpp",
        srcDir .. "/csv-export.cpp",
        srcDir .. "/csv-export.h",
        srcDir .. "/DynamicAtlas.h",
        srcDir .. "/DynamicAtlas.hpp",
        srcDir .. "/FontGeometry.cpp",
        srcDir .. "/FontGeometry.h",
        srcDir .. "/GlyphBox.h",
        srcDir .. "/glyph-generators.cpp",
        srcDir .. "/glyph-generators.h",
        srcDir .. "/GlyphGeometry.cpp",
        srcDir .. "/GlyphGeometry.h",
        srcDir .. "/GridAtlasPacker.cpp",
        srcDir .. "/GridAtlasPacker.h",
        srcDir .. "/image-encode.cpp",
        srcDir .. "/image-encode.h",
        srcDir .. "/image-save.h",
        srcDir .. "/image-save.hpp",
        srcDir .. "/ImmediateAtlasGenerator.h",
        srcDir .. "/ImmediateAtlasGenerator.hpp",
        srcDir .. "/json-export.cpp",
        srcDir .. "/json-export.h",
        srcDir .. "/msdf-atlas-gen.h",
        srcDir .. "/Padding.cpp",
        srcDir .. "/Padding.h",
        srcDir .. "/Rectangle.h",
        srcDir .. "/RectanglePacker.cpp",
        srcDir .. "/RectanglePacker.h",
        srcDir .. "/rectangle-packing.h",
        srcDir .. "/rectangle-packing.hpp",
        srcDir .. "/Remap.h",
        srcDir .. "/shadron-preview-generator.cpp",
        srcDir .. "/shadron-preview-generator.h",
        srcDir .. "/size-selectors.cpp",
        srcDir .. "/size-selectors.h",
        srcDir .. "/TightAtlasPacker.cpp",
        srcDir .. "/TightAtlasPacker.h",
        srcDir .. "/types.h",
        srcDir .. "/utf8.cpp",
        srcDir .. "/utf8.h",
        srcDir .. "/Workload.cpp",
        srcDir .. "/Workload.h",
    }

    warnings "Off"
    
    Solution.Util.SetDefines(defines)
    Solution.Util.SetLinks(links)
    Solution.Util.SetFiles(files)
    Solution.Util.SetIncludes(dep.Path)
end)

Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
    Solution.Util.SetIncludes(dep.Path)
    Solution.Util.SetLinks({ dep.Name })
end)