local dep = Solution.Util.CreateDepTable("freetype", {})

Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
    Solution.Util.SetLanguage("C++")
    Solution.Util.SetCppDialect(20)

    local rootDir = dep.Path .. "/freetype"
    local sourceDir = rootDir .. "/src"
    local buildsDir = rootDir .. "/builds"
    local includeDir = rootDir .. "/include"
    
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS", "_CRT_SECURE_NO_WARNINGS", "FT2_BUILD_LIBRARY" }
    local links = {}
    local files =
    {
        sourceDir .. "/autofit/autofit.c",
        sourceDir .. "/bdf/bdf.c",
        sourceDir .. "/cff/cff.c",
        sourceDir .. "/base/ftbase.c",
        sourceDir .. "/base/ftbitmap.c",
        sourceDir .. "/cache/ftcache.c",
        sourceDir .. "/base/ftfstype.c",
        sourceDir .. "/base/ftglyph.c",
        sourceDir .. "/gzip/ftgzip.c",
        sourceDir .. "/base/ftinit.c",
        sourceDir .. "/lzw/ftlzw.c",
        sourceDir .. "/base/ftstroke.c",
        sourceDir .. "/base/ftsystem.c",
        sourceDir .. "/smooth/smooth.c",

        sourceDir .. "/base/ftbbox.c",
        sourceDir .. "/base/ftgxval.c",
        sourceDir .. "/base/ftlcdfil.c",
        sourceDir .. "/base/ftmm.c",
        sourceDir .. "/base/ftotval.c",
        sourceDir .. "/base/ftpatent.c",
        sourceDir .. "/base/ftpfr.c",
        sourceDir .. "/base/ftsynth.c",
        sourceDir .. "/base/ftfstype.c",
        sourceDir .. "/pcf/pcf.c",
        sourceDir .. "/pfr/pfr.c",
        sourceDir .. "/psaux/psaux.c",
        sourceDir .. "/pshinter/pshinter.c",
        sourceDir .. "/psnames/psmodule.c",
        sourceDir .. "/raster/raster.c",
        sourceDir .. "/sdf/ftbsdf.c",
        sourceDir .. "/sdf/ftsdf.c",
        sourceDir .. "/sdf/ftsdfcommon.c",
        sourceDir .. "/sdf/ftsdfrend.c",
        sourceDir .. "/sdf/sdf.c",
        sourceDir .. "/sfnt/sfnt.c",
        sourceDir .. "/svg/ftsvg.c",
        sourceDir .. "/truetype/truetype.c",
        sourceDir .. "/type1/type1.c",
        sourceDir .. "/cid/type1cid.c",
        sourceDir .. "/type42/type42.c",
        sourceDir .. "/winfonts/winfnt.c",
    }

    if os.target() == "windows" then
        local platformFiles =
        {
            buildsDir .. "/windows/ftdebug.c", -- TODO: Windows only
        }
        Solution.Util.MergeIntoTable(files, platformFiles)
        Solution.Util.MergeIntoTable(defines, { "WIN32", "WIN32_LEAN_AND_MEAN", "VC_EXTRALEAN" } )
    else
       -- TODO Linux
    end

    warnings "Off"
    
    Solution.Util.SetDefines(defines)
    Solution.Util.SetLinks(links)
    Solution.Util.SetFiles(files)
    Solution.Util.SetIncludes(includeDir)
end)

Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()

    local includeDir = dep.Path .. "/freetype/include"

    Solution.Util.SetIncludes(includeDir)
    Solution.Util.SetLinks({ dep.Name })
end)