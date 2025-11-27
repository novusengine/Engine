#pragma once
#include "CVarSystem.h"

#include <Base/Types.h>
#include <Base/Util/StringUtils.h>

#include <array>
#include <robinhood/robinhood.h>
#include <shared_mutex>

enum class CVarType : u8
{
    INT,
    FLOAT,
    STRING,
    FLOATVEC,
    INTVEC,
    SHOWFLAG
};

class CVarParameter
{
public:
    friend class CVarSystemImpl;

    i32 arrayIndex;

    CVarCategory category;
    CVarType type;
    CVarFlags flags;
    std::string name;
    std::string description;
};

template<typename T>
struct CVarStorage
{
public:
    T initial;
    T current;
    CVarParameter* parameter;
    std::vector<std::function<void(const T&)>> onValueChangedCallbacks;
};

template<typename T>
struct CVarArray
{
public:
    std::vector<CVarStorage<T>> cvars;
    i32 lastCVar{ 0 };

    CVarArray(size_t size)
    {
        cvars.resize(size);
    }

    CVarStorage<T>* GetCurrentStorage(i32 index)
    {
        return &cvars[index];
    }

    T* GetCurrentPtr(i32 index)
    {
        return &((cvars.data() + index)->current);
    };

    T GetCurrent(i32 index)
    {
        return cvars[index].current;
    };

    void SetCurrent(const T& val, i32 index);

    int Add(const T& value, CVarParameter* param);

    int Add(const T& initialValue, const T& currentValue, CVarParameter* param);
};

class CVarSystemImpl : public CVarSystem
{
public:
    CVarParameter* GetCVar(StringUtils::StringHash hash) override final;
    CVarParameter* GetCVar(CVarCategory category, StringUtils::StringHash hash) override final;

    CVarParameter* CreateFloatCVar(CVarCategory category, const char* name, const char* description, f64 defaultValue, f64 currentValue, CVarFlags flags = CVarFlags::None) override final;
    CVarParameter* CreateIntCVar(CVarCategory category, const char* name, const char* description, i32 defaultValue, i32 currentValue, CVarFlags flags = CVarFlags::None) override final;
    CVarParameter* CreateStringCVar(CVarCategory category, const char* name, const char* description, const char* defaultValue, const char* currentValue, CVarFlags flags = CVarFlags::None) override final;
    CVarParameter* CreateVecFloatCVar(CVarCategory category, const char* name, const char* description, const vec4& defaultValue, const vec4& currentValue, CVarFlags flags = CVarFlags::None) override final;
    CVarParameter* CreateVecIntCVar(CVarCategory category, const char* name, const char* description, const ivec4& defaultValue, const ivec4& currentValue, CVarFlags flags = CVarFlags::None) override final;
    CVarParameter* CreateShowFlagCVar(CVarCategory category, const char* name, const char* description, const ShowFlag& defaultValue, const ShowFlag& currentValue, CVarFlags flags = CVarFlags::None) override final;

    f64* GetFloatCVar(StringUtils::StringHash hash) override final;
    f64* GetFloatCVar(CVarCategory category, StringUtils::StringHash hash) override final;
    i32* GetIntCVar(StringUtils::StringHash hash) override final;
    i32* GetIntCVar(CVarCategory category, StringUtils::StringHash hash) override final;
    const char* GetStringCVar(StringUtils::StringHash hash) override final;
    const char* GetStringCVar(CVarCategory category, StringUtils::StringHash hash) override final;
    vec4* GetVecFloatCVar(StringUtils::StringHash hash) override final;
    vec4* GetVecFloatCVar(CVarCategory category, StringUtils::StringHash hash) override final;
    ivec4* GetVecIntCVar(StringUtils::StringHash hash) override final;
    ivec4* GetVecIntCVar(CVarCategory category, StringUtils::StringHash hash) override final;
    ShowFlag* GetShowFlagCVar(StringUtils::StringHash hash) override final;
    ShowFlag* GetShowFlagCVar(CVarCategory category, StringUtils::StringHash hash) override final;

    void SetFloatCVar(StringUtils::StringHash hash, f64 value) override final;
    void SetFloatCVar(CVarCategory category, StringUtils::StringHash hash, f64 value) override final;
    void SetIntCVar(StringUtils::StringHash hash, i32 value) override final;
    void SetIntCVar(CVarCategory category, StringUtils::StringHash hash, i32 value) override final;
    void SetStringCVar(StringUtils::StringHash hash, const char* value) override final;
    void SetStringCVar(CVarCategory category, StringUtils::StringHash hash, const char* value) override final;
    void SetVecFloatCVar(StringUtils::StringHash hash, const vec4& value) override final;
    void SetVecFloatCVar(CVarCategory category, StringUtils::StringHash hash, const vec4& value) override final;
    void SetVecIntCVar(StringUtils::StringHash hash, const ivec4& value) override final;
    void SetVecIntCVar(CVarCategory category, StringUtils::StringHash hash, const ivec4& value) override final;
    void SetShowFlagCVar(StringUtils::StringHash hash, const ShowFlag& value) override final;
    void SetShowFlagCVar(CVarCategory category, StringUtils::StringHash hash, const ShowFlag& value) override final;

