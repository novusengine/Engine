#include "DefineUtil.h"

namespace Network::Util::DefineUtil
{
	u8 GetSocketIDVersion(SocketID socketID)
	{
		return static_cast<u32>(socketID) >> SOCKET_ID_VALUE_BITS;
	}

	u32 GetSocketIDValue(SocketID socketID)
	{
		return static_cast<u32>(socketID) & SOCKET_ID_VALUE_MASK;
	}

	SocketID CreateSocketID(u8 version, u32 value)
	{
		return static_cast<SocketID>(value | (version << SOCKET_ID_VALUE_BITS));
	}
}