local function Include()
    local includePath = path.getabsolute("base64/", Engine.Engine.dependencyDir);
    AddIncludeDirs(includePath)
end

CreateDep("base64", Include)