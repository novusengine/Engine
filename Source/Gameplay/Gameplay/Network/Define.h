#pragma once
#include "Base/Types.h"

namespace Network
{
    enum class ConnectResult : u8
    {
        None,
        Success,
        Failed
    };

    enum class ConnectionStatus : u8
    {
        None,
        Connected
    };
}