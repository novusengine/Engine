#include "CVarSystemPrivate.h"

#include "Base/Util/DebugHandler.h"

#include <algorithm>

f64* CVarSystemImpl::GetFloatCVar(StringUtils::StringHash hash)
{
    return GetCVarCurrent<f64>(hash);
}

f64* CVarSystemImpl::GetFloatCVar(CVarCategory category, StringUtils::StringHash hash)
{
    u32 qualifiedNameHash = 0;
    if (!LookupCVar(category, hash, qualifiedNameHash))
    {
        return nullptr;
    }
    return GetFloatCVar(qualifiedNameHash);
}

i32* CVarSystemImpl::GetIntCVar(StringUtils::StringHash hash)
{
    return GetCVarCurrent<i32>(hash);
}

i32* CVarSystemImpl::GetIntCVar(CVarCategory category, StringUtils::StringHash hash)
{
    u32 qualifiedNameHash = 0;
    if (!LookupCVar(category, hash, qualifiedNameHash))
    {
        return nullptr;
    }
    return GetIntCVar(qualifiedNameHash);
}

const char* CVarSystemImpl::GetStringCVar(StringUtils::StringHash hash)
{
    return GetCVarCurrent<std::string>(hash)->c_str();
}

const char* CVarSystemImpl::GetStringCVar(CVarCategory category, StringUtils::StringHash hash)
{
    u32 qualifiedNameHash = 0;
    if (!LookupCVar(category, hash, qualifiedNameHash))
    {
        return nullptr;
    }
    return GetStringCVar(qualifiedNameHash);
}

vec4* CVarSystemImpl::GetVecFloatCVar(StringUtils::StringHash hash)
{
    return GetCVarCurrent<vec4>(hash);
}

vec4* CVarSystemImpl::GetVecFloatCVar(CVarCategory category, StringUtils::StringHash hash)
{
    u32 qualifiedNameHash = 0;
    if (!LookupCVar(category, hash, qualifiedNameHash))
    {
        return nullptr;
    }
    return GetVecFloatCVar(qualifiedNameHash);
}

ivec4* CVarSystemImpl::GetVecIntCVar(StringUtils::StringHash hash)
{
    return GetCVarCurrent<ivec4>(hash);
}

ivec4* CVarSystemImpl::GetVecIntCVar(CVarCategory category, StringUtils::StringHash hash)
{
    u32 qualifiedNameHash = 0;
    if (!LookupCVar(category, hash, qualifiedNameHash))
    {
        return nullptr;
    }
    return GetVecIntCVar(qualifiedNameHash);
}

ShowFlag* CVarSystemImpl::GetShowFlagCVar(StringUtils::StringHash hash)
{
    return GetCVarCurrent<ShowFlag>(hash);
}

ShowFlag* CVarSystemImpl::GetShowFlagCVar(CVarCategory category, StringUtils::StringHash hash)
{
    u32 qualifiedNameHash = 0;
    if (!LookupCVar(category, hash, qualifiedNameHash))
    {
        return nullptr;
    }
    return GetShowFlagCVar(qualifiedNameHash);
}

CVarSystem* CVarSystem::Get()
{
    static CVarSystemImpl cvarSys{};
    return &cvarSys;
}

