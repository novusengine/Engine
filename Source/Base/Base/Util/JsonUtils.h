#pragma once
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

	void LoadCVarsIntoJson(nlohmann::json& json);
	void LoadJsonIntoCVars(nlohmann::json& json);

	void LoadCVarsIntoJson(nlohmann::ordered_json& json);
	void LoadJsonIntoCVars(nlohmann::ordered_json& json);
}