#include "PipelineHandlerVK.h"
#include "FormatConverterVK.h"
#include "RenderDeviceVK.h"
#include "ShaderHandlerVK.h"
#include "ImageHandlerVK.h"
#include "SpirvReflect.h"
#include "DebugMarkerUtilVK.h"

#include <Base/Util/DebugHandler.h>
#include <xxhash/xxhash64.h>
#include <Base/Memory/Allocator.h>
#include <Base/Container/PersistentBitSet.h>

#include <vulkan/vulkan.h>
#include <unordered_set>

namespace Renderer
{
    namespace Backend
    {
        PRAGMA_NO_PADDING_START;
        struct GraphicsPipelineCacheDesc
        {
            RasterizerState rasterizerState;
            DepthStencilState depthStencilState;
            BlendState blendState;
            InputLayout inputLayouts[GraphicsPipelineDesc::MAX_INPUT_LAYOUTS];
            PrimitiveTopology primitiveTopology = PrimitiveTopology::Triangles;

            u8 shaderStageType = 0;
            VertexShaderID vertexShader = VertexShaderID::Invalid();
            TaskShaderID taskShader = TaskShaderID::Invalid();
            MeshShaderID meshShader = MeshShaderID::Invalid();
            PixelShaderID pixelShader = PixelShaderID::Invalid();

            ImageFormat renderTargetFormats[MAX_RENDER_TARGETS] = { ImageFormat::UNKNOWN };
            DepthImageFormat depthStencilFormat = DepthImageFormat::UNKNOWN;
        };
        PRAGMA_NO_PADDING_END;

        struct GraphicsPipeline
        {
            GraphicsPipelineDesc desc;
            u64 cacheDescHash;

            VkPipelineLayout pipelineLayout;
            VkPipeline pipeline;

            u32 numRenderTargets = 0;
            uvec2 resolution = uvec2(0,0);

            std::vector<DescriptorSetLayoutData> descriptorSetLayoutDatas;
            std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

            std::vector<VkPushConstantRange> pushConstantRanges;

            std::unordered_set<u32> setsUsed;
            std::unordered_map<u32, PersistentBitSet> usedBindingsPerSlot;
            u16 usedDescriptorSetMask = 0; // bit `slot` set for each statically-used descriptor set slot (including DEBUG)
        };

        struct ComputePipelineCacheDesc
        {
            ComputeShaderID shader;
        };

        struct ComputePipeline
        {
            ComputePipelineDesc desc;
            u64 cacheDescHash;

            VkPipelineLayout pipelineLayout;
            VkPipeline pipeline;

            std::vector<DescriptorSetLayoutData> descriptorSetLayoutDatas;
            std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

            std::vector<VkPushConstantRange> pushConstantRanges;

            std::unordered_set<u32> setsUsed;
            std::unordered_map<u32, PersistentBitSet> usedBindingsPerSlot;
            u16 usedDescriptorSetMask = 0; // bit `slot` set for each statically-used descriptor set slot (including DEBUG)
        };

        struct PipelineHandlerVKData : IPipelineHandlerVKData
        {
            std::vector<GraphicsPipeline> graphicsPipelines;
            std::vector<ComputePipeline> computePipelines;
        };

        void PipelineHandlerVK::Init(Memory::Allocator* allocator, RenderDeviceVK* device, ShaderHandlerVK* shaderHandler, ImageHandlerVK* imageHandler, BufferHandlerVK* bufferHandler)
        {
            _allocator = allocator;
            _device = device;
            _shaderHandler = shaderHandler;
            _imageHandler = imageHandler;
            _bufferHandler = bufferHandler;
            _data = new PipelineHandlerVKData();
        }

        void PipelineHandlerVK::RecreatePipelines()
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);

            for (u32 i = 0; i < data.graphicsPipelines.size(); i++)
            {
                auto& pipeline = data.graphicsPipelines[i];

                vkDestroyPipeline(_device->_device, pipeline.pipeline, nullptr);
                vkDestroyPipelineLayout(_device->_device, pipeline.pipelineLayout, nullptr);

                for (VkDescriptorSetLayout& layout : pipeline.descriptorSetLayouts)
                {
                    vkDestroyDescriptorSetLayout(_device->_device, layout, nullptr);
                }

                pipeline.descriptorSetLayouts.clear();
                pipeline.descriptorSetLayoutDatas.clear();
                pipeline.pushConstantRanges.clear();

                CreatePipelineInternal(pipeline, pipeline.desc, pipeline.numRenderTargets);
            }

