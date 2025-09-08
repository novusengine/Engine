#include "LuaManager.h"

#include <Base/CVarSystem/CVarSystem.h>
#include <Base/Memory/Bytebuffer.h>
#include <Base/Memory/FileReader.h>
#include <Base/Util/DebugHandler.h>

#include <Luau/Compiler.h>
#include <Luau/CodeGen.h>

#include <lua.h>
#include <lualib.h>

#include <vector>
#include <filesystem>
namespace fs = std::filesystem;

AutoCVar_String CVAR_ScriptDir(CVarCategory::Client, "scriptingDirectory", "defines the directory from where scripts are loaded", "Data/Scripts");
AutoCVar_String CVAR_ScriptExtension(CVarCategory::Client, "scriptingExtension", "defines the file extension to recognized as a script file", ".luau");
AutoCVar_String CVAR_ScriptMotd(CVarCategory::Client, "scriptingMotd", "defines the message of the day passed in the GameLoaded Event", "Welcome to Novuscore");
AutoCVar_Int CVAR_ScriptNativeCompilation(CVarCategory::Client, "scriptingNativeCompilation", "enables native compilation of luau scripts, may improve performance but increases load times", 0, CVarFlags::EditCheckbox);

namespace Scripting
{
    static i32 lua_require(::lua_State* state)
    {
        lua_getglobal(state, "Zenith");
        if (lua_islightuserdata(state, -1) == 0)
        {
            luaL_error(state, "error requiring module, global 'Zenith' missing");
            return 0;
        }

        void* zenithPtr = lua_touserdata(state, -1);
        lua_pop(state, 1);

        LuaManager* luaManager = reinterpret_cast<LuaManager*>(zenithPtr);
        ZenithStateManager& zenithStateManager = luaManager->GetZenithStateManager();
        ZenithBytecodeCache& bytecodeCache = zenithStateManager.GetBytecodeCache();
        Zenith* zenith = zenithStateManager.Get(state);

        if (zenith->GetTop() != 1)
        {
            luaL_error(state, "error requiring module, invalid number of arguments");
            return 0;
        }

        std::string name = luaL_checkstring(state, 1);

        const char* scriptDirPath = zenith->GetGlobalField<const char*>("path");
        if (!scriptDirPath)
        {
            luaL_error(state, "error requiring module, global 'path' missing");
            return 0;
        }

        if (StringUtils::BeginsWith(name, "@src/"))
        {
            name = name.substr(5);
        }

        std::string scriptPath = std::string(scriptDirPath) + "/" + name + ".luau";
        std::replace(scriptPath.begin(), scriptPath.end(), '\\', '/');
        zenith->Pop();

        if (!fs::exists(scriptPath))
        {
            luaL_error(state, "error requiring module, script not found");
            return 0;
        }

        u32 moduleHash = StringUtils::fnv1a_32(scriptPath.c_str(), scriptPath.size());
        if (!bytecodeCache.apiHashToBytecodeIndex.contains(moduleHash))
        {
            luaL_error(state, "error requiring module, bytecode not found");
            return 0;
        }

        // Stack: Name, ModuleProxy
        zenith->CreateTable();

        // Stack: Name, ModuleProxy, __ImportedModules__
        zenith->GetGlobalKey("__ImportedModules__");

        // Stack: Name, ModuleProxy, __ImportedModules__, Module
        if (!zenith->GetTableField(scriptPath.c_str()))
        {
            bool enableNativeCompilation = CVAR_ScriptNativeCompilation.Get() != 0;

            u32 bytecodeIndex = bytecodeCache.apiHashToBytecodeIndex[moduleHash];
            ZenithBytecodeEntry& bytecodeEntry = bytecodeCache.apiBytecodeList[bytecodeIndex];

            i32 result = zenith->LoadBytecode(bytecodeEntry.filePath, bytecodeEntry.bytecode, 0);
            if (result != LUA_OK)
            {
                lua_error(zenith->state);
                return false;
            }

            if (enableNativeCompilation)
            {
                Luau::CodeGen::CompilationResult codeGenResult = Luau::CodeGen::compile(state, -1, 0);
                if (codeGenResult.hasErrors() && codeGenResult.result != Luau::CodeGen::CodeGenCompilationResult::NothingToCompile)
                    return false;
            }

            if (!zenith->PCall(0, 1))
                return false;

            // Stack: Name, ModuleProxy, __ImportedModules__, Module
            if (zenith->GetTop() == 0)
            {
                luaL_error(state, "error requiring module, module does not return a value");
                return 0;
            }
            else if (!lua_istable(state, -1))
            {
                luaL_error(state, "error requiring module, module must return a table");
                return 0;
            }

            // Stack: Name, ModuleProxy, __ImportedModules__, Module, Module
            zenith->PushValue();

            // Stack: Name, ModuleProxy, __ImportedModules__, Module
            zenith->SetTableKey("__index");

            // Set the ModuleTable as readonly
            lua_setreadonly(state, -1, true);

            // Add Table to __ImportedModules__
            // Stack: Name, ModuleProxy, __ImportedModules__
            zenith->SetTableKey(scriptPath.c_str());

            // Push Table to the stack
            zenith->GetTableField(scriptPath.c_str());

            zenith->loadedModuleHashes.insert(moduleHash);
        }

        // Stack: Name, ModuleProxy, __ImportedModules__
        lua_setmetatable(state, -3);

        // Stack: Name, ModuleProxy
        zenith->Pop();

        // Stack: ModuleProxy
        lua_replace(state, -2);

        return 1;
    }

