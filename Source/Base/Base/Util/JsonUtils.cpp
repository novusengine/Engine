#include "JsonUtils.h"

#include "Base/CVarSystem/CVarSystemPrivate.h"

#include <fstream>

namespace glm
{
    void to_json(nlohmann::json& j, const vec4& P) 
    {
        j = { { "x", P.x }, { "y", P.y }, { "z", P.z }, { "w", P.w } };
    };

    void from_json(const nlohmann::json& j, vec4& P) 
    {
        P.x = j.at("x").get<f32>();
        P.y = j.at("y").get<f32>();
        P.z = j.at("z").get<f32>();
        P.w = j.at("w").get<f32>();
    }

    void to_json(nlohmann::json& j, const ivec4& P) 
    {
        j = { { "x", P.x }, { "y", P.y }, { "z", P.z }, { "w", P.w } };
    };

    void from_json(const nlohmann::json& j, ivec4& P) 
    {
        P.x = j.at("x").get<i32>();
        P.y = j.at("y").get<i32>();
        P.z = j.at("z").get<i32>();
        P.w = j.at("w").get<i32>();
    }
}

namespace nlohmann 
{
    template <>
    struct adl_serializer<ShowFlag> {
        static void to_json(json& j, const ShowFlag& flag) 
        {
            j = flag == ShowFlag::ENABLED ? "ENABLED" : "DISABLED";
        }

        static void from_json(const json& j, ShowFlag& flag) 
        {
            std::string s = j.get<std::string>();
            if (s == "ENABLED") 
            {
                flag = ShowFlag::ENABLED;
            }
            else if (s == "DISABLED") 
            {
                flag = ShowFlag::DISABLED;
            }
        }
    };
}

namespace JsonUtils
{
    bool LoadFromPath(nlohmann::json& json, const std::filesystem::path& path)
    {
        std::ifstream fileStream(path, std::ifstream::in);
        if (!fileStream)
            return false;

        fileStream >> json;
        fileStream.close();

        return true;
    }

    bool LoadFromPathOrCreate(nlohmann::json& json, const nlohmann::json& fallback, const std::filesystem::path& path)
    {
        if (std::filesystem::exists(path))
            return LoadFromPath(json, path);

        if (!SaveToPath(fallback, path))
            return false;

        json = fallback;
        return true;
    }

    bool SaveToPath(const nlohmann::json& json, const std::filesystem::path& path)
    {
        std::ofstream fileStream(path);
        if (!fileStream)
            return false;

        fileStream << json.dump(4);
        fileStream.close();
        return true;
    }

    bool LoadFromPath(nlohmann::ordered_json& json, const std::filesystem::path& path)
    {
        std::ifstream fileStream(path, std::ifstream::in);
        if (!fileStream)
            return false;

        fileStream >> json;
        fileStream.close();

        return true;
    }

    bool LoadFromPathOrCreate(nlohmann::ordered_json& json, const nlohmann::ordered_json& fallback, const std::filesystem::path& path)
    {
        if (std::filesystem::exists(path))
            return LoadFromPath(json, path);

        if (!SaveToPath(fallback, path))
            return false;

        json = fallback;
        return true;
    }

    bool SaveToPath(const nlohmann::ordered_json& json, const std::filesystem::path& path)
    {
        std::ofstream fileStream(path);
        if (!fileStream)
            return false;

        fileStream << json.dump(4);
        fileStream.close();
        return true;
    }

    void VerifyCVarsOrFallback(nlohmann::json& json, const nlohmann::json& fallback)
    {
        // First implementation of CVARs didn't have a version field
        if (json.find("version") == json.end())
        {
            json = fallback;
            return;
        }
        
        u32 version = json["version"].get<u32>();
        if (version != CVAR_VERSION)
        {
            json = fallback;
        }
    }

