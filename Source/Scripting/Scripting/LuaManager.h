#pragma once
#include "Defines.h"

#include <Base/Types.h>
#include <Base/Util/StringUtils.h>

#include <Scripting/Zenith.h>

#include <vector>

namespace enki
{
    class TaskSet;
}

namespace Scripting
{
    class LuaManager
    {
    public:
        LuaManager();

        void Init();
        void Update(f32 deltaTime, u64 tick);

        bool ReloadZenith(const ZenithInfoKey& key, Zenith* zenith);
        bool DoString(Zenith* zenith, const std::string& code);

        bool IsDirty() const { return _isDirty; }
        void SetDirty() { _isDirty = true; }
        void ClearDirty() { _isDirty = false; }

        ZenithStateManager& GetZenithStateManager()
        {
            return _zenithStateManager;
        }

        template <typename T>
        T* GetLuaHandler(LuaHandlerID handler)
        {
            u32 index = static_cast<u32>(handler);
            if (index >= _luaHandlers.size())
                return nullptr;

            return reinterpret_cast<T*>(_luaHandlers[index]);
        }

        void PrepareToAddLuaHandlers(u16 numHandlers);

        void SetLuaHandler(LuaHandlerID handlerID, LuaHandlerBase* luaHandler);

    private:
        u32 GetPathDepth(const std::string& rootPath, const std::string& filePath);
        bool LoadScripts();
        bool LoadBytecode(const ZenithInfoKey& key, Zenith* zenith);
        bool NeedsRecompilation() const;

    private:
        bool _isDirty = false;

        u64 _currentTick = 0;
        u64 _lastRecompiledTick = 0;

        std::vector<LuaHandlerBase*> _luaHandlers;
        ZenithStateManager _zenithStateManager;
    };
}