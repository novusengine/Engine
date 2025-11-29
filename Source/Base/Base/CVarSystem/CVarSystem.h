#pragma once
#include "Base/Types.h"
#include "Base/Util/StringUtils.h"

enum class CVarFlags : u32
{
    None = 0,
    Hidden = 1 << 1,
    EditReadOnly = 1 << 2,
    Advanced = 1 << 3,

    EditCheckbox = 1 << 8,
    EditFloatDrag = 1 << 9,

    RuntimeCreated = 1 << 20,
    DoNotSave = 1 << 21,
};
DECLARE_GENERIC_BITWISE_OPERATORS(CVarFlags);

enum class ShowFlag : bool
{
    ENABLED,
    DISABLED
};

enum class CVarCategory : u8
{
    Server = 1 << 0,
    Client = 1 << 1,
    Rendering = 1 << 2,
    Physics = 1 << 3,
    Network = 1 << 4,
    Database = 1 << 5,

    COUNT = 6, // Keep this updated!
};
DECLARE_GENERIC_BITWISE_OPERATORS(CVarCategory);

static const char* CVarCategoryToString[u8(CVarCategory::COUNT)] =
{
    "Server",
    "Client",
    "Rendering",
    "Physics",
    "Network",
    "Database"
};

static const char* CVarCategoryToPrefix[u8(CVarCategory::COUNT)] =
{
    "sv_",
    "cl_",
    "r_",
    "phys_",
    "net_",
    "db_"
};

class CVarParameter;
class CVarSystem
{
public:
    static CVarSystem* Get();
    static std::string GetQualifiedName(CVarCategory category, const char* name);

    //pimpl
    virtual CVarParameter* GetCVar(StringUtils::StringHash hash) = 0;
    virtual CVarParameter* GetCVar(CVarCategory category, StringUtils::StringHash hash) = 0;

    virtual f64* GetFloatCVar(StringUtils::StringHash hash) = 0;
    virtual f64* GetFloatCVar(CVarCategory category, StringUtils::StringHash hash) = 0;
    virtual i32* GetIntCVar(StringUtils::StringHash hash) = 0;
    virtual i32* GetIntCVar(CVarCategory category, StringUtils::StringHash hash) = 0;
    virtual const char* GetStringCVar(StringUtils::StringHash hash) = 0;
    virtual const char* GetStringCVar(CVarCategory category, StringUtils::StringHash hash) = 0;
    virtual vec4* GetVecFloatCVar(StringUtils::StringHash hash) = 0;
    virtual vec4* GetVecFloatCVar(CVarCategory category, StringUtils::StringHash hash) = 0;
    virtual ivec4* GetVecIntCVar(StringUtils::StringHash hash) = 0;
    virtual ivec4* GetVecIntCVar(CVarCategory category, StringUtils::StringHash hash) = 0;
    virtual ShowFlag* GetShowFlagCVar(StringUtils::StringHash hash) = 0;
    virtual ShowFlag* GetShowFlagCVar(CVarCategory category, StringUtils::StringHash hash) = 0;

    virtual void SetFloatCVar(StringUtils::StringHash hash, f64 value) = 0;
    virtual void SetFloatCVar(CVarCategory category, StringUtils::StringHash hash, f64 value) = 0;
    virtual void SetIntCVar(StringUtils::StringHash hash, i32 value) = 0;
    virtual void SetIntCVar(CVarCategory category, StringUtils::StringHash hash, i32 value) = 0;
    virtual void SetStringCVar(StringUtils::StringHash hash, const char* value) = 0;
    virtual void SetStringCVar(CVarCategory category, StringUtils::StringHash hash, const char* value) = 0;
    virtual void SetVecFloatCVar(StringUtils::StringHash hash, const vec4& value) = 0;
    virtual void SetVecFloatCVar(CVarCategory category, StringUtils::StringHash hash, const vec4& value) = 0;
    virtual void SetVecIntCVar(StringUtils::StringHash hash, const ivec4& value) = 0;
    virtual void SetVecIntCVar(CVarCategory category, StringUtils::StringHash hash, const ivec4& value) = 0;
    virtual void SetShowFlagCVar(StringUtils::StringHash hash, const ShowFlag& value) = 0;
    virtual void SetShowFlagCVar(CVarCategory category, StringUtils::StringHash hash, const ShowFlag& value) = 0;

