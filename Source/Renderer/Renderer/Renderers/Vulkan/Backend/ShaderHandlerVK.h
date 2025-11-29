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
#include <span>
#include <robinhood/robinhood.h>

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

            VkShaderModule GetShaderModule(const VertexShaderID id) { return _vertexShaderStore.shaders[static_cast<vsIDType>(id)].module; }
            VkShaderModule GetShaderModule(const PixelShaderID id) { return _pixelShaderStore.shaders[static_cast<psIDType>(id)].module; }
            VkShaderModule GetShaderModule(const ComputeShaderID id) { return _computeShaderStore.shaders[static_cast<csIDType>(id)].module; }

            const BindReflection& GetBindReflection(const VertexShaderID id)
            {
                return  _vertexShaderStore.shaders[static_cast<vsIDType>(id)].bindReflection;
            }
            const BindReflection& GetBindReflection(const PixelShaderID id)
            { 
                return _pixelShaderStore.shaders[static_cast<psIDType>(id)].bindReflection;
            }
            const BindReflection& GetBindReflection(const ComputeShaderID id)
            {
                return _computeShaderStore.shaders[static_cast<psIDType>(id)].bindReflection;
            }

        private:
            struct Shader
            {
            public:
                std::string path;
                std::string sourcePath;

                std::vector<PermutationField> permutationFields;

                VkShaderModule module;
                ShaderBinary spirv;

                BindReflection bindReflection;
            };

            struct ShaderStoreBase
            {
            public:
                std::vector<Shader> shaders;
                robin_hood::unordered_map<u32, u32> hashToIndex;
            };

            template <typename T>
            struct ShaderStore : public ShaderStoreBase
            {
            public:
                void Clear()
                {
                    shaders.clear();
                    hashToIndex.clear();
                    descs.clear();
                }

                std::vector<T> descs;
            };

        private:
            template <typename T>
            T LoadShaderFromFile(const std::string& shaderPath, const std::vector<PermutationField>& permutationFields, ShaderStoreBase& shaderStore, bool& wasCached)
            {
                size_t id;
                using idType = type_safe::underlying_type<T>;

                std::string permutationPath = GetPermutationPath(shaderPath, permutationFields);

                // If shader is already loaded, return ID of already loaded version
                if (TryFindExistingShader(permutationPath, shaderStore, id))
                {
                    wasCached = true;
                    return T(static_cast<idType>(id));
                }

                // Check if we need to compile it before loading
                if (NeedsCompile(shaderPath))
                {
                    //NC_LOG_INFO("[ShaderCooker]: Compiling {0}", shaderPath);
                    if (!CompileShader(shaderPath))
                    {
                        NC_LOG_WARNING("[ShaderCooker]: Compiling {0} failed, using old version", shaderPath);
                    }
                }

                std::string shaderBinPath = GetShaderBinPathString(permutationPath);

                id = shaderStore.shaders.size();
                assert(id < T::MaxValue());

                Shader& shader = shaderStore.shaders.emplace_back();
                ReadFile(shaderBinPath, shader.spirv);
                shader.path = permutationPath;

                u32* data = reinterpret_cast<u32*>(shader.spirv.data());
                const size_t size = shader.spirv.size();
                shader.module = CreateShaderModule(data, size, shaderPath);
                shader.permutationFields = permutationFields;

                ReflectShader(data, size, shader.bindReflection, shaderPath);

                u32 permutationPathHash = StringUtils::fnv1a_32(permutationPath.c_str(), permutationPath.length());
                shaderStore.hashToIndex[permutationPathHash] = static_cast<u32>(id);

                wasCached = false;
                return T(static_cast<idType>(id));
            }

            template <typename T>
            T LoadShaderFromMemory(const ShaderEntry& shaderEntry, ShaderStoreBase& shaderStore, bool& wasCached)
            {
                size_t id;
                using idType = type_safe::underlying_type<T>;

                // If shader is already loaded, return ID of already loaded version
                if (TryFindExistingShader(shaderEntry.permutationNameHash, shaderStore, id))
                {
                    wasCached = true;
                    return T(static_cast<idType>(id));
                }

                id = shaderStore.shaders.size();
                assert(id < T::MaxValue());

                Shader& shader = shaderStore.shaders.emplace_back();
                u32* data = reinterpret_cast<u32*>(shaderEntry.shaderData);
                const u32 size = shaderEntry.shaderSize;
                shader.module = CreateShaderModule(data, size, shaderEntry.debugName);

                ReflectShader(data, size, shader.bindReflection, shaderEntry.debugName);

                shaderStore.hashToIndex[shaderEntry.permutationNameHash] = static_cast<u32>(id);

                wasCached = false;
                return T(static_cast<idType>(id));
            }

            void ReflectShader(const uint32_t* shaderData, size_t shaderSize, BindReflection& bindReflection, const std::string& shaderPath);
            
            void ReadFile(const std::string& filename, ShaderBinary& binary);
            VkShaderModule CreateShaderModule(const uint32_t* shaderData, size_t shaderSize, const std::string& debugName);
            bool TryFindExistingShader(const std::string& shaderPath, ShaderStoreBase& shaderStore, size_t& id);
            bool TryFindExistingShader(u32 shaderHash, ShaderStoreBase& shaderStore, size_t& id);

            std::string GetShaderBinPathString(const std::string& shaderPath);
            std::string GetPermutationPath(const std::string& shaderPathString, const std::vector<PermutationField>& permutationFields);

            bool NeedsCompile(const std::string& shaderPath);
            bool CompileShader(const std::string& shaderPath);

        private:
            RenderDeviceVK* _device;

            ShaderCooker::ShaderCache* _shaderCache;
            ShaderCooker::ShaderCompiler* _shaderCompiler;
            bool _forceRecompileAll = false;


            ShaderStore<VertexShaderDesc> _vertexShaderStore;
            ShaderStore<PixelShaderDesc> _pixelShaderStore;
            ShaderStore<ComputeShaderDesc> _computeShaderStore;
        };
    }
}
