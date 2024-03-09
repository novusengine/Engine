-- Dependencies
Engine.dependencyDir = path.getabsolute("Dependencies/", Engine.rootDir)

print("-- Creating Dependencies --")

Engine.dependencyGroup = (Engine.name .. "/Dependencies")
group (Engine.dependencyGroup)

local dependencies =
{
    "vulkan.lua",
    "base64/base64.lua",
    "catch2/catch2.lua",
    "dxcompiler/dxcompiler.lua",
    "enkiTS/enkiTS.lua",
    "entt/entt.lua",
    "glfw/glfw.lua",
    "gli/gli.lua",
    "glm/glm.lua",
    "imgui/imgui.lua",
    "json/json.lua",
    "refl-cpp/refl-cpp.lua",
    "robinhood/robinhood.lua",
    "spdlog/spdlog.lua",
    "tracyprofiler/tracy.lua",
    "typesafe/typesafe.lua",
    "utfcpp/utfcpp.lua",
    "luau/luau.lua",
}

for k,v in pairs(dependencies) do
    filter { }
    include(v)
end

filter { }
group (Engine.name)

print("-- Finished with Dependencies --\n")