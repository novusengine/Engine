#pragma once

#define DO_PRAGMA(X) _Pragma(#X)

#if !defined(_MSC_VER)
#include <csignal>
#endif

#if defined(__clang__)
    #define PRAGMA_CLANG_DIAGNOSTIC_PUSH        DO_PRAGMA(GCC diagnostic push)
    #define PRAGMA_CLANG_DIAGNOSTIC_POP         DO_PRAGMA(GCC diagnostic pop)
    #define PRAGMA_CLANG_DIAGNOSTIC_IGNORE(x)   DO_PRAGMA(GCC diagnostic ignored #x)
#else
    #define PRAGMA_CLANG_DIAGNOSTIC_PUSH
    #define PRAGMA_CLANG_DIAGNOSTIC_POP
    #define PRAGMA_CLANG_DIAGNOSTIC_IGNORE(x)
#endif

#if defined(_MSC_VER)
    #define PRAGMA_MSVC_IGNORE_WARNING(X)   DO_PRAGMA(warning(disable : X))
#else
    #define PRAGMA_MSVC_IGNORE_WARNING(X)
#endif

#if defined(__clang__)
    #define PRAGMA_ENABLE_OPTIMIZATION _Pragma("clang optimize on")
    #define PRAGMA_DISABLE_OPTIMIZATION _Pragma("clang optimize off")
#elif defined(_MSC_VER)
    #define PRAGMA_ENABLE_OPTIMIZATION _Pragma("optimize(\"\", on)")
    #define PRAGMA_DISABLE_OPTIMIZATION _Pragma("optimize(\"\", off)")
#elif defined(__GNUC__)
    #define PRAGMA_ENABLE_OPTIMIZATION __attribute__((optimize("O3")))
    #define PRAGMA_DISABLE_OPTIMIZATION __attribute__((optimize("O0")))
#else
//    static_assert(false, "Please add PRAGMA_ENABLE_OPTIMIZATION/PRAGMA_DISABLE_OPTIMIZATION implementation for whatever compiler you are porting to");
#endif

#if WIN32
    PRAGMA_DISABLE_OPTIMIZATION;
    inline void ReleaseModeBreakpoint()
    {
        __debugbreak();
    }
    PRAGMA_ENABLE_OPTIMIZATION;
#else
inline void ReleaseModeBreakpoint()
{
    raise(SIGTRAP);
}
#endif

#ifdef WIN32
#define PRAGMA_NO_PADDING_START __pragma(pack(push, 1))
#define PRAGMA_NO_PADDING_END __pragma(pack(pop))
#else
#define PRAGMA_NO_PADDING_START _Pragma("pack(push, 1)")
#define PRAGMA_NO_PADDING_END _Pragma("pack(pop)")
#endif

#if !WIN32
#define strcpy_s(dest, count)  strcpy((dest), (count))
#define ARRAYSIZE(arr) (sizeof(arr)/sizeof(arr[0]))

template<typename... Args>
inline void printf_s(const char* format, Args... args) {
    static_assert(std::is_same_v<const char*, decltype(format)>, "Format must be a string literal");
    printf(format, args...);
}
#endif
