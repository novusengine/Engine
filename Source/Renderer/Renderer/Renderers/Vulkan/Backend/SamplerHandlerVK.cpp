#include "SamplerHandlerVK.h"
#include "RenderDeviceVK.h"
#include "TextureHandlerVK.h"
#include "PipelineHandlerVK.h"
#include "FormatConverterVK.h"

#include <Base/Util/DebugHandler.h>
#include <Base/Util/StringUtils.h>
#include <Base/Util/XXHash64.h>

#include <vulkan/vulkan.h>

#include <vector>


namespace Renderer
{
    namespace Backend
    {
        using _SamplerID = type_safe::underlying_type<SamplerID>;
        struct Sampler
        {
            u64 samplerHash;
            SamplerDesc desc;

            VkSampler sampler;
        };

        struct SamplerHandlerVKData : ISamplerHandlerVKData
        {
            std::vector<Sampler> samplers;
        };

        void SamplerHandlerVK::Init(RenderDeviceVK* device)
        {
            _device = device;
            _data = new SamplerHandlerVKData();
        }

        SamplerID SamplerHandlerVK::CreateSampler(const SamplerDesc& desc)
        {
            SamplerHandlerVKData& data = static_cast<SamplerHandlerVKData&>(*_data);

            // Check the cache
            size_t nextID;
            u64 samplerHash = CalculateSamplerHash(desc);
            if (TryFindExistingSampler(samplerHash, nextID))
            {
                return SamplerID(static_cast<SamplerID::type>(nextID));
            }
            nextID = data.samplers.size();

            // Make sure we haven't exceeded the limit of the SamplerID type, if this hits you need to change type of SamplerID to something bigger
            assert(nextID < SamplerID::MaxValue());

            Sampler sampler;
            sampler.samplerHash = samplerHash;
            sampler.desc = desc;

            // Create sampler
            VkSamplerCreateInfo samplerInfo = {};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = FormatConverterVK::ToVkFilterMag(desc.filter);
            samplerInfo.minFilter = FormatConverterVK::ToVkFilterMin(desc.filter);
            samplerInfo.addressModeU = FormatConverterVK::ToVkSamplerAddressMode(desc.addressU);
            samplerInfo.addressModeV = FormatConverterVK::ToVkSamplerAddressMode(desc.addressV);
            samplerInfo.addressModeW = FormatConverterVK::ToVkSamplerAddressMode(desc.addressW);
            samplerInfo.anisotropyEnable = FormatConverterVK::ToAnisotropyEnabled(desc.filter);
            samplerInfo.maxAnisotropy = static_cast<f32>(desc.maxAnisotropy);
            samplerInfo.borderColor = FormatConverterVK::ToVkBorderColor(desc.borderColor);
            samplerInfo.unnormalizedCoordinates = desc.unnormalizedCoordinates;
            samplerInfo.compareEnable = desc.comparisonEnabled;
            samplerInfo.compareOp = FormatConverterVK::ToVkCompareOp(desc.comparisonFunc);
            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            samplerInfo.mipLodBias = desc.mipLODBias;
            samplerInfo.minLod = desc.minLOD;
            samplerInfo.maxLod = desc.maxLOD;


            VkSamplerReductionModeCreateInfoEXT createInfoReduction = { VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO_EXT };

            if (desc.mode != SamplerReductionMode::NONE)
            {
                switch (desc.mode)
                {
                    case SamplerReductionMode::MAX: createInfoReduction.reductionMode = VK_SAMPLER_REDUCTION_MODE_MAX; break;
                    case SamplerReductionMode::MIN: createInfoReduction.reductionMode = VK_SAMPLER_REDUCTION_MODE_MIN; break;

                    default: break;
                }

                samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
                //reduction mode has to override compare enable. Maybe log this?
                samplerInfo.compareEnable = VK_FALSE;
                //hook pnext
                samplerInfo.pNext = &createInfoReduction;
            }

            if (vkCreateSampler(_device->_device, &samplerInfo, nullptr, &sampler.sampler) != VK_SUCCESS)
            {
                DebugHandler::PrintFatal("Failed to create sampler!");
            }

            data.samplers.push_back(sampler);
            return SamplerID(static_cast<SamplerID::type>(nextID));
        }

        VkSampler& SamplerHandlerVK::GetSampler(const SamplerID samplerID)
        {
            SamplerHandlerVKData& data = static_cast<SamplerHandlerVKData&>(*_data);

            // Lets make sure this id exists
            assert(data.samplers.size() > static_cast<SamplerID::type>(samplerID));
            return data.samplers[static_cast<SamplerID::type>(samplerID)].sampler;
        }

        const SamplerDesc& SamplerHandlerVK::GetSamplerDesc(const SamplerID samplerID)
        {
            SamplerHandlerVKData& data = static_cast<SamplerHandlerVKData&>(*_data);

            // Lets make sure this id exists
            assert(data.samplers.size() > static_cast<SamplerID::type>(samplerID));
            return data.samplers[static_cast<SamplerID::type>(samplerID)].desc;
        }

        u64 SamplerHandlerVK::CalculateSamplerHash(const SamplerDesc& desc)
        {
            return XXHash64::hash(&desc, sizeof(desc), 0);
        }

        bool SamplerHandlerVK::TryFindExistingSampler(u64 descHash, size_t& id)
        {
            SamplerHandlerVKData& data = static_cast<SamplerHandlerVKData&>(*_data);
            id = 0;

            for (auto& sampler : data.samplers)
            {
                if (descHash == sampler.samplerHash)
                {
                    return true;
                }
                id++;
            }

            return false;
        }
    }
}