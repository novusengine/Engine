local function Include()
    local includeDir = path.getabsolute("entt/", Engine.dependencyDir)
    AddIncludeDirs(includeDir)
end

CreateDep("entt", Include)