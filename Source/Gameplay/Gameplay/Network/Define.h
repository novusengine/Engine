#pragma once
#include "NetFields.h"

#include <Base/Types.h>

#include <Meta/Generated/Shared/NetFieldsEnum.h>

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

    using ObjectNetFields = NetFields<Generated::ObjectNetFieldsEnum>;
    using ObjectNetFieldsListener = NetFieldListener<Generated::ObjectNetFieldsEnum>;

    using UnitNetFields = NetFields<Generated::UnitNetFieldsEnum>;
    using UnitNetFieldsListener = NetFieldListener<Generated::UnitNetFieldsEnum>;
}