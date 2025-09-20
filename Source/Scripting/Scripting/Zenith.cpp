#include "Zenith.h"

#include <Base/Util/DebugHandler.h>

namespace Scripting
{
    void Zenith::SetState(lua_State* newState)
    {
        NC_ASSERT(state == nullptr && newState != nullptr, "Invalid SetState call on Zenith");
        state = newState;
    }

    void Zenith::Clear()
    {
        if (state != nullptr)
            lua_close(state);

        _isDirty = false;

        state = nullptr;
        eventState.Clear();
        loadedModuleHashes.clear();
    }

    i32 Zenith::GetStatus()
    {
        return lua_status(state);
    }

    i32 Zenith::GetTop()
    {
        return lua_gettop(state);
    }

    void Zenith::SetTop(i32 index)
    {
        lua_settop(state, index);
    }

    void Zenith::GetRaw(i32 index)
    {
        lua_rawget(state, index);
    }

    void Zenith::GetRawI(i32 index, i32 n)
    {
        lua_rawgeti(state, index, n);
    }

    i32 Zenith::GetRef(i32 index)
    {
        return lua_ref(state, index);
    }

    void Zenith::GetGlobalKey(const char* key)
    {
        lua_getglobal(state, key);
    }
    template<> bool Zenith::GetGlobalField<bool>(const char* key)
    {
        GetGlobalKey(key);
        return Get<bool>(-1);
    }
    template<> i32 Zenith::GetGlobalField<i32>(const char* key)
    {
        GetGlobalKey(key);
        return Get<i32>(-1);
    }
    template<> u32 Zenith::GetGlobalField<u32>(const char* key)
    {
        GetGlobalKey(key);
        return Get<u32>(-1);
    }
    template<> f32 Zenith::GetGlobalField<f32>(const char* key)
    {
        GetGlobalKey(key);
        return Get<f32>(-1);
    }
    template<> f64 Zenith::GetGlobalField<f64>(const char* key)
    {
        GetGlobalKey(key);
        return Get<f64>(-1);
    }
    template<> const char* Zenith::GetGlobalField<const char*>(const char* key)
    {
        GetGlobalKey(key);
        return Get<const char*>(-1);
    }
    template<> vec3 Zenith::GetGlobalField<vec3>(const char* key)
    {
        GetGlobalKey(key);
        return Get<vec3>(-1);
    }

    void Zenith::SetGlobalKey(const char* key)
    {
        lua_setglobal(state, key);
    }
    void Zenith::AddGlobalField(const char* key, bool value)
    {
        Push(value);
        lua_setglobal(state, key);
    }
    void Zenith::AddGlobalField(const char* key, i32 value)
    {
        Push(value);
        lua_setglobal(state, key);
    }
    void Zenith::AddGlobalField(const char* key, u32 value)
    {
        Push(value);
        lua_setglobal(state, key);
    }
    void Zenith::AddGlobalField(const char* key, f32 value)
    {
        Push(value);
        lua_setglobal(state, key);
    }
    void Zenith::AddGlobalField(const char* key, f64 value)
    {
        Push(value);
        lua_setglobal(state, key);
    }
    void Zenith::AddGlobalField(const char* key, const char* value)
    {
        Push(value);
        lua_setglobal(state, key);
    }
    void Zenith::AddGlobalField(const char* key, const vec3& value)
    {
        Push(value);
        lua_setglobal(state, key);
    }
    void Zenith::AddGlobalField(const char* key, const lua_CFunction value)
    {
        Push(value);
        lua_setglobal(state, key);
    }
    void Zenith::SetField(const char* key, i32 index)
    {
        lua_setfield(state, index, key);
    }