            for (u32 i = 0; i < data.computePipelines.size(); i++)
            {
                auto& pipeline = data.computePipelines[i];

                vkDestroyPipeline(_device->_device, pipeline.pipeline, nullptr);
                vkDestroyPipelineLayout(_device->_device, pipeline.pipelineLayout, nullptr);

                for (VkDescriptorSetLayout& layout : pipeline.descriptorSetLayouts)
                {
                    vkDestroyDescriptorSetLayout(_device->_device, layout, nullptr);
                }
                pipeline.descriptorSetLayouts.clear();
                pipeline.descriptorSetLayoutDatas.clear();
                pipeline.pushConstantRanges.clear();

                CreatePipelineInternal(pipeline, pipeline.desc);
            }
        }

        GraphicsPipelineID PipelineHandlerVK::CreatePipeline(const GraphicsPipelineDesc& desc)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);

            // -- Get number of render targets and attachments --
            u8 numAttachments = 0;
            for (int i = 0; i < MAX_RENDER_TARGETS; i++)
            {
                if (desc.states.renderTargetFormats[i] == ImageFormat::UNKNOWN)
                    break;

                numAttachments++;
            }
            
            // Check the cache
            size_t nextID;
            u64 cacheDescHash = CalculateCacheDescHash(desc);
            if (TryFindExistingGPipeline(cacheDescHash, nextID))
            {
                // Update the desc in case the ResourceToID functions has changed
                data.graphicsPipelines[nextID].desc = desc;

                return GraphicsPipelineID(static_cast<gIDType>(nextID));
            }
            nextID = data.graphicsPipelines.size();
            
            // Make sure we haven't exceeded the limit of the GraphicsPipelineID type, if this hits you need to change type of GraphicsPipelineID to something bigger
            assert(nextID < GraphicsPipelineID::MaxValue());

            GraphicsPipeline pipeline;
            pipeline.desc = desc;
            pipeline.cacheDescHash = cacheDescHash;
            pipeline.numRenderTargets = numAttachments;

            CreatePipelineInternal(pipeline, desc, numAttachments);

            GraphicsPipelineID pipelineID = GraphicsPipelineID(static_cast<gIDType>(nextID));
            data.graphicsPipelines.push_back(pipeline);

            return pipelineID;
        }

        ComputePipelineID PipelineHandlerVK::CreatePipeline(const ComputePipelineDesc& desc)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);

            // Check the cache
            size_t nextID;
            u64 cacheDescHash = CalculateCacheDescHash(desc);
            if (TryFindExistingCPipeline(cacheDescHash, nextID))
            {
                return ComputePipelineID(static_cast<ComputePipelineID::type>(nextID));
            }
            nextID = data.computePipelines.size();

            ComputePipeline pipeline;
            pipeline.desc = desc;
            pipeline.cacheDescHash = cacheDescHash;

            CreatePipelineInternal(pipeline, desc);

            ComputePipelineID pipelineID = ComputePipelineID(static_cast<cIDType>(nextID));
            data.computePipelines.push_back(pipeline);

            return pipelineID;
        }

        const GraphicsPipelineDesc& PipelineHandlerVK::GetDesc(GraphicsPipelineID id)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            return data.graphicsPipelines[static_cast<gIDType>(id)].desc;
        }

        const ComputePipelineDesc& PipelineHandlerVK::GetDesc(ComputePipelineID id)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            return data.computePipelines[static_cast<cIDType>(id)].desc;
        }

        GraphicsPipelineDesc& PipelineHandlerVK::GetMutableDesc(GraphicsPipelineID id)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            return data.graphicsPipelines[static_cast<gIDType>(id)].desc;
        }

        ComputePipelineDesc& PipelineHandlerVK::GetMutableDesc(ComputePipelineID id)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            return data.computePipelines[static_cast<cIDType>(id)].desc;
        }

        VkPipeline PipelineHandlerVK::GetPipeline(GraphicsPipelineID id)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            return data.graphicsPipelines[static_cast<gIDType>(id)].pipeline;
        }

        VkPipeline PipelineHandlerVK::GetPipeline(ComputePipelineID id)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            return data.computePipelines[static_cast<cIDType>(id)].pipeline;
        }

        uvec2 PipelineHandlerVK::GetRenderPassResolution(GraphicsPipelineID id)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            return data.graphicsPipelines[static_cast<gIDType>(id)].resolution;
        }

        u32 PipelineHandlerVK::GetNumPushConstantRanges(GraphicsPipelineID id)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            return static_cast<u32>(data.graphicsPipelines[static_cast<gIDType>(id)].pushConstantRanges.size());
        }

        u32 PipelineHandlerVK::GetNumPushConstantRanges(ComputePipelineID id)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            return static_cast<u32>(data.computePipelines[static_cast<cIDType>(id)].pushConstantRanges.size());
        }

        const VkPushConstantRange& PipelineHandlerVK::GetPushConstantRange(GraphicsPipelineID id, u32 index)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            return data.graphicsPipelines[static_cast<gIDType>(id)].pushConstantRanges[index];
        }

        const VkPushConstantRange& PipelineHandlerVK::GetPushConstantRange(ComputePipelineID id, u32 index)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            return data.computePipelines[static_cast<cIDType>(id)].pushConstantRanges[index];
        }

        u32 PipelineHandlerVK::GetNumDescriptorSetLayouts(GraphicsPipelineID id)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            return static_cast<u32>(data.graphicsPipelines[static_cast<gIDType>(id)].descriptorSetLayoutDatas.size());
        }

        u32 PipelineHandlerVK::GetNumDescriptorSetLayouts(ComputePipelineID id)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            return static_cast<u32>(data.computePipelines[static_cast<cIDType>(id)].descriptorSetLayoutDatas.size());
        }

        bool PipelineHandlerVK::UsesDescriptorSet(GraphicsPipelineID id, u32 setNumber)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            return static_cast<u32>(data.graphicsPipelines[static_cast<gIDType>(id)].setsUsed.contains(setNumber));
        }

        bool PipelineHandlerVK::UsesDescriptorSet(ComputePipelineID id, u32 setNumber)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            return static_cast<u32>(data.computePipelines[static_cast<cIDType>(id)].setsUsed.contains(setNumber));
        }

        u16 PipelineHandlerVK::GetUsedDescriptorSetMask(GraphicsPipelineID id)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            return data.graphicsPipelines[static_cast<gIDType>(id)].usedDescriptorSetMask;
        }

        u16 PipelineHandlerVK::GetUsedDescriptorSetMask(ComputePipelineID id)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            return data.computePipelines[static_cast<cIDType>(id)].usedDescriptorSetMask;
        }

        const PersistentBitSet* PipelineHandlerVK::GetUsedBindings(GraphicsPipelineID id, u32 slot)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            auto& map = data.graphicsPipelines[static_cast<gIDType>(id)].usedBindingsPerSlot;
            auto it = map.find(slot);
            return (it != map.end()) ? &it->second : nullptr;
        }

        const PersistentBitSet* PipelineHandlerVK::GetUsedBindings(ComputePipelineID id, u32 slot)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            auto& map = data.computePipelines[static_cast<cIDType>(id)].usedBindingsPerSlot;
            auto it = map.find(slot);
            return (it != map.end()) ? &it->second : nullptr;
        }

        DescriptorSetLayoutData& PipelineHandlerVK::GetDescriptorSetLayoutData(GraphicsPipelineID id, u32 index)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            return data.graphicsPipelines[static_cast<gIDType>(id)].descriptorSetLayoutDatas[index];
        }
        
        DescriptorSetLayoutData& PipelineHandlerVK::GetDescriptorSetLayoutData(ComputePipelineID id, u32 index)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            return data.computePipelines[static_cast<cIDType>(id)].descriptorSetLayoutDatas[index];
        }

        VkDescriptorSetLayout& PipelineHandlerVK::GetDescriptorSetLayout(GraphicsPipelineID id, u32 index)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            return data.graphicsPipelines[static_cast<gIDType>(id)].descriptorSetLayouts[index];
        }

        VkDescriptorSetLayout& PipelineHandlerVK::GetDescriptorSetLayout(ComputePipelineID id, u32 index)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            return data.computePipelines[static_cast<cIDType>(id)].descriptorSetLayouts[index];
        }

        VkPipelineLayout& PipelineHandlerVK::GetPipelineLayout(GraphicsPipelineID id)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            return data.graphicsPipelines[static_cast<gIDType>(id)].pipelineLayout;
        }

        VkPipelineLayout& PipelineHandlerVK::GetPipelineLayout(ComputePipelineID id)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            return data.computePipelines[static_cast<cIDType>(id)].pipelineLayout;
        }

        u64 PipelineHandlerVK::CalculateCacheDescHash(const GraphicsPipelineDesc& desc)
        {
            GraphicsPipelineCacheDesc cacheDesc = {};
            cacheDesc.rasterizerState = desc.states.rasterizerState;
            cacheDesc.depthStencilState = desc.states.depthStencilState;
            cacheDesc.blendState = desc.states.blendState;
            for (u32 i = 0; i < GraphicsPipelineDesc::MAX_INPUT_LAYOUTS; i++)
            {
                cacheDesc.inputLayouts[i] = desc.states.inputLayouts[i];
            }
            cacheDesc.primitiveTopology = desc.states.primitiveTopology;
            cacheDesc.pixelShader = desc.states.pixelShader;
            for (u32 i = 0; i < MAX_RENDER_TARGETS; i++)
            {
                cacheDesc.renderTargetFormats[i] = desc.states.renderTargetFormats[i];
            }
            cacheDesc.depthStencilFormat = desc.states.depthStencilFormat;

            if (std::holds_alternative<VertexPipelineStages>(desc.shaderStages))
            {
                const VertexPipelineStages& stages = std::get<VertexPipelineStages>(desc.shaderStages);
                cacheDesc.vertexShader = stages.vertexShader;
            }
            else
            {
                const MeshPipelineStages& stages = std::get<MeshPipelineStages>(desc.shaderStages);
                cacheDesc.shaderStageType = 1;
                cacheDesc.taskShader = stages.taskShader;
                cacheDesc.meshShader = stages.meshShader;
            }

            u64 hash = XXHash64::hash(&cacheDesc, sizeof(GraphicsPipelineCacheDesc), 0);

            return hash;
        }

        u64 PipelineHandlerVK::CalculateCacheDescHash(const ComputePipelineDesc& desc)
        {
            ComputePipelineCacheDesc cacheDesc = {};
            cacheDesc.shader = desc.computeShader;

            u64 hash = XXHash64::hash(&cacheDesc, sizeof(ComputePipelineCacheDesc), 0);

            return hash;
        }

        bool PipelineHandlerVK::TryFindExistingGPipeline(u64 descHash, size_t& id)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            id = 0;

            for (auto& pipeline : data.graphicsPipelines)
            {
                if (descHash == pipeline.cacheDescHash)
                {
                    return true;
                }
                id++;
            }

            return false;
        }

        bool PipelineHandlerVK::TryFindExistingCPipeline(u64 descHash, size_t& id)
        {
            PipelineHandlerVKData& data = static_cast<PipelineHandlerVKData&>(*_data);
            id = 0;

            for (auto& pipeline : data.computePipelines)
            {
                if (descHash == pipeline.cacheDescHash)
                {
                    return true;
                }
                id++;
            }

            return false;
        }

        DescriptorSetLayoutData& PipelineHandlerVK::GetDescriptorSet(i32 setNumber, std::vector<DescriptorSetLayoutData>& sets)
        {
            while (static_cast<i32>(sets.size())-1 < setNumber)
            {
                DescriptorSetLayoutData setLayoutData = {};
                setLayoutData.createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                setLayoutData.createInfo.bindingCount = 0;
                setLayoutData.createInfo.pBindings = nullptr;

                sets.push_back(setLayoutData);
            }

            return sets[setNumber];
        }

        void PipelineHandlerVK::CreatePipelineInternal(GraphicsPipeline& pipeline, const GraphicsPipelineDesc& desc, u32 numAttachments)
        {
            u32 numSupportedTextures = _device->HasExtendedTextureSupport() ? 8192 : 4096;

            VertexShaderID vertexShader = VertexShaderID::Invalid();
            TaskShaderID taskShader = TaskShaderID::Invalid();
            MeshShaderID meshShader = MeshShaderID::Invalid();

            if (std::holds_alternative<VertexPipelineStages>(desc.shaderStages))
            {
                vertexShader = std::get<VertexPipelineStages>(desc.shaderStages).vertexShader;
            }
            else
            {
                const MeshPipelineStages& stages = std::get<MeshPipelineStages>(desc.shaderStages);
                taskShader = stages.taskShader;
                meshShader = stages.meshShader;
            }

            const bool hasVertexShader = vertexShader != VertexShaderID::Invalid();
            const bool hasTaskShader = taskShader != TaskShaderID::Invalid();
            const bool hasMeshShader = meshShader != MeshShaderID::Invalid();

            if (hasVertexShader == hasMeshShader)
            {
                NC_LOG_CRITICAL("Graphics pipeline '{0}' must specify exactly one vertex-producing shader: vertex or mesh", desc.debugName);
            }
            if (hasTaskShader && !hasMeshShader)
            {
                NC_LOG_CRITICAL("Graphics pipeline '{0}' specifies a task shader without a mesh shader", desc.debugName);
            }
            if (hasTaskShader && !_device->GetMeshShaderProperties().taskShaderSupported)
            {
                NC_LOG_CRITICAL("Graphics pipeline '{0}' requires task shaders, but the selected GPU does not support them", desc.debugName);
            }
            if (hasMeshShader && desc.states.inputLayouts[0].enabled)
            {
                NC_LOG_CRITICAL("Graphics pipeline '{0}' uses a mesh shader and cannot specify vertex input layouts", desc.debugName);
            }

            // -- Get Reflection data from shader --
            std::vector<BindInfo> bindInfos;
            std::vector<BindInfoPushConstant> bindInfoPushConstants;
            auto mergeShaderReflection = [&](auto shaderID, const char* shaderStageName)
            {
                const BindReflection& usedBindReflection = _shaderHandler->GetUsedBindReflection(shaderID);
                for (const BindInfo& bindInfo : usedBindReflection.dataBindings)
                {
                    pipeline.setsUsed.insert(bindInfo.set);
                    pipeline.usedBindingsPerSlot[bindInfo.set].Set(bindInfo.binding);
                }

                const BindReflection& bindReflection = _shaderHandler->GetFullBindReflection(shaderID);

                for (const BindInfo& dataBinding : bindReflection.dataBindings)
                {
                    bool found = false;
                    for (BindInfo& bindInfo : bindInfos)
                    {
                        if (dataBinding.set == bindInfo.set &&
                            dataBinding.binding == bindInfo.binding)
                        {
                            if (dataBinding.nameHash == bindInfo.nameHash &&
                                dataBinding.descriptorType == bindInfo.descriptorType &&
                                dataBinding.count == bindInfo.count)
                            {
                                bindInfo.stageFlags |= dataBinding.stageFlags;
                            }
                            else
                            {
                                NC_LOG_CRITICAL("{0} in graphics pipeline '{1}' conflicts with another shader at descriptor set {2}, binding {3}", shaderStageName, desc.debugName, dataBinding.set, dataBinding.binding);
                            }
                            found = true;
                            break;
                        }
                    }

                    if (!found)
                    {
                        bindInfos.push_back(dataBinding);
                    }
                }

                for (const BindInfoPushConstant& pushConstant : bindReflection.pushConstants)
                {
                    if (!bindInfoPushConstants.empty())
                    {
                        BindInfoPushConstant& existing = bindInfoPushConstants[0];

                        u32 end = glm::max(existing.offset + existing.size, pushConstant.offset + pushConstant.size);
                        existing.offset = glm::min(existing.offset, pushConstant.offset);
                        existing.size = end - existing.offset;
                        existing.stageFlags |= pushConstant.stageFlags;
                    }
                    else
                    {
                        bindInfoPushConstants.push_back(pushConstant);
                    }
                }
            };

            if (hasVertexShader)
            {
                mergeShaderReflection(vertexShader, "Vertex shader");
            }
            if (hasTaskShader)
            {
                mergeShaderReflection(taskShader, "Task shader");
            }
            if (hasMeshShader)
            {
                mergeShaderReflection(meshShader, "Mesh shader");
            }
            if (desc.states.pixelShader != PixelShaderID::Invalid())
            {
                mergeShaderReflection(desc.states.pixelShader, "Pixel shader");
            }

            // Build the used-set bitmask from reflection. DEBUG is included: if a shader actively uses the DEBUG set we want
            // the draw validator to flag a missing bind. The asymmetric "binding DEBUG that the pipeline doesn't use" case
            // is still tolerated because BindDescriptorSet silently early-returns for DEBUG when the pipeline doesn't use it.
            pipeline.usedDescriptorSetMask = 0;
            for (u32 slot : pipeline.setsUsed)
            {
                pipeline.usedDescriptorSetMask |= static_cast<u16>(1u << slot);
            }

            // -- Create Descriptor Set Layout from reflected SPIR-V --
            for (BindInfo& bindInfo : bindInfos)
            {
                DescriptorSetLayoutData& layout = GetDescriptorSet(bindInfo.set, pipeline.descriptorSetLayoutDatas);
                VkDescriptorSetLayoutBinding layoutBinding = {};

                layoutBinding.binding = bindInfo.binding;
                layoutBinding.descriptorType = bindInfo.descriptorType;
                layoutBinding.descriptorCount = (bindInfo.count == 0) ? numSupportedTextures : bindInfo.count;
                layoutBinding.stageFlags = bindInfo.stageFlags;

                layout.bindings.push_back(layoutBinding);

                bool isTextureType = bindInfo.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE ||
                    bindInfo.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                bool isTextureArray = isTextureType && (bindInfo.count == 0 || bindInfo.count > 1);
                layout.isTextureArray.push_back(isTextureArray);
                layout.isVariableBinding.push_back(bindInfo.count == 0);
            }

            size_t numDescriptorSets = pipeline.descriptorSetLayoutDatas.size();
            pipeline.descriptorSetLayouts.resize(numDescriptorSets);

            for (size_t i = 0; i < numDescriptorSets; i++)
            {
                DescriptorSetLayoutData& layoutData = pipeline.descriptorSetLayoutDatas[i];

                std::vector<VkDescriptorBindingFlags> bindingFlags(layoutData.bindings.size(), VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);
                for (size_t j = 0; j < layoutData.bindings.size(); j++)
                {
                    if (layoutData.isTextureArray[j])
                    {
                        bindingFlags[j] |= VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
                    }
                    if (layoutData.isVariableBinding[j])
                    {
                        bindingFlags[j] |= VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;
                    }
                }

                VkDescriptorSetLayoutBindingFlagsCreateInfo flagsInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO };
                flagsInfo.bindingCount = static_cast<u32>(bindingFlags.size());
                flagsInfo.pBindingFlags = bindingFlags.data();

                layoutData.createInfo.pNext = &flagsInfo;
                layoutData.createInfo.bindingCount = static_cast<u32>(layoutData.bindings.size());
                layoutData.createInfo.pBindings = layoutData.bindings.data();
                layoutData.createInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

                if (vkCreateDescriptorSetLayout(_device->_device, &layoutData.createInfo, nullptr, &pipeline.descriptorSetLayouts[i]) != VK_SUCCESS)
                {
                    NC_LOG_CRITICAL("Failed to create descriptor set layout!");
                }
                DebugMarkerUtilVK::SetObjectName(_device->_device, (uint64_t)pipeline.descriptorSetLayouts[i], VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, desc.debugName.c_str());
            }

            // -- Create Push Constant Range from reflected SPIR-V --
            for (BindInfoPushConstant& pushConstant : bindInfoPushConstants)
            {
                VkPushConstantRange& range = pipeline.pushConstantRanges.emplace_back();
                range.offset = pushConstant.offset;
                range.size = pushConstant.size;
                range.stageFlags = pushConstant.stageFlags;
            }

            std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
            if (hasVertexShader)
            {
                VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
                vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

                vertShaderStageInfo.module = _shaderHandler->GetShaderModule(vertexShader);
                vertShaderStageInfo.pName = "main";

                shaderStages.push_back(vertShaderStageInfo);
            }
            if (hasTaskShader)
            {
                VkPipelineShaderStageCreateInfo taskShaderStageInfo = {};
                taskShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                taskShaderStageInfo.stage = VK_SHADER_STAGE_TASK_BIT_EXT;
                taskShaderStageInfo.module = _shaderHandler->GetShaderModule(taskShader);
                taskShaderStageInfo.pName = "main";
                shaderStages.push_back(taskShaderStageInfo);
            }
            if (hasMeshShader)
            {
                VkPipelineShaderStageCreateInfo meshShaderStageInfo = {};
                meshShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                meshShaderStageInfo.stage = VK_SHADER_STAGE_MESH_BIT_EXT;
                meshShaderStageInfo.module = _shaderHandler->GetShaderModule(meshShader);
                meshShaderStageInfo.pName = "main";
                shaderStages.push_back(meshShaderStageInfo);
            }
            if (desc.states.pixelShader != PixelShaderID::Invalid())
            {
                VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
                fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;

                fragShaderStageInfo.module = _shaderHandler->GetShaderModule(desc.states.pixelShader);
                fragShaderStageInfo.pName = "main";

                shaderStages.push_back(fragShaderStageInfo);
            }

            // Now we need to create vertex input bindings, one (if necessary) for per-vertex data, one (if necessary) for per-instance data
            u8 numVertexAttributes = 0;
            u32 vertexStride = 0;

            u8 numInstanceAttributes = 0;
            u32 instanceStride = 0;

            for (auto& inputLayout : desc.states.inputLayouts)
            {
                if (!inputLayout.enabled)
                    break;

                if (inputLayout.inputClassification == InputClassification::PER_VERTEX)
                {
                    numVertexAttributes++;
                    vertexStride += FormatConverterVK::ToByteSize(inputLayout.format);
                }
                else
                {
                    numInstanceAttributes++;
                    instanceStride += FormatConverterVK::ToByteSize(inputLayout.format);
                }
            }

            // -- Create binding description(s) --
            std::vector<VkVertexInputBindingDescription> inputBindingDescriptions;

            u8 vertexBinding = 0;
            if (numVertexAttributes > 0)
            {
                VkVertexInputBindingDescription bindingDescription = {};
                bindingDescription.binding = static_cast<u32>(inputBindingDescriptions.size());
                bindingDescription.stride = vertexStride;
                bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                inputBindingDescriptions.push_back(bindingDescription);
            }

            u8 instanceBinding = 0;
            if (numInstanceAttributes > 0)
            {
                instanceBinding = static_cast<u8>(inputBindingDescriptions.size());

                VkVertexInputBindingDescription bindingDescription = {};
                bindingDescription.binding = instanceBinding;
                bindingDescription.stride = instanceStride;
                bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

                inputBindingDescriptions.push_back(bindingDescription);
            }

            std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
            attributeDescriptions.reserve(numVertexAttributes + numInstanceAttributes);

            u8 attributeCounts[2] = { 0 };
            u32 attributeOffsets[2] = { 0 };

            for (auto& inputLayout : desc.states.inputLayouts)
            {
                if (!inputLayout.enabled)
                    break;

                bool isPerVertex = inputLayout.inputClassification == InputClassification::PER_VERTEX;

                u8 binding = (isPerVertex) ? vertexBinding : instanceBinding;

                u8& attributeCount = attributeCounts[isPerVertex];
                u32& attributeOffset = attributeOffsets[isPerVertex];

                VkVertexInputAttributeDescription attributeDescription;
                attributeDescription.binding = binding;
                attributeDescription.location = attributeCount++;
                attributeDescription.format = FormatConverterVK::ToVkFormat(inputLayout.format);
                attributeDescription.offset = attributeOffset;

                attributeOffset += FormatConverterVK::ToByteSize(inputLayout.format);

                attributeDescriptions.push_back(attributeDescription);
            }

            VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
            vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputInfo.vertexBindingDescriptionCount = static_cast<u32>(inputBindingDescriptions.size());
            vertexInputInfo.pVertexBindingDescriptions = inputBindingDescriptions.data();
            vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
            vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

            VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = FormatConverterVK::ToVkPrimitiveTopology(desc.states.primitiveTopology);
            inputAssembly.primitiveRestartEnable = VK_FALSE;

            // -- Set viewport and scissor rect --
            VkPipelineViewportStateCreateInfo viewportState = {};
            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.pViewports = nullptr; // These are dynamic
            viewportState.scissorCount = 1;
            viewportState.pScissors = nullptr; // These are dynamic

            // -- Rasterizer --
            VkPipelineRasterizationStateCreateInfo rasterizer = {};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.polygonMode = FormatConverterVK::ToVkPolygonMode(desc.states.rasterizerState.fillMode);
            rasterizer.lineWidth = 1.0f;
            rasterizer.cullMode = FormatConverterVK::ToVkCullModeFlags(desc.states.rasterizerState.cullMode);
            rasterizer.frontFace = FormatConverterVK::ToVkFrontFace(desc.states.rasterizerState.frontFaceMode);
            rasterizer.depthBiasEnable = desc.states.rasterizerState.depthBiasEnabled;
            rasterizer.depthBiasConstantFactor = static_cast<f32>(desc.states.rasterizerState.depthBias);
            rasterizer.depthBiasClamp = desc.states.rasterizerState.depthBiasClamp;
            rasterizer.depthBiasSlopeFactor = desc.states.rasterizerState.depthBiasSlopeFactor;
            rasterizer.depthClampEnable = desc.states.rasterizerState.depthClampEnabled;

            // -- Multisampling --
            VkPipelineMultisampleStateCreateInfo multisampling = {};
            multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.rasterizationSamples = FormatConverterVK::ToVkSampleCount(desc.states.rasterizerState.sampleCount);
            multisampling.minSampleShading = 1.0f; // Optional
            multisampling.pSampleMask = nullptr; // Optional
            multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
            multisampling.alphaToOneEnable = VK_FALSE; // Optional

            // -- DepthStencil --
            VkPipelineDepthStencilStateCreateInfo depthStencil = {};
            depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencil.depthTestEnable = desc.states.depthStencilState.depthEnable;
            depthStencil.depthWriteEnable = desc.states.depthStencilState.depthWriteEnable;
            depthStencil.depthCompareOp = FormatConverterVK::ToVkCompareOp(desc.states.depthStencilState.depthFunc);
            depthStencil.stencilTestEnable = desc.states.depthStencilState.stencilEnable;

            depthStencil.front = {};
            depthStencil.front.failOp = FormatConverterVK::ToVkStencilOp(desc.states.depthStencilState.frontFace.stencilFailOp);
            depthStencil.front.passOp = FormatConverterVK::ToVkStencilOp(desc.states.depthStencilState.frontFace.stencilPassOp);
            depthStencil.front.depthFailOp = FormatConverterVK::ToVkStencilOp(desc.states.depthStencilState.frontFace.stencilDepthFailOp);
            depthStencil.front.compareOp = FormatConverterVK::ToVkCompareOp(desc.states.depthStencilState.frontFace.stencilFunc);

            depthStencil.back = {};
            depthStencil.back.failOp = FormatConverterVK::ToVkStencilOp(desc.states.depthStencilState.backFace.stencilFailOp);
            depthStencil.back.passOp = FormatConverterVK::ToVkStencilOp(desc.states.depthStencilState.backFace.stencilPassOp);
            depthStencil.back.depthFailOp = FormatConverterVK::ToVkStencilOp(desc.states.depthStencilState.backFace.stencilDepthFailOp);
            depthStencil.back.compareOp = FormatConverterVK::ToVkCompareOp(desc.states.depthStencilState.backFace.stencilFunc);

            // -- Blenders --
            std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments(pipeline.numRenderTargets);

            for (u32 i = 0; i < pipeline.numRenderTargets; i++)
            {
                colorBlendAttachments[i].blendEnable = desc.states.blendState.renderTargets[i].blendEnable;
                colorBlendAttachments[i].srcColorBlendFactor = FormatConverterVK::ToVkBlendFactor(desc.states.blendState.renderTargets[i].srcBlend);
                colorBlendAttachments[i].dstColorBlendFactor = FormatConverterVK::ToVkBlendFactor(desc.states.blendState.renderTargets[i].destBlend);
                colorBlendAttachments[i].colorBlendOp = FormatConverterVK::ToVkBlendOp(desc.states.blendState.renderTargets[i].blendOp);
                colorBlendAttachments[i].srcAlphaBlendFactor = FormatConverterVK::ToVkBlendFactor(desc.states.blendState.renderTargets[i].srcBlendAlpha);
                colorBlendAttachments[i].dstAlphaBlendFactor = FormatConverterVK::ToVkBlendFactor(desc.states.blendState.renderTargets[i].destBlendAlpha);
                colorBlendAttachments[i].alphaBlendOp = FormatConverterVK::ToVkBlendOp(desc.states.blendState.renderTargets[i].blendOpAlpha);
                colorBlendAttachments[i].colorWriteMask = FormatConverterVK::ToVkColorComponentFlags(desc.states.blendState.renderTargets[i].renderTargetWriteMask);
            }

            VkPipelineColorBlendStateCreateInfo colorBlending = {};
            colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable = desc.states.blendState.renderTargets[0].logicOpEnable;
            colorBlending.logicOp = FormatConverterVK::ToVkLogicOp(desc.states.blendState.renderTargets[0].logicOp);
            colorBlending.attachmentCount = pipeline.numRenderTargets;
            colorBlending.pAttachments = colorBlendAttachments.data();
            colorBlending.blendConstants[0] = 0.0f; // TODO: Blend constants
            colorBlending.blendConstants[1] = 0.0f; // TODO: Blend constants
            colorBlending.blendConstants[2] = 0.0f; // TODO: Blend constants
            colorBlending.blendConstants[3] = 0.0f; // TODO: Blend constants

            VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = static_cast<u32>(pipeline.descriptorSetLayouts.size());
            pipelineLayoutInfo.pSetLayouts = pipeline.descriptorSetLayouts.data();
            pipelineLayoutInfo.pushConstantRangeCount = static_cast<u32>(pipeline.pushConstantRanges.size()); // Optional
            pipelineLayoutInfo.pPushConstantRanges = pipeline.pushConstantRanges.data();

            if (vkCreatePipelineLayout(_device->_device, &pipelineLayoutInfo, nullptr, &pipeline.pipelineLayout) != VK_SUCCESS)
            {
                NC_LOG_CRITICAL("Failed to create pipeline layout!");
            }
            DebugMarkerUtilVK::SetObjectName(_device->_device, (uint64_t)pipeline.pipelineLayout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, desc.debugName.c_str());

            // Set up dynamic rendering
            std::vector<VkFormat> colorAttachmentFormats;
            for (u32 i = 0; i < numAttachments; i++)
            {
                colorAttachmentFormats.push_back(FormatConverterVK::ToVkFormat(desc.states.renderTargetFormats[i]));
            }

            VkFormat depthFormat = VK_FORMAT_UNDEFINED;
            VkFormat stencilFormat = VK_FORMAT_UNDEFINED;
            if (desc.states.depthStencilFormat != DepthImageFormat::UNKNOWN)
            {
                depthFormat = FormatConverterVK::ToVkFormat(desc.states.depthStencilFormat);
            }

            // Prepare the dynamic rendering create info structure
            VkPipelineRenderingCreateInfo pipelineRenderingInfo = {};
            pipelineRenderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
            pipelineRenderingInfo.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentFormats.size());
            pipelineRenderingInfo.pColorAttachmentFormats = colorAttachmentFormats.data();
            pipelineRenderingInfo.depthAttachmentFormat = depthFormat;
            pipelineRenderingInfo.stencilAttachmentFormat = stencilFormat;

            // Set up dynamic viewport and scissor
            std::vector<VkDynamicState> dynamicStates;
            dynamicStates.reserve(3);

            dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
            dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);
            dynamicStates.push_back(VK_DYNAMIC_STATE_DEPTH_BIAS);

            VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
            dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicStateCreateInfo.dynamicStateCount = static_cast<u32>(dynamicStates.size());
            dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

            VkGraphicsPipelineCreateInfo pipelineInfo = {};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = static_cast<u32>(shaderStages.size());
            pipelineInfo.pStages = shaderStages.data();
            pipelineInfo.pVertexInputState = hasMeshShader ? nullptr : &vertexInputInfo;
            pipelineInfo.pInputAssemblyState = hasMeshShader ? nullptr : &inputAssembly;
            pipelineInfo.pViewportState = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizer;
            pipelineInfo.pMultisampleState = &multisampling;
            pipelineInfo.pDepthStencilState = &depthStencil;
            pipelineInfo.pColorBlendState = &colorBlending;
            pipelineInfo.pDynamicState = &dynamicStateCreateInfo;
            pipelineInfo.layout = pipeline.pipelineLayout;
            pipelineInfo.renderPass = VK_NULL_HANDLE;
            pipelineInfo.subpass = 0;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
            pipelineInfo.basePipelineIndex = -1; // Optional
            pipelineInfo.pNext = &pipelineRenderingInfo;

            if (vkCreateGraphicsPipelines(_device->_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.pipeline) != VK_SUCCESS)
            {
                NC_LOG_CRITICAL("Failed to create graphics pipeline!");
            }
            DebugMarkerUtilVK::SetObjectName(_device->_device, (uint64_t)pipeline.pipeline, VK_OBJECT_TYPE_PIPELINE, desc.debugName.c_str());
        }

        void PipelineHandlerVK::CreatePipelineInternal(ComputePipeline& pipeline, const ComputePipelineDesc& desc)
        {
            u32 numSupportedTextures = _device->HasExtendedTextureSupport() ? 8192 : 4096;

            // Find which sets are used by the shader
            const BindReflection& usedBindReflection = _shaderHandler->GetUsedBindReflection(desc.computeShader);
            for(const BindInfo& bindInfo : usedBindReflection.dataBindings)
            {
                pipeline.setsUsed.insert(bindInfo.set);
                pipeline.usedBindingsPerSlot[bindInfo.set].Set(bindInfo.binding);
            }

            // Build the used-set bitmask from reflection. DEBUG is included: if a shader actively uses the DEBUG set we want
            // the draw validator to flag a missing bind. The asymmetric "binding DEBUG that the pipeline doesn't use" case
            // is still tolerated because BindDescriptorSet silently early-returns for DEBUG when the pipeline doesn't use it.
            pipeline.usedDescriptorSetMask = 0;
            for (u32 slot : pipeline.setsUsed)
            {
                pipeline.usedDescriptorSetMask |= static_cast<u16>(1u << slot);
            }

            std::vector<BindInfo> bindInfos;
            std::vector<BindInfoPushConstant> bindInfoPushConstants;
            const BindReflection& bindReflection = _shaderHandler->GetFullBindReflection(desc.computeShader);
            bindInfos.insert(bindInfos.end(), bindReflection.dataBindings.begin(), bindReflection.dataBindings.end());
            bindInfoPushConstants.insert(bindInfoPushConstants.end(), bindReflection.pushConstants.begin(), bindReflection.pushConstants.end());

            for (BindInfo& bindInfo : bindInfos)
            {
                DescriptorSetLayoutData& layout = GetDescriptorSet(bindInfo.set, pipeline.descriptorSetLayoutDatas);
                VkDescriptorSetLayoutBinding layoutBinding = {};
                layoutBinding.binding = bindInfo.binding;
                layoutBinding.descriptorType = bindInfo.descriptorType;
                layoutBinding.descriptorCount = (bindInfo.count == 0) ? numSupportedTextures : bindInfo.count;
                layoutBinding.stageFlags = bindInfo.stageFlags;
                layout.bindings.push_back(layoutBinding);

                bool isTextureType = bindInfo.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE ||
                    bindInfo.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                bool isTextureArray = isTextureType && (bindInfo.count == 0 || bindInfo.count > 1);
                layout.isTextureArray.push_back(isTextureArray);
                layout.isVariableBinding.push_back(bindInfo.count == 0);
            }

            for (BindInfoPushConstant& pushConstant : bindInfoPushConstants)
            {
                VkPushConstantRange& range = pipeline.pushConstantRanges.emplace_back();
                range.offset = pushConstant.offset;
                range.size = pushConstant.size;
                range.stageFlags = pushConstant.stageFlags;
            }

            size_t numDescriptorSets = pipeline.descriptorSetLayoutDatas.size();
            pipeline.descriptorSetLayouts.resize(numDescriptorSets);
            for (size_t i = 0; i < numDescriptorSets; i++)
            {
                DescriptorSetLayoutData& layoutData = pipeline.descriptorSetLayoutDatas[i];

                // Build binding flags
                std::vector<VkDescriptorBindingFlags> bindingFlags(layoutData.bindings.size(), VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);
                for (size_t j = 0; j < layoutData.bindings.size(); j++)
                {
                    if (layoutData.isTextureArray[j])
                    {
                        bindingFlags[j] |= VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
                    }
                    if (layoutData.isVariableBinding[j])
                    {
                        bindingFlags[j] |= VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;
                    }
                }

                VkDescriptorSetLayoutBindingFlagsCreateInfo flagsInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO };
                flagsInfo.bindingCount = static_cast<u32>(bindingFlags.size());
                flagsInfo.pBindingFlags = bindingFlags.data();

                layoutData.createInfo.pNext = &flagsInfo;
                layoutData.createInfo.bindingCount = static_cast<u32>(layoutData.bindings.size());
                layoutData.createInfo.pBindings = layoutData.bindings.data();
                layoutData.createInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

                if (vkCreateDescriptorSetLayout(_device->_device, &layoutData.createInfo, nullptr, &pipeline.descriptorSetLayouts[i]) != VK_SUCCESS)
                {
                    NC_LOG_CRITICAL("Failed to create descriptor set layout!");
                }
                DebugMarkerUtilVK::SetObjectName(_device->_device, (uint64_t)pipeline.descriptorSetLayouts[i], VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, desc.debugName.c_str());
            }

            VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = static_cast<u32>(pipeline.descriptorSetLayouts.size());
            pipelineLayoutInfo.pSetLayouts = pipeline.descriptorSetLayouts.data();
            pipelineLayoutInfo.pushConstantRangeCount = static_cast<u32>(pipeline.pushConstantRanges.size()); // Optional
            pipelineLayoutInfo.pPushConstantRanges = pipeline.pushConstantRanges.data();

            if (vkCreatePipelineLayout(_device->_device, &pipelineLayoutInfo, nullptr, &pipeline.pipelineLayout) != VK_SUCCESS)
            {
                NC_LOG_CRITICAL("Failed to create pipeline layout!");
            }
            DebugMarkerUtilVK::SetObjectName(_device->_device, (uint64_t)pipeline.pipelineLayout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, desc.debugName.c_str());

            VkPipelineShaderStageCreateInfo shaderStage = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
            shaderStage.module = _shaderHandler->GetShaderModule(desc.computeShader);
            shaderStage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
            shaderStage.pName = "main";

            VkComputePipelineCreateInfo pipelineInfo = { VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };
            pipelineInfo.stage = shaderStage;
            pipelineInfo.layout = pipeline.pipelineLayout;

            if (vkCreateComputePipelines(_device->_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.pipeline) != VK_SUCCESS)
            {
                NC_LOG_CRITICAL("Failed to create compute pipeline!");
            }
            DebugMarkerUtilVK::SetObjectName(_device->_device, (uint64_t)pipeline.pipeline, VK_OBJECT_TYPE_PIPELINE, desc.debugName.c_str());
        }

    }
}
