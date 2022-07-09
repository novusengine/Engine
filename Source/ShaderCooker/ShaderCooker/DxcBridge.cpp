#include "DxcBridge.h"
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"

#include <Base/Platform.h>
#include <Base/Util/DebugHandler.h>
#include <Base/Util/StringUtils.h>

// NOTE: This will need edits to add Linux support to ShaderCooker
#ifdef _WINDOWS
#include <wrl/client.h>
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
            , m_cRef(1)
        {
        }

        void AddIncludeDirectory(const fs::path& path)
        {
            _includeDirectories.push_back(path);
        }

        ULONG AddRef() override
        {
            InterlockedIncrement(&m_cRef);
            return m_cRef;
        }

        PRAGMA_CLANG_DIAGNOSTIC_PUSH;
        PRAGMA_CLANG_DIAGNOSTIC_IGNORE(-Wdelete-non-abstract-non-virtual-dtor);
        ULONG Release() override
        {
            // Decrement the object's internal counter.
            ULONG ulRefCount = InterlockedDecrement(&m_cRef);
            if (0 == m_cRef)
            {
                delete this;
            }
            return ulRefCount;
        }
        PRAGMA_CLANG_DIAGNOSTIC_POP;

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
            // Always set out parameter to NULL, validating it first.
            if (!ppvObj)
                return E_INVALIDARG;
            *ppvObj = NULL;
            if (riid == IID_IUnknown || riid == __uuidof(IDxcIncludeHandler))
            {
                // Increment the reference count and return the pointer.
                *ppvObj = (LPVOID)this;
                AddRef();
                return NOERROR;
            }
            return E_NOINTERFACE;
        }

        IDxcUtils* _utils;
        volatile ULONG m_cRef;

        std::vector<fs::path> _includeDirectories;
    };

    struct DxcBridgeData : IDxcBridgeData
    {
        std::vector<DxcDefine> extraDefines;

        Microsoft::WRL::ComPtr<IDxcUtils> utils;
        Microsoft::WRL::ComPtr<IDxcCompiler> compiler;
    };

    DxcBridge::DxcBridge()
    {
        DxcBridgeData* data = new DxcBridgeData();
        _data = data;

        HRESULT r;

        r = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(data->utils.GetAddressOf()));
        if (r != S_OK)
        {
            DebugHandler::PrintFatal("Failed to create DXC Utils");
        }

        r = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(data->compiler.GetAddressOf()));
        if (r != S_OK)
        {
            DebugHandler::PrintFatal("Failed to create DXC Compiler");
        }

        _includeHandler = new IncludeHandler(data->utils.Get());
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
        defines.push_back(MakeDefine(L"SHADOWS", L"2"));
        defines.push_back(MakeDefine(L"PER_PASS", L"3"));
        defines.push_back(MakeDefine(L"PER_DRAW", L"4"));
        defines.push_back(MakeDefine(L"TERRAIN", L"5"));
        defines.push_back(MakeDefine(L"MAPOBJECT", L"6"));
        defines.push_back(MakeDefine(L"CMODEL", L"7"));

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

    bool DxcBridge::Compile(std::filesystem::path path, std::string& source, char*& blob, size_t& blobSize)
    {
        DxcBridgeData* data = static_cast<DxcBridgeData*>(_data);

        HRESULT r;
        Microsoft::WRL::ComPtr<IDxcBlobEncoding> sourceBlob;
        r = data->utils->CreateBlob(source.c_str(), static_cast<u32>(source.length()), CP_UTF8, sourceBlob.GetAddressOf());

        if (r != S_OK)
        {
            DebugHandler::PrintFatal("Could not load shader blob");
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

        Microsoft::WRL::ComPtr<IDxcOperationResult> compileResult;
        r = data->compiler->Compile(sourceBlob.Get(), path.filename().c_str(), L"main", profile.c_str(), &args[0], sizeof(args) / sizeof(args[0]), defines.data(), static_cast<u32>(defines.size()), _includeHandler, compileResult.GetAddressOf());
        if (r != S_OK)
        {
            DebugHandler::PrintFatal("Compiler would not even give us back a result");
            return false;
        }

        if (compileResult->GetStatus(&r) != S_OK)
        {
            DebugHandler::PrintFatal("Compiler gave us something but we could not get a result from it");
            return false;
        }

        if (r < 0)
        {
            Microsoft::WRL::ComPtr<IDxcBlobEncoding> printBlob;
            if (compileResult->GetErrorBuffer(printBlob.GetAddressOf()) != S_OK)
            {
                DebugHandler::PrintFatal("Compiler gave us an error, but we could not get the text from it");
                return false;
            }

            std::string error((const char*)printBlob->GetBufferPointer());

            if (StringUtils::BeginsWith(error, path.filename().string()))
            {
                // This can't be an ErrorPrint or it won't be clickable in the Error List, just a regular Print will work fine
                DebugHandler::Print("%s%s\n", path.parent_path().c_str(), error.c_str());
            }
            else
            {
                DebugHandler::Print("%s\n", error.c_str());
            }
                        
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
            DebugHandler::PrintError("Filename \"%s\" should end with .XX.hlsl where XX is one of these valid profiles depending on shader type: %s", filename.string(), validProfiles);

            return false;
        }

        std::string extension = withoutHlsl.extension().string().substr(1); // Get the extension (.vs) as a string, then remove the first char which will be the "."

        if (extension.length() != 2 && extension.length() != 3)
        {
            DebugHandler::PrintError("Filename \"%s\" should end with .XX.hlsl where XX is one of these valid profiles depending on shader type: %s", filename.string(), validProfiles);

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
            DebugHandler::PrintError("Filename \"%s\" should end with .XX.hlsl where XX is one of these valid profiles depending on shader type: %s", filename.string(), validProfiles);
            return false;
        }

        profileType = StringUtils::StringToWString(extension);

        extension = extension.append(defaultProfileVersion); // Turns the string from "vs" to "vs_6_5" or whatever version we set in defaultProfileVersion
        profile = StringUtils::StringToWString(extension);
        return true;
    }
}