    LuaManager::LuaManager()
    {
        _luaHandlers.reserve(32);
    }

    void LuaManager::Init()
    {
        _currentTick = 0;
    }

    void LuaManager::Update(f32 deltaTime, u64 tick)
    {
        _currentTick = tick;

        bool isLuaManagerDirty = IsDirty();
        if (isLuaManagerDirty)
            ClearDirty();

        u32 numHandlers = static_cast<u32>(_luaHandlers.size());
        bool failedToCompile = false;

        for (auto& [key, zenith] : _zenithStateManager)
        {
            bool result = true;
            bool isStateDirty = zenith->IsDirty();
            bool isDirty = !failedToCompile && (isLuaManagerDirty || zenith->IsDirty());

            // If we previously failed to compile, we want to keep the state as it is
            if (failedToCompile && isStateDirty)
                zenith->ClearDirty();

            if (isDirty)
            {
                bool loadedScripts = true;
                bool reloadedZenith = false;

                // Recompile scripts if needed
                if (NeedsRecompilation())
                    loadedScripts = LoadScripts();

                // Reload Zenith
                if (loadedScripts)
                    reloadedZenith = ReloadZenith(key, zenith.get());

                // If we failed to load scripts or reload Zenith, we want to mark that we failed to compile
                failedToCompile = !(loadedScripts && reloadedZenith);
            }

            for (u32 i = 0; i < numHandlers; i++)
            {
                LuaHandlerBase* luaHandler = _luaHandlers[i];

                if (isDirty)
                    luaHandler->PostLoad(zenith.get());

                luaHandler->Update(zenith.get(), deltaTime);
            }
        }
    }

    bool LuaManager::ReloadZenith(const ZenithInfoKey& key, Zenith* zenith)
    {
        u32 numHandlers = static_cast<u32>(_luaHandlers.size());
        for (u32 i = 0; i < numHandlers; i++)
        {
            LuaHandlerBase* luaHandler = _luaHandlers[i];
            luaHandler->Clear(zenith);
        }

        bool result = LoadBytecode(key, zenith);
        zenith->ClearDirty();

        return result;
    }

