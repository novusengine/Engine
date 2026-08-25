#pragma once
#include "Renderer/Descriptors/BufferDesc.h"
#include "Renderer/Descriptors/DescriptorSetDesc.h"
#include "Renderer/Descriptors/TextureDesc.h"
#include "Renderer/Descriptors/TextureArrayDesc.h"
#include "Renderer/DescriptorType.h"

#include <Base/Platform.h>
#include <Base/Types.h>

#include <vulkan/vulkan_core.h>

class PersistentBitSet;
class BitSet;

namespace Renderer
{
    struct DescriptorPoolStats;
    class TrackedBufferBitSets;

    namespace Backend
    {
        class RenderDeviceVK;
        class TextureHandlerVK;
        class BufferHandlerVK;
        struct DescriptorSet;

        struct IDescriptorHandlerData {};

        class DescriptorHandlerVK
        {
        public:
            void Init(RenderDeviceVK* device, TextureHandlerVK* textureHandler, BufferHandlerVK* bufferHandler);

            DescriptorSetID CreateDescriptorSet(const DescriptorSetDesc& desc);
            void DestroyDescriptorSet(DescriptorSetID setID);
            DescriptorPoolStats GetPoolStats() const;

            void BindDescriptor(DescriptorSetID setID, u32 binding, BufferID bufferID, DescriptorType type, u32 frameIndex);

            // [Frame-safe descriptor rebind] Apply buffer descriptor writes that were deferred while their
            // frame slot was in flight. Call once per frame, right after that slot's fence has been waited.
            void FlushPendingBufferWrites(u32 frameIndex);
            bool HasPendingBufferWrites(DescriptorSetID setID, u32 frameIndex) const;

            // [Temp descriptor sets] Snapshot the set's current contents (canonical frame copy plus this
            // frame's pending buffer binds) into a fresh set from a per-frame transient pool, so a bind
            // recorded now is unaffected by later rewrites of the set. Transient sets live until the
            // frame slot's next FlipFrame.
            u32 SnapshotTempDescriptorSet(DescriptorSetID setID, u32 frameIndex);
            VkDescriptorSet GetTransientVkDescriptorSet(u32 transientSetIndex, u32 frameIndex);

            void MarkBound(DescriptorSetID setID);
            void FlipFrame(u32 frameIndex);
            void OnFrameEnd();
            void BindDescriptor(DescriptorSetID setID, u32 binding, VkImageView image, DescriptorType type, bool isRT, u32 frameIndex);
            void BindDescriptorArray(DescriptorSetID setID, u32 binding, VkImageView image, u32 arrayOffset, DescriptorType type, bool isRT, u32 frameIndex);
            void BindDescriptorArray(DescriptorSetID setID, u32 binding, std::vector<VkImageView>& images, u32 arrayOffset, DescriptorType type, bool isRT, u32 frameIndex);
            void BindDescriptor(DescriptorSetID setID, u32 binding, VkSampler sampler, u32 frameIndex);
            void BindDescriptorArray(DescriptorSetID setID, u32 binding, VkSampler sampler, u32 arrayIndex, u32 frameIndex);
            void BindDescriptor(DescriptorSetID setID, u32 binding, TextureArrayID textureArrayID); // Texture arrays don't take a frameIndex for simplicity, we don't expect them to change per-frame

            // Updates only a range of descriptors in a texture array binding (for incremental updates)
            void UpdateTextureArrayDescriptors(DescriptorSetID setID, u32 binding, const TextureID* textureIDs, u32 startIndex, u32 count);

            void ValidatePermissions(u32 slot, DescriptorSetID descriptorSetID, const TrackedBufferBitSets* bufferPermissions, bool isGraphicsPipeline, const PersistentBitSet* usedBindings = nullptr);
            VkDescriptorSet GetVkDescriptorSet(DescriptorSetID descriptorSetID, u32 frameIndex);
            VkDescriptorSetLayout GetVkDescriptorSetLayout(DescriptorSetID descriptorSetID);

        private:
            void CreateDescriptorPool();
            void CreateDescriptorSet(DescriptorSet& descriptorSet);
            void WriteBufferDescriptor(VkDescriptorSet dstSet, u32 binding, VkBuffer buffer, DescriptorType type);
            void WriteBufferDescriptor(DescriptorSet& descriptorSet, u32 binding, VkBuffer buffer, DescriptorType type, u32 frameIndex);
            void WarnIfBoundThisFrame(DescriptorSet& descriptorSet, u32 binding);
            bool ValidatePermissionViolations(u32 slot, const DescriptorSet& descriptorSet, const PersistentBitSet& accesses, const BitSet& permissions, const char* permissionName, const PersistentBitSet* usedBindings = nullptr);

        private:
            RenderDeviceVK* _device;
            TextureHandlerVK* _textureHandler;
            BufferHandlerVK* _bufferHandler;

            IDescriptorHandlerData* _data;

            u64 _frameGeneration = 0;
            bool _inFrameRecording = false;
        };
    }
}
