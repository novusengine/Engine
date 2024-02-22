local function Include()
    local includeDir = path.getabsolute("utfcpp/", Engine.dependencyDir)
    AddIncludeDirs(includeDir)
end
CreateDep("utfcpp", Include)