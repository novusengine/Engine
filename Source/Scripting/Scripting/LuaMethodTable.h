#pragma once
#include "LuaManager.h"
#include "Zenith.h"

#include <Base/Types.h>
#include <Base/Util/DebugHandler.h>

#include <lua.h>
#include <lualib.h>

#include <type_traits>

namespace Scripting
{
    template <typename UserDataType = void>
    struct LuaRegister
    {
    public:
        const char* name;
        typename std::conditional<std::is_same_v<UserDataType, void>, i32(*)(Zenith*), i32(*)(Zenith*, UserDataType*)>::type func;
    };

    template <typename UserDataType>
    struct LuaMetaTable
    {
    public:
        static void Register(Zenith* zenith, const char* name)
        {
            if constexpr (std::is_same_v<UserDataType, void>)
            {
                NC_ASSERT(false, "LuaMetaTable::Register should not be called when using LuaMethodTable")
            }

            NC_ASSERT(name, "Name is null");
            NC_ASSERT(zenith->state, "Lua state is null");

            // Ensure the table doesn't already exist
            if (!luaL_newmetatable(zenith->state, name))
            {
                lua_pop(zenith->state, 1);
                NC_LOG_ERROR("Lua Global Table already exists: {0}", name);
                return;
            }

            _name = name;

            // Create the meta table
            i32 table = lua_gettop(zenith->state);

            // Set ToString
            lua_pushcfunction(zenith->state, ToString, "ToString");
            lua_setfield(zenith->state, table, "__tostring");

            // Set __index index
            lua_pushvalue(zenith->state, table);
            lua_setfield(zenith->state, table, "__index");

            // Set __add index
            lua_pushcfunction(zenith->state, Add, "Add");
            lua_setfield(zenith->state, table, "__add");

            // Set __sub index
            lua_pushcfunction(zenith->state, Substract, "Substract");
            lua_setfield(zenith->state, table, "__sub");

            // Set __mul index
            lua_pushcfunction(zenith->state, Multiply, "Multiply");
            lua_setfield(zenith->state, table, "__mul");

            // Set __div index
            lua_pushcfunction(zenith->state, Divide, "Divide");
            lua_setfield(zenith->state, table, "__div");

            // Set __mod index
            lua_pushcfunction(zenith->state, Mod, "Mod");
            lua_setfield(zenith->state, table, "__mod");

            // Set __pow index
            lua_pushcfunction(zenith->state, Pow, "Pow");
            lua_setfield(zenith->state, table, "__pow");

            // Set __unm index
            lua_pushcfunction(zenith->state, UnaryMinus, "UnaryMinus");
            lua_setfield(zenith->state, table, "__unm");

            // Set __concat index
            lua_pushcfunction(zenith->state, Concat, "Concat");
            lua_setfield(zenith->state, table, "__concat");

            // Set __len index
            lua_pushcfunction(zenith->state, Length, "Length");
            lua_setfield(zenith->state, table, "__len");

            // Set __eq index
            lua_pushcfunction(zenith->state, Equal, "Equal");
            lua_setfield(zenith->state, table, "__eq");

            // Set __lt index
            lua_pushcfunction(zenith->state, Less, "Less");
            lua_setfield(zenith->state, table, "__lt");

            // Set __le index
            lua_pushcfunction(zenith->state, LessOrEqual, "LessOrEqual");
            lua_setfield(zenith->state, table, "__le");

            // Set __call index
            lua_pushcfunction(zenith->state, Call, "Call");
            lua_setfield(zenith->state, table, "__call");

            // Pop the table
            lua_pop(zenith->state, 1);
        }

