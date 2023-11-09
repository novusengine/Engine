#pragma once
#include <Base/Types.h>
#include <Base/Platform.h>

namespace DB2::WDC3::Definitions
{
	struct Map
	{
		u32 directory = 0;
		u32 mapName = 0;
		u32 mapDesc1 = 0;
		u32 mapDesc2 = 0;
		u32 pvpShortDesc = 0;
		u32 pvpLongDesc = 0;
		u32 corpsePos[2] = { 0 };
		u32 mapType = 0;
		u8 instanceType = 0;
		u8 expansionID = 0;
		u16 areaTableID = 0;
		u16 loadingScreenID = 0;
		u16 timeOfDayOverride = 0;
		u16 parentMapID = 0;
		u32 cosmeticParentMapID = 0;
		u8 timeOffset = 0;
		u32 minimapIconScale = 0;
		u16 corpseMapID = 0;
		u8 maxPlayers = 0;
		u8 windSettingsID = 0;
		u32 zmpFileDataID = 0;
		u32 wdtFileDataID = 0;
		u32 flags[3] = { };
	};

	struct Map340
	{
		u32 directory = 0;
		u32 mapName = 0;
		u32 mapDescription0 = 0;
		u32 mapDescription1 = 0;
		u32 pvpShortDescription = 0;
		u32 pvpLongDescription = 0;
		u8 mapType = 0;
		u8 instanceType = 0;
		u8 expansionID = 0;
		u16 areaTableID = 0;
		u16 loadingScreenID = 0;
		u16 timeOfDayOverride = 0;
		u16 parentMapID = 0;
		u16 cosmeticParentMapID = 0;
		u8 timeOffset = 0;
		u32 minimapIconScale = 0;
		u32 raidOffset = 0;
		u16 corpseMapID = 0;
		u8 maxPlayers = 0;
		u16 windSettingsID = 0;
		u32 zmpFileDataID = 0;
		u32 flags[3] = { 0 };
	};

	/*struct Map340
	{
		u32 directory = 0;
		u32 mapName = 0;
		u32 mapDescription0 = 0;
		u32 mapDescription1 = 0;
		u32 pvpShortDescription = 0;
		u32 pvpLongDescription = 0;
		u32 mapType = 0;
		u8 instanceType = 0;
		u8 expansionID = 0;
		u16 areaTableID = 0;
		u16 loadingScreenID = 0;
		u32 timeOfDayOverride = 0;
		u32 parentMapID = 0;
		u16 cosmeticParentMapID = 0;
		u8 timeOffset = 0;
		u32 minimapIconScale = 0;
		u32 raidOffset = 0;
		u16 corpseMapID = 0;
		u32 maxPlayers = 0;
		u32 windSettingsID = 0;
		u32 zmpFileDataID = 0;
		u32 flags[3] = { 0 };
	};*/
}
