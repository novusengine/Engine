local function Include()
    local includePath = path.getabsolute("base64/", Engine.dependencyDir);
    AddIncludeDirs(includePath)
end

CreateDep("base64", Include)