    bool Zenith::IsNil(i32 index)
    {
        return lua_isnoneornil(state, index);
    }
    bool Zenith::IsBoolean(i32 index)
    {
        return lua_isboolean(state, index);
    }
    bool Zenith::IsNumber(i32 index)
    {
        return lua_isnumber(state, index);
    }
    bool Zenith::IsString(i32 index)
    {
        return lua_isstring(state, index);
    }
    bool Zenith::IsVector(i32 index)
    {
        return lua_isvector(state, index);
    }
    bool Zenith::IsFunction(i32 index)
    {
        return lua_isfunction(state, index);
    }
    bool Zenith::IsTable(i32 index)
    {
        return lua_istable(state, index);
    }
    bool Zenith::IsUserData(i32 index)
    {
        return lua_isuserdata(state, index);
    }
    bool Zenith::IsLightUserData(i32 index)
    {
        return lua_islightuserdata(state, index);
    }

    bool Zenith::ToBoolean(i32 index)
    {
        return lua_toboolean(state, index) != 0;
    }
    i32 Zenith::ToInteger(i32 index)
    {
        return lua_tointeger(state, index);
    }
    u32 Zenith::ToUnsigned(i32 index)
    {
        return lua_tounsigned(state, index);
    }
    f64 Zenith::ToNumber(i32 index)
    {
        return lua_tonumber(state, index);
    }
    const char* Zenith::ToString(i32 index)
    {
        return lua_tostring(state, index);
    }
    vec3 Zenith::ToVector(i32 index)
    {
        const f32* vec = lua_tovector(state, index);

        vec3 result = vec3(vec[0], vec[1], vec[2]);
        return result;
    }
    lua_CFunction Zenith::ToCFunction(i32 index)
    {
        return lua_tocfunction(state, index);
    }
    void* Zenith::ToUserData(i32 index)
    {
        return lua_touserdata(state, index);
    }
    void* Zenith::ToLightUserData(i32 index)
    {
        return lua_tolightuserdata(state, index);
    }

    template<typename T>
    void Zenith::Push(const T& value)
    {
        static_assert(sizeof(T) == 0, "Push is not supported for this type");
    }

    template<>
    void Zenith::Push<vec2>(const vec2& value)
    {
        lua_pushvector(state, value.x, value.y, 0.0f);
    }

    template<>
    void Zenith::Push<ivec2>(const ivec2& value)
    {
        lua_pushvector(state, (f32)value.x, (f32)value.y, 0.0f);
    }

    template<>
    void Zenith::Push<uvec2>(const uvec2& value)
    {
        lua_pushvector(state, (f32)value.x, (f32)value.y, 0.0f);
    }

    template<>
    void Zenith::Push<vec3>(const vec3& value)
    {
        lua_pushvector(state, value.x, value.y, value.z);
    }

    template<>
    void Zenith::Push<ivec3>(const ivec3& value)
    {
        lua_pushvector(state, (f32)value.x, (f32)value.y, (f32)value.z);
    }

    template<>
    void Zenith::Push<uvec3>(const uvec3& value)
    {
        lua_pushvector(state, (f32)value.x, (f32)value.y, (f32)value.z);
    }

    // Specialization for std::string
    template<>
    void Zenith::Push<std::string>(const std::string& value)
    {
        lua_pushstring(state, value.c_str());
    }
    void Zenith::Push()
    {
        lua_pushnil(state);
    }
    void Zenith::Push(bool value)
    {
        lua_pushboolean(state, value);
    }
    void Zenith::Push(i8 value)
    {
        lua_pushinteger(state, (i32)value);
    }
    void Zenith::Push(i16 value)
    {
        lua_pushinteger(state, (i32)value);
    }
    void Zenith::Push(i32 value)
    {
        lua_pushinteger(state, value);
    }
    void Zenith::Push(i64 value)
    {
        f64 number = static_cast<f64>(value);
        Push(number);
    }
    void Zenith::Push(u8 value)
    {
        lua_pushunsigned(state, (u32)value);
    }
    void Zenith::Push(u16 value)
    {
        lua_pushunsigned(state, (u32)value);
    }
    void Zenith::Push(u32 value)
    {
        lua_pushunsigned(state, value);
    }
    void Zenith::Push(u64 value)
    {
        f64 number = static_cast<f64>(value);
        Push(number);
    }
    void Zenith::Push(f32 value)
    {
        lua_pushnumber(state, value);
    }
    void Zenith::Push(f64 value)
    {
        lua_pushnumber(state, value);
    }
    void Zenith::Push(const char* value)
    {
        lua_pushstring(state, value);
    }
    void Zenith::Push(lua_CFunction value, const char* debugName)
    {
        lua_pushcfunction(state, value, debugName);
    }
    void Zenith::PushValue(i32 index)
    {
        lua_pushvalue(state, index);
    }
    void Zenith::Pop(i32 numPops /*= 1*/)
    {
        lua_pop(state, numPops);
    }

