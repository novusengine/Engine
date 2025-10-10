#include "DxcBridge.h"
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"

#include <Base/Platform.h>
#include <Base/Util/DebugHandler.h>
#include <Base/Util/StringUtils.h>

#ifdef WIN32
#include <atlbase.h>
#include <wrl/client.h>
#else
#include <WinAdapter.h>
#endif
#include "dxcapi.h"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <cwctype>

namespace fs = std::filesystem;

DxcDefine MakeDefine(const std::wstring& name, const std::wstring& value)
{
    DxcDefine define;
    define.Name = new wchar_t[name.size() + 1];
    wmemcpy((wchar_t*)define.Name, name.c_str(), name.size() + 1);

    define.Value = new wchar_t[value.size() + 1];
    wmemcpy((wchar_t*)define.Value, value.c_str(), value.size() + 1);

    return define;
}

namespace ShaderCooker
{
    class IncludeHandler : public IDxcIncludeHandler
    {
    public:
        IncludeHandler(IDxcUtils* utils)
            : _utils(utils)
            , _ref(1)
        {
        }

        void AddIncludeDirectory(const fs::path& path)
        {
            _includeDirectories.push_back(path);
        }

        ULONG AddRef() override
        {
            //InterlockedIncrement(&_ref);
            return static_cast<ULONG>(_ref.fetch_add(1) + 1);
        }

        ULONG Release() override
        {
            if (_ref <= 0)
            {
                NC_LOG_CRITICAL("Inconsistent call to Release()");
            }
            return static_cast<ULONG>(_ref.fetch_add(-1) - 1);
        }

    private:
        HRESULT STDMETHODCALLTYPE LoadSource(
            _In_z_ LPCWSTR pFilename,                                 // Candidate filename.
            _COM_Outptr_result_maybenull_ IDxcBlob** ppIncludeSource  // Resultant source object for included file, nullptr if not found.
        ) override
        {
            const fs::path filePath = pFilename;
            for (const fs::path& includeDir : _includeDirectories)
            {
                const fs::path fullPath = includeDir / filePath;
                if (fs::exists(fullPath))
                {
                    _utils->LoadFile(fullPath.wstring().c_str(), nullptr, (IDxcBlobEncoding**)ppIncludeSource);
                    return S_OK;
                }
            }

            *ppIncludeSource = nullptr;
            return E_FAIL;
        }

        HRESULT QueryInterface(REFIID riid, LPVOID* ppvObj) override
        {
            return E_FAIL;
        }

        IDxcUtils* _utils;
        std::atomic<i64> _ref;

        std::vector<fs::path> _includeDirectories;
    };

    struct DxcBridgeData : IDxcBridgeData
    {
        std::vector<DxcDefine> extraDefines;

        CComPtr<IDxcUtils> utils;
        CComPtr<IDxcCompiler> compiler;
    };