    void AddOnFloatValueChanged(StringUtils::StringHash hash, std::function<void(const f64&)> callback) override final;
    void AddOnFloatValueChanged(CVarCategory category, StringUtils::StringHash hash, std::function<void(const f64&)> callback) override final;
    void AddOnIntValueChanged(StringUtils::StringHash hash, std::function<void(const i32&)> callback) override final;
    void AddOnIntValueChanged(CVarCategory category, StringUtils::StringHash hash, std::function<void(const i32&)> callback) override final;
    void AddOnStringValueChanged(StringUtils::StringHash hash, std::function<void(const std::string&)> callback) override final;
    void AddOnStringValueChanged(CVarCategory category, StringUtils::StringHash hash, std::function<void(const std::string&)> callback) override final;
    void AddOnVecFloatValueChanged(StringUtils::StringHash hash, std::function<void(const vec4&)> callback) override final;
    void AddOnVecFloatValueChanged(CVarCategory category, StringUtils::StringHash hash, std::function<void(const vec4&)> callback) override final;
    void AddOnVecIntValueChanged(StringUtils::StringHash hash, std::function<void(const ivec4&)> callback) override final;
    void AddOnVecIntValueChanged(CVarCategory category, StringUtils::StringHash hash, std::function<void(const ivec4&)> callback) override final;
    void AddOnShowFlagValueChanged(StringUtils::StringHash hash, std::function<void(const ShowFlag&)> callback) override final;
    void AddOnShowFlagValueChanged(CVarCategory category, StringUtils::StringHash hash, std::function<void(const ShowFlag&)> callback) override final;

    constexpr static int MAX_INT_CVARS = 1000;

    CVarArray<i32> intCVars2{ MAX_INT_CVARS };

    constexpr static int MAX_FLOAT_CVARS = 1000;
    CVarArray<f64> floatCVars{ MAX_FLOAT_CVARS };

    constexpr static int MAX_STRING_CVARS = 200;
    CVarArray<std::string> stringCVars{ MAX_STRING_CVARS };

    constexpr static int MAX_FLOATVEC_CVARS = 400;
    CVarArray<vec4> fvecCVars{ MAX_FLOATVEC_CVARS };

    constexpr static int MAX_INTVEC_CVARS = 400;
    CVarArray<ivec4> ivecCVars{ MAX_INTVEC_CVARS };

    constexpr static int MAX_SHOWFLAG_CVARS = 100;
    CVarArray<ShowFlag> showFlagCVars{ MAX_SHOWFLAG_CVARS };

    template<typename T>
    inline CVarArray<T>* GetCVarArray();

    //templated get-set cvar versions for syntax sugar
    template<typename T>
    T* GetCVarCurrent(u32 namehash) {
        auto par = GetCVar(namehash);
        return GetCVarArray<T>()->GetCurrentPtr(par->arrayIndex);
    }

    template<typename T>
    void SetCVarCurrent(u32 namehash, const T& value)
    {
        GetCVarArray<T>()->SetCurrent(value, GetCVar(namehash)->arrayIndex);
    }

    static CVarSystemImpl* Get()
    {
        return static_cast<CVarSystemImpl*>(CVarSystem::Get());
    }

private:
    std::shared_mutex mutex_;

    // Returns true if the CVar was created, false if it already existed
    bool InitCVar(CVarCategory category, const char* name, const char* description, CVarParameter*& outParam);

    bool LookupCVar(CVarCategory category, StringUtils::StringHash nameHash, u32& outHash);

    robin_hood::unordered_map<u32, CVarParameter> savedCVars;

    using CVarLookupTable = robin_hood::unordered_map<u32, u32>; // Keyed by nameHash, gives qualifiedNameHash into savedCVars
    robin_hood::unordered_map<CVarCategory, CVarLookupTable> cvarCategoryLookupTable; // Keyed by category
};

template<typename T>
void CVarArray<T>::SetCurrent(const T &val, i32 index)
{
    cvars[index].current = val;

    CVarSystemImpl::Get()->MarkDirty();

    // Call any callbacks
    for (auto& callback : cvars[index].onValueChangedCallbacks)
    {
        callback(val);
    }
}

template<typename T>
int CVarArray<T>::Add(const T &value, CVarParameter *param)
{
    int index = lastCVar;

    cvars[index].current = value;
    cvars[index].initial = value;
    cvars[index].parameter = param;

    param->arrayIndex = index;
    lastCVar++;

    CVarSystemImpl::Get()->MarkDirty();

    return index;
}

template<typename T>
int CVarArray<T>::Add(const T &initialValue, const T &currentValue, CVarParameter *param)
{
    int index = lastCVar;

    cvars[index].current = currentValue;
    cvars[index].initial = initialValue;
    cvars[index].parameter = param;

    param->arrayIndex = index;
    lastCVar++;

    CVarSystemImpl::Get()->MarkDirty();

    return index;
}

template<>
inline CVarArray<i32> *CVarSystemImpl::GetCVarArray()
{
    return &intCVars2;
}

template<>
inline CVarArray<f64> *CVarSystemImpl::GetCVarArray()
{
    return &floatCVars;
}

template<>
inline CVarArray<std::string> *CVarSystemImpl::GetCVarArray()
{
    return &stringCVars;
}

template<>
inline CVarArray<vec4> *CVarSystemImpl::GetCVarArray()
{
    return &fvecCVars;
}

template<>
inline CVarArray<ivec4> *CVarSystemImpl::GetCVarArray()
{
    return &ivecCVars;
}

template<>
inline CVarArray<ShowFlag> *CVarSystemImpl::GetCVarArray()
{
    return &showFlagCVars;
}