        template <class MethodTableUserDataType, size_t N>
        static void Set(Zenith* zenith, const LuaRegister<MethodTableUserDataType> (&methodTable)[N], const char* globalName = nullptr)
        {
            static_assert(std::is_same_v<UserDataType, void> || std::is_same_v<UserDataType, MethodTableUserDataType> || std::is_base_of_v<MethodTableUserDataType, UserDataType>, "LuaMetaTable::Set - LuaRegister's UserDataType must either match LuaMetaTable's UserDataType, be derived from it or be void");
            constexpr bool isGlobal = std::is_same_v<UserDataType, void>;
            const char* name = nullptr;

            NC_ASSERT(zenith->state, "Lua state is null");
            NC_ASSERT(methodTable, "MethodTable is null");

            if constexpr (isGlobal)
            {
                // Push the global table
                if (globalName)
                {
                    zenith->CreateTable(globalName);
                }
                else
                {
                    lua_pushvalue(zenith->state, LUA_GLOBALSINDEX);
                }
            }
            else
            {
                NC_ASSERT(!globalName, "GlobalName cannot be set when calling SetMethodTable on a MetaTable, globalName is used when setting a global table, use Register to setup a MetaTable");
                NC_ASSERT(_name, "Name is null");

                lua_pushstring(zenith->state, _name);
                lua_rawget(zenith->state, LUA_REGISTRYINDEX);
                NC_ASSERT(zenith->IsTable(-1), "MetaTable is illformed");
            }

            for (size_t i = 0; i < N; ++i)
            {
                const auto& method = methodTable[i];

                lua_pushstring(zenith->state, method.name);

                lua_pushlightuserdata(zenith->state, (void*)&method);
                lua_pushcclosure(zenith->state, thunk, "Thunk", 1);
                lua_rawset(zenith->state, -3);
            }

            lua_pop(zenith->state, 1);
        }

        static i32 thunk(lua_State* state)
        {
            lua_getglobal(state, "Zenith");
            if (lua_islightuserdata(state, -1) == 0)
            {
                luaL_error(state, "error calling function, global 'Zenith' missing");
                return 0;
            }

            void* zenithPtr = lua_touserdata(state, -1);
            lua_pop(state, 1);

            LuaManager* luaManager = reinterpret_cast<LuaManager*>(zenithPtr);
            Zenith* zenith = luaManager->GetZenithStateManager().Get(state);
            NC_ASSERT(zenith, "Zenith is null");

            LuaRegister<UserDataType>* method = static_cast<LuaRegister<UserDataType>*>(lua_touserdata(state, lua_upvalueindex(1)));

            constexpr bool isGlobal = std::is_same_v<UserDataType, void>;
            
            // we only check self if the method is not a global
            UserDataType* userData = nullptr;
            if constexpr (!isGlobal)
            {
                if (!zenith->IsUserData(1))
                    return 0;

                userData = reinterpret_cast<UserDataType*>(zenith->ToUserData(1));
                if (!userData)
                    return 0;
            }
            
            i32 top = zenith->GetTop();
            
            i32 expected = 0;
            if constexpr (isGlobal)
                expected = method->func(zenith); // global method
            else
                expected = method->func(zenith, userData); // non-global method
            
            i32 args = zenith->GetTop() - top;
            if (args < 0 || args > expected)
            {
                NC_LOG_ERROR("[Scripting]: C Function {0} pushed {1} arguments but was expected to push {0}.", method->name, args, expected);
                NC_ASSERT(false, "Lua C Function pushed unexpected number of arguments");
            }

            zenith->SetTop(top + expected);
            return expected;
        }

    private:
        static i32 ToString(lua_State* state)
        {
            lua_pushfstring(state, "%s", _name);
            return 1;
        }

        static i32 ArithmeticError(lua_State* state) { luaL_error(state, "attempt to perform arithmetic on a %s value", _name);  return 1; }
        static i32 CompareError(lua_State* state) { luaL_error(state, "attempt to compare %s", _name);  return 1; }
        static i32 Add(lua_State* state) { return ArithmeticError(state); }
        static i32 Substract(lua_State* state) { return ArithmeticError(state); }
        static i32 Multiply(lua_State* state) { return ArithmeticError(state); }
        static i32 Divide(lua_State* state) { return ArithmeticError(state); }
        static i32 Mod(lua_State* state) { return ArithmeticError(state); }
        static i32 Pow(lua_State* state) { return ArithmeticError(state); }
        static i32 UnaryMinus(lua_State* state) { return ArithmeticError(state); }
        static i32 Concat(lua_State* state) { luaL_error(state, "attempt to concatenate a %s value", _name); return 1; }
        static i32 Length(lua_State* state) { luaL_error(state, "attempt to get length of a %s value", _name); return 1; }
        static i32 Equal(lua_State* state) { return CompareError(state); }
        static i32 Less(lua_State* state) { return CompareError(state); }
        static i32 LessOrEqual(lua_State* state) { return CompareError(state); }
        static i32 Call(lua_State* state) { luaL_error(state, "attempt to call a %s value", _name); return 1; }

    public:
        static const char* _name;
    };

    using LuaMethodTable = LuaMetaTable<void>;

    template<typename UserDataType> const char* LuaMetaTable<UserDataType>::_name = nullptr;
}