#pragma once
#include "SpirvReflect.h"
#include "Renderer/Descriptors/VertexShaderDesc.h"
#include "Renderer/Descriptors/PixelShaderDesc.h"
#include "Renderer/Descriptors/ComputeShaderDesc.h"

#include <Base/Types.h>
#include <Base/Util/DebugHandler.h>
#include <Base/Util/StringUtils.h>

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <vector>
#include <unordered_map>

namespace ShaderCooker
{
    class ShaderCompiler;
    class ShaderCache;
}

namespace Renderer
{
    namespace Backend
    {
        class RenderDeviceVK;

        typedef std::vector<char> ShaderBinary;

        struct BindInfo
        {
            std::string name;
            u32 nameHash;

            VkDescriptorType descriptorType;
            u32 set;
            u32 binding;
            u32 count;
            u32 stageFlags;

            bool isUsed = false;
            bool isWrite = false;
        };

        struct BindInfoPushConstant
        {
            u32 stageFlags;
            u32 offset;
            u32 size;
        };

        struct BindReflection
        {
            std::vector<BindInfo> dataBindings;
            std::vector<BindInfoPushConstant> pushConstants;
        };

        class ShaderHandlerVK
        {
            using vsIDType = type_safe::underlying_type<VertexShaderID>;
            using psIDType = type_safe::underlying_type<PixelShaderID>;
            using csIDType = type_safe::underlying_type<ComputeShaderID>;

        public:
            void Init(RenderDeviceVK* device);
            void SetShaderSourceDirectory(const std::string& path);
            void ReloadShaders(bool forceRecompileAll);

            VertexShaderID LoadShader(const VertexShaderDesc& desc);
            PixelShaderID LoadShader(const PixelShaderDesc& desc);
            ComputeShaderID LoadShader(const ComputeShaderDesc& desc);

            VkShaderModule GetShaderModule(const VertexShaderID id) { return _vertexShaders[static_cast<vsIDType>(id)].module; }
            VkShaderModule GetShaderModule(const PixelShaderID id) { return _pixelShaders[static_cast<psIDType>(id)].module; }
            VkShaderModule GetShaderModule(const ComputeShaderID id) { return _computeShaders[static_cast<csIDType>(id)].module; }

            const BindReflection& GetBindReflection(const VertexShaderID id)
            {
                return  _vertexShaders[static_cast<vsIDType>(id)].bindReflection;
            }
            const BindReflection& GetBindReflection(const PixelShaderID id)
            { 
                return _pixelShaders[static_cast<psIDType>(id)].bindReflection;
            }
            const BindReflection& GetBindReflection(const ComputeShaderID id)
            {
                return _computeShaders[static_cast<psIDType>(id)].bindReflection;
            }

        private:
            struct Shader
            {
                std::string path;
                std::string sourcePath;

                std::vector<PermutationField> permutationFields;

                VkShaderModule module;
                ShaderBinary spirv;

                BindReflection bindReflection;
            };

