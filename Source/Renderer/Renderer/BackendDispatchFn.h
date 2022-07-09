#pragma once
#include "Descriptors/CommandListDesc.h"

namespace Renderer
{
    class Renderer;
    typedef void (*BackendDispatchFunction)(Renderer*, CommandListID, const void*);
}