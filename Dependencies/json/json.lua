local function Include()
    local includeDir = path.getabsolute("json/", Engine.dependencyDir)
    AddIncludeDirs(includeDir)
end
CreateDep("json", Include)