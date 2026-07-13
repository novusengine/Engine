local info = debug.getinfo(1, "S")
local scriptPath = info.source:sub(2)
local root = scriptPath:match("(.*/)")
package.path = root .. "?.lua;" .. package.path

local TestSuite = require("TestSuite")

dofile(root .. "ModelTests.lua")
dofile(root .. "PostgresTests.lua")

newaction
{
    trigger = "metagen-all-tests",
    description = "Run all MetaGen test suites",
    execute = TestSuite.RunAll
}
