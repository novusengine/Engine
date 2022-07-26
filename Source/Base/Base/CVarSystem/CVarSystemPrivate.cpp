#include "CVarSystemPrivate.h"

#include "Base/Util/DebugHandler.h"

#include <algorithm>

u32 Hash(const char* str)
{
    return StringUtils::fnv1a_32(str, strlen(str));
}

f64* CVarSystemImpl::GetFloatCVar(StringUtils::StringHash hash)
{
    return GetCVarCurrent<f64>(hash);
}

i32* CVarSystemImpl::GetIntCVar(StringUtils::StringHash hash)
{
    return GetCVarCurrent<i32>(hash);
}

const char* CVarSystemImpl::GetStringCVar(StringUtils::StringHash hash)
{
    return GetCVarCurrent<std::string>(hash)->c_str();
}

vec4* CVarSystemImpl::GetVecFloatCVar(StringUtils::StringHash hash)
{
    return GetCVarCurrent<vec4>(hash);
}

ivec4* CVarSystemImpl::GetVecIntCVar(StringUtils::StringHash hash)
{
    return GetCVarCurrent<ivec4>(hash);
}

CVarSystem* CVarSystem::Get()
{
    static CVarSystemImpl cvarSys{};
    return &cvarSys;
}

CVarParameter* CVarSystemImpl::GetCVar(StringUtils::StringHash hash)
{
    std::shared_lock lock(mutex_);
    auto it = savedCVars.find(hash);

    if (it != savedCVars.end())
    {
        return &(*it).second;
    }

    return nullptr;
}

void CVarSystemImpl::SetFloatCVar(StringUtils::StringHash hash, f64 value)
{
    SetCVarCurrent<f64>(hash, value);
}

void CVarSystemImpl::SetIntCVar(StringUtils::StringHash hash, i32 value)
{
    SetCVarCurrent<i32>(hash, value);
}

void CVarSystemImpl::SetStringCVar(StringUtils::StringHash hash, const char* value)
{
    SetCVarCurrent<std::string>(hash, value);
}

void CVarSystemImpl::SetVecFloatCVar(StringUtils::StringHash hash, const vec4& value)
{
    SetCVarCurrent<vec4>(hash, value);
}

void CVarSystemImpl::SetVecIntCVar(StringUtils::StringHash hash, const ivec4& value)
{
    SetCVarCurrent<ivec4>(hash, value);
}

CVarParameter* CVarSystemImpl::CreateFloatCVar(const char* name, const char* description, f64 defaultValue, f64 currentValue)
{
    CVarParameter* param = InitCVar(name, description);
    if (!param) return nullptr;

    std::unique_lock lock(mutex_);
    param->type = CVarType::FLOAT;

    GetCVarArray<f64>()->Add(defaultValue, currentValue, param);

    return param;
}

CVarParameter* CVarSystemImpl::CreateIntCVar(const char* name, const char* description, i32 defaultValue, i32 currentValue)
{
    CVarParameter* param = InitCVar(name, description);
    if (!param) return nullptr;
    std::unique_lock lock(mutex_);
    param->type = CVarType::INT;

    GetCVarArray<i32>()->Add(defaultValue, currentValue, param);

    return param;
}

CVarParameter* CVarSystemImpl::CreateStringCVar(const char* name, const char* description, const char* defaultValue, const char* currentValue)
{
    CVarParameter* param = InitCVar(name, description);
    if (!param) return nullptr;

    std::unique_lock lock(mutex_);
    param->type = CVarType::STRING;

    GetCVarArray<std::string>()->Add(defaultValue, currentValue, param);

    return param;
}

CVarParameter* CVarSystemImpl::CreateVecFloatCVar(const char* name, const char* description, const vec4& defaultValue, const vec4& currentValue)
{
    CVarParameter* param = InitCVar(name, description);
    if (!param) return nullptr;

    std::unique_lock lock(mutex_);
    param->type = CVarType::FLOATVEC;

    GetCVarArray<vec4>()->Add(defaultValue, currentValue, param);

    return param;
}

CVarParameter* CVarSystemImpl::CreateVecIntCVar(const char* name, const char* description, const ivec4& defaultValue, const ivec4& currentValue)
{
    CVarParameter* param = InitCVar(name, description);
    if (!param) return nullptr;

    std::unique_lock lock(mutex_);
    param->type = CVarType::INTVEC;

    GetCVarArray<ivec4>()->Add(defaultValue, currentValue, param);

    return param;
}

