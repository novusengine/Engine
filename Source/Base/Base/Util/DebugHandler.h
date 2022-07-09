#pragma once
#include "Base/Types.h"
#include "Base/Platform.h"

#include <string>

PRAGMA_CLANG_DIAGNOSTIC_PUSH;
PRAGMA_CLANG_DIAGNOSTIC_IGNORE(-Wformat-security);
PRAGMA_CLANG_DIAGNOSTIC_IGNORE(-Wnon-pod-varargs);
enum ColorCode
{
	GREEN = 10,
	YELLOW = 14,
	MAGENTA = 5,
	RED = 12,
};

struct IDebugHandlerData {};
class DebugHandler
{
public:
	template <typename... Args>
	inline static void Print(std::string message, Args... args)
	{
		if (!_isInitialized)
		{
			Initialize();
		}

		printf((message + "\n").c_str(), args...);
	}

	template <typename... Args>
	inline static void PrintNoNewline(std::string message, Args... args)
	{
		if (!_isInitialized)
		{
			Initialize();
		}

		printf(message.c_str(), args...);
	}

	template <typename... Args>
	inline static void PrintWarning(std::string message, Args... args)
	{
		if (!_isInitialized)
		{
			Initialize();
		}

		PrintColor("[Warning]: ", ColorCode::YELLOW);
		Print(message, args...);
	}

	template <typename... Args>
	inline static void PrintDeprecated(std::string message, Args... args)
	{
		if (!_isInitialized)
		{
			Initialize();
		}

		PrintColor("[Deprecated]: ", ColorCode::YELLOW);
		Print(message, args...);
	}

	template <typename... Args>
	inline static void PrintError(std::string message, Args... args)
	{
		if (!_isInitialized)
		{
			Initialize();
		}

		PrintColor("[Error]: ", ColorCode::MAGENTA);
		Print(message, args...);
	}

	template <typename... Args>
	inline static void PrintFatal(std::string message, Args... args)
	{
		if (!_isInitialized)
		{
			Initialize();
		}

		PrintColor("[Fatal]: ", ColorCode::RED);
		Print(message, args...);

		ReleaseModeBreakpoint();
	}

	template <typename... Args>
	inline static void PrintSuccess(std::string message, Args... args)
	{
		if (!_isInitialized)
		{
			Initialize();
		}

		PrintColor("[Success]: ", ColorCode::GREEN);
		Print(message, args...);
	}

	template <typename... Args>
	inline static void PrintColor(std::string message, ColorCode color, Args... args)
	{
		PushColor(color);
		printf(message.c_str(), args...);
		PopColor();
	}

private:
	static void Initialize();
	static void PushColor(ColorCode color);
	static void PopColor();

private:
	static bool _isInitialized;
	static IDebugHandlerData* _data;
};
PRAGMA_CLANG_DIAGNOSTIC_POP;