    void SaveCVarsToJson(nlohmann::json& json)
    {
        CVarSystemImpl* cvarSystem = CVarSystemImpl::Get();

        // Save Integers
        {
            nlohmann::json& config = json["integer"];
            for (i32 i = 0; i < cvarSystem->GetCVarArray<i32>()->lastCVar; i++)
            {
                CVarStorage<i32>& cvar = cvarSystem->GetCVarArray<i32>()->cvars[i];
                CVarParameter* parameter = cvar.parameter;

                if ((parameter->flags & CVarFlags::DoNotSave) != CVarFlags::None)
                    continue;

                nlohmann::json object = nlohmann::json::object();
                {
                    object["category"] = parameter->category;
                    object["name"] = parameter->name;
                    object["initial"] = cvar.initial;
                    object["current"] = cvar.current;
                    object["type"] = parameter->type;
                    object["flags"] = parameter->flags;
                    object["description"] = parameter->description;
                }

                std::string qualifiedName = CVarSystemImpl::GetQualifiedName(parameter->category, parameter->name.c_str());
                config[qualifiedName] = object;
            }
        }

        // Save Doubles
        {
            nlohmann::json& config = json["double"];
            for (i32 i = 0; i < cvarSystem->GetCVarArray<f64>()->lastCVar; i++)
            {
                CVarStorage<f64>& cvar = cvarSystem->GetCVarArray<f64>()->cvars[i];
                CVarParameter* parameter = cvar.parameter;

                if ((parameter->flags & CVarFlags::DoNotSave) != CVarFlags::None)
                    continue;

                nlohmann::json object = nlohmann::json::object();
                {
                    object["category"] = parameter->category;
                    object["name"] = parameter->name;
                    object["initial"] = cvar.initial;
                    object["current"] = cvar.current;
                    object["type"] = parameter->type;
                    object["flags"] = parameter->flags;
                    object["description"] = parameter->description;
                }

                std::string qualifiedName = CVarSystemImpl::GetQualifiedName(parameter->category, parameter->name.c_str());
                config[qualifiedName] = object;
            }
        }

        // Save Strings
        {
            nlohmann::json& config = json["string"];
            for (i32 i = 0; i < cvarSystem->GetCVarArray<std::string>()->lastCVar; i++)
            {
                CVarStorage<std::string>& cvar = cvarSystem->GetCVarArray<std::string>()->cvars[i];
                CVarParameter* parameter = cvar.parameter;

                if ((parameter->flags & CVarFlags::DoNotSave) != CVarFlags::None)
                    continue;

                nlohmann::json object = nlohmann::json::object();
                {
                    object["category"] = parameter->category;
                    object["name"] = parameter->name;
                    object["initial"] = cvar.initial;
                    object["current"] = cvar.current;
                    object["type"] = parameter->type;
                    object["flags"] = parameter->flags;
                    object["description"] = parameter->description;
                }

                std::string qualifiedName = CVarSystemImpl::GetQualifiedName(parameter->category, parameter->name.c_str());
                config[qualifiedName] = object;
            }
        }

        // Save Vec4s
        {
            nlohmann::json& config = json["vec4"];
            for (i32 i = 0; i < cvarSystem->GetCVarArray<vec4>()->lastCVar; i++)
            {
                CVarStorage<vec4>& cvar = cvarSystem->GetCVarArray<vec4>()->cvars[i];
                CVarParameter* parameter = cvar.parameter;

                if ((parameter->flags & CVarFlags::DoNotSave) != CVarFlags::None)
                    continue;

                nlohmann::json object = nlohmann::json::object();
                {
                    object["category"] = parameter->category;
                    object["name"] = parameter->name;
                    object["initial"] = cvar.initial;
                    object["current"] = cvar.current;
                    object["type"] = parameter->type;
                    object["flags"] = parameter->flags;
                    object["description"] = parameter->description;
                }

                std::string qualifiedName = CVarSystemImpl::GetQualifiedName(parameter->category, parameter->name.c_str());
                config[qualifiedName] = object;
            }
        }

        // Save IVec4s
        {
            nlohmann::json& config = json["ivec4"];
            for (i32 i = 0; i < cvarSystem->GetCVarArray<ivec4>()->lastCVar; i++)
            {
                CVarStorage<ivec4>& cvar = cvarSystem->GetCVarArray<ivec4>()->cvars[i];
                CVarParameter* parameter = cvar.parameter;

                if ((parameter->flags & CVarFlags::DoNotSave) != CVarFlags::None)
                    continue;

                nlohmann::json object = nlohmann::json::object();
                {
                    object["category"] = parameter->category;
                    object["name"] = parameter->name;
                    object["initial"] = cvar.initial;
                    object["current"] = cvar.current;
                    object["type"] = parameter->type;
                    object["flags"] = parameter->flags;
                    object["description"] = parameter->description;
                }

                std::string qualifiedName = CVarSystemImpl::GetQualifiedName(parameter->category, parameter->name.c_str());
                config[qualifiedName] = object;
            }
        }

        // Save ShowFlags
        {
            nlohmann::json& config = json["showflag"];
            for (i32 i = 0; i < cvarSystem->GetCVarArray<ShowFlag>()->lastCVar; i++)
            {
                CVarStorage<ShowFlag>& cvar = cvarSystem->GetCVarArray<ShowFlag>()->cvars[i];
                CVarParameter* parameter = cvar.parameter;

                if ((parameter->flags & CVarFlags::DoNotSave) != CVarFlags::None)
                    continue;

                nlohmann::json object = nlohmann::json::object();
                {
                    object["category"] = parameter->category;
                    object["name"] = parameter->name;
                    object["initial"] = cvar.initial;
                    object["current"] = cvar.current;
                    object["type"] = parameter->type;
                    object["flags"] = parameter->flags;
                    object["description"] = parameter->description;
                }

                std::string qualifiedName = CVarSystemImpl::GetQualifiedName(parameter->category, parameter->name.c_str());
                config[qualifiedName] = object;
            }
        }
    }