    bool LuaManager::DoString(Zenith* zenith, const std::string& code)
    {
        Luau::CompileOptions compileOptions;
        {
            compileOptions.optimizationLevel = 1;
            compileOptions.debugLevel = 2;
            compileOptions.typeInfoLevel = 1;
            compileOptions.coverageLevel = 2;
        }

        Luau::ParseOptions parseOptions;
        
        std::string bytecode = Luau::compile(code, compileOptions, parseOptions);
        i32 result = zenith->LoadBytecode("", bytecode, 0);
        if (result != LUA_OK)
        {
            zenith->ReportError();
            return false;
        }
        
        bool success = zenith->PCall(0, 0);
        return success;
    }

    void LuaManager::PrepareToAddLuaHandlers(u16 numHandlers)
    {
        _luaHandlers.resize(numHandlers, nullptr);
    }

    void LuaManager::SetLuaHandler(LuaHandlerID handlerID, LuaHandlerBase* luaHandler)
    {
        u16 index = static_cast<u16>(handlerID);
        _luaHandlers[index] = luaHandler;
    }

    u32 LuaManager::GetPathDepth(const std::string& rootPath, const std::string& filePath)
    {
        std::string relativePath = filePath.substr(rootPath.length());

        u32 depth = 0;
        for (char c : relativePath)
        {
            // Handle both forward and backslashes
            if (c == '/' || c == '\\')
            {
                depth++;
            }
        }

        return depth;
    }