    DxcBridge::DxcBridge()
    {
        DxcBridgeData* data = new DxcBridgeData();
        _data = data;

        HRESULT r;

        r = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&data->utils));
        if (r != S_OK)
        {
            NC_LOG_CRITICAL("Failed to create DXC Utils");
        }

        r = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&data->compiler));
        if (r != S_OK)
        {
            NC_LOG_CRITICAL("Failed to create DXC Compiler");
        }

        _includeHandler = new IncludeHandler(data->utils);
    }

    DxcBridge::~DxcBridge()
    {
        _includeHandler->Release();
    }

    void DxcBridge::AddDefine(DxcDefine* define)
    {
        DxcBridgeData* data = static_cast<DxcBridgeData*>(_data);
        data->extraDefines.push_back(*define);
    }

    void DxcBridge::ClearDefines()
    {
        DxcBridgeData* data = static_cast<DxcBridgeData*>(_data);
        data->extraDefines.clear();
    }

    void DxcBridge::AddIncludeDir(std::filesystem::path path)
    {
        _includeHandler->AddIncludeDirectory(path);
    }

    constexpr char* validProfilesArray[9] =
    {
        (char*)"ps",  // Pixel Shader
        (char*)"vs",  // Vertex Shader
        (char*)"gs",  // Geometry Shader
        (char*)"hs",  // Hull shader
        (char*)"ds",  // Domain shader
        (char*)"cs",  // Compute Shader
        (char*)"lib", // Libraries, used for raytracing
        (char*)"ms",  // Mesh Shader
        (char*)"as"   // Amplification Shader (used with Mesh Shaders)
    };
    constexpr char* defaultProfileVersion = (char*)"_6_5";

    std::vector<DxcDefine> GetDefaultDefines()
    {
        std::vector<DxcDefine> defines;
        defines.reserve(32);

        defines.push_back(MakeDefine(L"DEBUG", L"0"));
        defines.push_back(MakeDefine(L"GLOBAL", L"1"));
        defines.push_back(MakeDefine(L"TILES", L"2"));
        defines.push_back(MakeDefine(L"LIGHT", L"3"));
        defines.push_back(MakeDefine(L"PER_PASS", L"4"));
        defines.push_back(MakeDefine(L"PER_DRAW", L"5"));
        defines.push_back(MakeDefine(L"TERRAIN", L"6"));
        defines.push_back(MakeDefine(L"MODEL", L"7"));

        // Define all SHADER_*PROFILE* to 0
        for (const char* profile : validProfilesArray)
        {
            std::wstring profileWString = StringUtils::StringToWString(std::string(profile));
            std::wstring profileName = L"SHADER_" + profileWString;
            std::transform(profileName.begin(), profileName.end(), profileName.begin(), std::towupper);

            defines.push_back(MakeDefine(profileName, L"0"));
        }

        return defines;
    }

    void replaceAll(std::string& str, const std::string& from, const std::string& to) {
        if (from.empty())
            return;
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
    }

    bool DxcBridge::Compile(std::filesystem::path path, std::string& source, char*& blob, size_t& blobSize, std::filesystem::path sourceDir)
    {
        DxcBridgeData* data = static_cast<DxcBridgeData*>(_data);

        HRESULT r;
        CComPtr<IDxcBlobEncoding> sourceBlob;
        r = data->utils->CreateBlob(source.c_str(), static_cast<u32>(source.length()), CP_UTF8, &sourceBlob);

        if (r != S_OK)
        {
            NC_LOG_CRITICAL("Could not load shader blob");
            return false;
        }

        const wchar_t* args[] =
        {
            L"-spirv",          // Generate SPIR-V for Vulkan
            L"-fvk-use-gl-layout", // Set memory layout
            L"-Zpr",            //Row-major matrices
            L"-WX",             //Warnings as errors
            L"-enable-16bit-types", // Enable 16 bit types
            L"-fspv-target-env=vulkan1.1", // Target Vulkan 1.1
            L"-HV 2021",
    #ifdef _DEBUG
            L"-Zi",             //Debug info
            //L"-Qembed_debug", //Embed debug info into the shader
            L"-Od",             //Disable optimization
    #else
            L"-O3",             //Optimization level 3
    #endif
        };

        std::vector<DxcDefine> defines = GetDefaultDefines();
        std::vector<DxcDefine>& permutationDefines = data->extraDefines;

        std::wstring profile;
        std::wstring profileType;
        if (!GetProfileFromFilename(path.filename(), profile, profileType))
        {
            return false;
        }

        // Set this profiles SHADER_*PROFILE* to 1
        std::wstring profileDefineName = L"SHADER_" + profileType;
        std::transform(profileDefineName.begin(), profileDefineName.end(), profileDefineName.begin(), std::towupper);

        for (DxcDefine& define : defines)
        {
            if (define.Name == profileDefineName)
                define.Value = L"1";
        }

        // Merge default defines and permutation defines
        defines.insert(defines.end(), permutationDefines.begin(), permutationDefines.end());

        std::string filenameAsStr = path.filename().string();
        std::wstring fileName = StringUtils::StringToWString(filenameAsStr);

        CComPtr<IDxcOperationResult> compileResult;
        r = data->compiler->Compile(sourceBlob, fileName.c_str(), L"main", profile.c_str(), &args[0], sizeof(args) / sizeof(args[0]), defines.data(), static_cast<u32>(defines.size()), _includeHandler, &compileResult);
        if (r != S_OK)
        {
            NC_LOG_CRITICAL("Compiler would not even give us back a result");
            return false;
        }

        if (compileResult->GetStatus(&r) != S_OK)
        {
            NC_LOG_CRITICAL("Compiler gave us something but we could not get a result from it");
            return false;
        }

        if (r < 0)
        {
            CComPtr<IDxcBlobEncoding> printBlob;
            if (compileResult->GetErrorBuffer(&printBlob) != S_OK)
            {
                NC_LOG_CRITICAL("Compiler gave us an error, but we could not get the text from it");
                return false;
            }

            std::string error((const char*)printBlob->GetBufferPointer());
            std::string fixedError = error;
            
            // Fix clickable links for included files
            size_t firstSemiColon = error.find_first_of(':');
            std::string errorRelativePath = error.substr(0, firstSemiColon);

            bool hasIncludeError = errorRelativePath[0] == '.';
            if (hasIncludeError)
            {
                std::string errorFixedPath = sourceDir.string() + "/" + errorRelativePath.substr(2);

                
                replaceAll(fixedError, errorRelativePath, errorFixedPath);
            }

            // Fix clickable links for original file
            std::string filenameString = path.filename().string();
            std::string pathString = path.string();
            replaceAll(fixedError, filenameString, pathString);

            //NC_LOG_INFO("{0}\n", fixedError.c_str());
            printf("%s", fixedError.c_str()); // Skip the timestamp that DebugHandler attaches so we can goto the errors through the visual studio output

            return false;
        }

        IDxcBlob* resultBlob;
        compileResult->GetResult(&resultBlob);

        blob = (char*)resultBlob->GetBufferPointer();
        blobSize = resultBlob->GetBufferSize();

        return true;
    }

    bool DxcBridge::GetProfileFromFilename(std::filesystem::path filename, std::wstring& profile, std::wstring& profileType)
    {
        static std::string validProfiles = "";

        if (validProfiles == "")
        {
            bool first = true;
            for (auto& validProfile : validProfilesArray)
            {
                if (!first)
                {
                    validProfiles += ", ";
                }

                validProfiles += validProfile;
                first = false;
            }
        }

        // We expect filename to end with .XX.hlsl where XX is the profile of the shader, for example vs for vertex shader, ps for pixel shader, cs for compute etc
        // First we remove the .hlsl part of the name
        fs::path withoutHlsl = filename.replace_extension();

        if (!withoutHlsl.has_extension())
        {
            NC_LOG_ERROR("Filename \"{0}\" should end with .XX.hlsl where XX is one of these valid profiles depending on shader type: {1}", filename.string(), validProfiles);

            return false;
        }

        std::string extension = withoutHlsl.extension().string().substr(1); // Get the extension (.vs) as a string, then remove the first char which will be the "."

        if (extension.length() != 2 && extension.length() != 3)
        {
            NC_LOG_ERROR("Filename \"{0}\" should end with .XX.hlsl where XX is one of these valid profiles depending on shader type: {1}", filename.string(), validProfiles);

            return false;
        }

        // Make sure the profile is valid
        bool isValidProfile = false;
        for (auto& validProfile : validProfilesArray)
        {
            if (extension == validProfile)
            {
                isValidProfile = true;
                break;
            }
        }

        if (!isValidProfile)
        {
            NC_LOG_ERROR("Filename \"{0}\" should end with .XX.hlsl where XX is one of these valid profiles depending on shader type: {1}", filename.string(), validProfiles);
            return false;
        }

        profileType = StringUtils::StringToWString(extension);

        extension = extension.append(defaultProfileVersion); // Turns the string from "vs" to "vs_6_5" or whatever version we set in defaultProfileVersion
        profile = StringUtils::StringToWString(extension);
        return true;
    }
}
