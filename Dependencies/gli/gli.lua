local function Include()
    local includeDir = path.getabsolute("gli/", Engine.dependencyDir)
    AddIncludeDirs(includeDir)
end

CreateDep("gli", Include)