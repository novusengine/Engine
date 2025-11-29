#pragma once
#include <Base/Types.h>

#include <FileFormat/Novus/ShaderPack/ShaderPack.h>

#include <string>
#include <filesystem>

struct DxcDefine;

namespace ShaderCooker
{
    struct ISlangBridgeData {};

    class SlangBridge
    {
    public:
        SlangBridge(std::filesystem::path sourceDir);
        ~SlangBridge();

        void AddDefine(const std::string& name, const std::string& value);
        void ClearDefines();

        void AddIncludeDir(std::filesystem::path path);
        bool Compile(std::filesystem::path path, const std::string& source, FileFormat::ShaderInMemory& shaderInMemory);

    private:
        bool GetProfileFromFilename(std::filesystem::path filename, std::string& profile, std::string& profileType);

    private:
        ISlangBridgeData* _data;
    };
}