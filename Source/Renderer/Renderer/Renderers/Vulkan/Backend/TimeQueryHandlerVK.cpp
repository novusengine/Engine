#include "TimeQueryHandlerVK.h"

#include "RenderDeviceVK.h"

#include <Base/Util/StringUtils.h>

namespace Renderer
{
    namespace Backend
    {
        PFN_vkResetQueryPoolEXT TimeQueryHandlerVK::fnVkResetQueryPoolEXT = nullptr;

        constexpr u32 NUM_FRAMES_IN_FLIGHT = 2;

        struct TimeQuery
        {
            std::string name;
            u32 nameHash;

            static const u32 NUM_TIMESTAMPS_PER_FRAME = 2; // because we need Begin and End time
            u32 timestampOffset = 0;
            u32 timestampCount = 0;

            u64 lastTimestamps[NUM_TIMESTAMPS_PER_FRAME] = {};
        };

        struct TimeQueryHandlerVKData : ITimeQueryHandlerVKData
        {
            VkQueryPool queryPool;

            static const u32 NUM_TOTAL_TIMESTAMPS = 200;
            u32 numTimestamps = 0;

            std::vector<TimeQuery> timeQueries;

            u32 currentFrameIndex = 0;
        };

        void TimeQueryHandlerVK::Init(RenderDeviceVK* device)
        {
            _data = new TimeQueryHandlerVKData();
            TimeQueryHandlerVKData& data = static_cast<TimeQueryHandlerVKData&>(*_data);

            _device = device;

            VkQueryPoolCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
            createInfo.pNext = nullptr; // Optional
            createInfo.flags = 0; // Reserved for future use, must be 0!

            createInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
            createInfo.queryCount = TimeQueryHandlerVKData::NUM_TOTAL_TIMESTAMPS;

            VkResult result = vkCreateQueryPool(_device->_device, &createInfo, nullptr, &data.queryPool);
            NC_ASSERT(result == VK_SUCCESS, "TimeQueryHandlerVK : Init failed to create time query pool!");

            fnVkResetQueryPoolEXT = (PFN_vkResetQueryPoolEXT)vkGetDeviceProcAddr(_device->_device, "vkResetQueryPoolEXT");

            // Queries must be reset on creation.
            fnVkResetQueryPoolEXT(_device->_device, data.queryPool, 0, TimeQueryHandlerVKData::NUM_TOTAL_TIMESTAMPS);
        }

        void TimeQueryHandlerVK::FlipFrame(u32 frameIndex)
        {
            TimeQueryHandlerVKData& data = static_cast<TimeQueryHandlerVKData&>(*_data);

            data.currentFrameIndex = frameIndex;

            for (TimeQuery& timeQuery : data.timeQueries)
            {
                u32 frameOffset = data.currentFrameIndex * NUM_FRAMES_IN_FLIGHT;

                u32 offset = timeQuery.timestampOffset + frameOffset;
                u32 count = TimeQuery::NUM_TIMESTAMPS_PER_FRAME;

                u64 buffer[TimeQuery::NUM_TIMESTAMPS_PER_FRAME];
                VkResult result = vkGetQueryPoolResults(_device->_device, data.queryPool, offset, count, sizeof(u64) * count, buffer, sizeof(u64),
                    VK_QUERY_RESULT_64_BIT);
                if (result == VK_NOT_READY)
                {
                    continue;
                }
                else if (result == VK_SUCCESS)
                {
                    memcpy(timeQuery.lastTimestamps, buffer, sizeof(u64) * TimeQuery::NUM_TIMESTAMPS_PER_FRAME);
                }
                else
                {
                    NC_LOG_CRITICAL("TimeQueryHandlerVK : FlipFrame failed to receive query results!");
                }

                // Queries must be reset after each individual use.
                fnVkResetQueryPoolEXT(_device->_device, data.queryPool, offset, count);
            }
        }

