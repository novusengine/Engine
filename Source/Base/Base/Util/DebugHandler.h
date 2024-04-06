#pragma once
#include "Base/Types.h"
#include "Base/Platform.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/bundled/color.h>

#include <format>
#include <string>
#include <source_location>

PRAGMA_CLANG_DIAGNOSTIC_PUSH;
PRAGMA_CLANG_DIAGNOSTIC_IGNORE(-Wformat-security);
PRAGMA_CLANG_DIAGNOSTIC_IGNORE(-Wnon-pod-varargs);

namespace Logging
{
    using source_location = std::source_location;
    [[nodiscard]] constexpr auto get_log_source_location(const source_location& location)
    {
        return spdlog::source_loc { location.file_name(), static_cast<std::int32_t>(location.line()), location.function_name() };
    }

    struct format_with_location
    {
        std::string_view value;
        spdlog::source_loc loc;

        template <typename String>
        format_with_location(const String& s, const source_location& location = source_location::current()) : value{ s }, loc{ get_log_source_location(location) } {}
    };
}

class DebugHandler
{
public:
    template <typename... Args>
    inline static void Print(const std::string_view fmt, Args... args)
    {
        spdlog::default_logger_raw()->log(spdlog::level::info, std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...)));
    }

    template <typename... Args>
    inline static void PrintTrace(const Logging::format_with_location& fmt, Args... args)
    {
        spdlog::default_logger_raw()->log(fmt.loc, spdlog::level::trace, std::vformat(fmt.value, std::make_format_args(std::forward<Args>(args)...)));
    }

    template <typename... Args>
    inline static void PrintDebug(const Logging::format_with_location& fmt, Args... args)
    {
        spdlog::default_logger_raw()->log(fmt.loc, spdlog::level::debug, std::vformat(fmt.value, std::make_format_args(std::forward<Args>(args)...)));
    }

    template <typename... Args>
    inline static void PrintWarning(const Logging::format_with_location& fmt, Args... args)
    {
        spdlog::default_logger_raw()->log(fmt.loc, spdlog::level::warn, std::vformat(fmt.value, std::make_format_args(std::forward<Args>(args)...)));
    }

    template <typename... Args>
    inline static void PrintError(const Logging::format_with_location& fmt, Args... args)
    {
        spdlog::default_logger_raw()->log(fmt.loc, spdlog::level::err, std::vformat(fmt.value, std::make_format_args(std::forward<Args>(args)...)));
    }

    template <typename... Args>
    inline static void PrintFatal(const Logging::format_with_location& fmt, Args... args)
    {
        spdlog::default_logger_raw()->log(fmt.loc, spdlog::level::critical, std::vformat(fmt.value, std::make_format_args(std::forward<Args>(args)...)));

        ReleaseModeBreakpoint();
    }

    template <typename... Args>
    inline static void Assert(bool assertion, const Logging::format_with_location& fmt, Args... args)
    {
        if (!assertion)
        {
            PrintFatal(fmt, std::forward<Args>(args)...);
        }
    }

private:
};
PRAGMA_CLANG_DIAGNOSTIC_POP;