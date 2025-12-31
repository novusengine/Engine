#include "SlangBridge.h"
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"

#include <Base/Platform.h>
#include <Base/Util/DebugHandler.h>
#include <Base/Util/StringUtils.h>

#include <slang.h>
#include <slang-com-helper.h>
#include <slang-com-ptr.h>

namespace fs = std::filesystem;

namespace ShaderCooker
{
    struct SlangBridgeData : ISlangBridgeData
    {
        Slang::ComPtr<slang::IGlobalSession> globalSession;

        std::string sourceDir;

        Slang::ComPtr<slang::IBlob> diagnostics;

        std::vector<slang::PreprocessorMacroDesc> permutationDefines;
    };

    SlangBridge::SlangBridge(std::filesystem::path sourceDir)
    {
        SlangBridgeData* data = new SlangBridgeData();
        _data = data;

        data->sourceDir = sourceDir.string();

        SlangGlobalSessionDesc globalSessionDesc = {};
        SlangResult result = slang::createGlobalSession(&globalSessionDesc, data->globalSession.writeRef());
        NC_ASSERT(SLANG_SUCCEEDED(result), "Failed to create Slang global session");
    }

    SlangBridge::~SlangBridge()
    {
        
    }

    void SlangBridge::AddDefine(const std::string& name, const std::string& value)
    {
        SlangBridgeData* data = static_cast<SlangBridgeData*>(_data);
        data->permutationDefines.push_back(slang::PreprocessorMacroDesc{ .name = name.c_str(), .value = value.c_str()});
    }

    void SlangBridge::ClearDefines()
    {
        SlangBridgeData* data = static_cast<SlangBridgeData*>(_data);
        data->permutationDefines.clear();
    }