        TimeQueryID TimeQueryHandlerVK::CreateTimeQuery(const TimeQueryDesc& desc)
        {
            TimeQueryHandlerVKData& data = static_cast<TimeQueryHandlerVKData&>(*_data);

            // Check if a timequery with this name already exists
            u32 nameHash = StringUtils::fnv1a_32(desc.name.c_str(), desc.name.size());

            for (u32 i = 0; i < data.timeQueries.size(); i++)
            {
                if (nameHash == data.timeQueries[i].nameHash)
                {
                    return TimeQueryID(static_cast<TimeQueryID::type>(i));
                }
            }

            // Else create it
            TimeQueryID::type index = static_cast<TimeQueryID::type>(data.timeQueries.size());
            TimeQuery& timeQuery = data.timeQueries.emplace_back();
            timeQuery.name = desc.name;
            timeQuery.nameHash = nameHash;

            const u32 queryCount = TimeQuery::NUM_TIMESTAMPS_PER_FRAME * NUM_FRAMES_IN_FLIGHT;

            NC_ASSERT(data.numTimestamps + queryCount < data.NUM_TOTAL_TIMESTAMPS, "TimeQueryHandlerVK : CreateTimeQuery overflowed NUM_TOTAL_QUERIES, maybe increase it?");

            timeQuery.timestampOffset = data.numTimestamps;
            timeQuery.timestampCount = queryCount;
            data.numTimestamps += queryCount;

            return TimeQueryID(index);
        }

        void TimeQueryHandlerVK::Begin(VkCommandBuffer buffer, TimeQueryID id)
        {
            TimeQueryHandlerVKData& data = static_cast<TimeQueryHandlerVKData&>(*_data);

            TimeQueryID::type typedID = static_cast<TimeQueryID::type>(id);
            NC_ASSERT(typedID < data.timeQueries.size(), "TimeQueryHandlerVK : Begin tried to get TimeQuery ({}) that doesn't exist (Num: {})", typedID, data.timeQueries.size());

            TimeQuery& timeQuery = data.timeQueries[typedID];
            u32 timestampOffset = timeQuery.timestampOffset + (data.currentFrameIndex * TimeQuery::NUM_TIMESTAMPS_PER_FRAME);

            vkCmdWriteTimestamp(buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, data.queryPool, timestampOffset);
        }

        void TimeQueryHandlerVK::End(VkCommandBuffer buffer, TimeQueryID id)
        {
            TimeQueryHandlerVKData& data = static_cast<TimeQueryHandlerVKData&>(*_data);

            TimeQueryID::type typedID = static_cast<TimeQueryID::type>(id);
            NC_ASSERT(typedID < data.timeQueries.size(), "TimeQueryHandlerVK : End tried to get TimeQuery ({}) that doesn't exist (Num: {})", typedID, data.timeQueries.size());

            TimeQuery& timeQuery = data.timeQueries[typedID];
            u32 timestampOffset = timeQuery.timestampOffset + (data.currentFrameIndex * TimeQuery::NUM_TIMESTAMPS_PER_FRAME) + 1;

            vkCmdWriteTimestamp(buffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, data.queryPool, timestampOffset);
        }

        const std::string& TimeQueryHandlerVK::GetName(TimeQueryID id)
        {
            TimeQueryHandlerVKData& data = static_cast<TimeQueryHandlerVKData&>(*_data);

            TimeQueryID::type typedID = static_cast<TimeQueryID::type>(id);
            NC_ASSERT(typedID < data.timeQueries.size(), "TimeQueryHandlerVK : GetLastTime tried to get TimeQuery ({}) that doesn't exist (Num: {})", typedID, data.timeQueries.size());

            TimeQuery& timeQuery = data.timeQueries[typedID];
            return timeQuery.name;
        }

        f32 TimeQueryHandlerVK::GetLastTime(TimeQueryID id)
        {
            TimeQueryHandlerVKData& data = static_cast<TimeQueryHandlerVKData&>(*_data);

            TimeQueryID::type typedID = static_cast<TimeQueryID::type>(id);
            NC_ASSERT(typedID < data.timeQueries.size(), "TimeQueryHandlerVK : GetLastTime tried to get TimeQuery ({}) that doesn't exist (Num: {})", typedID, data.timeQueries.size());

            TimeQuery& timeQuery = data.timeQueries[typedID];
            u64 timeStampDiff = timeQuery.lastTimestamps[1] - timeQuery.lastTimestamps[0];

            f64 durationNanoseconds = static_cast<f64>(timeStampDiff) * _device->_timestampNanosecondsPerIncrement;
            f64 durationMilliseconds = durationNanoseconds / 1000000.0;

            return static_cast<f32>(durationMilliseconds);
        }
    }
}