    void LoadCVarsFromJson(nlohmann::json& json)
    {
        CVarSystemImpl* cvarSystem = CVarSystemImpl::Get();

        // Load Integers
        {
            nlohmann::json& config = json["integer"];

            for (auto it = config.begin(); it != config.end(); it++)
            {
                const std::string& key = it.key();
                nlohmann::json& value = it.value();

                CVarCategory category = value["category"].get<CVarCategory>();
                std::string& name = value["name"].get_ref<std::string&>();

                u32 cvarQualifiedNameHash = StringUtils::fnv1a_32(key.c_str(), key.length());
                i32 initial = value["initial"].get<i32>();
                i32 current = value["current"].get<i32>();
                std::string& description = value["description"].get_ref<std::string&>();
                CVarFlags flags = value["flags"].get<CVarFlags>();

                CVarParameter* parameter = cvarSystem->GetCVar(cvarQualifiedNameHash);
                if (!parameter)
                {
                    if ((flags & CVarFlags::RuntimeCreated) != CVarFlags::None)
                    {
                        parameter = cvarSystem->CreateIntCVar(category, name.c_str(), description.c_str(), initial, current, flags);
                    }
                }
                else
                {
                    CVarStorage<i32>* storage = cvarSystem->GetCVarArray<i32>()->GetCurrentStorage(parameter->arrayIndex);

                    storage->initial = initial;
                    storage->current = current;
                    parameter->description = description;
                    parameter->type = value["type"].get<CVarType>();
                    parameter->flags = flags;
                }
            }
        }

        // Load Doubles
        {
            nlohmann::json& config = json["double"];

            for (auto it = config.begin(); it != config.end(); it++)
            {
                const std::string& key = it.key();
                nlohmann::json& value = it.value();

                CVarCategory category = value["category"].get<CVarCategory>();
                std::string& name = value["name"].get_ref<std::string&>();

                u32 cvarQualifiedNameHash = StringUtils::fnv1a_32(key.c_str(), key.length());
                f64 initial = value["initial"].get<f64>();
                f64 current = value["current"].get<f64>();
                std::string& description = value["description"].get_ref<std::string&>();
                CVarFlags flags = value["flags"].get<CVarFlags>();

                CVarParameter* parameter = cvarSystem->GetCVar(cvarQualifiedNameHash);
                if (!parameter)
                {
                    if ((flags & CVarFlags::RuntimeCreated) != CVarFlags::None)
                    {
                        parameter = cvarSystem->CreateFloatCVar(category, name.c_str(), description.c_str(), initial, current, flags);
                    }
                }
                else
                {
                    CVarStorage<f64>* storage = cvarSystem->GetCVarArray<f64>()->GetCurrentStorage(parameter->arrayIndex);

                    storage->initial = initial;
                    storage->current = current;
                    parameter->description = description;
                    parameter->type = value["type"].get<CVarType>();
                    parameter->flags = value["flags"].get<CVarFlags>();
                }
            }
        }

        // Load Strings
        {
            nlohmann::json& config = json["string"];

            for (auto it = config.begin(); it != config.end(); it++)
            {
                const std::string& key = it.key();
                nlohmann::json& value = it.value();

                CVarCategory category = value["category"].get<CVarCategory>();
                std::string& name = value["name"].get_ref<std::string&>();

                u32 cvarQualifiedNameHash = StringUtils::fnv1a_32(key.c_str(), key.length());
                std::string& initial = value["initial"].get_ref<std::string&>();
                std::string& current = value["current"].get_ref<std::string&>();
                std::string& description = value["description"].get_ref<std::string&>();
                CVarFlags flags = value["flags"].get<CVarFlags>();

                CVarParameter* parameter = cvarSystem->GetCVar(cvarQualifiedNameHash);
                if (!parameter)
                {
                    if ((flags & CVarFlags::RuntimeCreated) != CVarFlags::None)
                    {
                        parameter = cvarSystem->CreateStringCVar(category, name.c_str(),description.c_str(), initial.c_str(), current.c_str(), flags);
                    }
                }
                else
                {
                    CVarStorage<std::string>* storage = cvarSystem->GetCVarArray<std::string>()->GetCurrentStorage(parameter->arrayIndex);

                    storage->initial = initial;
                    storage->current = current;
                    parameter->description = description;
                    parameter->type = value["type"].get<CVarType>();
                    parameter->flags = value["flags"].get<CVarFlags>();
                }
            }
        }

        // Load Vec4s
        {
            nlohmann::json& config = json["vec4"];

            for (auto it = config.begin(); it != config.end(); it++)
            {
                const std::string& key = it.key();
                nlohmann::json& value = it.value();

                CVarCategory category = value["category"].get<CVarCategory>();
                std::string& name = value["name"].get_ref<std::string&>();

                u32 cvarQualifiedNameHash = StringUtils::fnv1a_32(key.c_str(), key.length());
                vec4 initial = value["initial"].get<vec4>();
                vec4 current = value["current"].get<vec4>();
                std::string& description = value["description"].get_ref<std::string&>();
                CVarFlags flags = value["flags"].get<CVarFlags>();

                CVarParameter* parameter = cvarSystem->GetCVar(cvarQualifiedNameHash);
                if (!parameter)
                {
                    if ((flags & CVarFlags::RuntimeCreated) != CVarFlags::None)
                    {
                        parameter = cvarSystem->CreateVecFloatCVar(category, name.c_str(), description.c_str(), initial, current, flags);
                    }
                }
                else
                {
                    CVarStorage<vec4>* storage = cvarSystem->GetCVarArray<vec4>()->GetCurrentStorage(parameter->arrayIndex);

                    storage->initial = initial;
                    storage->current = current;
                    parameter->description = description;
                    parameter->type = value["type"].get<CVarType>();
                    parameter->flags = value["flags"].get<CVarFlags>();
                }
            }
        }

        // Load IVec4s
        {
            nlohmann::json& config = json["ivec4"];

            for (auto it = config.begin(); it != config.end(); it++)
            {
                const std::string& key = it.key();
                nlohmann::json& value = it.value();

                CVarCategory category = value["category"].get<CVarCategory>();
                std::string& name = value["name"].get_ref<std::string&>();

                u32 cvarQualifiedNameHash = StringUtils::fnv1a_32(key.c_str(), key.length());
                ivec4 initial = value["initial"].get<ivec4>();
                ivec4 current = value["current"].get<ivec4>();
                std::string& description = value["description"].get_ref<std::string&>();
                CVarFlags flags = value["flags"].get<CVarFlags>();

                CVarParameter* parameter = cvarSystem->GetCVar(cvarQualifiedNameHash);
                if (!parameter)
                {
                    if ((flags & CVarFlags::RuntimeCreated) != CVarFlags::None)
                    {
                        parameter = cvarSystem->CreateVecFloatCVar(category, name.c_str(), description.c_str(), initial, current, flags);
                    }
                }
                else
                {
                    CVarStorage<ivec4>* storage = cvarSystem->GetCVarArray<ivec4>()->GetCurrentStorage(parameter->arrayIndex);

                    storage->initial = initial;
                    storage->current = current;
                    parameter->description = description;
                    parameter->type = value["type"].get<CVarType>();
                    parameter->flags = value["flags"].get<CVarFlags>();
                }
            }
        }

        // Load ShowFlags
        {
            nlohmann::json& config = json["showflag"];

            for (auto it = config.begin(); it != config.end(); it++)
            {
                const std::string& key = it.key();
                nlohmann::json& value = it.value();

                CVarCategory category = value["category"].get<CVarCategory>();
                std::string& name = value["name"].get_ref<std::string&>();

                u32 cvarQualifiedNameHash = StringUtils::fnv1a_32(key.c_str(), key.length());
                ShowFlag initial = value["initial"].get<ShowFlag>();
                ShowFlag current = value["current"].get<ShowFlag>();
                std::string& description = value["description"].get_ref<std::string&>();
                CVarFlags flags = value["flags"].get<CVarFlags>();

                CVarParameter* parameter = cvarSystem->GetCVar(cvarQualifiedNameHash);
                if (!parameter)
                {
                    if ((flags & CVarFlags::RuntimeCreated) != CVarFlags::None)
                    {
                        parameter = cvarSystem->CreateShowFlagCVar(category, name.c_str(), description.c_str(), initial, current, flags);
                    }
                }
                else
                {
                    CVarStorage<ShowFlag>* storage = cvarSystem->GetCVarArray<ShowFlag>()->GetCurrentStorage(parameter->arrayIndex);

                    storage->initial = initial;
                    storage->current = current;
                    parameter->description = description;
                    parameter->type = value["type"].get<CVarType>();
                    parameter->flags = value["flags"].get<CVarFlags>();
                }
            }
        }
    }