CVarParameter* CVarSystemImpl::InitCVar(const char* name, const char* description)
{
    if (GetCVar(name)) return nullptr; //return null if the cvar already exists

    std::unique_lock lock(mutex_);
    u32 namehash = StringUtils::StringHash{ name };
    savedCVars[namehash] = CVarParameter{};

    CVarParameter& newParam = savedCVars[namehash];

    newParam.name = name;
    newParam.description = description;

    return &newParam;
}

AutoCVar_Float::AutoCVar_Float(const char* name, const char* description, f64 defaultValue, CVarFlags flags)
{
    CVarParameter* cvar = CVarSystem::Get()->CreateFloatCVar(name, description, defaultValue, defaultValue);
    cvar->flags = flags;
    index = cvar->arrayIndex;
}

f64 AutoCVar_Float::Get()
{
    return CVarSystemImpl::Get()->GetCVarArray<f64>()->GetCurrent(index);
}

f64* AutoCVar_Float::GetPtr()
{
    return CVarSystemImpl::Get()->GetCVarArray<f64>()->GetCurrentPtr(index);
}

f32 AutoCVar_Float::GetFloat()
{
    return static_cast<f32>(Get());
}

f32* AutoCVar_Float::GetFloatPtr()
{
    f32* result = reinterpret_cast<f32*>(GetPtr());
    return result;
}

void AutoCVar_Float::Set(f64 f)
{
    CVarSystemImpl::Get()->GetCVarArray<f64>()->SetCurrent(f, index);
}

AutoCVar_Int::AutoCVar_Int(const char* name, const char* description, i32 defaultValue, CVarFlags flags)
{
    CVarParameter* cvar = CVarSystem::Get()->CreateIntCVar(name, description, defaultValue, defaultValue);
    cvar->flags = flags;
    index = cvar->arrayIndex;
}

i32 AutoCVar_Int::Get()
{
    return CVarSystemImpl::Get()->GetCVarArray<i32>()->GetCurrent(index);
}

i32* AutoCVar_Int::GetPtr()
{
    return CVarSystemImpl::Get()->GetCVarArray<i32>()->GetCurrentPtr(index);
}

void AutoCVar_Int::Set(i32 val)
{
    CVarSystemImpl::Get()->GetCVarArray<i32>()->SetCurrent(val, index);
}

void AutoCVar_Int::Toggle()
{
    bool enabled = Get() != 0;

    Set(enabled ? 0 : 1);
}

AutoCVar_String::AutoCVar_String(const char* name, const char* description, const char* defaultValue, CVarFlags flags)
{
    CVarParameter* cvar = CVarSystem::Get()->CreateStringCVar(name, description, defaultValue, defaultValue);
    cvar->flags = flags;
    index = cvar->arrayIndex;
}

const char* AutoCVar_String::Get()
{
    return CVarSystemImpl::Get()->GetCVarArray<std::string>()->GetCurrentPtr(index)->c_str();
};

void AutoCVar_String::Set(std::string&& val)
{
    CVarSystemImpl::Get()->GetCVarArray<std::string>()->SetCurrent(val, index);
}

AutoCVar_VecFloat::AutoCVar_VecFloat(const char* name, const char* description, const vec4& defaultValue, CVarFlags flags /*= CVarFlags::None*/)
{
    CVarParameter* cvar = CVarSystem::Get()->CreateVecFloatCVar(name, description, defaultValue, defaultValue);
    cvar->flags = flags;
    index = cvar->arrayIndex;
}

vec4 AutoCVar_VecFloat::Get()
{
    return CVarSystemImpl::Get()->GetCVarArray<vec4>()->GetCurrent(index);
}

void AutoCVar_VecFloat::Set(const vec4& val)
{
    CVarSystemImpl::Get()->GetCVarArray<vec4>()->SetCurrent(val, index);
}

AutoCVar_VecInt::AutoCVar_VecInt(const char* name, const char* description, const ivec4& defaultValue, CVarFlags flags)
{
    CVarParameter* cvar = CVarSystem::Get()->CreateVecIntCVar(name, description, defaultValue, defaultValue);
    cvar->flags = flags;
    index = cvar->arrayIndex;
}

ivec4 AutoCVar_VecInt::Get()
{
    return CVarSystemImpl::Get()->GetCVarArray<ivec4>()->GetCurrent(index);
}

void AutoCVar_VecInt::Set(const ivec4& val)
{
    CVarSystemImpl::Get()->GetCVarArray<ivec4>()->SetCurrent(val, index);
}