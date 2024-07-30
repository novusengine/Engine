-- Determines the system premake will generate a solution for
local compiler = iif(os.istarget("windows"), "vs2022", "gmake2")
BuildSettings:Add("Solution Type", compiler, true)


BuildSettings:Add("Multithreaded Compilation", true) -- Used to enable multi threaded compilation
BuildSettings:Add("Multithreaded Core Count", 0) -- Used to adjust the allowed number of cores when enabling multi threaded compilation (0 = All) (Assuming the selected build environment supports it)

-- Enables/Disables Tracy
BuildSettings:Add("Enable Tracy", false)

-- Enables/Disables the UnitTest Project

BuildSettings:Add("Build UnitTests", true)

-- Settings for Luau
BuildSettings:Add("Luau Warnings as Errors", false)

-- Settings for EnkiTS
BuildSettings:Add("EnkiTS Num Task Priorities", 3)

-- Settings for Typesafe
BuildSettings:Add("Typesafe Enable Assertions", false)
BuildSettings:Add("Typesafe Enable Precondition Checks", false)
BuildSettings:Add("Typesafe Enable Wrapper", false)
BuildSettings:Add("Typesafe Arithmetic Policy", 0) -- (0 = "default", 1 = "ub", 2 = "checked")

-- Custom (DO NOT TOUCH)
local isMSVC = string.sub(_ACTION, 1, 2) == "vs"
BuildSettings:Add("Using MSVC", isMSVC)