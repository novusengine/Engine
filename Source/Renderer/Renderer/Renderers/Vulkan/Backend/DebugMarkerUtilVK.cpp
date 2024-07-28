#include "DebugMarkerUtilVK.h"

namespace Renderer
{
    namespace Backend
    {
        bool DebugMarkerUtilVK::_initialized = false;
        bool DebugMarkerUtilVK::_debugUtilsEnabled = false;
        bool DebugMarkerUtilVK::_debugUtilsAvailable = false;

        PFN_vkSetDebugUtilsObjectTagEXT DebugMarkerUtilVK::fnSetDebugUtilsObjectTag = nullptr;
        PFN_vkSetDebugUtilsObjectNameEXT DebugMarkerUtilVK::fnSetDebugUtilsObjectName = nullptr;
        PFN_vkCmdBeginDebugUtilsLabelEXT DebugMarkerUtilVK::fnCmdBeginDebugUtilsLabel = nullptr;
        PFN_vkCmdEndDebugUtilsLabelEXT DebugMarkerUtilVK::fnCmdEndDebugUtilsLabel = nullptr;
        PFN_vkCmdInsertDebugUtilsLabelEXT DebugMarkerUtilVK::fnCmdInsertDebugUtilsLabel = nullptr;
    }
}