    bool LuaManager::LoadScripts()
    {
        const char* scriptDir = CVAR_ScriptDir.Get();
        const char* scriptExtension = CVAR_ScriptExtension.Get();
        bool enableNativeCompilation = CVAR_ScriptNativeCompilation.Get() != 0;

        fs::path scriptDirectory = fs::absolute(scriptDir);
        fs::path scriptAPIDirectory = scriptDirectory / "API";
        fs::path scriptBootstrapDirectory = scriptDirectory / "Bootstrap";
        std::string scriptDirectoryAsString = scriptDirectory.string();
        std::string scriptAPIDirectoryAsString = scriptAPIDirectory.string();
        std::string scriptBootstrapDirectoryAsString = scriptBootstrapDirectory.string();

        if (!fs::exists(scriptDirectory))
            fs::create_directories(scriptDirectory);

        if (!fs::exists(scriptAPIDirectory))
            fs::create_directories(scriptAPIDirectory);

        if (!fs::exists(scriptBootstrapDirectory))
            fs::create_directories(scriptBootstrapDirectory);

        ZenithBytecodeCache bytecodeCache;

        Zenith zenith;
        zenith.SetState(luaL_newstate());

        if (enableNativeCompilation)
            Luau::CodeGen::create(zenith.state);

        zenith.RegisterDefaultLibraries();
        zenith.AddGlobalField("path", scriptDirectoryAsString.c_str());
        zenith.AddGlobalField("require", lua_require);

        for (u32 i = 0; i < _luaHandlers.size(); i++)
        {
            LuaHandlerBase* base = _luaHandlers[i];
            base->Register(&zenith);
        }

        luaL_sandbox(zenith.state);
        luaL_sandboxthread(zenith.state);

        zenith.CreateTable("__ImportedModules__");
        zenith.Pop();

        // TODO : Figure out if this catches hidden folders, and if so exclude them
        // TODO : Should we use a custom file extension for "include" files? Force load any files that for example use ".ext"
        std::vector<std::pair<u32, fs::path>> paths;
        std::vector<std::pair<u32, fs::path>> apiPaths;
        std::vector<std::pair<u32, fs::path>> bootstrapPaths;
        paths.reserve(1024);
        apiPaths.reserve(1024);
        bootstrapPaths.reserve(1024);

        fs::recursive_directory_iterator fsScriptAPIDir { scriptAPIDirectory };
        for (const auto& dirEntry : fsScriptAPIDir)
        {
            if (!dirEntry.is_regular_file())
                continue;

            fs::path path = dirEntry.path();
            if (path.extension() != scriptExtension)
                continue;

            std::string pathStr = path.string();
            u32 depth = GetPathDepth(scriptAPIDirectoryAsString, pathStr);
            apiPaths.push_back({ depth, path });
        }
        std::sort(apiPaths.begin(), apiPaths.end());

        fs::recursive_directory_iterator fsScriptBootstrapDir { scriptBootstrapDirectory };
        for (const auto& dirEntry : fsScriptBootstrapDir)
        {
            if (!dirEntry.is_regular_file())
                continue;

            fs::path path = dirEntry.path();
            if (path.extension() != scriptExtension)
                continue;

            std::string pathStr = path.string();
            u32 depth = GetPathDepth(scriptBootstrapDirectoryAsString, pathStr);
            bootstrapPaths.push_back({ depth, path });
        }
        std::sort(bootstrapPaths.begin(), bootstrapPaths.end());

        fs::recursive_directory_iterator fsScriptDir { scriptDirectory };
        for (const auto& dirEntry : fsScriptDir)
        {
            if (!dirEntry.is_regular_file())
                continue;

            fs::path path = dirEntry.path();
            if (path.extension() != scriptExtension)
                continue;

            fs::path relativePath = fs::relative(path, scriptDirectory);
            std::string relativePathAsString = relativePath.string();
            std::replace(relativePathAsString.begin(), relativePathAsString.end(), '\\', '/');

            if (StringUtils::BeginsWith(relativePathAsString, "API/") || StringUtils::BeginsWith(relativePathAsString, "Bootstrap/"))
                continue;

            std::string pathStr = path.string();
            u32 depth = GetPathDepth(scriptBootstrapDirectoryAsString, pathStr);
            paths.push_back({ depth, path });
        }
        std::sort(paths.begin(), paths.end());

        Luau::CompileOptions compileOptions;
        {
            compileOptions.optimizationLevel = 1;
            compileOptions.debugLevel = 2;
            compileOptions.typeInfoLevel = 1;
            compileOptions.coverageLevel = 2;
        }
        Luau::ParseOptions parseOptions;

        bool didFail = false;

        for (auto& path : apiPaths)
        {
            const std::string pathAsStr = path.second.string();
            FileReader reader(pathAsStr);

            if (!reader.Open())
                continue;

            u32 bufferSize = static_cast<u32>(reader.Length());
            std::shared_ptr<Bytebuffer> buffer = Bytebuffer::BorrowRuntime(bufferSize);
            reader.Read(buffer.get(), bufferSize);

            std::string luaCode;
            luaCode.resize(bufferSize);

            if (!buffer->GetString(luaCode, bufferSize))
                continue;

            fs::path relPath = fs::relative(path.second, scriptDirectory);
            std::string scriptPath = scriptDirectoryAsString + "/" + relPath.string();
            std::replace(scriptPath.begin(), scriptPath.end(), '\\', '/');

            StringUtils::StringHash pathHash = StringUtils::fnv1a_32(scriptPath.c_str(), scriptPath.size());

            ZenithBytecodeEntry bytecodeEntry
            {
                pathHash,
                path.second.filename().string(),
                pathAsStr,
                Luau::compile(luaCode, compileOptions, parseOptions)
            };

            u32 index = static_cast<u32>(bytecodeCache.apiBytecodeList.size());
            bytecodeCache.apiBytecodeList.push_back(bytecodeEntry);
            bytecodeCache.apiHashToBytecodeIndex[pathHash] = index;
        }

        for (auto& path : bootstrapPaths)
        {
            const std::string pathAsStr = path.second.string();
            FileReader reader(pathAsStr);

            if (!reader.Open())
                continue;

            u32 bufferSize = static_cast<u32>(reader.Length());
            std::shared_ptr<Bytebuffer> buffer = Bytebuffer::BorrowRuntime(bufferSize);
            reader.Read(buffer.get(), bufferSize);

            std::string luaCode;
            luaCode.resize(bufferSize);

            if (!buffer->GetString(luaCode, bufferSize))
                continue;

            fs::path relPath = fs::relative(path.second, scriptDirectory);
            std::string scriptPath = scriptDirectoryAsString + "/" + relPath.string();
            std::replace(scriptPath.begin(), scriptPath.end(), '\\', '/');

            StringUtils::StringHash pathHash = StringUtils::fnv1a_32(scriptPath.c_str(), scriptPath.size());

            ZenithBytecodeEntry bytecodeEntry
            {
                pathHash,
                path.second.filename().string(),
                pathAsStr,
                Luau::compile(luaCode, compileOptions, parseOptions)
            };

            u32 index = static_cast<u32>(bytecodeCache.bytecodeList.size());
            bytecodeCache.bytecodeList.push_back(bytecodeEntry);
        }

        for (auto& path : paths)
        {
            const std::string pathAsStr = path.second.string();
            FileReader reader(pathAsStr);

            if (!reader.Open())
                continue;

            u32 bufferSize = static_cast<u32>(reader.Length());
            std::shared_ptr<Bytebuffer> buffer = Bytebuffer::BorrowRuntime(bufferSize);
            reader.Read(buffer.get(), bufferSize);

            std::string luaCode;
            luaCode.resize(bufferSize);

            if (!buffer->GetString(luaCode, bufferSize))
                continue;

            fs::path relPath = fs::relative(path.second, scriptDirectory);
            std::string scriptPath = scriptDirectoryAsString + "/" + relPath.string();
            std::replace(scriptPath.begin(), scriptPath.end(), '\\', '/');

            StringUtils::StringHash pathHash = StringUtils::fnv1a_32(scriptPath.c_str(), scriptPath.size());

            ZenithBytecodeEntry bytecodeEntry
            {
                pathHash,
                path.second.filename().string(),
                pathAsStr,
                Luau::compile(luaCode, compileOptions, parseOptions)
            };

            u32 index = static_cast<u32>(bytecodeCache.bytecodeList.size());
            bytecodeCache.bytecodeList.push_back(bytecodeEntry);
        }

        if (!didFail)
        {
            u32 numScriptsToLoad = static_cast<u32>(bytecodeCache.bytecodeList.size());

            for (u32 i = 0; i < numScriptsToLoad; i++)
            {
                ZenithBytecodeEntry& bytecodeEntry = bytecodeCache.bytecodeList[i];

                bool isLoaded = zenith.loadedModuleHashes.contains(bytecodeEntry.hash);
                if (isLoaded)
                    continue;

                i32 result = zenith.LoadBytecode(bytecodeEntry.filePath, bytecodeEntry.bytecode, 0);
                if (result != LUA_OK)
                {
                    std::string error = lua_tostring(zenith.state, -1);

                    NC_LOG_ERROR("Failed to Compile script : {0}\n{1}", bytecodeEntry.filePath, error);
                    didFail = true;
                    break;
                }

                if (enableNativeCompilation)
                {
                    Luau::CodeGen::CompilationResult codeGenResult = Luau::CodeGen::compile(zenith.state, -1, 0);
                    if (codeGenResult.hasErrors() && codeGenResult.result != Luau::CodeGen::CodeGenCompilationResult::NothingToCompile)
                    {
                        didFail = true;
                        break;
                    }
                }
            }
        }

        _lastRecompiledTick = _currentTick;

        if (!didFail)
            _zenithStateManager.GetBytecodeCache() = std::move(bytecodeCache);

        zenith.Clear();
        return !didFail;
    }