    template<> bool Zenith::Get<bool>(i32 index)
    {
        return ToBoolean(index);
    }
    template<> i32 Zenith::Get<i32>(i32 index)
    {
        return ToInteger(index);
    }
    template<> u32 Zenith::Get<u32>(i32 index)
    {
        return ToUnsigned(index);
    }
    template<> f32 Zenith::Get<f32>(i32 index)
    {
        return static_cast<f32>(ToNumber(index));
    }
    template<> f64 Zenith::Get<f64>(i32 index)
    {
        return ToNumber(index);
    }
    template<> const char* Zenith::Get<const char*>(i32 index)
    {
        return ToString(index);
    }
    template<> vec3 Zenith::Get<vec3>(i32 index)
    {
        return ToVector(index);
    }

    static i64 CheckSigned(Zenith* zenith, i32 index, i64 min, i64 max)
    {
        f64 value = luaL_checknumber(zenith->state, index);
        if (value < min || value > max)
            luaL_error(zenith->state, "signed integer expected, got %f (Must be between %u...%u)", value, min, max);

        i64 result = static_cast<i64>(value);
        return result;
    }

    static u64 CheckUnsigned(Zenith* zenith, i32 index, u64 max)
    {
        f64 value = luaL_checknumber(zenith->state, index);
        if (value < 0 || value > max)
            luaL_error(zenith->state, "unsigned integer expected, got %f (Must be between 0...%u)", value, max);

        u64 result = static_cast<u64>(value);
        return result;
    }

    template<> bool Zenith::CheckVal<bool>(i32 index)
    {
        return ToBoolean(index);
    }
    template<> i8 Zenith::CheckVal<i8>(i32 index)
    {
        return (i8)CheckSigned(this, index, std::numeric_limits<i8>().min(), std::numeric_limits<i8>().max());
    }
    template<> i16 Zenith::CheckVal<i16>(i32 index)
    {
        return (i16)CheckSigned(this, index, std::numeric_limits<i16>().min(), std::numeric_limits<i16>().max());
    }
    template<> i32 Zenith::CheckVal<i32>(i32 index)
    {
        return (i32)CheckSigned(this, index, std::numeric_limits<i32>().min(), std::numeric_limits<i32>().max());
    }
    template<> i64 Zenith::CheckVal<i64>(i32 index)
    {
        return CheckSigned(this, index, std::numeric_limits<i64>().min(), std::numeric_limits<i64>().max());
    }
    template<> u8 Zenith::CheckVal<u8>(i32 index)
    {
        return (u8)CheckUnsigned(this, index, std::numeric_limits<u8>().max());
    }
    template<> u16 Zenith::CheckVal<u16>(i32 index)
    {
        return (u16)CheckUnsigned(this, index, std::numeric_limits<u16>().max());
    }
    template<> u32 Zenith::CheckVal<u32>(i32 index)
    {
        return (u32)CheckUnsigned(this, index, std::numeric_limits<u32>().max());
    }
    template<> u64 Zenith::CheckVal<u64>(i32 index)
    {
        return CheckUnsigned(this, index, std::numeric_limits<u64>().max());
    }
    template<> f32 Zenith::CheckVal<f32>(i32 index)
    {
        return (f32)luaL_checknumber(state, index);
    }
    template<> f64 Zenith::CheckVal<f64>(i32 index)
    {
        return luaL_checknumber(state, index);
    }
    template<> const char* Zenith::CheckVal<const char*>(i32 index)
    {
        return luaL_checkstring(state, index);
    }
    template<> vec3 Zenith::CheckVal<vec3>(i32 index)
    {
        const f32* vec = luaL_checkvector(state, index);
        return vec3(vec[0], vec[1], vec[2]);
    }

