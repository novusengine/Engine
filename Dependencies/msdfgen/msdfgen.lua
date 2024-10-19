local dep = Solution.Util.CreateDepTable("msdfgen", { "freetype" })

Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
    Solution.Util.SetLanguage("C++")
    Solution.Util.SetCppDialect(20)

    local rootDir = dep.Path .. "/msdfgen"
    local coreDir = rootDir .. "/core"
    local extDir = rootDir .. "/ext"
    
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }
    local links = {}
    local files =
    {
        rootDir .. "/msdfgen.h",
        rootDir .. "/msdfgen-ext.h",

        coreDir .. "/arithmetics.h",
        coreDir .. "/base.h",
        coreDir .. "/Bitmap.h",
        coreDir .. "/Bitmap.hpp",
        coreDir .. "/bitmap-interpolation.hpp",
        coreDir .. "/BitmapRef.hpp",
        coreDir .. "/Contour.cpp",
        coreDir .. "/contour-combiners.cpp",
        coreDir .. "/contour-combiners.h",
        coreDir .. "/DistanceMapping.cpp",
        coreDir .. "/DistanceMapping.h",
        coreDir .. "/EdgeColor.h",
        coreDir .. "/edge-coloring.cpp",
        coreDir .. "/edge-coloring.h",
        coreDir .. "/EdgeHolder.cpp",
        coreDir .. "/EdgeHolder.h",
        coreDir .. "/edge-segments.cpp",
        coreDir .. "/edge-segments.h",
        coreDir .. "/edge-selectors.cpp",
        coreDir .. "/edge-selectors.h",
        coreDir .. "/equation-solver.cpp",
        coreDir .. "/equation-solver.h",
        coreDir .. "/export-svg.cpp",
        coreDir .. "/export-svg.h",
        coreDir .. "/generator-config.h",
        coreDir .. "/MSDFErrorCorrection.h",
        coreDir .. "/msdf-error-correction.h",
        coreDir .. "/MSDFErrorCorrection.cpp",
        coreDir .. "/msdf-error-correction.cpp",
        coreDir .. "/msdfgen.cpp",
        coreDir .. "/pixel-conversion.hpp",
        coreDir .. "/Projection.cpp",
        coreDir .. "/Projection.h",
        coreDir .. "/Range.hpp",
        coreDir .. "/rasterization.cpp",
        coreDir .. "/rasterization.h",
        coreDir .. "/render-sdf.cpp",
        coreDir .. "/render-sdf.h",
        coreDir .. "/save-bmp.h",
        coreDir .. "/save-fl32.cpp",
        coreDir .. "/save-fl32.h",
        coreDir .. "/save-rgba.cpp",
        coreDir .. "/save-rgba.h",
        coreDir .. "/save-tiff.cpp",
        coreDir .. "/save-tiff.h",
        coreDir .. "/Scanline.cpp",
        coreDir .. "/Scanline.h",
        coreDir .. "/sdf-error-estimation.cpp",
        coreDir .. "/sdf-error-estimation.h",
        coreDir .. "/SDFTransformation.h",
        coreDir .. "/Shape.cpp",
        coreDir .. "/Shape.h",
        coreDir .. "/shape-description.cpp",
        coreDir .. "/shape-description.h",
        coreDir .. "/ShapeDistanceFinder.h",
        coreDir .. "/ShapeDistanceFinder.hpp",
        coreDir .. "/SignedDistance.hpp",
        coreDir .. "/Vector2.hp",

        extDir .. "/import-font.cpp",
        extDir .. "/import-font.h",
        extDir .. "/import-svg.cpp",
        extDir .. "/import-svg.h",
        extDir .. "/resolve-shape-geometry.cpp",
        extDir .. "/resolve-shape-geometry.h",
        extDir .. "/save-png.cpp",
        extDir .. "/save-png.h",
    }
    
    Solution.Util.SetDefines(defines)
    Solution.Util.SetLinks(links)
    Solution.Util.SetFiles(files)
    Solution.Util.SetIncludes(dep.Path)
end)

Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
    local msdfgenDir = dep.Path .. "/msdfgen"

    Solution.Util.SetIncludes({ dep.Path, msdfgenDir })
    Solution.Util.SetLinks({ dep.Name })
end)