    void SlangBridge::AddIncludeDir(std::filesystem::path path)
    {
        
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
    constexpr i32 ProfileToProfileIndex(const std::string& profile)
    {
        i32 currentProfileIndex = 0;
        for (char* validProfile : validProfilesArray)
        {
            if (strcmp(profile.c_str(), validProfile) == 0)
            {
                return currentProfileIndex;
            }
            currentProfileIndex++;
        }

        return -1;
    }

    constexpr char* defaultProfileVersion = (char*)"_6_5";

    constexpr bool IsSupportedStage(SlangStage stage)
    {
        switch (stage)
        {
        case SlangStage::SLANG_STAGE_VERTEX:
        case SlangStage::SLANG_STAGE_HULL:
        case SlangStage::SLANG_STAGE_DOMAIN:
        case SlangStage::SLANG_STAGE_GEOMETRY:
        case SlangStage::SLANG_STAGE_FRAGMENT:
        case SlangStage::SLANG_STAGE_COMPUTE:
        case SlangStage::SLANG_STAGE_MESH:
        case SlangStage::SLANG_STAGE_AMPLIFICATION:
            return true;

        case SlangStage::SLANG_STAGE_RAY_GENERATION:
        case SlangStage::SLANG_STAGE_INTERSECTION:
        case SlangStage::SLANG_STAGE_ANY_HIT:
        case SlangStage::SLANG_STAGE_CLOSEST_HIT:
        case SlangStage::SLANG_STAGE_MISS:
        case SlangStage::SLANG_STAGE_CALLABLE:
        case SlangStage::SLANG_STAGE_NONE:
        case SlangStage::SLANG_STAGE_DISPATCH:
        default:
            return false;
        }
    }

    SlangStage ProfileTypeToSlangStage(const std::string& profileType)
    {
        if (strcmp(profileType.c_str(), "ps") == 0)
            return SlangStage::SLANG_STAGE_FRAGMENT;
        if (strcmp(profileType.c_str(), "vs") == 0)
            return SlangStage::SLANG_STAGE_VERTEX;
        if (strcmp(profileType.c_str(), "hs") == 0)
            return SlangStage::SLANG_STAGE_HULL;
        if (strcmp(profileType.c_str(), "ds") == 0)
            return SlangStage::SLANG_STAGE_DOMAIN;
        if (strcmp(profileType.c_str(), "gs") == 0)
            return SlangStage::SLANG_STAGE_GEOMETRY;
        if (strcmp(profileType.c_str(), "cs") == 0)
            return SlangStage::SLANG_STAGE_COMPUTE;
        if (strcmp(profileType.c_str(), "ma") == 0)
            return SlangStage::SLANG_STAGE_MESH;
        if (strcmp(profileType.c_str(), "as") == 0)
            return SlangStage::SLANG_STAGE_AMPLIFICATION;
        
        return SlangStage::SLANG_STAGE_NONE;
    }

    void AddProfileDefines(std::vector<slang::PreprocessorMacroDesc>& defines, std::vector<std::string>& defineNames, i32 stageProfileIndex)
    {
        i32 currentProfile = 0;
        for (const char* profile : validProfilesArray)
        {
            std::string& defineName = defineNames.emplace_back();//;
            defineName.append("SHADER_");
            defineName.append(profile);
            std::transform(defineName.begin(), defineName.end(), defineName.begin(), [](unsigned char c)
            { 
                return std::toupper(c); 
            });

            bool isCurrentProfile = (currentProfile++) == stageProfileIndex;
            defines.push_back(slang::PreprocessorMacroDesc{ .name = defineName.c_str(), .value = (isCurrentProfile ? "1" : "0")});
        }
    }

    void CreateSession(SlangBridgeData* data, Slang::ComPtr<slang::ISession>& session, std::vector<slang::PreprocessorMacroDesc>& defines, std::string& profile)
    {
        // Set up target
        slang::TargetDesc targetDesc = {};
        targetDesc.format = SLANG_SPIRV;
        targetDesc.profile = data->globalSession->findProfile("vk_1_1");//profile.c_str());//"vk_1_2"); // Double check this profile

        // Set up compiler options
        std::vector<slang::CompilerOptionEntry> compilerOptions;
        compilerOptions.reserve(8);
        compilerOptions.push_back({ slang::CompilerOptionName::Language, { .intValue0 = SlangSourceLanguage::SLANG_SOURCE_LANGUAGE_HLSL }}); // Input language is HLSL
        compilerOptions.push_back({ slang::CompilerOptionName::VulkanUseGLLayout, {.intValue0 = true } }); // Use GL layouts
        compilerOptions.push_back({ slang::CompilerOptionName::EmitSpirvDirectly, {.intValue0 = true } }); // Emit SPIR-V directly (needed for debugging)
        compilerOptions.push_back({ slang::CompilerOptionName::DebugInformation, {.intValue0 = SLANG_DEBUG_INFO_LEVEL_STANDARD } }); // Emit SPIR-V directly (needed for debugging)

        // warning 30081: implicit conversion from X to Y is not recommended
        // warning 41000: unreachable code detected (happens within #ifdef blocks)
        compilerOptions.push_back({ slang::CompilerOptionName::DisableWarnings, { .stringValue0 = "30081,41000" }}); 

        // Set up session
        slang::SessionFlags sessionFlags = slang::kSessionFlags_None;
        SlangMatrixLayoutMode matrixLayoutMode = SLANG_MATRIX_LAYOUT_ROW_MAJOR;

        slang::SessionDesc sessionDesc = {};
        sessionDesc.flags = sessionFlags;
        sessionDesc.defaultMatrixLayoutMode = matrixLayoutMode;

        sessionDesc.compilerOptionEntries = compilerOptions.data();
        sessionDesc.compilerOptionEntryCount = static_cast<u32>(compilerOptions.size());

        sessionDesc.targets = &targetDesc;
        sessionDesc.targetCount = 1;

        const char* searchPaths[] = { data->sourceDir.c_str() };
        sessionDesc.searchPaths = searchPaths;
        sessionDesc.searchPathCount = 1;

        sessionDesc.preprocessorMacros = defines.data();
        sessionDesc.preprocessorMacroCount = static_cast<u32>(defines.size());

        Slang::Result result = data->globalSession->createSession(sessionDesc, session.writeRef());
        NC_ASSERT(SLANG_SUCCEEDED(result), "Failed to create Slang session");
    }
    
    bool ShouldPrintDiag(const std::string& warningStr)
    {
        if (StringUtils::Contains(warningStr, "error")) // Compiler bug is not letting us disable a very annoying warning, so we ignore it on our side
        {
            return true;
        }

        return false;
    }

    inline u32 GetPermutationNameHash(const std::string& shaderName, const std::vector<slang::PreprocessorMacroDesc>& permutationFields)
    {
        std::filesystem::path shaderPath = shaderName;
        std::string filename = shaderPath.filename().string();

        size_t firstExtensionOffset = filename.find_first_of('.');

        std::string permutationName = filename.substr(0, firstExtensionOffset);
        std::string extension = filename.substr(firstExtensionOffset);

        for (const slang::PreprocessorMacroDesc& permutationField : permutationFields)
        {
            permutationName += "-";
            permutationName = permutationName.append(permutationField.name);
            permutationName = permutationName.append(permutationField.value);
        }

        shaderPath = shaderPath.parent_path() / (permutationName + extension);

        return StringUtils::fnv1a_32(shaderPath.string().c_str(), shaderPath.string().length());
    }

    FileFormat::DescriptorTypeReflection GetDescriptorTypeReflection(slang::TypeLayoutReflection* typeLayout, u32& count)
    {
        slang::TypeReflection::Kind kind = typeLayout->getKind();
        switch (kind)
        {
            case slang::TypeReflection::Kind::ConstantBuffer:
            {
                return FileFormat::DescriptorTypeReflection::ConstantBuffer;
            }
            case slang::TypeReflection::Kind::SamplerState:
            {
                return FileFormat::DescriptorTypeReflection::SamplerState;
            }
            case slang::TypeReflection::Kind::Resource:
            {
                SlangResourceShape shape = typeLayout->getResourceShape();

                switch (shape)
                {
                    case SlangResourceShape::SLANG_BYTE_ADDRESS_BUFFER:
                        return FileFormat::DescriptorTypeReflection::ByteAddressBuffer;
                    case SlangResourceShape::SLANG_STRUCTURED_BUFFER:
                        return FileFormat::DescriptorTypeReflection::StructuredBuffer;
                    case SlangResourceShape::SLANG_TEXTURE_1D:
                    case SlangResourceShape::SLANG_TEXTURE_2D:
                    case SlangResourceShape::SLANG_TEXTURE_3D:
                    case SlangResourceShape::SLANG_TEXTURE_CUBE:
                        return FileFormat::DescriptorTypeReflection::Texture;
                    case SlangResourceShape::SLANG_TEXTURE_1D_ARRAY:
                    case SlangResourceShape::SLANG_TEXTURE_2D_ARRAY:
                    case SlangResourceShape::SLANG_TEXTURE_CUBE_ARRAY:
                        return FileFormat::DescriptorTypeReflection::TextureArray;
                    default:
                        NC_LOG_CRITICAL("Unsupported resource shape {0} in GetDescriptorTypeReflection", static_cast<u32>(shape));
                }
                break;
            }
            case slang::TypeReflection::Kind::Array:
            {
                size_t elementCount = typeLayout->getElementCount();
                if (elementCount == ~size_t(0))
                {
                    // Unbounded array
                    count = 0; // 0 means it's gonna be unbounded
                }
                else
                {
                    count = static_cast<u32>(elementCount);
                }
                return FileFormat::DescriptorTypeReflection::Array;
            }
        }

        NC_LOG_CRITICAL("Unsupported type kind {0} in GetDescriptorTypeReflection", static_cast<u32>(kind));
        return FileFormat::DescriptorTypeReflection::Unknown;
    }

    FileFormat::DescriptorAccessTypeReflection ToAccessType(SlangResourceAccess access)
    {
        switch(access)
        {
            case SLANG_RESOURCE_ACCESS_NONE:
                return FileFormat::DescriptorAccessTypeReflection::None;
            case SLANG_RESOURCE_ACCESS_READ:
                return FileFormat::DescriptorAccessTypeReflection::Read;
            case SLANG_RESOURCE_ACCESS_READ_WRITE:
                return FileFormat::DescriptorAccessTypeReflection::ReadWrite;
            case SLANG_RESOURCE_ACCESS_RASTER_ORDERED:
                return FileFormat::DescriptorAccessTypeReflection::RasterOrdered;
            case SLANG_RESOURCE_ACCESS_APPEND:
                return FileFormat::DescriptorAccessTypeReflection::Append;
            case SLANG_RESOURCE_ACCESS_CONSUME:
                return FileFormat::DescriptorAccessTypeReflection::Consume;
            case SLANG_RESOURCE_ACCESS_WRITE:
                return FileFormat::DescriptorAccessTypeReflection::Write;
            case SLANG_RESOURCE_ACCESS_FEEDBACK:
                return FileFormat::DescriptorAccessTypeReflection::Feedback;
        }

        return FileFormat::DescriptorAccessTypeReflection::None;
    }

    bool ReflectDescriptors(slang::ProgramLayout* programLayout, Slang::ComPtr<slang::IMetadata> entryPointMetadata, FileFormat::ShaderReflection& out)
    {
        SlangResult result;
        bool success = true;

        u32 parameterCount = programLayout->getParameterCount();

        for (u32 i = 0; i < parameterCount; i++)
        {
            slang::VariableLayoutReflection* variableLayout = programLayout->getParameterByIndex(i);
            slang::TypeLayoutReflection* typeLayout = variableLayout->getTypeLayout();

            u32 slot = variableLayout->getBindingSpace();
            const char* name = variableLayout->getName();
            u32 binding = variableLayout->getBindingIndex();
            FileFormat::DescriptorAccessTypeReflection accessType = ToAccessType(typeLayout->getResourceAccess());
            bool isUsed;
            u32 byteOffset = 0;
            u32 byteSize = 0;

            if (slot == 4)
            {
                volatile int asd = 123;
            }

            SlangParameterCategory category = static_cast<SlangParameterCategory>(variableLayout->getCategory());
            result = entryPointMetadata->isParameterLocationUsed(category, slot, binding, isUsed);
            NC_ASSERT(SLANG_SUCCEEDED(result), "Failed to get parameter usage from metadata");

            u32 count = 1;
            FileFormat::DescriptorTypeReflection type = GetDescriptorTypeReflection(typeLayout, count);
            FileFormat::DescriptorTypeReflection subType = FileFormat::DescriptorTypeReflection::Unknown;

            // Get byteOffset and byteSize for ConstantBuffer types (push constants)
            if (type == FileFormat::DescriptorTypeReflection::ConstantBuffer)
            {
                byteOffset = static_cast<u32>(variableLayout->getOffset(slang::ParameterCategory::Uniform));
                slang::TypeLayoutReflection* elementTypeLayout = typeLayout->getElementTypeLayout();
                if (elementTypeLayout)
                {
                    byteSize = static_cast<u32>(elementTypeLayout->getSize(SLANG_PARAMETER_CATEGORY_UNIFORM));
                }
            }

            if (slot == 0)
            {
                if (type != FileFormat::DescriptorTypeReflection::ConstantBuffer)
                {
                    // Slot 0 is reserved for push constants
                    NC_LOG_ERROR("Descriptor found in slot 0 which is reserved for push constants: {0} (binding {1})\n You probably wanted to use the PER_PASS or PER_DRAW macros", name, binding);
                    success = false;
                    continue;
                }
                if (byteSize == 0)
                {
                    // Slot 0 is reserved for push constants
                    NC_LOG_ERROR("Push constant buffer has size 0: {0} (binding {1})", name, binding);
                    success = false;
                    continue;
                }
            }

            // Get the subtype if this is an array
            if (type == FileFormat::DescriptorTypeReflection::Array)
            {
                slang::TypeLayoutReflection* elementTypeLayout = typeLayout->getElementTypeLayout();
                subType = GetDescriptorTypeReflection(elementTypeLayout, count);
            }

            FileFormat::DescriptorSetReflection& descriptorSet = out.GetDescriptorSetBySlot(slot);
            if (descriptorSet.descriptors.contains(binding))
            {
                // Make sure the existing descriptor matches
                FileFormat::DescriptorReflection& existingDescriptor = descriptorSet.descriptors[binding];
                
                if (existingDescriptor.name != name ||
                    existingDescriptor.type != type ||
                    existingDescriptor.subType != subType ||
                    existingDescriptor.count != count ||
                    existingDescriptor.accessType != accessType ||
                    existingDescriptor.isUsed != isUsed ||
                    existingDescriptor.byteOffset != byteOffset ||
                    existingDescriptor.byteSize != byteSize)
                {
                    std::string existingDescriptorStr = existingDescriptor.ToString();
                    std::string newDescriptorStr = FileFormat::DescriptorReflection{ .binding = binding, .name = name, .type = type, .subType = subType, .count = count }.ToString();
                    NC_LOG_ERROR("Descriptor binding conflict at slot {0}, binding {1}:\nExisting: {2}\nNew: {3}",
                        slot,
                        binding,
                        existingDescriptorStr.c_str(),
                        newDescriptorStr.c_str());

                    success = false;
                }
            }
            else
            {
                // New binding
                FileFormat::DescriptorReflection descriptor;
                descriptor.binding = binding;
                descriptor.name = name;
                descriptor.type = type;
                descriptor.subType = subType;
                descriptor.count = count;
                descriptor.accessType = accessType;
                descriptor.isUsed = isUsed;
                descriptor.byteOffset = byteOffset;
                descriptor.byteSize = byteSize;

                // TODO: Check so we don't have implicit padding (explicit padding is fine)
                
                descriptorSet.descriptors[binding] = descriptor;
            }
        }

        return success;
    }

    bool SlangBridge::Compile(std::filesystem::path path, const std::string& source, FileFormat::ShaderInMemory& shaderInMemory)
    {
        SlangBridgeData* data = static_cast<SlangBridgeData*>(_data);

        std::string profile;
        std::string profileType;
        if (!GetProfileFromFilename(path.filename(), profile, profileType))
        {
            NC_LOG_ERROR("Slang profile not supported ({0})", path.string().c_str());
            return false;
        }

        // Set up defines
        std::vector<slang::PreprocessorMacroDesc> defines;
        defines.reserve(32);
        // 1 indexed because we let 0 be the push constants, it's not technically correct but we get nice error checking if we do this
        defines.push_back({ "DEBUG", "1" });
        defines.push_back({ "GLOBAL", "2" });
        defines.push_back({ "LIGHT", "3" });
        defines.push_back({ "TERRAIN", "4" });
        defines.push_back({ "MODEL", "5" });
        defines.push_back({ "PER_PASS", "6" });
        defines.push_back({ "PER_DRAW", "7" });

        // Add extra defines
        defines.insert(defines.end(), data->permutationDefines.begin(), data->permutationDefines.end());

        i32 stageProfileIndex = ProfileToProfileIndex(profileType);
        if (stageProfileIndex == -1)
        {
            NC_LOG_ERROR("Slang profile not supported, but caught during profile index? ({0})", path.string().c_str());
            return false;
        }

        // Add profile defines
        std::vector<std::string> defineNames;
        defineNames.reserve(32);

        AddProfileDefines(defines, defineNames, stageProfileIndex);

        Slang::ComPtr<slang::ISession> session;
        CreateSession(data, session, defines, profile);

        // Load module from source string
        slang::IModule* module = session->loadModuleFromSourceString(path.filename().string().c_str(), path.string().c_str(), source.c_str(), data->diagnostics.writeRef());
        if (data->diagnostics)
        {
            bool actualError = module == nullptr;
            std::string diagStr = { (char*)data->diagnostics->getBufferPointer(), data->diagnostics->getBufferSize() };

            if (ShouldPrintDiag(diagStr) || actualError)
            {
                NC_LOG_ERROR("[Slang] Module:\n{0} ({1})", diagStr.c_str(), path.string().c_str());
            }

            bool isWarning = StringUtils::Contains(diagStr, "warning");
            if (!isWarning || actualError)
            {
                return false;
            }
        }

        // Set up composition
        SlangStage stage = ProfileTypeToSlangStage(profileType);
        const char* entryName = "main";

        Slang::ComPtr<slang::IEntryPoint> entryPoint;
        SlangResult result = module->findEntryPointByName(entryName, entryPoint.writeRef());
        if (!SLANG_SUCCEEDED(result))
        {
            NC_LOG_ERROR("[Slang] Failed to get entrypoint {0} ({1})", entryName, path.string().c_str());
            return false;
        }

        slang::IComponentType* componentType[] = { module, entryPoint.get() };
        Slang::ComPtr<slang::IComponentType> program;
        result = session->createCompositeComponentType(componentType, 2, program.writeRef());
        if (!SLANG_SUCCEEDED(result))
        {
            NC_LOG_ERROR("[Slang] Failed to create composite component type ({0})", path.string().c_str());
            return false;
        }

        // Get program layout
        slang::ProgramLayout* programLayout = program->getLayout();

        // Link program
        Slang::ComPtr<slang::IComponentType> linkedProgram;
        result = program->link(linkedProgram.writeRef(), data->diagnostics.writeRef());
        if (data->diagnostics)
        {
            std::string diagStr = { (char*)data->diagnostics->getBufferPointer(), data->diagnostics->getBufferSize() };
            NC_LOG_ERROR("[Slang] Link: {0} ({1})", diagStr.c_str(), path.string().c_str());

            bool isWarning = StringUtils::Contains(diagStr, "warning");
            if (!isWarning)
            {
                return false;
            }
        }
        if (!SLANG_SUCCEEDED(result))
        {
            NC_LOG_ERROR("[Slang] Failed to create Slang session ({0})", path.string().c_str());
            return false;
        }

        // Get compiled kernel blob
        i32 entryPointIndex = 0;
        i32 targetIndex = 0;
        Slang::ComPtr<slang::IBlob> kernelBlob;
        result = linkedProgram->getEntryPointCode(entryPointIndex, targetIndex, kernelBlob.writeRef(), data->diagnostics.writeRef());
        if (data->diagnostics)
        {
            std::string diagStr = { (char*)data->diagnostics->getBufferPointer(), data->diagnostics->getBufferSize() };
            NC_LOG_ERROR("[Slang] Get Entrypoint: {0} ({1})", diagStr.c_str(), path.string().c_str());

            bool isWarning = StringUtils::Contains(diagStr, "warning");
            if (!isWarning)
            {
                return false;
            }
        }
        if (!SLANG_SUCCEEDED(result))
        {
            NC_LOG_ERROR("[Slang] Failed to get entrypoint after linking ({0})", path.string().c_str());
            return false;
        }

        Slang::ComPtr<slang::IMetadata> entryPointMetadata;
        result = linkedProgram->getEntryPointMetadata(0, 0, entryPointMetadata.writeRef(), data->diagnostics.writeRef());
        if (data->diagnostics)
        {
            std::string diagStr = { (char*)data->diagnostics->getBufferPointer(), data->diagnostics->getBufferSize() };
            NC_LOG_ERROR("[Slang] Get Entrypoint metadata: {0} ({1})", diagStr.c_str(), path.string().c_str());
        }
        if (!SLANG_SUCCEEDED(result))
        {
            NC_LOG_ERROR("[Slang] Failed to get entrypoint metadata ({0})", path.string().c_str());
            return false;
        }

        // Reflect descriptors
        if (!ReflectDescriptors(programLayout, entryPointMetadata, shaderInMemory.reflection))
        {
            return false;
        }

        // Copy to output blob
        shaderInMemory.data = new u8[kernelBlob->getBufferSize()];
        memcpy(shaderInMemory.data, kernelBlob->getBufferPointer(), kernelBlob->getBufferSize());
        shaderInMemory.size = static_cast<u32>(kernelBlob->getBufferSize());

        return true;
    }

    bool SlangBridge::GetProfileFromFilename(std::filesystem::path filename, std::string& profile, std::string& profileType)
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

        // We expect filename to end with .XX.slang where XX is the profile of the shader, for example vs for vertex shader, ps for pixel shader, cs for compute etc
        // First we remove the .slang part of the name
        fs::path withoutSlang = filename.replace_extension();

        if (!withoutSlang.has_extension())
        {
            NC_LOG_ERROR("Filename \"{0}\" should end with .XX.slang where XX is one of these valid profiles depending on shader type: {1}", filename.string(), validProfiles);

            return false;
        }

        std::string extension = withoutSlang.extension().string().substr(1); // Get the extension (.vs) as a string, then remove the first char which will be the "."

        if (extension.length() != 2 && extension.length() != 3)
        {
            NC_LOG_ERROR("Filename \"{0}\" should end with .XX.slang where XX is one of these valid profiles depending on shader type: {1}", filename.string(), validProfiles);

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
            NC_LOG_ERROR("Filename \"{0}\" should end with .XX.slang where XX is one of these valid profiles depending on shader type: {1}", filename.string(), validProfiles);
            return false;
        }

        profileType = extension;

        extension = extension.append(defaultProfileVersion); // Turns the string from "vs" to "vs_6_5" or whatever version we set in defaultProfileVersion
        profile = extension;
        return true;
    }
}