        private:
            template <typename T>
            T LoadShader(const std::string& shaderPath, const std::vector<PermutationField>& permutationFields, std::vector<Shader>& shaders)
            {
                size_t id;
                using idType = type_safe::underlying_type<T>;

                std::string permutationPath = GetPermutationPath(shaderPath, permutationFields);

                // If shader is already loaded, return ID of already loaded version
                if (TryFindExistingShader(permutationPath, shaders, id))
                {
                    return T(static_cast<idType>(id));
                }

                // Check if we need to compile it before loading
                if (NeedsCompile(shaderPath))
                {
                    //DebugHandler::Print("[ShaderCooker]: Compiling {0}", shaderPath);
                    if (!CompileShader(shaderPath))
                    {
                        DebugHandler::PrintWarning("[ShaderCooker]: Compiling {0} failed, using old version", shaderPath);
                    }
                }

                std::string shaderBinPath = GetShaderBinPathString(permutationPath);

                id = shaders.size();
                assert(id < T::MaxValue());

                shaders.emplace_back();
                Shader& shader = shaders.back();
                ReadFile(shaderBinPath, shader.spirv);
                shader.path = permutationPath;
                shader.module = CreateShaderModule(shader.spirv, shaderPath);
                shader.permutationFields = permutationFields;

                // Reflect descriptor sets
                SpvReflectShaderModule reflectModule;
                SpvReflectResult result = spvReflectCreateShaderModule(shader.spirv.size(), shader.spirv.data(), &reflectModule);

                if (result != SPV_REFLECT_RESULT_SUCCESS)
                {
                    DebugHandler::PrintFatal("We failed to reflect the spirv of {0}", shaderPath);
                }

                uint32_t descriptorSetCount = 0;
                result = spvReflectEnumerateDescriptorSets(&reflectModule, &descriptorSetCount, NULL);

                if (result != SPV_REFLECT_RESULT_SUCCESS)
                {
                    DebugHandler::PrintFatal("We failed to reflect the spirv descriptor set count of {0}", shaderPath);
                }

                if (descriptorSetCount > 0)
                {
                    std::vector<SpvReflectDescriptorSet*> descriptorSets(descriptorSetCount);
                    
                    result = spvReflectEnumerateDescriptorSets(&reflectModule, &descriptorSetCount, descriptorSets.data());

                    if (result != SPV_REFLECT_RESULT_SUCCESS)
                    {
                        DebugHandler::PrintFatal("We failed to reflect the spirv descriptor sets of {0}", shaderPath);
                    }

                    for (auto* descriptorSet : descriptorSets)
                    {
                        for (uint32_t binding = 0; binding < descriptorSet->binding_count; binding++)
                        {
                            const SpvReflectDescriptorBinding* reflectionBinding = descriptorSet->bindings[binding];
                            BindInfo bindInfo;
                            bindInfo.descriptorType = static_cast<VkDescriptorType>(reflectionBinding->descriptor_type);
                            bindInfo.set = descriptorSet->set;
                            bindInfo.binding = reflectionBinding->binding;
                            bindInfo.count = reflectionBinding->count;
                            bindInfo.stageFlags = static_cast<VkShaderStageFlagBits>(reflectModule.shader_stage);

                            if (bindInfo.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
                            {
                                bindInfo.isUsed = reflectionBinding->accessed;
                                bindInfo.isWrite = (reflectionBinding->resource_type & SPV_REFLECT_RESOURCE_FLAG_UAV);
                            }

                            bindInfo.name = reflectionBinding->name;
                            bindInfo.nameHash = StringUtils::fnv1a_32(bindInfo.name.c_str(), bindInfo.name.length());

                            shader.bindReflection.dataBindings.push_back(bindInfo);
                        }
                    }
                }

                uint32_t pushConstantCount = 0;
                result = spvReflectEnumeratePushConstantBlocks(&reflectModule, &pushConstantCount, NULL);

                if (result != SPV_REFLECT_RESULT_SUCCESS)
                {
                    DebugHandler::PrintFatal("We failed to reflect the spirv push constant count of {0}", shaderPath);
                }

                if (pushConstantCount > 0)
                {
                    std::vector<SpvReflectBlockVariable*> blockVariables(pushConstantCount);

                    result = spvReflectEnumeratePushConstantBlocks(&reflectModule, &pushConstantCount, blockVariables.data());

                    for (SpvReflectBlockVariable* variable : blockVariables)
                    {
                        BindInfoPushConstant& pushConstant = shader.bindReflection.pushConstants.emplace_back();
                        pushConstant.offset = variable->offset;
                        pushConstant.size = variable->size;
                        pushConstant.stageFlags = static_cast<VkShaderStageFlagBits>(reflectModule.shader_stage);
                    }
                }
                
                return T(static_cast<idType>(id));
            }
            
            void ReadFile(const std::string& filename, ShaderBinary& binary);
            VkShaderModule CreateShaderModule(const ShaderBinary& binary, const std::string& debugName);
            bool TryFindExistingShader(const std::string& shaderPath, std::vector<Shader>& shaders, size_t& id);

            std::string GetShaderBinPathString(const std::string& shaderPath);
            std::string GetPermutationPath(const std::string& shaderPathString, const std::vector<PermutationField>& permutationFields);

            bool NeedsCompile(const std::string& shaderPath);
            bool CompileShader(const std::string& shaderPath);

        private:
            RenderDeviceVK* _device;

            ShaderCooker::ShaderCache* _shaderCache;
            ShaderCooker::ShaderCompiler* _shaderCompiler;
            bool _forceRecompileAll = false;

            std::vector<Shader> _vertexShaders;
            std::vector<Shader> _pixelShaders;
            std::vector<Shader> _computeShaders;
        };
    }
}
