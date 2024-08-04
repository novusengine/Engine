#pragma once
#include "Base/Types.h"
#include "Base/Platform.h"

#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/Logger.h"
#include "quill/sinks/ConsoleSink.h"

PRAGMA_CLANG_DIAGNOSTIC_PUSH;
PRAGMA_CLANG_DIAGNOSTIC_IGNORE(-Wformat-security);
PRAGMA_CLANG_DIAGNOSTIC_IGNORE(-Wnon-pod-varargs);

#define NC_GET_LOGGER() quill::Frontend::get_logger("root")
#define NC_LOG_TRACE_L3(fmt, ...) LOG_TRACE_L3(NC_GET_LOGGER(), fmt, ##__VA_ARGS__);
#define NC_LOG_TRACE_L2(fmt, ...) LOG_TRACE_L2(NC_GET_LOGGER(), fmt, ##__VA_ARGS__);
#define NC_LOG_TRACE_L1(fmt, ...) LOG_TRACE_L1(NC_GET_LOGGER(), fmt, ##__VA_ARGS__);
#define NC_LOG_DEBUG(fmt, ...) LOG_DEBUG(NC_GET_LOGGER(), fmt, ##__VA_ARGS__);
#define NC_LOG_INFO(fmt, ...) LOG_INFO(NC_GET_LOGGER(), fmt, ##__VA_ARGS__);
#define NC_LOG_WARNING(fmt, ...) LOG_WARNING(NC_GET_LOGGER(), fmt, ##__VA_ARGS__);
#define NC_LOG_ERROR(fmt, ...) LOG_ERROR(NC_GET_LOGGER(), fmt, ##__VA_ARGS__);
#define NC_LOG_CRITICAL(fmt, ...) LOG_CRITICAL(NC_GET_LOGGER(), fmt, ##__VA_ARGS__); NC_GET_LOGGER()->flush_log(); ReleaseModeBreakpoint();
#define NC_LOG_BACKTRACE(fmt, ...) LOG_BACKTRACE(NC_GET_LOGGER(), fmt, ##__VA_ARGS__);
#define NC_ASSERT(assertion, fmt, ...) if (!(assertion)) { NC_LOG_CRITICAL(fmt, ##__VA_ARGS__); }

PRAGMA_CLANG_DIAGNOSTIC_POP;