-- Dependencies
Solution.Util.Print("-- Creating Dependencies --")
Solution.Util.ClearFilter()
Solution.Util.SetGroup(Solution.DependencyGroup)

local dependencies =
{
    "vulkan/vulkan.lua",
    "base64/base64.lua",
    "catch2/catch2.lua",
    "enkits/enkiTS.lua",
    "entt/entt.lua",
    "glfw/glfw.lua",
    "gli/gli.lua",
    "glm/glm.lua",
    "imgui/imgui.lua",
    "json/json.lua",
    "refl-cpp/refl-cpp.lua",
    "robinhood/robinhood.lua",
    "tracyprofiler/tracy.lua",
    "typesafe/typesafe.lua",
    "utfcpp/utfcpp.lua",
    "luau/luau.lua",
    "quill/quill.lua",
    "asio/asio.lua",
    "hwloc/hwloc.lua",
    "freetype/freetype.lua",
    "msdfgen/msdfgen.lua",
    "msdf-atlas-gen/msdf-atlas-gen.lua",
    "rtree/rtree.lua",
    "libsodium/libsodium.lua",
    "spake2-ee/spake2-ee.lua",
    "recastnavigation/recastnavigation.lua",
    "slang/slang.lua"
}

for k,v in pairs(dependencies) do
    include(v)
    Solution.Util.ClearFilter()
end

Solution.Util.SetGroup("")
Solution.Util.Print("-- Finished with Dependencies --\n")