    void SaveCVarsToJson(nlohmann::ordered_json& json)
    {
        CVarSystemImpl* cvarSystem = CVarSystemImpl::Get();

        // Save Integers
        {
            nlohmann::ordered_json& config = json["integer"];
            for (i32 i = 0; i < cvarSystem->GetCVarArray<i32>()->lastCVar; i++)
            {
                CVarStorage<i32>& cvar = cvarSystem->GetCVarArray<i32>()->cvars[i];
                CVarParameter* parameter = cvar.parameter;

                if ((parameter->flags & CVarFlags::DoNotSave) != CVarFlags::None)
                    continue;

                nlohmann::json object = nlohmann::json::object();
                {
                    object["category"] = parameter->category;
                    object["name"] = parameter->name;
                    object["initial"] = cvar.initial;
                    object["current"] = cvar.current;
                    object["type"] = parameter->type;
                    object["flags"] = parameter->flags;
                    object["description"] = parameter->description;
                }

                std::string qualifiedName = CVarSystemImpl::GetQualifiedName(parameter->category, parameter->name.c_str());
                config[qualifiedName] = object;
            }
        }

        // Save Doubles
        {
            nlohmann::ordered_json& config = json["double"];
            for (i32 i = 0; i < cvarSystem->GetCVarArray<f64>()->lastCVar; i++)
            {
                CVarStorage<f64>& cvar = cvarSystem->GetCVarArray<f64>()->cvars[i];
                CVarParameter* parameter = cvar.parameter;

                if ((parameter->flags & CVarFlags::DoNotSave) != CVarFlags::None)
                    continue;

                nlohmann::json object = nlohmann::json::object();
                {
                    object["category"] = parameter->category;
                    object["name"] = parameter->name;
                    object["initial"] = cvar.initial;
                    object["current"] = cvar.current;
                    object["type"] = parameter->type;
                    object["flags"] = parameter->flags;
                    object["description"] = parameter->description;
                }

                std::string qualifiedName = CVarSystemImpl::GetQualifiedName(parameter->category, parameter->name.c_str());
                config[qualifiedName] = object;
            }
        }

        // Save Strings
        {
            nlohmann::ordered_json& config = json["string"];
            for (i32 i = 0; i < cvarSystem->GetCVarArray<std::string>()->lastCVar; i++)
            {
                CVarStorage<std::string>& cvar = cvarSystem->GetCVarArray<std::string>()->cvars[i];
                CVarParameter* parameter = cvar.parameter;

                if ((parameter->flags & CVarFlags::DoNotSave) != CVarFlags::None)
                    continue;

                nlohmann::json object = nlohmann::json::object();
                {
                    object["category"] = parameter->category;
                    object["name"] = parameter->name;
                    object["initial"] = cvar.initial;
                    object["current"] = cvar.current;
                    object["type"] = parameter->type;
                    object["flags"] = parameter->flags;
                    object["description"] = parameter->description;
                }

                std::string qualifiedName = CVarSystemImpl::GetQualifiedName(parameter->category, parameter->name.c_str());
                config[qualifiedName] = object;
            }
        }

        // Save Vec4s
        {
            nlohmann::ordered_json& config = json["vec4"];
            for (i32 i = 0; i < cvarSystem->GetCVarArray<vec4>()->lastCVar; i++)
            {
                CVarStorage<vec4>& cvar = cvarSystem->GetCVarArray<vec4>()->cvars[i];
                CVarParameter* parameter = cvar.parameter;

                if ((parameter->flags & CVarFlags::DoNotSave) != CVarFlags::None)
                    continue;

                nlohmann::json object = nlohmann::json::object();
                {
                    object["category"] = parameter->category;
                    object["name"] = parameter->name;
                    object["initial"] = cvar.initial;
                    object["current"] = cvar.current;
                    object["type"] = parameter->type;
                    object["flags"] = parameter->flags;
                    object["description"] = parameter->description;
                }

                std::string qualifiedName = CVarSystemImpl::GetQualifiedName(parameter->category, parameter->name.c_str());
                config[qualifiedName] = object;
            }
        }

        // Save IVec4s
        {
            nlohmann::ordered_json& config = json["ivec4"];
            for (i32 i = 0; i < cvarSystem->GetCVarArray<ivec4>()->lastCVar; i++)
            {
                CVarStorage<ivec4>& cvar = cvarSystem->GetCVarArray<ivec4>()->cvars[i];
                CVarParameter* parameter = cvar.parameter;

                if ((parameter->flags & CVarFlags::DoNotSave) != CVarFlags::None)
                    continue;

                nlohmann::json object = nlohmann::json::object();
                {
                    object["category"] = parameter->category;
                    object["name"] = parameter->name;
                    object["initial"] = cvar.initial;
                    object["current"] = cvar.current;
                    object["type"] = parameter->type;
                    object["flags"] = parameter->flags;
                    object["description"] = parameter->description;
                }

                std::string qualifiedName = CVarSystemImpl::GetQualifiedName(parameter->category, parameter->name.c_str());
                config[qualifiedName] = object;
            }
        }

        // Save ShowFlags
        {
            nlohmann::ordered_json& config = json["showflag"];
            for (i32 i = 0; i < cvarSystem->GetCVarArray<ShowFlag>()->lastCVar; i++)
            {
                CVarStorage<ShowFlag>& cvar = cvarSystem->GetCVarArray<ShowFlag>()->cvars[i];
                CVarParameter* parameter = cvar.parameter;

                if ((parameter->flags & CVarFlags::DoNotSave) != CVarFlags::None)
                    continue;

                nlohmann::json object = nlohmann::json::object();
                {
                    object["category"] = parameter->category;
                    object["name"] = parameter->name;
                    object["initial"] = cvar.initial;
                    object["current"] = cvar.current;
                    object["type"] = parameter->type;
                    object["flags"] = parameter->flags;
                    object["description"] = parameter->description;
                }

                std::string qualifiedName = CVarSystemImpl::GetQualifiedName(parameter->category, parameter->name.c_str());
                config[qualifiedName] = object;
            }
        }
    }

