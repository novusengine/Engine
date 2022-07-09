#include "DebugHandler.h"

#include <cassert>

#if defined(_WIN32)
#include <Windows.h>
#endif

bool DebugHandler::_isInitialized = false;
IDebugHandlerData* DebugHandler::_data = nullptr;

struct DebugHandlerData : IDebugHandlerData
{
	HANDLE handle;
	u32 defaultColor;
};

void DebugHandler::Initialize()
{
	DebugHandlerData* data = new DebugHandlerData();
	_data = data;

#ifdef _WIN32
	data->handle = GetStdHandle(STD_OUTPUT_HANDLE); // MAYBE this instead: GetConsoleWindow();
	CONSOLE_SCREEN_BUFFER_INFO info;

	if (!GetConsoleScreenBufferInfo(data->handle, &info))
	{
		//assert(false);
	}

	data->defaultColor = info.wAttributes;
#endif
	_isInitialized = true;
}

void DebugHandler::PushColor(ColorCode color)
{
	DebugHandlerData* data = static_cast<DebugHandlerData*>(_data);
#ifdef _WIN32
	SetConsoleTextAttribute(data->handle, color);
#endif
}

void DebugHandler::PopColor()
{
	DebugHandlerData* data = static_cast<DebugHandlerData*>(_data);
#ifdef _WIN32
	SetConsoleTextAttribute(data->handle, data->defaultColor);
#endif
}