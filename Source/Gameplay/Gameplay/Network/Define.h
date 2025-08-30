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

    enum class CheatCommands : u8
    {
        None,
        Damage,
        Heal,
        Kill,
        Resurrect,
        Morph,
        Demorph,
        Teleport,
        CreateCharacter,
        DeleteCharacter,
        SetRace,
        SetGender,
        SetClass,
        SetLevel,
        SetItemTemplate,
        SetItemStatTemplate,
        SetItemArmorTemplate,
        SetItemWeaponTemplate,
        SetItemShieldTemplate,
        AddItem,
        RemoveItem,
        TriggerAdd,
        TriggerRemove
    };
}