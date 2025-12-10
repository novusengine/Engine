#pragma once
#include "NetFields.h"

#include <Base/Types.h>

#include <MetaGen/EnumTraits.h>
#include <MetaGen/Shared/NetField/NetField.h>

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

    using ObjectNetFields = NetFields<MetaGen::Shared::NetField::ObjectNetFieldEnum>;
    using ObjectNetFieldsListener = NetFieldListener<MetaGen::Shared::NetField::ObjectNetFieldEnum>;

    using UnitNetFields = NetFields<MetaGen::Shared::NetField::UnitNetFieldEnum>;
    using UnitNetFieldsListener = NetFieldListener<MetaGen::Shared::NetField::UnitNetFieldEnum>;
}