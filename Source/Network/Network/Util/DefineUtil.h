#pragma once
#include "Network/Define.h"

#include "Base/Types.h"

namespace Network::Util::DefineUtil
{
	u8 GetSocketIDVersion(SocketID socketID);
	u32 GetSocketIDValue(SocketID socketID);
	SocketID CreateSocketID(u8 version, u32 value);
}