    virtual void AddOnFloatValueChanged(StringUtils::StringHash hash, std::function<void(const f64&)> callback) = 0;
    virtual void AddOnFloatValueChanged(CVarCategory category, StringUtils::StringHash hash, std::function<void(const f64&)> callback) = 0;
    virtual void AddOnIntValueChanged(StringUtils::StringHash hash, std::function<void(const i32&)> callback) = 0;
    virtual void AddOnIntValueChanged(CVarCategory category, StringUtils::StringHash hash, std::function<void(const i32&)> callback) = 0;
    virtual void AddOnStringValueChanged(StringUtils::StringHash hash, std::function<void(const std::string&)> callback) = 0;
    virtual void AddOnStringValueChanged(CVarCategory category, StringUtils::StringHash hash, std::function<void(const std::string&)> callback) = 0;
    virtual void AddOnVecFloatValueChanged(StringUtils::StringHash hash, std::function<void(const vec4&)> callback) = 0;
    virtual void AddOnVecFloatValueChanged(CVarCategory category, StringUtils::StringHash hash, std::function<void(const vec4&)> callback) = 0;
    virtual void AddOnVecIntValueChanged(StringUtils::StringHash hash, std::function<void(const ivec4&)> callback) = 0;
    virtual void AddOnVecIntValueChanged(CVarCategory category, StringUtils::StringHash hash, std::function<void(const ivec4&)> callback) = 0;
    virtual void AddOnShowFlagValueChanged(StringUtils::StringHash hash, std::function<void(const ShowFlag&)> callback) = 0;
    virtual void AddOnShowFlagValueChanged(CVarCategory category, StringUtils::StringHash hash, std::function<void(const ShowFlag&)> callback) = 0;

    virtual CVarParameter* CreateFloatCVar(CVarCategory category, const char* name, const char* description, f64 defaultValue, f64 currentValue, CVarFlags flags = CVarFlags::None) = 0;
    virtual CVarParameter* CreateIntCVar(CVarCategory category, const char* name, const char* description, i32 defaultValue, i32 currentValue, CVarFlags flags = CVarFlags::None) = 0;
    virtual CVarParameter* CreateStringCVar(CVarCategory category, const char* name, const char* description, const char* defaultValue, const char* currentValue, CVarFlags flags = CVarFlags::None) = 0;
    virtual CVarParameter* CreateVecFloatCVar(CVarCategory category, const char* name, const char* description, const vec4& defaultValue, const vec4& currentValue, CVarFlags flags = CVarFlags::None) = 0;
    virtual CVarParameter* CreateVecIntCVar(CVarCategory category, const char* name, const char* description, const ivec4& defaultValue, const ivec4& currentValue, CVarFlags flags = CVarFlags::None) = 0;
    virtual CVarParameter* CreateShowFlagCVar(CVarCategory category, const char* name, const char* description, const ShowFlag& defaultValue, const ShowFlag& currentValue, CVarFlags flags = CVarFlags::None) = 0;

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
    AutoCVar_Float(CVarCategory category, const char* name, const char* description, f64 defaultValue, CVarFlags flags = CVarFlags::None);

    f64 Get();
    f64* GetPtr();
    f32 GetFloat();
    f32* GetFloatPtr();
    void Set(f64 val);

    void AddOnValueChanged(std::function<void(const f64&)> callback);
};

struct AutoCVar_Int : AutoCVar
{
public:
    AutoCVar_Int(CVarCategory category, const char* name, const char* description, i32 defaultValue, CVarFlags flags = CVarFlags::None);
    i32 Get();
    u32 GetU32();
    i32* GetPtr();
    void Set(i32 val);

    void Toggle();

    void AddOnValueChanged(std::function<void(const i32&)> callback);
};

struct AutoCVar_String : AutoCVar
{
public:
    AutoCVar_String(CVarCategory category, const char* name, const char* description, const char* defaultValue, CVarFlags flags = CVarFlags::None);

    const char* Get();
    void Set(std::string&& val);

    void AddOnValueChanged(std::function<void(const std::string&)> callback);
};

struct AutoCVar_VecFloat : AutoCVar
{
public:
    AutoCVar_VecFloat(CVarCategory category, const char* name, const char* description, const vec4& defaultValue, CVarFlags flags = CVarFlags::None);

    vec4 Get();
    void Set(const vec4& val);

    void AddOnValueChanged(std::function<void(const vec4&)> callback);
};

struct AutoCVar_VecInt : AutoCVar
{
public:
    AutoCVar_VecInt(CVarCategory category, const char* name, const char* description, const ivec4& defaultValue, CVarFlags flags = CVarFlags::None);

    ivec4 Get();
    void Set(const ivec4& val);

    void AddOnValueChanged(std::function<void(const ivec4&)> callback);
};

struct AutoCVar_ShowFlag : AutoCVar
{
public:
    AutoCVar_ShowFlag(CVarCategory category, const char* name, const char* description, const ShowFlag& defaultValue, CVarFlags flags = CVarFlags::None);

    ShowFlag Get();
    void Set(const ShowFlag& val);

    void AddOnValueChanged(std::function<void(const ShowFlag&)> callback);
};