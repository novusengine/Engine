#include "DebugMarkerUtilVK.h"

namespace Renderer
{
    namespace Backend
    {
        bool DebugMarkerUtilVK::_initialized = false;
        bool DebugMarkerUtilVK::_debugMarkersAvailable = false;
        bool DebugMarkerUtilVK::_debugMarkersEnabled = false;
        PFN_vkDebugMarkerSetObjectTagEXT DebugMarkerUtilVK::fnDebugMarkerSetObjectTag = nullptr;
        PFN_vkDebugMarkerSetObjectNameEXT DebugMarkerUtilVK::fnDebugMarkerSetObjectName = nullptr;
        PFN_vkCmdDebugMarkerBeginEXT DebugMarkerUtilVK::fnCmdDebugMarkerBegin = nullptr;
        PFN_vkCmdDebugMarkerEndEXT DebugMarkerUtilVK::fnCmdDebugMarkerEnd = nullptr;
        PFN_vkCmdDebugMarkerInsertEXT DebugMarkerUtilVK::fnCmdDebugMarkerInsert = nullptr;
    }
}