    void Zenith::CreateTable()
    {
        lua_newtable(state);
    }
    void Zenith::CreateTable(const char* name)
    {
        NC_ASSERT(name, "Name is null");

        CreateTable();

        PushValue(-1);
        SetGlobalKey(name);
    }
    void Zenith::CreateMetaTable(const char* name)
    {
        luaL_newmetatable(state, name);
    }

    bool Zenith::GetTableField(const char* key, i32 index)
    {
        if (!IsTable(index))
        {
            return false;
        }

        index += (-1 * (index < 0));

        lua_pushstring(state, key);
        lua_gettable(state, index);

        if (IsNil(-1))
        {
            lua_pop(state, 1);
            return false;
        }

        return true;
    }
    void Zenith::SetTableKey(i32 key)
    {
        lua_rawseti(state, -2, key);
    }
    void Zenith::SetTableKey(const char* key)
    {
        SetField(key);
    }
    void Zenith::AddTableField(const char* key, const bool value)
    {
        Push(value);
        SetField(key);
    }
    void Zenith::AddTableField(const char* key, const i8 value)
    {
        Push(value);
        SetField(key);
    }
    void Zenith::AddTableField(const char* key, const i16 value)
    {
        Push(value);
        SetField(key);
    }
    void Zenith::AddTableField(const char* key, const i32 value)
    {
        Push(value);
        SetField(key);
    }
    void Zenith::AddTableField(const char* key, const i64 value)
    {
        Push(value);
        SetField(key);
    }
    void Zenith::AddTableField(const char* key, const u8 value)
    {
        Push(value);
        SetField(key);
    }
    void Zenith::AddTableField(const char* key, const u16 value)
    {
        Push(value);
        SetField(key);
    }
    void Zenith::AddTableField(const char* key, const u32 value)
    {
        Push(value);
        SetField(key);
    }
    void Zenith::AddTableField(const char* key, const u64 value)
    {
        Push(value);
        SetField(key);
    }
    void Zenith::AddTableField(const char* key, const f32 value)
    {
        Push(value);
        SetField(key);
    }
    void Zenith::AddTableField(const char* key, const f64 value)
    {
        Push(value);
        SetField(key);
    }
    void Zenith::AddTableField(const char* key, const char* value)
    {
        Push(value);
        SetField(key);
    }
    void Zenith::AddTableField(const char* key, const vec3& value)
    {
        Push(value);
        SetField(key);
    }
    void Zenith::AddTableField(const char* key, const lua_CFunction value)
    {
        Push(value);
        SetField(key);
    }
    void Zenith::AddTableField(i32 key, const bool value)
    {
        Push(value);
        SetTableKey(key);
    }
    void Zenith::AddTableField(i32 key, const i8 value)
    {
        Push(value);
        SetTableKey(key);
    }
    void Zenith::AddTableField(i32 key, const i16 value)
    {
        Push(value);
        SetTableKey(key);
    }
    void Zenith::AddTableField(i32 key, const i32 value)
    {
        Push(value);
        SetTableKey(key);
    }
    void Zenith::AddTableField(i32 key, const i64 value)
    {
        Push(value);
        SetTableKey(key);
    }
    void Zenith::AddTableField(i32 key, const u8 value)
    {
        Push(value);
        SetTableKey(key);
    }
    void Zenith::AddTableField(i32 key, const u16 value)
    {
        Push(value);
        SetTableKey(key);
    }
    void Zenith::AddTableField(i32 key, const u32 value)
    {
        Push(value);
        SetTableKey(key);
    }
    void Zenith::AddTableField(i32 key, const u64 value)
    {
        Push(value);
        SetTableKey(key);
    }
    void Zenith::AddTableField(i32 key, const f32 value)
    {
        Push(value);
        SetTableKey(key);
    }
    void Zenith::AddTableField(i32 key, const f64 value)
    {
        Push(value);
        SetTableKey(key);
    }
    void Zenith::AddTableField(i32 key, const char* value)
    {
        Push(value);
        SetTableKey(key);
    }
    void Zenith::AddTableField(i32 key, const vec3& value)
    {
        Push(value);
        SetTableKey(key);
    }
    void Zenith::AddTableField(i32 key, const lua_CFunction value)
    {
        Push(value);
        SetTableKey(key);
    }

