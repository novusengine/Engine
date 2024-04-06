#pragma once
#include <Base/Types.h>
#include <json/json.hpp>

#include <filesystem>

class CVarSystem;
namespace JsonUtils
{
    bool LoadFromPath(nlohmann::json& json, const std::filesystem::path& path);
    bool LoadFromPathOrCreate(nlohmann::json& json, const nlohmann::json& fallback, const std::filesystem::path& path);
    bool SaveToPath(const nlohmann::json& json, const std::filesystem::path& path);

    bool LoadFromPath(nlohmann::ordered_json& json, const std::filesystem::path& path);
    bool LoadFromPathOrCreate(nlohmann::ordered_json& json, const nlohmann::ordered_json& fallback, const std::filesystem::path& path);
    bool SaveToPath(const nlohmann::ordered_json& json, const std::filesystem::path& path);

    constexpr u32 CVAR_VERSION = 1u;
    void VerifyCVarsOrFallback(nlohmann::json& json, const nlohmann::json& fallback);
    void SaveCVarsToJson(nlohmann::json& json);
    void LoadCVarsFromJson(nlohmann::json& json);

    void SaveCVarsToJson(nlohmann::ordered_json& json);
    void LoadCVarsFromJson(nlohmann::ordered_json& json);
}