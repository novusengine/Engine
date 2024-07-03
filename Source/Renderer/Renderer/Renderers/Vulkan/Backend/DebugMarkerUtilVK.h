#pragma once
#include <Base/Types.h>
#include <Base/Math/Color.h>

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <vector>

namespace Renderer
{
    namespace Backend
    {
        class DebugMarkerUtilVK
        {
        public:
            
            static bool GetDebugMarkersEnabled() 
            {
                return _debugUtilsEnabled; 
            }

            static void SetObjectName(VkDevice device, uint64_t object, VkObjectType objectType, const char* name)
            {
                if (_debugUtilsAvailable && _debugUtilsEnabled)
                {
                    const VkDebugUtilsObjectNameInfoEXT nameInfo =
                    {
                        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
                        .pNext = NULL,
                        .objectType = objectType,
                        .objectHandle = object,
                        .pObjectName = name,
                    };
                    fnSetDebugUtilsObjectName(device, &nameInfo);
                }
            }

            static void PushMarker(VkCommandBuffer commandBuffer, Color color, std::string name)
            {
                if (_debugUtilsAvailable && _debugUtilsEnabled)
                {
                    VkDebugUtilsLabelEXT label =
                    {
                        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
                        .pNext = NULL,
                        .pLabelName = NULL,
                        .color = { 0.0f, 0.0f, 0.0f, 0.0f },
                    };

                    label.color[0] = color.r;
                    label.color[1] = color.g;
                    label.color[2] = color.b;
                    label.color[3] = color.a;

                    label.pLabelName = name.c_str();
                    fnCmdBeginDebugUtilsLabel(commandBuffer, &label);
                }
            }

            static void PopMarker(VkCommandBuffer commandBuffer)
            {
                if (_debugUtilsAvailable && _debugUtilsEnabled)
                {
                    fnCmdEndDebugUtilsLabel(commandBuffer);
                }
            }

        private:
            static void InitializeFunctions(VkInstance instance)
            {
                assert(!_initialized);
                _initialized = true;

                if (_debugUtilsAvailable && _debugUtilsEnabled)
                {
                    fnSetDebugUtilsObjectTag = (PFN_vkSetDebugUtilsObjectTagEXT)vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectTagEXT");
                    fnSetDebugUtilsObjectName = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT");
                    fnCmdBeginDebugUtilsLabel = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkCmdBeginDebugUtilsLabelEXT");
                    fnCmdEndDebugUtilsLabel = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkCmdEndDebugUtilsLabelEXT");
                    fnCmdInsertDebugUtilsLabel = (PFN_vkCmdInsertDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkCmdInsertDebugUtilsLabelEXT");
                }
            }

            static void CheckExtension(const VkExtensionProperties& extension)
            {
                if (_debugUtilsEnabled)
                {
                    if (!strcmp(extension.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
                    {
                        _debugUtilsAvailable = true;
                    }
                }
            }

            static void AddValidationLayer(std::vector<const char*>& layers) 
            {
                if (_debugUtilsAvailable && _debugUtilsEnabled)
                    layers.push_back("VK_EXT_DEBUG_UTILS_EXTENSION_NAME"); 
            }

            static void AddEnabledExtension(std::vector<const char*>& extensions)
            {
                if (_debugUtilsAvailable && _debugUtilsEnabled)
                    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }

            static void SetDebugUtilsEnabled(bool available) { _debugUtilsEnabled = available; }

        private:
            static bool _initialized;
            static bool _debugUtilsEnabled;
            static bool _debugUtilsAvailable;

            static PFN_vkSetDebugUtilsObjectTagEXT fnSetDebugUtilsObjectTag;
            static PFN_vkSetDebugUtilsObjectNameEXT fnSetDebugUtilsObjectName;
            static PFN_vkCmdBeginDebugUtilsLabelEXT fnCmdBeginDebugUtilsLabel;
            static PFN_vkCmdEndDebugUtilsLabelEXT fnCmdEndDebugUtilsLabel;
            static PFN_vkCmdInsertDebugUtilsLabelEXT fnCmdInsertDebugUtilsLabel;

            friend class RenderDeviceVK;
        };
    }
}