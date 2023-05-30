#pragma once
#include "Base/Types.h"
#include "Base/Util/StringUtils.h"

enum class CVarFlags : u32
{
    None = 0,
    Noedit = 1 << 1,
    EditReadOnly = 1 << 2,
    Advanced = 1 << 3,

    EditCheckbox = 1 << 8,
    EditFloatDrag = 1 << 9,
};

enum class ShowFlag : bool
{
    ENABLED,
    DISABLED
};

class CVarParameter;
class CVarSystem
{
public:
    static CVarSystem* Get();

    //pimpl
    virtual CVarParameter* GetCVar(StringUtils::StringHash hash) = 0;

    virtual f64* GetFloatCVar(StringUtils::StringHash hash) = 0;
    virtual i32* GetIntCVar(StringUtils::StringHash hash) = 0;
    virtual const char* GetStringCVar(StringUtils::StringHash hash) = 0;
    virtual vec4* GetVecFloatCVar(StringUtils::StringHash hash) = 0;
    virtual ivec4* GetVecIntCVar(StringUtils::StringHash hash) = 0;
    virtual ShowFlag* GetShowFlagCVar(StringUtils::StringHash hash) = 0;

    virtual void SetFloatCVar(StringUtils::StringHash hash, f64 value) = 0;
    virtual void SetIntCVar(StringUtils::StringHash hash, i32 value) = 0;
    virtual void SetStringCVar(StringUtils::StringHash hash, const char* value) = 0;
    virtual void SetVecFloatCVar(StringUtils::StringHash hash, const vec4& value) = 0;
    virtual void SetVecIntCVar(StringUtils::StringHash hash, const ivec4& value) = 0;
    virtual void SetShowFlagCVar(StringUtils::StringHash hash, const ShowFlag& value) = 0;

    virtual CVarParameter* CreateFloatCVar(const char* name, const char* description, f64 defaultValue, f64 currentValue) = 0;
    virtual CVarParameter* CreateIntCVar(const char* name, const char* description, i32 defaultValue, i32 currentValue) = 0;
    virtual CVarParameter* CreateStringCVar(const char* name, const char* description, const char* defaultValue, const char* currentValue) = 0;
    virtual CVarParameter* CreateVecFloatCVar(const char* name, const char* description, const vec4& defaultValue, const vec4& currentValue) = 0;
    virtual CVarParameter* CreateVecIntCVar(const char* name, const char* description, const ivec4& defaultValue, const ivec4& currentValue) = 0;
    virtual CVarParameter* CreateShowFlagCVar(const char* name, const char* description, const ShowFlag& defaultValue, const ShowFlag& currentValue) = 0;

    void MarkDirty() { _isDirty = true; }
    void ClearDirty() { _isDirty = false; }
    bool IsDirty() { return _isDirty; }

private:
    bool _isDirty = false;
};

struct AutoCVar
{
protected:
    int index;
};

struct AutoCVar_Float : AutoCVar
{
public:
    AutoCVar_Float(const char* name, const char* description, f64 defaultValue, CVarFlags flags = CVarFlags::None);

    f64 Get();
    f64* GetPtr();
    f32 GetFloat();
    f32* GetFloatPtr();
    void Set(f64 val);
};

struct AutoCVar_Int : AutoCVar
{
public:
    AutoCVar_Int(const char* name, const char* description, i32 defaultValue, CVarFlags flags = CVarFlags::None);
    i32 Get();
    i32* GetPtr();
    void Set(i32 val);

    void Toggle();
};

struct AutoCVar_String : AutoCVar
{
public:
    AutoCVar_String(const char* name, const char* description, const char* defaultValue, CVarFlags flags = CVarFlags::None);

    const char* Get();
    void Set(std::string&& val);
};

struct AutoCVar_VecFloat : AutoCVar
{
public:
    AutoCVar_VecFloat(const char* name, const char* description, const vec4& defaultValue, CVarFlags flags = CVarFlags::None);

    vec4 Get();
    void Set(const vec4& val);
};

struct AutoCVar_VecInt : AutoCVar
{
public:
    AutoCVar_VecInt(const char* name, const char* description, const ivec4& defaultValue, CVarFlags flags = CVarFlags::None);

    ivec4 Get();
    void Set(const ivec4& val);
};

struct AutoCVar_ShowFlag : AutoCVar
{
public:
    AutoCVar_ShowFlag(const char* name, const char* description, const ShowFlag& defaultValue);

    ShowFlag Get();
    void Set(const ShowFlag& val);
};