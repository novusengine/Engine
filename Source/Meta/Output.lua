local M = {}

function M.Write(context, path, callback)
    local writer = assert(context.writer, "MetaGen output requires a writer")
    local openOutput = context.openOutput or function(outputPath)
        return assert(io.open(outputPath, "w"))
    end

    local file = openOutput(path)
    if file == nil then error("MetaGen failed to open output '" .. path .. "'", 0) end

    local previousFile = writer.currentFile
    local previousIndent = writer.currentIndent
    writer.currentFile = file

    local succeeded, result = xpcall(callback, debug.traceback)
    local closed, closeError = pcall(function() file:close() end)

    writer.currentFile = previousFile
    writer.currentIndent = previousIndent

    if not succeeded then error(result, 0) end
    if not closed then error("MetaGen failed to close output '" .. path .. "': " .. tostring(closeError), 0) end
end

function M.WriteText(context, path, value)
    M.Write(context, path, function()
        context.writer.currentFile:write(value)
    end)
end

return M