std::string CVarSystem::GetQualifiedName(CVarCategory category, const char* name)
{
    // Construct the qualified name
    std::string qualifiedName;

    // Add all prefixes
    for (u32 i = 0; i < static_cast<u32>(CVarCategory::COUNT); i++)
    {
        u32 mask = 1 << i;
        if (static_cast<u32>(category) & mask)
        {
            qualifiedName += CVarCategoryToPrefix[i];
        }
    }

    qualifiedName += name;

    return qualifiedName;
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

CVarParameter* CVarSystemImpl::GetCVar(CVarCategory category, StringUtils::StringHash hash)
{
    u32 qualifiedNameHash = 0;
    if (!LookupCVar(category, hash, qualifiedNameHash))
    {
        return nullptr;
    }

    return GetCVar(qualifiedNameHash);
}

void CVarSystemImpl::SetFloatCVar(StringUtils::StringHash hash, f64 value)
{
    SetCVarCurrent<f64>(hash, value);
}

void CVarSystemImpl::SetFloatCVar(CVarCategory category, StringUtils::StringHash hash, f64 value)
{
    u32 qualifiedNameHash = 0;
    if (!LookupCVar(category, hash, qualifiedNameHash))
    {
        return;
    }

    SetFloatCVar(qualifiedNameHash, value);
}

void CVarSystemImpl::SetIntCVar(StringUtils::StringHash hash, i32 value)
{
    SetCVarCurrent<i32>(hash, value);
}

void CVarSystemImpl::SetIntCVar(CVarCategory category, StringUtils::StringHash hash, i32 value)
{
    u32 qualifiedNameHash = 0;
    if (!LookupCVar(category, hash, qualifiedNameHash))
    {
        return;
    }

    SetIntCVar(qualifiedNameHash, value);
}

void CVarSystemImpl::SetStringCVar(StringUtils::StringHash hash, const char* value)
{
    SetCVarCurrent<std::string>(hash, value);
}

void CVarSystemImpl::SetStringCVar(CVarCategory category, StringUtils::StringHash hash, const char* value)
{
    u32 qualifiedNameHash = 0;
    if (!LookupCVar(category, hash, qualifiedNameHash))
    {
        return;
    }

    SetStringCVar(qualifiedNameHash, value);
}

void CVarSystemImpl::SetVecFloatCVar(StringUtils::StringHash hash, const vec4& value)
{
    SetCVarCurrent<vec4>(hash, value);
}

void CVarSystemImpl::SetVecFloatCVar(CVarCategory category, StringUtils::StringHash hash, const vec4& value)
{
    u32 qualifiedNameHash = 0;
    if (!LookupCVar(category, hash, qualifiedNameHash))
    {
        return;
    }

    SetVecFloatCVar(qualifiedNameHash, value);
}

void CVarSystemImpl::SetVecIntCVar(StringUtils::StringHash hash, const ivec4& value)
{
    SetCVarCurrent<ivec4>(hash, value);
}

void CVarSystemImpl::SetVecIntCVar(CVarCategory category, StringUtils::StringHash hash, const ivec4& value)
{
    u32 qualifiedNameHash = 0;
    if (!LookupCVar(category, hash, qualifiedNameHash))
    {
        return;
    }

    SetVecIntCVar(qualifiedNameHash, value);
}

void CVarSystemImpl::SetShowFlagCVar(StringUtils::StringHash hash, const ShowFlag& value)
{
    SetCVarCurrent<ShowFlag>(hash, value);
}

void CVarSystemImpl::SetShowFlagCVar(CVarCategory category, StringUtils::StringHash hash, const ShowFlag& value)
{
    u32 qualifiedNameHash = 0;
    if (!LookupCVar(category, hash, qualifiedNameHash))
    {
        return;
    }

    SetShowFlagCVar(qualifiedNameHash, value);
}

void CVarSystemImpl::AddOnFloatValueChanged(StringUtils::StringHash hash, std::function<void(const f64&)> callback) 
{
    GetCVarArray<f64>()->GetCurrentStorage(GetCVar(hash)->arrayIndex)->onValueChangedCallbacks.push_back(callback);
}
void CVarSystemImpl::AddOnFloatValueChanged(CVarCategory category, StringUtils::StringHash hash, std::function<void(const f64&)> callback)
{
    u32 qualifiedNameHash = 0;
    if (!LookupCVar(category, hash, qualifiedNameHash))
    {
        return;
    }

    AddOnFloatValueChanged(qualifiedNameHash, callback);
}
void CVarSystemImpl::AddOnIntValueChanged(StringUtils::StringHash hash, std::function<void(const i32&)> callback)
{
    GetCVarArray<i32>()->GetCurrentStorage(GetCVar(hash)->arrayIndex)->onValueChangedCallbacks.push_back(callback);
}
void CVarSystemImpl::AddOnIntValueChanged(CVarCategory category, StringUtils::StringHash hash, std::function<void(const i32&)> callback)
{
    u32 qualifiedNameHash = 0;
    if (!LookupCVar(category, hash, qualifiedNameHash))
    {
        return;
    }

    AddOnIntValueChanged(qualifiedNameHash, callback);
}
void CVarSystemImpl::AddOnStringValueChanged(StringUtils::StringHash hash, std::function<void(const std::string&)> callback)
{
    GetCVarArray<std::string>()->GetCurrentStorage(GetCVar(hash)->arrayIndex)->onValueChangedCallbacks.push_back(callback);
}
void CVarSystemImpl::AddOnStringValueChanged(CVarCategory category, StringUtils::StringHash hash, std::function<void(const std::string&)> callback)
{
    u32 qualifiedNameHash = 0;
    if (!LookupCVar(category, hash, qualifiedNameHash))
    {
        return;
    }

    AddOnStringValueChanged(qualifiedNameHash, callback);
}
void CVarSystemImpl::AddOnVecFloatValueChanged(StringUtils::StringHash hash, std::function<void(const vec4&)> callback)
{
    GetCVarArray<vec4>()->GetCurrentStorage(GetCVar(hash)->arrayIndex)->onValueChangedCallbacks.push_back(callback);
}
void CVarSystemImpl::AddOnVecFloatValueChanged(CVarCategory category, StringUtils::StringHash hash, std::function<void(const vec4&)> callback)
{
    u32 qualifiedNameHash = 0;
    if (!LookupCVar(category, hash, qualifiedNameHash))
    {
        return;
    }

    AddOnVecFloatValueChanged(qualifiedNameHash, callback);
}
void CVarSystemImpl::AddOnVecIntValueChanged(StringUtils::StringHash hash, std::function<void(const ivec4&)> callback)
{
    GetCVarArray<ivec4>()->GetCurrentStorage(GetCVar(hash)->arrayIndex)->onValueChangedCallbacks.push_back(callback);
}
void CVarSystemImpl::AddOnVecIntValueChanged(CVarCategory category, StringUtils::StringHash hash, std::function<void(const ivec4&)> callback)
{
    u32 qualifiedNameHash = 0;
    if (!LookupCVar(category, hash, qualifiedNameHash))
    {
        return;
    }

    AddOnVecIntValueChanged(qualifiedNameHash, callback);
}
void CVarSystemImpl::AddOnShowFlagValueChanged(StringUtils::StringHash hash, std::function<void(const ShowFlag&)> callback)
{
    GetCVarArray<ShowFlag>()->GetCurrentStorage(GetCVar(hash)->arrayIndex)->onValueChangedCallbacks.push_back(callback);
}
void CVarSystemImpl::AddOnShowFlagValueChanged(CVarCategory category, StringUtils::StringHash hash, std::function<void(const ShowFlag&)> callback)
{
    u32 qualifiedNameHash = 0;
    if (!LookupCVar(category, hash, qualifiedNameHash))
    {
        return;
    }

    AddOnShowFlagValueChanged(qualifiedNameHash, callback);
}

CVarParameter* CVarSystemImpl::CreateFloatCVar(CVarCategory category, const char* name, const char* description, f64 defaultValue, f64 currentValue, CVarFlags flags)
{
    CVarParameter* param = nullptr;
    bool wasCreated = InitCVar(category, name, description, param);
    bool typeChanged = param->type != CVarType::FLOAT;

    std::unique_lock lock(mutex_);
    param->type = CVarType::FLOAT;
    param->flags = flags;

    CVarArray<f64>* cvarArray = GetCVarArray<f64>();
    if (wasCreated || typeChanged)
    {
        cvarArray->Add(defaultValue, currentValue, param);
    }

    return param;
}

CVarParameter* CVarSystemImpl::CreateIntCVar(CVarCategory category, const char* name, const char* description, i32 defaultValue, i32 currentValue, CVarFlags flags)
{
    CVarParameter* param = nullptr;
    bool wasCreated = InitCVar(category, name, description, param);
    bool typeChanged = param->type != CVarType::INT;

    std::unique_lock lock(mutex_);
    param->type = CVarType::INT;
    param->flags = flags;

    CVarArray<i32>* cvarArray = GetCVarArray<i32>();
    if (wasCreated || typeChanged)
    {
        cvarArray->Add(defaultValue, currentValue, param);
    }

    return param;
}

CVarParameter* CVarSystemImpl::CreateStringCVar(CVarCategory category, const char* name, const char* description, const char* defaultValue, const char* currentValue, CVarFlags flags)
{
    CVarParameter* param = nullptr;
    bool wasCreated = InitCVar(category, name, description, param);
    bool typeChanged = param->type != CVarType::STRING;

    std::unique_lock lock(mutex_);
    param->type = CVarType::STRING;
    param->flags = flags;

    CVarArray<std::string>* cvarArray = GetCVarArray<std::string>();
    if (wasCreated || typeChanged)
    {
        cvarArray->Add(defaultValue, currentValue, param);
    }

    return param;
}

CVarParameter* CVarSystemImpl::CreateVecFloatCVar(CVarCategory category, const char* name, const char* description, const vec4& defaultValue, const vec4& currentValue, CVarFlags flags)
{
    CVarParameter* param = nullptr;
    bool wasCreated = InitCVar(category, name, description, param);
    bool typeChanged = param->type != CVarType::FLOATVEC;

    std::unique_lock lock(mutex_);
    param->type = CVarType::FLOATVEC;
    param->flags = flags;

    CVarArray<vec4>* cvarArray = GetCVarArray<vec4>();
    if (wasCreated || typeChanged)
    {
        cvarArray->Add(defaultValue, currentValue, param);
    }

    return param;
}

CVarParameter* CVarSystemImpl::CreateVecIntCVar(CVarCategory category, const char* name, const char* description, const ivec4& defaultValue, const ivec4& currentValue, CVarFlags flags)
{
    CVarParameter* param = nullptr;
    bool wasCreated = InitCVar(category, name, description, param);
    bool typeChanged = param->type != CVarType::INTVEC;

    std::unique_lock lock(mutex_);
    param->type = CVarType::INTVEC;
    param->flags = flags;

    CVarArray<ivec4>* cvarArray = GetCVarArray<ivec4>();
    if (wasCreated || typeChanged)
    {
        cvarArray->Add(defaultValue, currentValue, param);
    }

    return param;
}

CVarParameter* CVarSystemImpl::CreateShowFlagCVar(CVarCategory category, const char* name, const char* description, const ShowFlag& defaultValue, const ShowFlag& currentValue, CVarFlags flags)
{
    CVarParameter* param = nullptr;
    bool wasCreated = InitCVar(category, name, description, param);
    bool typeChanged = param->type != CVarType::SHOWFLAG;

    std::unique_lock lock(mutex_);
    param->type = CVarType::SHOWFLAG;
    param->flags = flags;

    CVarArray<ShowFlag>* cvarArray = GetCVarArray<ShowFlag>();
    if (wasCreated || typeChanged)
    {
        cvarArray->Add(defaultValue, currentValue, param);
    }

    return param;
}

bool CVarSystemImpl::InitCVar(CVarCategory category, const char* name, const char* description, CVarParameter*& outParam)
{
    std::string qualifiedName = GetQualifiedName(category, name);
    u32 qualifiedNameHash = StringUtils::StringHash{ qualifiedName };
    u32 nameHash = StringUtils::StringHash{ name };
    
    outParam = GetCVar(qualifiedName.c_str());
    std::unique_lock lock(mutex_);
    if (outParam)
    {
        CVarCategory existingCategory = outParam->category;
        if (category != existingCategory)
        {
            // Remove old category lookup
            cvarCategoryLookupTable[existingCategory].erase(nameHash);

            // Add new category lookup
            if (!cvarCategoryLookupTable.contains(category))
            {
                cvarCategoryLookupTable[category] = CVarLookupTable();
            }

            cvarCategoryLookupTable[category][nameHash] = qualifiedNameHash;
        }

        outParam->category = category;
        outParam->description = description;
        return false;
    }

    savedCVars[qualifiedNameHash] = CVarParameter{};

    CVarParameter& newParam = savedCVars[qualifiedNameHash];
    newParam.category = category;
    newParam.name = name;
    newParam.description = description;

    if (!cvarCategoryLookupTable.contains(category))
    {
        cvarCategoryLookupTable[category] = CVarLookupTable();
    }

    cvarCategoryLookupTable[category][nameHash] = qualifiedNameHash;

    outParam = &newParam;
    return true;
}

bool CVarSystemImpl::LookupCVar(CVarCategory category, StringUtils::StringHash nameHash, u32& outHash)
{
    outHash = 0;
    if (!cvarCategoryLookupTable.contains(category))
    {
        return false;
    }

    CVarLookupTable& table = cvarCategoryLookupTable[category];
    if (!table.contains(nameHash))
    {
        return false;
    }

    outHash = table[nameHash];
    return true;
}

AutoCVar_Float::AutoCVar_Float(CVarCategory category, const char* name, const char* description, f64 defaultValue, CVarFlags flags)
{
    CVarParameter* cvar = CVarSystem::Get()->CreateFloatCVar(category, name, description, defaultValue, defaultValue, flags);
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

void AutoCVar_Float::AddOnValueChanged(std::function<void(const f64&)> callback)
{
    CVarSystemImpl::Get()->GetCVarArray<f64>()->GetCurrentStorage(index)->onValueChangedCallbacks.push_back(callback);
}

AutoCVar_Int::AutoCVar_Int(CVarCategory category, const char* name, const char* description, i32 defaultValue, CVarFlags flags)
{
    CVarParameter* cvar = CVarSystem::Get()->CreateIntCVar(category, name, description, defaultValue, defaultValue, flags);
    index = cvar->arrayIndex;
}

i32 AutoCVar_Int::Get()
{
    return CVarSystemImpl::Get()->GetCVarArray<i32>()->GetCurrent(index);
}

u32 AutoCVar_Int::GetU32()
{
    return static_cast<u32>(CVarSystemImpl::Get()->GetCVarArray<i32>()->GetCurrent(index));
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

void AutoCVar_Int::AddOnValueChanged(std::function<void(const i32&)> callback)
{
    CVarSystemImpl::Get()->GetCVarArray<i32>()->GetCurrentStorage(index)->onValueChangedCallbacks.push_back(callback);
}

AutoCVar_String::AutoCVar_String(CVarCategory category, const char* name, const char* description, const char* defaultValue, CVarFlags flags)
{
    CVarParameter* cvar = CVarSystem::Get()->CreateStringCVar(category, name, description, defaultValue, defaultValue, flags);
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

void AutoCVar_String::AddOnValueChanged(std::function<void(const std::string&)> callback)
{
    CVarSystemImpl::Get()->GetCVarArray<std::string>()->GetCurrentStorage(index)->onValueChangedCallbacks.push_back(callback);
}

AutoCVar_VecFloat::AutoCVar_VecFloat(CVarCategory category, const char* name, const char* description, const vec4& defaultValue, CVarFlags flags /*= CVarFlags::None*/)
{
    CVarParameter* cvar = CVarSystem::Get()->CreateVecFloatCVar(category, name, description, defaultValue, defaultValue, flags);
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

void AutoCVar_VecFloat::AddOnValueChanged(std::function<void(const vec4&)> callback)
{
    CVarSystemImpl::Get()->GetCVarArray<vec4>()->GetCurrentStorage(index)->onValueChangedCallbacks.push_back(callback);
}

AutoCVar_VecInt::AutoCVar_VecInt(CVarCategory category, const char* name, const char* description, const ivec4& defaultValue, CVarFlags flags)
{
    CVarParameter* cvar = CVarSystem::Get()->CreateVecIntCVar(category, name, description, defaultValue, defaultValue, flags);
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

void AutoCVar_VecInt::AddOnValueChanged(std::function<void(const ivec4&)> callback)
{
    CVarSystemImpl::Get()->GetCVarArray<ivec4>()->GetCurrentStorage(index)->onValueChangedCallbacks.push_back(callback);
}

AutoCVar_ShowFlag::AutoCVar_ShowFlag(CVarCategory category, const char* name, const char* description, const ShowFlag& defaultValue, CVarFlags flags)
{
    CVarParameter* cvar = CVarSystem::Get()->CreateShowFlagCVar(category, name, description, defaultValue, defaultValue, flags);
    index = cvar->arrayIndex;
}

ShowFlag AutoCVar_ShowFlag::Get()
{
    return CVarSystemImpl::Get()->GetCVarArray<ShowFlag>()->GetCurrent(index);
}

void AutoCVar_ShowFlag::Set(const ShowFlag& val)
{
    CVarSystemImpl::Get()->GetCVarArray<ShowFlag>()->SetCurrent(val, index);
}

void AutoCVar_ShowFlag::AddOnValueChanged(std::function<void(const ShowFlag&)> callback)
{
    CVarSystemImpl::Get()->GetCVarArray<ShowFlag>()->GetCurrentStorage(index)->onValueChangedCallbacks.push_back(callback);
}