if BuildSettings:Get("Build UnitTests") == false then return end

local dependencies = { "base", "fileformat", "imgui", "catch2", "catch2-withmain" }
local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }
ProjectTemplate("UnitTests", "ConsoleApp", ".", Engine.binDir, dependencies, defines)