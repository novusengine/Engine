#pragma once

#if defined(__clang__)
    #define DO_PRAGMA(X) _Pragma(#X)
    #define PRAGMA_CLANG_DIAGNOSTIC_PUSH        DO_PRAGMA(GCC diagnostic push)
    #define PRAGMA_CLANG_DIAGNOSTIC_POP         DO_PRAGMA(GCC diagnostic pop)
    #define PRAGMA_CLANG_DIAGNOSTIC_IGNORE(x)   DO_PRAGMA(GCC diagnostic ignored #x)
#else
    #define PRAGMA_CLANG_DIAGNOSTIC_PUSH
    #define PRAGMA_CLANG_DIAGNOSTIC_POP
    #define PRAGMA_CLANG_DIAGNOSTIC_IGNORE(x)
#endif

#if defined(__clang__)
    #define PRAGMA_ENABLE_OPTIMIZATION _Pragma("clang optimize on")
    #define PRAGMA_DISABLE_OPTIMIZATION _Pragma("clang optimize off")
#elif (_MSC_VER)
    #define PRAGMA_ENABLE_OPTIMIZATION _Pragma("optimize(\"\", on)")
    #define PRAGMA_DISABLE_OPTIMIZATION _Pragma("optimize(\"\", off)")
#else
    static_assert(false, "Please add PRAGMA_ENABLE_OPTIMIZATION/PRAGMA_DISABLE_OPTIMIZATION implementation for whatever compiler you are porting to");
#endif

#if WIN32
    PRAGMA_DISABLE_OPTIMIZATION;
    inline void ReleaseModeBreakpoint()
    {
        __debugbreak();
    }
    PRAGMA_ENABLE_OPTIMIZATION;
#else
    static_assert(false, "Please add a ReleaseModeBreakpoint implementation for whatever platform you are porting to");
#endif

#ifdef WIN32
#define NOVUS_NO_PADDING_START __pragma(pack(push, 1))
#define NOVUS_NO_PADDING_END __pragma(pack(pop))
#else
    static_assert(false, "Implement these for whatever platform you are porting to!");
#endif