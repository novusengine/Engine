#pragma once
#include "Descriptors/ComputePipelineDesc.h"
#include "Descriptors/GraphicsPipelineDesc.h"

#include <Base/Types.h>
#include <Base/Util/StringUtils.h>

#include <robinhood/robinhood.h>

namespace Renderer
{
    enum class DescriptorMetaType
    {
        SAMPLER,
        COMBINED_IMAGE_SAMPLER,
        SAMPLED_IMAGE,
        STORAGE_IMAGE,
        UNIFORM_TEXEL_BUFFER,
        STORAGE_TEXEL_BUFFER,
        UNIFORM_BUFFER,
        STORAGE_BUFFER,

        // These are useless because we do indirect rendering but keeping for completeness
        UNIFORM_BUFFER_DYNAMIC,
        STORAGE_BUFFER_DYNAMIC,
        INPUT_ATTACHMENT
    };

    /*
    switch (descriptorType)
            {
                case VK_DESCRIPTOR_TYPE_SAMPLER:
                    return DescriptorMetaType::SAMPLER;
                case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                    return DescriptorMetaType::COMBINED_IMAGE_SAMPLER;
                case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                    return DescriptorMetaType::SAMPLED_IMAGE;
                case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                    return DescriptorMetaType::STORAGE_IMAGE;
                case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
                    return DescriptorMetaType::UNIFORM_TEXEL_BUFFER;
                case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
                    return DescriptorMetaType::STORAGE_TEXEL_BUFFER;
                case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                    return DescriptorMetaType::UNIFORM_BUFFER;
                case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
                    return DescriptorMetaType::STORAGE_BUFFER;
                case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
                    return DescriptorMetaType::UNIFORM_BUFFER_DYNAMIC;
                case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
                    return DescriptorMetaType::STORAGE_BUFFER_DYNAMIC;
                case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
                    return DescriptorMetaType::INPUT_ATTACHMENT;
                default:
                    NC_LOG_CRITICAL("This should never hit, did we forget to update this function after adding more descriptor types?");
            }
    
    */

    struct DescriptorMeta
    {
        std::string name;
        u32 nameHash;
        u32 bindingIndex;
        DescriptorMetaType type;
    };

    struct DescriptorMetaInfo
    {
        std::vector<DescriptorMeta> descriptors;
        robin_hood::unordered_map<u32, u32> nameHashToDescriptorIndex;
        robin_hood::unordered_map<u32, u32> bindingIndexToDescriptorIndex;
    };
}