    bool Zenith::PCall(i32 numArgs /*= 0*/, i32 numResults /*= 0*/, i32 errorfunc /*= 0*/)
    {
        i32 result = lua_pcall(state, numArgs, numResults, errorfunc);
        if (result != LUA_OK)
        {
            NC_LOG_ERROR("[Scripting] Failed to run a script. Please check the errors below and correct them");
            NC_LOG_ERROR("{0}", Get<const char*>(-1));
            Pop();
        }

        if (result == LUA_YIELD)
            result = lua_resume(state, nullptr, 0);

        return result == LUA_OK;
    }

    i32 Zenith::LoadBytecode(const std::string& chunkName, const std::string& bytecode, i32 env)
    {
        return luau_load(state, chunkName.c_str(), bytecode.c_str(), bytecode.size(), env);
    }
    i32 Zenith::Resume(lua_State* from, i32 nArg)
    {
        return lua_resume(state, from, nArg);
    }

    void Zenith::MakeReadOnly()
    {
        luaL_sandbox(state);
    }

    void Zenith::ReportError()
    {
        NC_LOG_ERROR("[Scripting] Please check the errors below and correct them");
        NC_LOG_ERROR("{0}", Get<const char*>(-1));
        Pop();
    }

    void Zenith::Close()
    {
        lua_close(state);
    }

    void Zenith::RegisterDefaultLibraries()
    {
        luaL_openlibs(state);
    }

    void* Zenith::AllocateUserData(lua_State* state, size_t size, LuaUserDataDtor dtor)
    {
        return lua_newuserdatadtor(state, size, dtor);
    }

    bool ZenithStateManager::Add(const ZenithInfoKey& key)
    {
        bool keyExists = key.IsValid() && _keyToZenithMap.contains(key);
        if (keyExists)
            return false;

        std::unique_ptr<Zenith> zenith = std::make_unique<Zenith>();
        zenith->key = key;
        zenith->SetDirty();

        _keyToZenithMap[key] = std::move(zenith);

        return true;
    }

    bool ZenithStateManager::Remove(const ZenithInfoKey& key)
    {
        bool keyExists = key.IsValid() && _keyToZenithMap.contains(key);
        if (!keyExists)
            return false;

        Zenith* zenith = _keyToZenithMap[key].get();
        zenith->key = ZenithInfoKey();

        if (zenith->state != nullptr)
        {
            u64 luaStateAddress = reinterpret_cast<u64>(zenith->state);
            _luaStateToKey.erase(luaStateAddress);

            zenith->Clear();
        }

        _keyToZenithMap.erase(key);
        return true;
    }

    bool ZenithStateManager::Add(const ZenithInfoKey& key, const lua_State* state)
    {
        bool keyExists = key.IsValid() && _keyToZenithMap.contains(key);
        bool stateExists = state && _luaStateToKey.contains(reinterpret_cast<u64>(state));

        if (!keyExists || stateExists)
            return false;

        u64 address = reinterpret_cast<u64>(state);
        _luaStateToKey[address] = key;

        return true;
    }

    bool ZenithStateManager::Remove(const lua_State* state)
    {
        u64 address = reinterpret_cast<u64>(state);

        bool stateExists = state && _luaStateToKey.contains(address);
        if (!stateExists)
            return false;

        _luaStateToKey.erase(address);

        return true;
    }

    Zenith* ZenithStateManager::Get(const ZenithInfoKey& key)
    {
        auto it = _keyToZenithMap.find(key);

        bool keyExists = key.IsValid() && it != _keyToZenithMap.end();
        if (!keyExists)
            return nullptr;

        return it->second.get();
    }
    Zenith* ZenithStateManager::Get(lua_State* state)
    {
        u64 address = reinterpret_cast<u64>(state);

        auto it = _luaStateToKey.find(address);
        if (it == _luaStateToKey.end())
            return nullptr;

        return Get(it->second);
    }
}