    void LoadCVarsFromJson(nlohmann::ordered_json& json)
    {
        CVarSystemImpl* cvarSystem = CVarSystemImpl::Get();

        // Load Integers
        {
            nlohmann::ordered_json& config = json["integer"];

            for (auto it = config.begin(); it != config.end(); it++)
            {
                const std::string& key = it.key();
                nlohmann::ordered_json& value = it.value();

                CVarCategory category = value["category"].get<CVarCategory>();
                std::string& name = value["name"].get_ref<std::string&>();

                u32 cvarQualifiedNameHash = StringUtils::fnv1a_32(key.c_str(), key.length());
                i32 initial = value["initial"].get<i32>();
                i32 current = value["current"].get<i32>();
                std::string& description = value["description"].get_ref<std::string&>();
                CVarFlags flags = value["flags"].get<CVarFlags>();

                CVarParameter* parameter = cvarSystem->GetCVar(cvarQualifiedNameHash);
                if (!parameter)
                {
                    if ((flags & CVarFlags::RuntimeCreated) != CVarFlags::None)
                    {
                        parameter = cvarSystem->CreateIntCVar(category, name.c_str(), description.c_str(), initial, current, flags);
                    }
                }
                else
                {
                    CVarStorage<i32>* storage = cvarSystem->GetCVarArray<i32>()->GetCurrentStorage(parameter->arrayIndex);

                    storage->initial = initial;
                    storage->current = current;
                    parameter->description = description;
                    parameter->type = value["type"].get<CVarType>();
                    parameter->flags = flags;
                }
            }
        }

        // Load Doubles
        {
            nlohmann::ordered_json& config = json["double"];

            for (auto it = config.begin(); it != config.end(); it++)
            {
                const std::string& key = it.key();
                nlohmann::ordered_json& value = it.value();

                CVarCategory category = value["category"].get<CVarCategory>();
                std::string& name = value["name"].get_ref<std::string&>();

                u32 cvarQualifiedNameHash = StringUtils::fnv1a_32(key.c_str(), key.length());
                f64 initial = value["initial"].get<f64>();
                f64 current = value["current"].get<f64>();
                std::string& description = value["description"].get_ref<std::string&>();
                CVarFlags flags = value["flags"].get<CVarFlags>();

                CVarParameter* parameter = cvarSystem->GetCVar(cvarQualifiedNameHash);
                if (!parameter)
                {
                    if ((flags & CVarFlags::RuntimeCreated) != CVarFlags::None)
                    {
                        parameter = cvarSystem->CreateFloatCVar(category, name.c_str(), description.c_str(), initial, current, flags);
                    }
                }
                else
                {
                    CVarStorage<f64>* storage = cvarSystem->GetCVarArray<f64>()->GetCurrentStorage(parameter->arrayIndex);

                    storage->initial = initial;
                    storage->current = current;
                    parameter->description = description;
                    parameter->type = value["type"].get<CVarType>();
                    parameter->flags = value["flags"].get<CVarFlags>();
                }
            }
        }

        // Load Strings
        {
            nlohmann::ordered_json& config = json["string"];

            for (auto it = config.begin(); it != config.end(); it++)
            {
                const std::string& key = it.key();
                nlohmann::ordered_json& value = it.value();

                CVarCategory category = value["category"].get<CVarCategory>();
                std::string& name = value["name"].get_ref<std::string&>();

                u32 cvarQualifiedNameHash = StringUtils::fnv1a_32(key.c_str(), key.length());
                std::string& initial = value["initial"].get_ref<std::string&>();
                std::string& current = value["current"].get_ref<std::string&>();
                std::string& description = value["description"].get_ref<std::string&>();
                CVarFlags flags = value["flags"].get<CVarFlags>();

                CVarParameter* parameter = cvarSystem->GetCVar(cvarQualifiedNameHash);
                if (!parameter)
                {
                    if ((flags & CVarFlags::RuntimeCreated) != CVarFlags::None)
                    {
                        parameter = cvarSystem->CreateStringCVar(category, name.c_str(), description.c_str(), initial.c_str(), current.c_str(), flags);
                    }
                }
                else
                {
                    CVarStorage<std::string>* storage = cvarSystem->GetCVarArray<std::string>()->GetCurrentStorage(parameter->arrayIndex);

                    storage->initial = initial;
                    storage->current = current;
                    parameter->description = description;
                    parameter->type = value["type"].get<CVarType>();
                    parameter->flags = value["flags"].get<CVarFlags>();
                }
            }
        }

        // Load Vec4s
        {
            nlohmann::ordered_json& config = json["vec4"];

            for (auto it = config.begin(); it != config.end(); it++)
            {
                const std::string& key = it.key();
                nlohmann::ordered_json& value = it.value();

                CVarCategory category = value["category"].get<CVarCategory>();
                std::string& name = value["name"].get_ref<std::string&>();

                u32 cvarQualifiedNameHash = StringUtils::fnv1a_32(key.c_str(), key.length());
                vec4 initial = value["initial"].get<vec4>();
                vec4 current = value["current"].get<vec4>();
                std::string& description = value["description"].get_ref<std::string&>();
                CVarFlags flags = value["flags"].get<CVarFlags>();

                CVarParameter* parameter = cvarSystem->GetCVar(cvarQualifiedNameHash);
                if (!parameter)
                {
                    if ((flags & CVarFlags::RuntimeCreated) != CVarFlags::None)
                    {
                        parameter = cvarSystem->CreateVecFloatCVar(category, name.c_str(), description.c_str(), initial, current, flags);
                    }
                }
                else
                {
                    CVarStorage<vec4>* storage = cvarSystem->GetCVarArray<vec4>()->GetCurrentStorage(parameter->arrayIndex);

                    storage->initial = initial;
                    storage->current = current;
                    parameter->description = description;
                    parameter->type = value["type"].get<CVarType>();
                    parameter->flags = value["flags"].get<CVarFlags>();
                }
            }
        }

        // Load IVec4s
        {
            nlohmann::ordered_json& config = json["ivec4"];

            for (auto it = config.begin(); it != config.end(); it++)
            {
                const std::string& key = it.key();
                nlohmann::ordered_json& value = it.value();

                CVarCategory category = value["category"].get<CVarCategory>();
                std::string& name = value["name"].get_ref<std::string&>();

                u32 cvarQualifiedNameHash = StringUtils::fnv1a_32(key.c_str(), key.length());
                ivec4 initial = value["initial"].get<ivec4>();
                ivec4 current = value["current"].get<ivec4>();
                std::string& description = value["description"].get_ref<std::string&>();
                CVarFlags flags = value["flags"].get<CVarFlags>();

                CVarParameter* parameter = cvarSystem->GetCVar(cvarQualifiedNameHash);
                if (!parameter)
                {
                    if ((flags & CVarFlags::RuntimeCreated) != CVarFlags::None)
                    {
                        parameter = cvarSystem->CreateVecFloatCVar(category, name.c_str(), description.c_str(), initial, current, flags);
                    }
                }
                else
                {
                    CVarStorage<ivec4>* storage = cvarSystem->GetCVarArray<ivec4>()->GetCurrentStorage(parameter->arrayIndex);

                    storage->initial = initial;
                    storage->current = current;
                    parameter->description = description;
                    parameter->type = value["type"].get<CVarType>();
                    parameter->flags = value["flags"].get<CVarFlags>();
                }
            }
        }

        // Load ShowFlags
        {
            nlohmann::ordered_json& config = json["showflag"];

            for (auto it = config.begin(); it != config.end(); it++)
            {
                const std::string& key = it.key();
                nlohmann::ordered_json& value = it.value();

                CVarCategory category = value["category"].get<CVarCategory>();
                std::string& name = value["name"].get_ref<std::string&>();

                u32 cvarQualifiedNameHash = StringUtils::fnv1a_32(key.c_str(), key.length());
                ShowFlag initial = value["initial"].get<ShowFlag>();
                ShowFlag current = value["current"].get<ShowFlag>();
                std::string& description = value["description"].get_ref<std::string&>();
                CVarFlags flags = value["flags"].get<CVarFlags>();

                CVarParameter* parameter = cvarSystem->GetCVar(cvarQualifiedNameHash);
                if (!parameter)
                {
                    if ((flags & CVarFlags::RuntimeCreated) != CVarFlags::None)
                    {
                        parameter = cvarSystem->CreateShowFlagCVar(category, name.c_str(), description.c_str(), initial, current, flags);
                    }
                }
                else
                {
                    CVarStorage<ShowFlag>* storage = cvarSystem->GetCVarArray<ShowFlag>()->GetCurrentStorage(parameter->arrayIndex);

                    storage->initial = initial;
                    storage->current = current;
                    parameter->description = description;
                    parameter->type = value["type"].get<CVarType>();
                    parameter->flags = value["flags"].get<CVarFlags>();
                }
            }
        }
    }
}