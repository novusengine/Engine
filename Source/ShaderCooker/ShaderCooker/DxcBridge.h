#pragma once
#include <Base/Types.h>

#include <string>
#include <filesystem>

struct DxcDefine;

namespace ShaderCooker
{
    class IncludeHandler;

    struct IDxcBridgeData {};

    class DxcBridge
    {
    public:
        DxcBridge();
        ~DxcBridge();

        void AddDefine(DxcDefine* define);
        void ClearDefines();

        void AddIncludeDir(std::filesystem::path path);
        bool Compile(std::filesystem::path path, std::string& source, char*& blob, size_t& blobSize);

    private:
        bool GetProfileFromFilename(std::filesystem::path filename, std::wstring& profile, std::wstring& profileType);

    private:
        IncludeHandler* _includeHandler;
        IDxcBridgeData* _data;
    };
}