    bool LuaManager::LoadBytecode(const ZenithInfoKey& key, Zenith* zenith)
    {
        const char* scriptDir = CVAR_ScriptDir.Get();
        bool enableNativeCompilation = CVAR_ScriptNativeCompilation.Get() != 0;

        fs::path scriptDirectory = fs::absolute(scriptDir);
        std::string scriptDirectoryAsString = scriptDirectory.string();

        const ZenithBytecodeCache& bytecodeCache = _zenithStateManager.GetBytecodeCache();

        // Special Key used for loading bytecode without overwriting the current state
        ZenithInfoKey tmpKey = ZenithInfoKey::Make(key.GetMapID(), key.GetInstanceID(), std::numeric_limits<u16>().max());
        lua_State* state = luaL_newstate();
        _zenithStateManager.Add(tmpKey);
        _zenithStateManager.Add(tmpKey, state);

        Zenith* tmpZenith = _zenithStateManager.Get(tmpKey);
        tmpZenith->SetState(state);

        if (enableNativeCompilation)
            Luau::CodeGen::create(tmpZenith->state);

        tmpZenith->RegisterDefaultLibraries();
        tmpZenith->AddGlobalField("path", scriptDirectoryAsString.c_str());
        tmpZenith->AddGlobalField("require", lua_require);

        tmpZenith->PushLightUserData(this);
        tmpZenith->SetGlobalKey("Zenith");

        for (u32 i = 0; i < _luaHandlers.size(); i++)
        {
            LuaHandlerBase* base = _luaHandlers[i];
            base->Register(tmpZenith);
        }

        luaL_sandbox(tmpZenith->state);
        luaL_sandboxthread(tmpZenith->state);

        tmpZenith->CreateTable("__ImportedModules__");
        tmpZenith->Pop();

        bool failed = false;
        u32 numScriptsToLoad = static_cast<u32>(bytecodeCache.bytecodeList.size());

        for (u32 i = 0; i < numScriptsToLoad; i++)
        {
            const ZenithBytecodeEntry& bytecodeEntry = bytecodeCache.bytecodeList[i];

            bool isLoaded = tmpZenith->loadedModuleHashes.contains(bytecodeEntry.hash);
            if (isLoaded)
                continue;

            tmpZenith->LoadBytecode(bytecodeEntry.filePath, bytecodeEntry.bytecode, 0);

            if (enableNativeCompilation)
                Luau::CodeGen::CompilationResult codeGenResult = Luau::CodeGen::compile(tmpZenith->state, -1, 0);

            i32 status = tmpZenith->Resume();
            if (status != LUA_OK)
            {
                std::string error = (status == LUA_YIELD) ? "thread yielded unexpectedly" : lua_tostring(tmpZenith->state, -1);
                error += "\nstacktrace:\n";
                error += lua_debugtrace(tmpZenith->state);
            
                NC_LOG_ERROR("Failed to load script : {0}\n{1}", bytecodeEntry.filePath, error);
                failed = true;
                break;
            }

            tmpZenith->loadedModuleHashes.insert(bytecodeEntry.hash);
        }

        // Remove the temporary state -> key mapping
        _zenithStateManager.Remove(tmpZenith->state);

        if (!failed)
        {
            // If the current zenith has a state, we need to remove the state -> key mapping
            if (zenith->state != nullptr)
                _zenithStateManager.Remove(zenith->state);

            // Prepare to move the temporary zenith state to the current zenith
            tmpZenith->ClearState();
            zenith->Clear();

            // Move the temporary zenith state to the current zenith
            zenith->SetState(state);
            zenith->eventState = std::move(tmpZenith->eventState);
            zenith->loadedModuleHashes = std::move(tmpZenith->loadedModuleHashes);

            // Add the new state -> key mapping
            _zenithStateManager.Add(key, zenith->state);
        }

        // Remove the temporary zenith state (Will be cleaned up by the state manager)
        _zenithStateManager.Remove(tmpKey);

        return !failed;
    }

    bool LuaManager::NeedsRecompilation() const
    {
        return _currentTick > _lastRecompiledTick;
    }
}